/*

  Bluefire Editor: Utility functions

  (c) Copyright 2005-2006 Jacob Hipps

  http://neoretro.net/

*/

#define BLUEFIRE_CLNT
#define _BF_MAIN

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bluefire.h"

typedef struct {
	float A,B,C,d;
	int count, correct;
} BF_PLANE;

void vswap(void* vertex1, void* vertex2);
void tswap(void* vertex1, void* vertex2);

/*
	Correct the normals =]

	For environment polys, use fix_normals(-1) or fix_normals(BF_ENV_POLYS)
	For objects, use fix_normals(OBJNUM), where OBJNUM = index number of object
*/
int bfe_fix_normals(int num) {

	int num_envs;
	BF_PLANE planes[4096]; // <----- make this dynamic later <<< FIX
	BF_TRIANGLE_EX *thispoly;
	BF_RNODE *thisobj;
    int j, h, i, z, found;
	float d;
	int scountx;
	int num_of_planes, corrections;

	if(num == -1) {
		scountx = 1;
	} else {
		thisobj = bf_getobject(num);
		scountx = thisobj->surfaces;	
	}

	zlogthis("Commencing Normal Correction on object [%i], %i surfaces\n",num,scountx);
	num_of_planes = 0;
	for(z = 0; z < scountx; z++) {
		if(num == -1) {
			num_envs = bf_poly_cnt();		
		} else {
			num_envs = thisobj->surfdex[z].scount;
		}

		zlogthis("--Surface [%i], Polys [%i]\n",z,num_envs);

		for(h = 0; h < num_envs; h++) {
			if(num == -1) {
				thispoly = bf_getpoly(h);
			} else {
				thispoly = (BF_TRIANGLE_EX*)&thisobj->vtexinfo[z][h];
			}

			// Calculate d for the polygon:
			// Using the plane equation (Ax + By + Cz = d)...
			d = (thispoly->normal.x * thispoly->vertex[0].x) +
				(thispoly->normal.y * thispoly->vertex[0].y) +
				(thispoly->normal.z * thispoly->vertex[0].z);			

			zlogthis("------Poly [%i], Normal (%0.03f,%0.03f,%0.03f), d [%0.03f]\n",h,thispoly->normal.x,thispoly->normal.y,thispoly->normal.z,d);

			// Go through the list of current planes. Check to see if it's already defined.
			// If so, then increment the count...
			found = 0;
			for(i = 0; i < num_of_planes; i++) {
				if(planes[i].A == thispoly->normal.x &&
					planes[i].B == thispoly->normal.y &&
					planes[i].C == thispoly->normal.z &&
					planes[i].d == d) {
						found = 1;
						planes[i].count++;
						break;
					}
			}

			// If it's not defined, then add it...
			if(!found) {
				planes[num_of_planes].A = thispoly->normal.x;
				planes[num_of_planes].B = thispoly->normal.y;
				planes[num_of_planes].C = thispoly->normal.z;
				planes[num_of_planes].d = d;
				planes[num_of_planes].count = 1;
				planes[num_of_planes].correct = -1;
				num_of_planes++;
			}
		}

		// Find any polygons that share the same d value, and see if their
		// normals are inverted...

		corrections = 0;
		for(i = 0; i < num_of_planes; i++) {
			for(j = (i + 1); j < num_of_planes; j++) {
				if(planes[i].d == planes[j].d) {
					if( ((planes[i].A + planes[j].A == 0.0f) && planes[j].A != 0.0f) ||
						((planes[i].B + planes[j].B == 0.0f) && planes[j].B != 0.0f) ||
						((planes[i].C + planes[j].C == 0.0f) && planes[j].C != 0.0f)) {
							if(planes[i].count > planes[j].count) {
								planes[j].correct = i;
							} else {
								planes[i].correct = j;
							}
							corrections++;
						}
				}
			}
		}

		zlogthis("---Number of planes [%i], Corrections made [%i]\n",num_of_planes,corrections);

		// Make the appropriate corrections...
		for(h = 0; h < num_envs; h++) {
			if(num == -1) {
				thispoly = bf_getpoly(h);
			} else {
				thispoly = (BF_TRIANGLE_EX*)&thisobj->vtexinfo[z][h];
			}

			for(i = 0; i < num_of_planes; i++) {
				if(planes[i].A == thispoly->normal.x &&
				   planes[i].B == thispoly->normal.y &&
				   planes[i].C == thispoly->normal.z &&
				   planes[i].d == d) {
					if(planes[i].correct != -1) {
						thispoly->normal.x = planes[planes[i].correct].A;
						thispoly->normal.y = planes[planes[i].correct].B;
						thispoly->normal.z = planes[planes[i].correct].C;
					}
					break;
				}
			}
		}
	}

	return 0;
}


int bfe_fix_vtex_rot(int num) {
	int num_envs;
	BF_TRIANGLE_EX *thispoly;
	BF_RNODE *thisobj;
    int j, h, i, z, found;
	float d;
	int scountx;

	float tmp_x, tmp_y, tmp_z;

	if(num == -1) {
		scountx = 1;
	} else {
		thisobj = bf_getobject(num);
		scountx = thisobj->surfaces;	
	}

	zlogthis("Commencing Vertex Rotation Correction on object [%i], %i surfaces\n",num,scountx);

	for(z = 0; z < scountx; z++) {
		if(num == -1) {
			num_envs = bf_poly_cnt();		
		} else {
			num_envs = thisobj->surfdex[z].scount;
		}

		zlogthis("--Surface [%i], Polys [%i]\n",z,num_envs);

		for(h = 0; h < num_envs; h++) {
			if(num == -1) {
				thispoly = bf_getpoly(h);
			} else {
				thispoly = (BF_TRIANGLE_EX*)&thisobj->vtexinfo[z][h];
			}

			zlogthis("Polygon %i : pre = (%0.02f,%0.02f,%0.02f), (%0.02f,%0.02f,%0.02f), (%0.02f,%0.02f,%0.02f)\n", h,thispoly->vertex[0].x, thispoly->vertex[0].y,thispoly->vertex[0].z, thispoly->vertex[1].x, thispoly->vertex[1].y,thispoly->vertex[1].z, thispoly->vertex[2].x, thispoly->vertex[2].y,thispoly->vertex[2].z);

			if(abs(thispoly->normal.x) >= abs(thispoly->normal.z) && abs(thispoly->normal.x) > abs(thispoly->normal.y)) {
				// tending towards YZ axis
				if(thispoly->vertex[0].z > thispoly->vertex[1].z) {
					vswap(&thispoly->vertex[0],&thispoly->vertex[1]);
					tswap(&thispoly->tex[0],&thispoly->tex[1]);
				}
				if(thispoly->vertex[0].z > thispoly->vertex[2].z) {
					vswap(&thispoly->vertex[0],&thispoly->vertex[2]);
					tswap(&thispoly->tex[0],&thispoly->tex[2]);
				}
				if(thispoly->vertex[1].z > thispoly->vertex[2].z) {
					vswap(&thispoly->vertex[1],&thispoly->vertex[2]);
					tswap(&thispoly->tex[1],&thispoly->tex[2]);
				}
				if(thispoly->vertex[0].z > thispoly->vertex[1].z) {
					vswap(&thispoly->vertex[0],&thispoly->vertex[1]);
					tswap(&thispoly->tex[0],&thispoly->tex[1]);
				}
				if(thispoly->vertex[0].z == thispoly->vertex[1].z) {
					if(thispoly->vertex[0].y > thispoly->vertex[1].y) {
						vswap(&thispoly->vertex[0],&thispoly->vertex[1]);
						tswap(&thispoly->tex[0],&thispoly->tex[1]);
					}
				}
				if(thispoly->normal.x < 0.0f) {
					vswap(&thispoly->vertex[0],&thispoly->vertex[2]);
					tswap(&thispoly->tex[0],&thispoly->tex[2]);
				}
			} else if(abs(thispoly->normal.z) >= abs(thispoly->normal.y)){
				// tending towards XY axis
				if(thispoly->vertex[0].x > thispoly->vertex[1].x) {
					vswap(&thispoly->vertex[0],&thispoly->vertex[1]);
					tswap(&thispoly->tex[0],&thispoly->tex[1]);
				}
				if(thispoly->vertex[0].x > thispoly->vertex[2].x) {					
					vswap(&thispoly->vertex[0],&thispoly->vertex[2]);
					tswap(&thispoly->tex[0],&thispoly->tex[2]);
				}
				if(thispoly->vertex[1].x > thispoly->vertex[2].x) {
					vswap(&thispoly->vertex[1],&thispoly->vertex[2]);
					tswap(&thispoly->tex[1],&thispoly->tex[2]);
				}
				if(thispoly->vertex[0].x > thispoly->vertex[1].x) {
					vswap(&thispoly->vertex[0],&thispoly->vertex[1]);
					tswap(&thispoly->tex[0],&thispoly->tex[1]);
				}
				if(thispoly->vertex[0].x == thispoly->vertex[1].x) {
					if(thispoly->vertex[0].y > thispoly->vertex[1].y) {
						vswap(&thispoly->vertex[0],&thispoly->vertex[1]);
						tswap(&thispoly->tex[0],&thispoly->tex[1]);
					}
				}
				if(thispoly->normal.z < 0.0f) {
					vswap(&thispoly->vertex[0],&thispoly->vertex[2]);
					tswap(&thispoly->tex[0],&thispoly->tex[2]);
				}
			} else {
				// tending towards XZ axis
				if(thispoly->vertex[0].z > thispoly->vertex[1].z) {
					vswap(&thispoly->vertex[0],&thispoly->vertex[1]);
					tswap(&thispoly->tex[0],&thispoly->tex[1]);
				}
				if(thispoly->vertex[0].z > thispoly->vertex[2].z) {
					vswap(&thispoly->vertex[0],&thispoly->vertex[2]);
					tswap(&thispoly->tex[0],&thispoly->tex[2]);
				}
				if(thispoly->vertex[1].z > thispoly->vertex[2].z) {
					vswap(&thispoly->vertex[1],&thispoly->vertex[2]);
					tswap(&thispoly->tex[1],&thispoly->tex[2]);
				}
				if(thispoly->vertex[0].z > thispoly->vertex[1].z) {
					vswap(&thispoly->vertex[0],&thispoly->vertex[1]);
					tswap(&thispoly->tex[0],&thispoly->tex[1]);
				}
				if(thispoly->vertex[0].z == thispoly->vertex[1].z) {
					if(thispoly->vertex[0].z > thispoly->vertex[1].z) {
						vswap(&thispoly->vertex[0],&thispoly->vertex[1]);
						tswap(&thispoly->tex[0],&thispoly->tex[1]);
					}
				}
				if(thispoly->normal.y < 0.0f) {
					vswap(&thispoly->vertex[0],&thispoly->vertex[2]);
					tswap(&thispoly->tex[0],&thispoly->tex[2]);
				}

			}

			zlogthis("Polygon %i : POST= (%0.02f,%0.02f,%0.02f), (%0.02f,%0.02f,%0.02f), (%0.02f,%0.02f,%0.02f)\n", h, thispoly->vertex[0].x, thispoly->vertex[0].y,thispoly->vertex[0].z, thispoly->vertex[1].x, thispoly->vertex[1].y,thispoly->vertex[1].z, thispoly->vertex[2].x, thispoly->vertex[2].y,thispoly->vertex[2].z);

		}
	}

	return 0;
}


void vswap(void* vertex1, void* vertex2) {
	int swapsz = sizeof(float) * 3;
	float temparea[3];

	// swaps:
	//   vertex1  <-> vertex2
	// via:
	//   vertex2  --> temparea
	//   vertex1  --> vertex2
	//   temparea --> vertex1

	memcpy(&temparea,vertex2,swapsz);
	memcpy(vertex2,vertex1,swapsz);
	memcpy(vertex1,&temparea,swapsz);

	return;
}

void tswap(void* vertex1, void* vertex2) {
	int swapsz = sizeof(float) * 2;
	float temparea[2];

	// swaps:
	//   vertex1  <-> vertex2
	// via:
	//   vertex2  --> temparea
	//   vertex1  --> vertex2
	//   temparea --> vertex1

	memcpy(&temparea,vertex2,swapsz);
	memcpy(vertex2,vertex1,swapsz);
	memcpy(vertex1,&temparea,swapsz);

	return;
}