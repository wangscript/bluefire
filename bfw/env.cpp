/*

	Bluefire: Environmental Polygon Upkeep Routines
	(c) Copyright 2009 Jacob Hipps/Neo-Retro Group
	http://bluefire.neoretro.net/spec/bfw/env


	Implements:
		bf_env_addpoly()	- Returns index of newly allocated EnvPoly
		bf_env_getpoly()	- Returns pointer to EnvPoly
		bf_env_count()		- Returns number of EnvPolys
		bf_env_flush()		- Removes all EnvPolys and frees memory


*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bluefire.h"

extern void (__cdecl * go_down)();

// Globals
BF_TRIANGLE_EX **env_polydex;	// Environmental Polygon pool
int bf_poly_count;				// environment polygon count
int env_capacity;				// environment polygon capacity

/***

bf_add_env_poly()
bf_env_addpoly()

	Description:
		Adds the specified polygon to the environment

	Arguments:
		BF_TRIANGLE_EX *polyptr		Pointer to the polygon to add

	Return:
		index of polygon in the environment polygon cache

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
int bf_add_env_poly(BF_TRIANGLE_EX *polyptr) {
	BEGIN_FUNC("bf_add_env_poly")

	bf_poly_count++;

	if(env_capacity < bf_poly_count) env_capacity += 8;

	if((env_polydex = (BF_TRIANGLE_EX**)realloc(env_polydex,env_capacity * sizeof(BF_TRIANGLE_EX*))) == NULL) {
		zlogthis("bf_add_env_poly: Memory reallocation error\n");
		go_down();
	}

	if((env_polydex[bf_poly_count - 1] = (BF_TRIANGLE_EX*)malloc(sizeof(BF_TRIANGLE_EX))) == NULL) {
		zlogthis("bf_add_env_poly: Memory allocation error\n");
		go_down();
	}

	if(polyptr != NULL) {
		memcpy(env_polydex[bf_poly_count - 1],polyptr,sizeof(BF_TRIANGLE_EX));
	} else {
		memset(env_polydex[bf_poly_count - 1],0,sizeof(BF_TRIANGLE_EX));
	}

	END_FUNC
	return (bf_poly_count - 1);
}

/***

bf_getpoly()
bf_env_getpoly()

	Description:
		Returns a pointer to the specified polygon

	Arguments:
		int num			Index of polygon

	Return:
		pointer to polygon (BF_TRIANGLE_EX*)

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
BF_TRIANGLE_EX* bf_getpoly(int num) {	
	BEGIN_FUNC("bf_getpoly")
	END_FUNC
	return env_polydex[num];
}

/***

bf_poly_cnt()
bf_env_count()

	Description:
		Returns the number of polygons in the environmental polygon cache (env polydex)

	Arguments:
		(none)

	Return:
		number of polygons in the env polydex (int)

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
int bf_poly_cnt() {	
	BEGIN_FUNC("bf_poly_cnt")
	END_FUNC
	return bf_poly_count;
}

/***

bf_flush_env()
bf_env_flush()

	Description:
		Flushes/removes all polygons from the env polydex.

	Arguments:
		(none)

	Return:
		none

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
void bf_flush_env() {
	BEGIN_FUNC("bf_flush_env")

	for(int i = 0; i < bf_poly_count; i++) {
		if(env_polydex[i]) free(env_polydex[i]);
	}
	if((env_polydex = (BF_TRIANGLE_EX**)realloc(env_polydex,sizeof(BF_TRIANGLE_EX*))) == NULL) {
		zlogthis("bf_flush_ex: Memory reallocation error!\n");
		go_down();
	}
	env_capacity = 1;

	zlogthis("bf_flush_env: Environment polygon cache flushed.\n");

	END_FUNC	
}
