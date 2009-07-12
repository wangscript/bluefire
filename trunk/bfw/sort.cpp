/*


  Bluefire: Polygon sorting routines

  (c) Copyright 2004 Jacob Hipps
  
  Bluefire MDL Core: Winchester

*/

//#define BLUEFIRE_CORE

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bluefire.h"

#pragma warning(disable:4244)

// imported superglobals
extern BF_TRIANGLE_EX **env_polydex;
extern int bf_poly_count;

// local functions
int qsort_func(const void *val1, const void *val2);	// qsort comparison function

// globals
float r_x, r_y, r_z;


void bf_dsort_env(float x, float y, float z) {
	r_x = x;
	r_y = y;
	r_z = z;
	qsort(env_polydex,bf_poly_count,sizeof(BF_TRIANGLE_EX*),qsort_func);
	return;
}

int qsort_func(const void *val1, const void *val2) {
	float x, y, z, d1, d2, dd;
	BF_TRIANGLE_EX* blah;

	memcpy(&blah,val1,sizeof(BF_TRIANGLE_EX*));	

	d1 = 0.0f;
	d2 = 0.0f;

	int i;

	for(i = 0; i < 3; i++) {
		x = blah->vertex[i].x;
		y = blah->vertex[i].y;
		z = blah->vertex[i].z;
	
		//dd = (x - r_x)*(x - r_x) + (y - r_y)*(y - r_y) + (z - r_z)*(z - r_z);	// square distance
		dd = (r_x - x)*(r_x - x) + (r_y - y)*(r_y - y) + (r_z - z)*(r_z - z);	// square distance
		if(dd < d1) d1 = dd;
	}


	memcpy(&blah,val2,sizeof(BF_TRIANGLE_EX*));

	for(i = 0; i < 3; i++) {
		x = blah->vertex[i].x;
		y = blah->vertex[i].y;
		z = blah->vertex[i].z;
	
		//dd = (x - r_x)*(x - r_x) + (y - r_y)*(y - r_y) + (z - r_z)*(z - r_z);	// square distance
		dd = (r_x - x)*(r_x - x) + (r_y - y)*(r_y - y) + (r_z - z)*(r_z - z);	// square distance

		if(dd < d2) d2 = dd;
	}

	if(d1 > d2)			return 1;
	else if(d2 > d1)	return -1;
	else				return 0;
}

