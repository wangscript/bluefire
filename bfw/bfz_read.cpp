/*


  Bluefire: BFZ Parser

  (c) Copyright 2004-2006 Jacob Hipps
  
  Bluefire MDL Core: Winchester

*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bfconv/libpng/png.h"
#include "../bfconv/zlib/zlib.h"
#include "bluefire.h"


// Dump all loaded textures to a RAW file for debugging
//#define IMG_DUMP_RAW

// Imported Superglobals

extern void (__cdecl * go_down)();

extern BF_TEXNODE *img_pool[MAX_NODES];
extern BF_RNODE *obj_pool[MAX_NODES];

extern int bf_imgpool_count;
extern int objects_otl;
extern int sounds_otl;

char *cur_file;

char bfz_handles[16][64];
int  bfz_refs[16];
BFZ_FILE bfzz[16];
int  bfz_count;

// Private functions

int load_png (FILE *thefile, esm_picture* png_img);
int esm_find2com (int num);


int bfz_open(char filename[], BFZ_FILE *bfzp) {
	BEGIN_FUNC("bfz_open")

	int i, ii;

	/*	
	int already_open;

	for(i = 0; i < 16; i++) {
		if(!memcmp(bfz_handles,filename,strlen(filename))) {
			already_open = 1;
		}
	}

	if(!already_open) {		
		if((bfzp->fptrx = fopen(filename,"rb")) == NULL) {
			zlogthis("error: unable to open resource file \"%s\".\n",filename);
		}
		bfz_count++;
	} else {

	}
	*/

	if((bfzp->fptrx = fopen(filename,"rb")) == NULL) {
		zlogthis("error: unable to open resource file \"%s\".\n",filename);
	}

	for(i = strlen(filename); i >= 0; i--) {
		if(filename[i] == '.') {
			break;
		}
	}
	
	for(ii = strlen(filename); ii >= 0; ii--) {
		if(filename[ii] == '\\' || filename[ii] == '/') {
			ii++;
			break;
		}
	}
	if(ii < 0) ii = 0;

	memcpy(bfzp->filez,filename + ii, i - ii);
	bfzp->filez[i - ii] = 0;

	//zlogthis("bfz_open: File %s opened.\n",bfzp->filez);
	//strcpy(bfzp->filez,filename);

	END_FUNC
	return 0;
}

void bfz_close(BFZ_FILE *bfzp) {
	BEGIN_FUNC("bfz_close")

	fclose(bfzp->fptrx);

	END_FUNC
	return;
}


int bfz_loadobj(BFZ_FILE *bfzp, char res_name[], int nummy) {
	BEGIN_FUNC("bfz_loadobj")

	/*
	char	signature[3];
	char	datatype;
	long	cdata_sz;
	long	ddata_sz;
	long	crc32;
	char	resid[64];
   */

	FILE *bfz_ptr = bfzp->fptrx;

	cur_file = bfzp->filez;

    zlogthis("bfz_loadobj: Loading object %s:%s...\n",bfzp->filez,res_name);

	BFZ_CHUNK_HEADER chunky;

	fseek(bfz_ptr,0,0);
	
	int found = 0;
	while(!feof(bfz_ptr)) {
		if(!fread(&chunky,sizeof(BFZ_CHUNK_HEADER),1,bfz_ptr)) break;
		if(!memcmp(chunky.signature,"BFZ",3)) {
			if(!strcmp(chunky.resid,res_name) && chunky.datatype == BFC_OBJ) {
				// allocate some memory for all the data
				char *marker;
				if((marker = (char*)malloc(chunky.cdata_sz)) == NULL) {
					zlogthis("Memory allocation error");
					go_down();
				}

				char *ddout;
				if((ddout = (char*)malloc(chunky.ddata_sz)) == NULL) {
					zlogthis("Memory allocation error");
					go_down();
				}


				// read the file into memory

				fread(marker,chunky.cdata_sz,1,bfz_ptr);

				// this part uses zlib to decompress the data

				z_stream steamy;							// parameters for zlib decompression routine
				steamy.next_in = (unsigned char*)marker;	// set input data to [marker]
				steamy.avail_in = chunky.cdata_sz;			// set input size to [chunky.cdata_sz]
				steamy.next_out = (unsigned char*)ddout;	// put the output in [ddout]
				steamy.avail_out = chunky.ddata_sz;			// output buffer size is [chunky.ddata_sz]
				steamy.zalloc = (alloc_func)0;				// we don't care about zalloc...
				steamy.zfree = (free_func)0;				// zfree...
				steamy.opaque = (voidpf)0;					// or opaque... (wtf?)

				inflateInit(&steamy);			// initialize the inflate routine
				inflate(&steamy,Z_FINISH);		// decompress using inflation (in a single step)
				inflateEnd(&steamy);			// end decompression
	
				// calculate the CRC32 of the decompressed data
				long thiscrc = crc32(0L, (unsigned char*)ddout, steamy.total_out);

				// check to see if the CRC32 matches the expected CRC32 (stored in the header)
				if(chunky.crc32 != thiscrc) {
					zlogthis("Data is corrupt: invalid CRC32!\n");
					zlogthis("bfz_loadobj: invalid CRC32 in object \"%s\".\n\tCRC32 = 0x%08x (expected 0x%08x)\n",chunky.resid,thiscrc,chunky.crc32);
					END_FUNC
					return -1;
				}

				free(marker);	// free the marker
	
				#ifdef _DEBUG
				FILE *giggi = fopen("dizzle.dat","wb");
				fwrite(ddout,chunky.ddata_sz,1,giggi);
				fclose(giggi);
				#endif

				// parse the ObjX data and insert it into the object pool
				bf_format_objx(ddout, nummy);

				sprintf(bf_getobject(nummy)->name,"%s:%s",bfzp->filez,res_name);
				
				free(ddout);	// free the raw data

				zlogthis("bfz_loadobj: Object loaded successfully.\n");	
				found = 1;
				break;
			} else {
				fseek(bfz_ptr,chunky.cdata_sz,SEEK_CUR);
			}
		} else {
			zlogthis("bfz_loadobj: Not a BFZ file!\n");	
			break;
		}		
	}

	if(!found) {
		zlogthis("bfz_loadobj: Object not found!");
		END_FUNC
		return -1;
	}



	END_FUNC
	return nummy;
}



int bfz_loadimg(BFZ_FILE *bfzp, char res_name[], int nummy) {
	BEGIN_FUNC("bfz_loadimg")

	zlogthis("bfz_loadimg: Loading image %s:%s... ",bfzp->filez,res_name);

	/*
	char	signature[3];
	char	datatype;
	long	cdata_sz;
	long	ddata_sz;
	long	crc32;
	char	resid[64];
	*/
	FILE *bfz_ptr = bfzp->fptrx;

	BFZ_CHUNK_HEADER chunky;

	fseek(bfz_ptr,0,0);
	
	int found = 0;
	while(!feof(bfz_ptr)) {
		if(!fread(&chunky,sizeof(BFZ_CHUNK_HEADER),1,bfz_ptr)) break;
		if(!memcmp(chunky.signature,"BFZ",3)) {
			if(!strcmp(chunky.resid,res_name) && chunky.datatype == BFC_IMG) {
														
				esm_picture pic_data;

				if(load_png(bfz_ptr,&pic_data)) {
					zlogthis("Error loading PNG image from resource file!\n");
					return -1;
				}

				#ifdef IMG_DUMP_RAW
				// dump the RAW texture data (for testing/debugging)
				char dfname[128];
				sprintf(dfname,"%s-%s.raw",bfzp->filez,res_name);
				FILE *dumppy = fopen(dfname,"wb");
				fwrite(pic_data.data_ptr,pic_data.height * pic_data.width * 4,1,dumppy);
				fclose(dumppy);
				#endif

				unsigned int new_tex;
				
				bf_bind_texture(&pic_data, &new_tex);

				free(pic_data.data_ptr);

				BF_TEXNODE *tizzie = bf_getimg(nummy);

				sprintf(tizzie->resid,"%s:%s",bfzp->filez,res_name);
				tizzie->tex_id = new_tex;
				tizzie->width  = pic_data.width;
				tizzie->height = pic_data.height;

				zlogthis("ok.\n");				
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
		END_FUNC
		return -1;
	}
	
	END_FUNC
	return bf_imgpool_count;
}


/*

Bluefire environment/scene data

Encapsulated within BFZ Deflate chunk.

BF_ENVIRON_DATA			Header
CUSTOM[x]				Custom data (if present)
BF_RESDATA_EX[x]		Resource data array
BF_TRIANGLE_EX[x]		Polygon information

*/


int bfz_loadenv(BFZ_FILE *bfzp, char res_name[], char *custom_data, int cdata_sz) {
	BEGIN_FUNC("bfz_loadenv")

	/*
	char	signature[3];
	char	datatype;
	long	cdata_sz;
	long	ddata_sz;
	long	crc32;
	char	resid[64];
   */

	FILE *bfz_ptr = bfzp->fptrx;

    zlogthis("bfz_loadenv: Loading environment %s:%s...\n",bfzp->filez,res_name);

	BFZ_CHUNK_HEADER chunky;

	fseek(bfz_ptr,0,0);
	
	int found = 0;
	while(!feof(bfz_ptr)) {
		if(!fread(&chunky,sizeof(BFZ_CHUNK_HEADER),1,bfz_ptr)) break;
		if(!memcmp(chunky.signature,"BFZ",3)) {
			if(!strcmp(chunky.resid,res_name) && chunky.datatype == BFC_ENV) {
				// allocate some memory for all the data
				char *marker;
				if((marker = (char*)malloc(chunky.cdata_sz)) == NULL) {
					zlogthis("Memory allocation error");
					go_down();
				}

				char *ddout;
				if((ddout = (char*)malloc(chunky.ddata_sz)) == NULL) {
					zlogthis("Memory allocation error");
					go_down();
				}


				// read the file into memory

				fread(marker,chunky.cdata_sz,1,bfz_ptr);

				// this part uses zlib to decompress the data

				z_stream steamy;							// parameters for zlib decompression routine
				steamy.next_in = (unsigned char*)marker;	// set input data to [marker]
				steamy.avail_in = chunky.cdata_sz;			// set input size to [chunky.cdata_sz]
				steamy.next_out = (unsigned char*)ddout;	// put the output in [ddout]
				steamy.avail_out = chunky.ddata_sz;			// output buffer size is [chunky.ddata_sz]
				steamy.zalloc = (alloc_func)0;				// we don't care about zalloc...
				steamy.zfree = (free_func)0;				// zfree...
				steamy.opaque = (voidpf)0;					// or opaque... (wtf?)

				inflateInit(&steamy);			// initialize the inflate routine
				inflate(&steamy,Z_FINISH);		// decompress using inflation (in a single step)
				inflateEnd(&steamy);			// end decompression
	
				// calculate the CRC32 of the decompressed data
				long thiscrc = crc32(0L, (unsigned char*)ddout, steamy.total_out);

				// check to see if the CRC32 matches the expected CRC32 (stored in the header)
				if(chunky.crc32 != thiscrc) {
					zlogthis("Data is corrupt: invalid CRC32!\n");
					zlogthis("bfz_loadenv: invalid CRC32 in environment \"%s\".\n\tCRC32 = 0x%08x (expected 0x%08x)\n",chunky.resid,thiscrc,chunky.crc32);
					return -1;
				}

				free(marker);	// free the marker
				
				BF_ENVIRON_DATA *environ_data;
				BF_RESDATA_EX   *resdata_ex;
				BF_TRIANGLE_EX	*triangle_ex;
				char			*custom_data;
				
				environ_data = (BF_ENVIRON_DATA*)ddout;

				zlogthis("ENVIRON_DATA chunk:\n");
				zlogthis("\tEnvironment ID: \"%s\"\n",environ_data->id);
				zlogthis("\tPolygons: %i\n",environ_data->polycount);
				zlogthis("\tObjects: %i\n",environ_data->objects);
				zlogthis("\tScripts: %i\n",environ_data->scripts);
				zlogthis("\tTextures: %i\n",environ_data->textures);
				zlogthis("\tSize of custom data: %i bytes\n",environ_data->sz_custom);
				
				// load custom data, if present and an adequate buffer is available
				if(environ_data->sz_custom > 0 && custom_data != NULL && cdata_sz >= environ_data->sz_custom) {
					memcpy(custom_data,ddout + sizeof(BF_ENVIRON_DATA),environ_data->sz_custom);
				}

				//bf_surfaces[.

				free(ddout);	// free the raw data

				zlogthis("bfz_loadenv: Environment loaded successfully.\n");
				found = 1;
				break;
			} else {
				fseek(bfz_ptr,chunky.cdata_sz,SEEK_CUR);
			}
		} else {
			zlogthis("bfz_loadenv: Not a BFZ file!\n");	
			break;
		}		
	}

	if(!found) {
		zlogthis("bfz_loadenv: Environment not found!");
		END_FUNC
		return -1;
	}



	END_FUNC
	return 0;
}


int bfz_loadexec(BFZ_FILE *bfzp, char res_name[]) {
	BEGIN_FUNC("bfz_loadexec")

	/*
	char	signature[3];
	char	datatype;
	long	cdata_sz;
	long	ddata_sz;
	long	crc32;
	char	resid[64];
   */

	FILE *bfz_ptr = bfzp->fptrx;
	int resindex;

	cur_file = bfzp->filez;

    zlogthisx("Loading script %s:%s...",bfzp->filez,res_name);

	BFZ_CHUNK_HEADER chunky;

	fseek(bfz_ptr,0,0);
	
	int found = 0;
	while(!feof(bfz_ptr)) {
		if(!fread(&chunky,sizeof(BFZ_CHUNK_HEADER),1,bfz_ptr)) break;
		if(!memcmp(chunky.signature,"BFZ",3)) {
			if(!strcmp(chunky.resid,res_name) && chunky.datatype == BFC_EXEC) {
				// allocate some memory for all the data
				char *marker;
				if((marker = (char*)malloc(chunky.cdata_sz)) == NULL) {
					zlogthisx("Memory allocation error");
					go_down();
				}

				char *ddout;
				if((ddout = (char*)malloc(chunky.ddata_sz)) == NULL) {
					zlogthisx("Memory allocation error");
					go_down();
				}


				// read the file into memory

				fread(marker,chunky.cdata_sz,1,bfz_ptr);

				// this part uses zlib to decompress the data

				z_stream steamy;							// parameters for zlib decompression routine
				steamy.next_in = (unsigned char*)marker;	// set input data to [marker]
				steamy.avail_in = chunky.cdata_sz;			// set input size to [chunky.cdata_sz]
				steamy.next_out = (unsigned char*)ddout;	// put the output in [ddout]
				steamy.avail_out = chunky.ddata_sz;			// output buffer size is [chunky.ddata_sz]
				steamy.zalloc = (alloc_func)0;				// we don't care about zalloc...
				steamy.zfree = (free_func)0;				// zfree...
				steamy.opaque = (voidpf)0;					// or opaque... (wtf?)

				inflateInit(&steamy);			// initialize the inflate routine
				inflate(&steamy,Z_FINISH);		// decompress using inflation (in a single step)
				inflateEnd(&steamy);			// end decompression
	
				// calculate the CRC32 of the decompressed data
				long thiscrc = crc32(0L, (unsigned char*)ddout, steamy.total_out);

				// check to see if the CRC32 matches the expected CRC32 (stored in the header)
				if(chunky.crc32 != thiscrc) {
					zlogthisx("data is corrupt!");
					zlogthisx("invalid CRC32 in object \"%s\". CRC32 = 0x%08x (expected 0x%08x)",chunky.resid,thiscrc,chunky.crc32);
					END_FUNC
					return -1;
				}

				free(marker);	// free the marker
	
				#ifdef _DEBUG
				FILE *giggi = fopen("dizzle9.dat","wb");
				fwrite(ddout,chunky.ddata_sz,1,giggi);
				fclose(giggi);
				#endif


				// load the script into the script pool
				char resname[256];

				sprintf(resname,"%s:%s",bfzp->filez,res_name);

				zlogthisx("ddata_sz = %i, strlen() = %i",chunky.ddata_sz,strlen(ddout));

				resindex = bfi_load_script(ddout, chunky.ddata_sz, resname);
				
				free(ddout);	// free the raw data

				zlogthisx("Script loaded successfully. (res index = %i)",resindex);

				found = 1;
				break;
			} else {
				fseek(bfz_ptr,chunky.cdata_sz,SEEK_CUR);
			}
		} else {
			zlogthisx("Not a BFZ file!");
			break;
		}		
	}

	if(!found) {
		zlogthisx("Script not found!");
		END_FUNC
		return -1;
	}



	END_FUNC
	return resindex;
}


int load_png (FILE *thefile, esm_picture* png_img) {
  BEGIN_FUNC("load_png")

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
  png_img->depth = 32; //png_get_bit_depth(png_ptr, info_ptr);

  // get the nearest powers of two
  int x_2com = esm_find2com(png_img->width);
  int y_2com = esm_find2com(png_img->height);
  
  // add an alpha channel
  bpx = 4; // 4 bytes/pixel
  png_img->type = 4;
  // this will make all the pixels of our image opaque and this ones
  // that are simply the buffer zone be transparent
  png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);


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
  free(row_pointers);

  png_img->width = x_2com;
  png_img->height = y_2com;

  END_FUNC
  return 0;
}



int esm_find2com (int num) {
	BEGIN_FUNC("esm_find2com")

    int fnum = 1;
    while (fnum < num) {
          fnum *= 2;
    }

	END_FUNC
    return fnum;
}
