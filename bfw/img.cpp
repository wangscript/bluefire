/*

	Bluefire: Image/Texture Upkeep Routines
	(c) Copyright 2009 Jacob Hipps/Neo-Retro Group
	http://bluefire.neoretro.net/spec/bfw/img


	Implements:
		bf_img_add()	- Returns index of a newly allocated image
		bf_img_getptr()	- Returns the pointer to an image structure
		bf_img_getnum()	- Returns the index of a named image resource
		bf_img_count()	- Returns the number of images in the image pool

*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bluefire.h"

extern void (__cdecl * go_down)();

// Globals
BF_TEXNODE *img_pool[MAX_NODES];	// Image/texture pool
int bf_imgpool_count;				// image count


/***

bf_addimg()
bf_img_add()

	Description:
		Adds an image to the image pool

	Arguments:
		(none)

	Return:
		nothing

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
int bf_addimg() {
	BEGIN_FUNC("bf_addimg")

	int nummy = bf_imgpool_count;
	bf_imgpool_count++;


	if((img_pool[nummy] = (BF_TEXNODE*)malloc(sizeof(BF_TEXNODE))) == NULL) {
		zlogthis("Memory allocation error!");
		go_down();
	}

	memset(img_pool[nummy],0,sizeof(BF_TEXNODE)); // zero-out the memory space

	END_FUNC
	return nummy;
}


/***

bf_getimg()
bf_img_getptr()

	Description:
		Returns the pointer of an image object in the image pool

	Arguments:
		int nummy		Index of image object

	Return:
		pointer to image object (BF_TEXNODE*)

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
BF_TEXNODE* bf_getimg(int nummy) {	
	BEGIN_FUNC("bf_getimg")
	END_FUNC
	return img_pool[nummy];
}

/***

bf_getimg_num()
bf_img_getnum()

	Description:
		Returns the pointer of an image object in the image pool.
		See bf_obj_getnum() for more details.

	Arguments:
		char *img_name		Name of image object

	Return:
		index of image object, or -1 if not found

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
int bf_getimg_num(char img_name[]) {
	BEGIN_FUNC("bf_getimg_num")

	int nummy = -1;

	for(int i = 0; i < bf_imgpool_count; i++) {
		if(!strcmp(img_name,img_pool[i]->resid)) {
			nummy = i;
			break;
		}
	}

	END_FUNC
	return nummy;
}

/***

bf_img_count()

	Description:
		Returns the number of images in the image pool

	Arguments:
		(none)

	Return:
		number of images in the image pool (int)

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
int bf_img_count() {
	BEGIN_FUNC("bf_img_count")
	END_FUNC
	return bf_imgpool_count;
}

