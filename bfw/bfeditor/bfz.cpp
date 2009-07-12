/*

	Bluefire Editor: BFZ Routines
	(c) Copyright 2004 Jacob Hipps
	http://thefro.net/~jacob/

	http://neoretro.net/

*/

#define BLUEFIRE_CLNT

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../bluefire.h"
#include "../../bfconv/libpng/png.h"
#include "../../bfconv/zlib/zlib.h"

extern int in_format;
extern int bfz_mode;
extern esm_picture png_img;
extern char item_name[64];


BFZ_CHUNK_HEADER chunky;

typedef struct {
	int index;
	char resid[64];
	long cdata_sz;
	long ddata_sz;
	char datatype;
} BFZ_ITEMS;


int bfz_read(FILE *fin, BFZ_ITEMS *items, int maxsize);
int bfz_img_read(FILE* bfz_ptr, char *res_name, esm_picture *pic_data);
//int bfz_obj_read(FILE* bfz_ptr, esm_picture *pic_data);

int load_png (FILE *thefile, esm_picture* png_img);

int bfz_read(FILE *fin, BFZ_ITEMS *items, int maxsize) {

	/*
	char	signature[3];
	char	datatype;
	long	cdata_sz;
	long	ddata_sz;
	long	crc32;
	char	resid[64];
	*/

	fseek(fin,0,SEEK_END);
	int fsize = ftell(fin);
	fseek(fin,0,0);
	

	int i = 0;

	while(!feof(fin)) {
		if(!fread(&chunky,sizeof(BFZ_CHUNK_HEADER),1,fin)) break;
		if(!memcmp(chunky.signature,"BFZ",3)) {
			if(i >= maxsize) return i;
			items[i].index = i;
			items[i].datatype = chunky.datatype;
			items[i].cdata_sz = chunky.cdata_sz;
			items[i].ddata_sz = chunky.ddata_sz;
			strcpy(items[i].resid,chunky.resid);
			i++;
			fseek(fin,chunky.cdata_sz,SEEK_CUR);
		} else {
			break;
		}		
	}

	return i;
}


int bfz_img_read(FILE* bfz_ptr, char *res_name, esm_picture *pic_data) {

	/*
	char	signature[3];
	char	datatype;
	long	cdata_sz;
	long	ddata_sz;
	long	crc32;
	char	resid[64];
	*/	

	BFZ_CHUNK_HEADER chunky;

	fseek(bfz_ptr,0,0);
	
	int found = 0;
	while(!feof(bfz_ptr)) {
		if(!fread(&chunky,sizeof(BFZ_CHUNK_HEADER),1,bfz_ptr)) break;
		if(!memcmp(chunky.signature,"BFZ",3)) {
			if(!strcmp(chunky.resid,res_name) && chunky.datatype == BFC_IMG) {

				if(load_png(bfz_ptr,pic_data)) {
					zlogthis("Error loading PNG image from resource file!\n");
					return -1;
				}

				found = 1;
				break;
			} else {
				fseek(bfz_ptr,chunky.cdata_sz,SEEK_CUR);
			}
		} else {
			zlogthis("not a BFZ file! ");
			break;
		}		
	}

	if(!found) {
		zlogthis("not found!");	
		return -1;
	}
		
	return 0;
}

int load_png (FILE *thefile, esm_picture* png_img) {

  png_img->type = 3;
  int bpx = 3;

  // check the header to make sure we've got a PNG
  png_byte header[8];
  fread(header,1,8,thefile);
  if(png_sig_cmp(header, 0, 8)) {
      // infidels!
      return 1;
  }

  png_structp png_ptr;
  png_infop info_ptr;
  png_infop end_info;

  if(!(png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL/*(png_voidp)user_error_ptr*/, NULL/*user_error_fn*/, NULL/*user_warning_fn*/))) {
      // memory allocation error! (probably)
      return 1;
  }
  if(!(info_ptr = png_create_info_struct(png_ptr))) {
      // memory allocation error! (probably)
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      return 1;
  }
  if(!(end_info = png_create_info_struct(png_ptr))) {
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
      return 1;
  }

  // initialize file i/o
  png_init_io(png_ptr, thefile);
  // tell libpng that we already read 8 bytes from the file to check for the signature/header
  png_set_sig_bytes(png_ptr, 8);
  // read it! take the low and broad way, which is good in this case, eh?
  png_read_info(png_ptr, info_ptr);
  // get parameters.
  png_img->height = png_get_image_height(png_ptr, info_ptr);
  png_img->width = png_get_image_width(png_ptr, info_ptr);
  png_img->depth = 24; //png_get_bit_depth(png_ptr, info_ptr);

  int x_2com = png_img->width;
  int y_2com = png_img->height;
  
  // strip the alpha channel
  png_set_strip_alpha(png_ptr);

  // add an alpha channel
  bpx = 3;
  png_img->type = 3;

  // allocate memory. ack.
  if((png_img->data_ptr = calloc(x_2com,y_2com * bpx)) == NULL) {
           zlogthis("error: load_png: memory allocation error while reading! bail out!\n");
           
           return 1;
  }

  // calculate the row pointers. egad.
  int i;
  
	png_bytep row_pointers[2048]; // negligence at work :)

      int img_size = png_img->height * png_img->width * bpx;
      int img_size2 = x_2com * y_2com * bpx;
      for (i = 0; i < png_img->height; i++) {
           // a bit complicated, eh? i will probably forget what i am doing here, so here is an explanation:
           // row_pointer[current scanline] = data pointer + (width * 3 * (current scanline + 1))
           //*(row_pointers + (i * sizeof(png_bytep))) = (png_byte*)((int)png_img->data_ptr + (img_size2 - (x_2com * bpx * (i + 1))));
		  row_pointers[i] = (png_byte*)((int)png_img->data_ptr + (x_2com * bpx * i));
      }

  // read the image!
  png_read_image(png_ptr, row_pointers);
  png_read_end(png_ptr, end_info);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  //free(row_pointers);

  png_img->width = x_2com;
  png_img->height = y_2com;

  return 0;
}


