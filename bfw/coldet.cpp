/*


  Bluefire: Collision Detection routines

  (c) Copyright 2006 Jacob Hipps
  
  Bluefire MDL Core: Winchester

*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bluefire.h"

#define BF_CLT_SPHERE		0
#define BF_CLT_BOX			1
#define BF_CLT_CYL			2
#define BF_CLT_RAY			3

typedef struct {	
	struct {
		float x, y, z, d; // velocity vector and magnitude
	} velocity;	
	float mass;			// object mass
	float density;		// object or material density
	int col_type;		// of type BF_CLT_*

	// collision bounds
	struct {
		union { float x; float r; }; // x size or radius
		float y;					 // y size
		float z;					 // z size
	} colb;
} BF_COLDATA;



// perform collision detection

int bf_coldet() {

	return 0;
}

