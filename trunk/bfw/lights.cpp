/*


  Bluefire: Light object routines

  (c) Copyright 2004-2009 Jacob Hipps

*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bluefire.h"

// Imports
extern void (__cdecl * go_down)();

// Globals
BF_LIGHT *lights[MAX_LIGHTS];
int bf_light_count;

// Light-pool accounting routines

int bf_addlight() {
	BEGIN_FUNC("bf_addlight")

	int nummy = bf_light_count;
	bf_light_count++;


	if((lights[nummy] = (BF_LIGHT*)malloc(sizeof(BF_LIGHT))) == NULL) {
		zlogthis("Memory allocation error!");
		go_down();
	}

	memset(lights[nummy],0,sizeof(BF_LIGHT)); // zero-out the memory space

	END_FUNC
	return nummy;
}


BF_LIGHT* bf_getlight(int nummy) {	
	BEGIN_FUNC("bf_getlight")
	END_FUNC
	return lights[nummy];
}


int bf_getlight_num(char light_name[]) {
	BEGIN_FUNC("bf_getlight_num")

	int nummy = -1;

	for(int i = 0; i < bf_light_count; i++) {
		if(!strcmp(light_name,lights[i]->resid)) {
			nummy = i;
			break;
		}
	}

	END_FUNC
	return nummy;
}


int bf_light_cnt() {
	BEGIN_FUNC("bf_light_cnt")
	END_FUNC
	return bf_light_count;
}
