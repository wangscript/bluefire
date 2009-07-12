/*


  Bluefire: Lighting/Raytracing routines

  (c) Copyright 2004-2009 Jacob Hipps
  

  Finally works after 22 straight hours of calculus and trig *fun*!! ^_^

*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bluefire.h"
#include <signal.h>

#pragma warning(disable:4244)

/*
typedef struct {	
	int lumens;			// luminous flux (lumens)
	float r, g, b;		// color
	float x, y, z;		// position
	float nx,ny,nz;		// vector normal (for spot lights)
	int l_type;			// 0 = point
						// 1 = direction
						// 2 = spot
						// 3 = ambient
						// 4 = area
						// 5 = specular
	bool bEnabled;		// enabled?
	bool bDynamic;		// effect dynamic shadowing/lighting?
	bool bNegative;		// is it a subtractive/negative light?
	bool bNoLMaps;		// does not effect lightmaps?
	char resid[64];		// resource id/name
} BF_LIGHT;
*/

typedef struct {
	float x, y, z;		// verticies
} BF_POINT3;


extern BF_LIGHT *lights[MAX_LIGHTS];

// collision tolerance for raytracing
// Note: lowering this value will cause false-positives,
//       raising this value will cause missed collisions
#define COL_TOLERANCE 0.005f

float lm_ratio = 3.0f;  // 3:1 (LM:WC)
float Tw = 32.0f;		// lmap texture width
float Th = 32.0f;		// lmap texture height


typedef struct {
	int res;			// lightmap resolution
	int oversample;		// oversampling anti-aliasing level
	int blend;			// blending type
						// 0 = BF_LM_AA_DISABLE = Disable blending
						// 1 = BF_LM_AA_8PT = 8 point weighted (default)
						// 2 = BF_LM_AA_BLEND = Blend neighbouring pixels
						// 3 = BF_LM_AA_LW8PT = Location-weighted 8 point						
	int lm_type;		// lightmap type
						// 0 = BF_LM_RGB8 = RGB 8bpc, 24bpp (default)
						// 1 = BF_LM_RGBA4 = RGBA 4bpc, 16bpp
						// 2 = BF_LM_YC8 = Greyscale 8bpp
						// 3 = BF_LM_RGBA8 = RGBA 8bpc, 32bpp
} BF_LM_PARAMS;


/*

int bf_gen_lmap()

Lighting raytracer and lightmap generator

To do:  Implement lightmap texture bounds detection and auto placement

Status: Incomplete (lacking key feautres), but working

*/

int bf_gen_lmap(BF_TRIANGLE_EX *triangle, int ident) {
	BEGIN_FUNC("bf_gen_lmap")

	int proj; // projection
	int i;
	BF_TRIANGLE_EX thislmap;

	float min_u, min_v, max_u, max_v;
	float du, dv, d, w;

	float edge1_x, edge1_y, edge1_z;
	float edge2_x, edge2_y, edge2_z;
	float newedge1_x, newedge1_y, newedge1_z;
	float newedge2_x, newedge2_y, newedge2_z;

	int li, lc, lp;
	int blix, bliy;			// lightmap X,Y coordinates
	float fallout;			// light fallout distance
	float Rx, Ry;
	float Wx, Wy, Wz;
	float Lx, Ly, Lz;
	float lumR, lumG, lumB;
	float Tw2, Th2;
	char *lumels;			// pointer for RGB values of lightmap texture =]	

	int polyz;

	float lv_x, lv_y, lv_z, l, anglex;

	// collision detection variables
	float Nx, Ny, Nz, Nd;
	float ssl_x, ssl_y, ssl_z;
	float ssr_x, ssr_y, ssr_z;
	float t, t_bot;
	float Px, Py, Pz;
	float R1x, R1y, R1z, Q1x, Q1y, Q1z, Q2x, Q2y, Q2z;
	float Q1_dd, Q2_dd, Q12_d, sc_mul, RQ1, RQ2;
	float w0, w1, w2;
	BF_POINT3 start_pt, v_pt;
	bool collision;
	float p_d, p_org, p_dest;


	// determine plane projection
	
	if((fabs(triangle->normal.x) > fabs(triangle->normal.y)) && (fabs(triangle->normal.x) > fabs(triangle->normal.z))) {
		// project upon yz plane
		proj = 0;
		//zlogthis("YZ Projection\n");
		thislmap.tex[0].u = triangle->vertex[0].y;
		thislmap.tex[0].v = triangle->vertex[0].z;
		thislmap.tex[1].u = triangle->vertex[1].y;
		thislmap.tex[1].v = triangle->vertex[1].z;
		thislmap.tex[2].u = triangle->vertex[2].y;
		thislmap.tex[2].v = triangle->vertex[2].z;
	} else if ((fabs(triangle->normal.y) > fabs(triangle->normal.z)) && (fabs(triangle->normal.y) > fabs(triangle->normal.x))) {
		// project upon xz plane
		proj = 1;
		//zlogthis("XZ Projection\n");
		thislmap.tex[0].u = triangle->vertex[0].x;
		thislmap.tex[0].v = triangle->vertex[0].z;
		thislmap.tex[1].u = triangle->vertex[1].x;
		thislmap.tex[1].v = triangle->vertex[1].z;
		thislmap.tex[2].u = triangle->vertex[2].x;
		thislmap.tex[2].v = triangle->vertex[2].z;
	} else {
		// project upon xy plane
		proj = 2;
		//zlogthis("XY Projection\n");
		thislmap.tex[0].u = triangle->vertex[0].x;
		thislmap.tex[0].v = triangle->vertex[0].y;
		thislmap.tex[1].u = triangle->vertex[1].x;
		thislmap.tex[1].v = triangle->vertex[1].y;
		thislmap.tex[2].u = triangle->vertex[2].x;
		thislmap.tex[2].v = triangle->vertex[2].y;
	}	

	// detect uv bounds
	min_u = thislmap.tex[0].u;
	min_v = thislmap.tex[0].v;
	max_u = thislmap.tex[0].u;
	max_v = thislmap.tex[0].v;

	for(i = 0; i < 3; i++) {
		if (thislmap.tex[i].u < min_u) min_u = thislmap.tex[i].u;
		if (thislmap.tex[i].v < min_v) min_v = thislmap.tex[i].v;
		if (thislmap.tex[i].u > max_u) max_u = thislmap.tex[i].u;
		if (thislmap.tex[i].v > max_v) max_v = thislmap.tex[i].v;
	}

	//min_u += 1.0f/Tw;
	//min_v += 1.0f/Th;
	//max_u -= 1.0f/Tw;
	//max_v -= 1.0f/Th;

	if((du = max_u - min_u) == 0.0f) {
		du = 0.01f;
	}
	if((dv = max_v - min_v) == 0.0f) {
		dv = 0.01f;
	}


	// normalize the texture coordinates
	for(i = 0; i < 3; i++) {
		triangle->lm_tex[i].u = thislmap.tex[i].u - min_u;
		triangle->lm_tex[i].v = thislmap.tex[i].v - min_v;
		triangle->lm_tex[i].u /= du;
		triangle->lm_tex[i].v /= dv;
		if(triangle->lm_tex[i].u <= 0.0f) triangle->lm_tex[i].u = 1.0f/Tw;
		if(triangle->lm_tex[i].v <= 0.0f) triangle->lm_tex[i].v = 1.0f/Th;
		if(triangle->lm_tex[i].u >= 1.0f) triangle->lm_tex[i].u = 1.0f - (1.0f/Tw);
		if(triangle->lm_tex[i].v >= 1.0f) triangle->lm_tex[i].v = 1.0f - (1.0f/Th);

		//zlogthis("lm tex vtex[%i](%0.02f,%0.02f)\n",i,triangle->lm_tex[i].u,triangle->lm_tex[i].v);
	}

	/*
	for(i = 0; i < 3; i++) {
		thislmap.tex[i].u -= min_u;
		thislmap.tex[i].v -= min_v;
		thislmap.tex[i].u /= du;
		thislmap.tex[i].v /= dv;
	}
	*/

	d = -(triangle->normal.x * triangle->vertex[0].x + triangle->normal.y * triangle->vertex[0].y + triangle->normal.z * triangle->vertex[0].z);

    switch (proj) {
		case 0: //YZ Plane
				w = -(triangle->normal.y * min_u + triangle->normal.z * min_v + d) / triangle->normal.x;
                thislmap.vertex[0].x = w;
                thislmap.vertex[0].y = min_u;
                thislmap.vertex[0].z = min_v;
                w = -(triangle->normal.y * max_u + triangle->normal.z * min_v + d) / triangle->normal.x;
                thislmap.vertex[1].x = w;
                thislmap.vertex[1].y = max_u;
                thislmap.vertex[1].z = min_v;
                w = -(triangle->normal.y * min_u + triangle->normal.z * max_v + d) / triangle->normal.x;
                thislmap.vertex[2].x = w;
                thislmap.vertex[2].y = min_u;
                thislmap.vertex[2].z = max_v;
				break;

            case 1: //XZ Plane
                w = -(triangle->normal.x * min_u + triangle->normal.z * min_v + d) / triangle->normal.y;
                thislmap.vertex[0].x = min_u;
                thislmap.vertex[0].y = w;
                thislmap.vertex[0].z = min_v;
                w = -(triangle->normal.x * max_u + triangle->normal.z * min_v + d) / triangle->normal.y;
                thislmap.vertex[1].x = max_u;
                thislmap.vertex[1].y = w;
                thislmap.vertex[1].z = min_v;
                w = -(triangle->normal.x * min_u + triangle->normal.z * max_v + d) / triangle->normal.y;
                thislmap.vertex[2].x = min_u;
                thislmap.vertex[2].y = w;
                thislmap.vertex[2].z = max_v;
				break;

            case 2: //XY Plane
                w = -(triangle->normal.x * min_u + triangle->normal.y * min_v + d) / triangle->normal.z;
                thislmap.vertex[0].x = min_u;
                thislmap.vertex[0].y = min_v;
                thislmap.vertex[0].z = w;
                w = -(triangle->normal.x * max_u + triangle->normal.y * min_v + d) / triangle->normal.z;
                thislmap.vertex[1].x = max_u;
                thislmap.vertex[1].y = min_v;
                thislmap.vertex[1].z = w;
                w = -(triangle->normal.x * min_u + triangle->normal.y * max_v + d) / triangle->normal.z;
                thislmap.vertex[2].x = min_u;
                thislmap.vertex[2].y = max_v;
                thislmap.vertex[2].z = w;
				break;
	}

	edge1_x = thislmap.vertex[1].x - thislmap.vertex[0].x;
	edge1_y = thislmap.vertex[1].y - thislmap.vertex[0].y;
	edge1_z = thislmap.vertex[1].z - thislmap.vertex[0].z;
	edge2_x = thislmap.vertex[2].x - thislmap.vertex[0].x;
	edge2_y = thislmap.vertex[2].y - thislmap.vertex[0].y;
	edge2_z = thislmap.vertex[2].z - thislmap.vertex[0].z;

	Tw2 = Tw - 2.0f;
	Th2 = Th - 2.0f;

	//zlogthis("edge1 (%0.02f,%0.02f,%0.02f), edge2 (%0.02f,%0.02f,%0.02f)\n",edge1_x,edge1_y,edge1_z,edge2_x,edge2_y,edge2_z);
	//zlogthis("min (%0.02f,%0.02f), max (%0.02f,%0.02f)\n",min_u,min_v,max_u,max_v);

	lc = bf_light_cnt();	// get # of lights
	//zlogthis("lc = %i\n",lc);

	// allocate memory for lightmap texture (RGB, 8bpc, 24bpp)
	if((lumels = (char*)malloc(Tw * Th * 3)) == NULL) {
		zlogthis("bf_gen_lmap: error: Memory allocation error!\n");
		bf_shutdown();
	}

	memset(lumels, 0, Tw * Th * 3);	// black-out
	polyz = bf_poly_cnt();


	for (bliy = 0; bliy < Th2; bliy++) {
		Ry = float(bliy)/Th2;	 // y scaling factor

		for(blix = 0; blix < Tw2; blix++) {
			Rx = float(blix)/Tw2; // x scaling factor

			newedge1_x = edge1_x * Rx;
			newedge1_y = edge1_y * Rx;
			newedge1_z = edge1_z * Rx;
			newedge2_x = edge2_x * Ry;
			newedge2_y = edge2_y * Ry;
			newedge2_z = edge2_z * Ry;

			Wx = thislmap.vertex[0].x + newedge2_x + newedge1_x;
			Wy = thislmap.vertex[0].y + newedge2_y + newedge1_y;
			Wz = thislmap.vertex[0].z + newedge2_z + newedge1_z;

			lumR = 0.0f;
			lumG = 0.0f;
			lumB = 0.0f;

			for(li = 0; li < lc; li++) {
				if(bf_getlight(li)->bEnabled && !bf_getlight(li)->bNoLMaps) {
					Lx = bf_getlight(li)->x;
					Ly = bf_getlight(li)->y;
					Lz = bf_getlight(li)->z;
					d = sqrtf(((Lx - Wx)*(Lx - Wx)) + ((Ly - Wy)*(Ly - Wy)) + ((Lz - Wz)*(Lz - Wz)));
					
					if(d == 0.0f) {
						continue;
					}

					collision = false;

					if(bf_getlight(li)->l_type == BF_LIGHT_POINT || bf_getlight(li)->l_type == BF_LIGHT_DIRECTION) {
						// new env poly collision detection ^_^
						for(lp = 0; lp < bf_env_count(); lp++) {

							p_d    = -(bf_env_getpoly(lp)->normal.x * bf_env_getpoly(lp)->vertex[0].x + bf_env_getpoly(lp)->normal.y * bf_env_getpoly(lp)->vertex[0].y + bf_env_getpoly(lp)->normal.z * bf_env_getpoly(lp)->vertex[0].z);
							p_org  = (((bf_env_getpoly(lp)->normal.x * Lx) + (bf_env_getpoly(lp)->normal.y * Ly) + (bf_env_getpoly(lp)->normal.z * Lz) + p_d) > 0.0f) ? 1.0f : -1.0f;
							p_dest = (((bf_env_getpoly(lp)->normal.x * Wx) + (bf_env_getpoly(lp)->normal.y * Wy) + (bf_env_getpoly(lp)->normal.z * Wz) + p_d) > 0.0f) ? 1.0f : -1.0f;
							

							//zlogthis("COLDET li=%i, lp=%i // p_org = %i, p_dest = %i --------------------<<\n",li,lp);
							// check to see if the begin and end points are on different sides of the plane
							//if(p_org + p_dest != 0.0f) continue;
							if(p_org + p_dest != 0.0f) continue;

							start_pt.x = Lx;
							start_pt.y = Ly;
							start_pt.z = Lz;

							// normalize direction vector to unit length
							v_pt.x = (Lx - Wx) * (1.0f / d);
							v_pt.y = (Ly - Wy) * (1.0f / d);
							v_pt.z = (Lz - Wz) * (1.0f / d);

							//zlogthis("V = < %0.3f, %0.3f, %0.3f > \n",v_pt.x,v_pt.y,v_pt.z);

							// calculate the triangle's surface normal
							// N = (B - A) x (C - A)

							// subtract vectors (B - A)
							ssl_x = bf_env_getpoly(lp)->vertex[1].x - bf_env_getpoly(lp)->vertex[0].x;
							ssl_y = bf_env_getpoly(lp)->vertex[1].y - bf_env_getpoly(lp)->vertex[0].y;
							ssl_z = bf_env_getpoly(lp)->vertex[1].z - bf_env_getpoly(lp)->vertex[0].z;

							// subtract vectors (C - A)
							ssr_x = bf_env_getpoly(lp)->vertex[2].x - bf_env_getpoly(lp)->vertex[0].x;
							ssr_y = bf_env_getpoly(lp)->vertex[2].y - bf_env_getpoly(lp)->vertex[0].y;
							ssr_z = bf_env_getpoly(lp)->vertex[2].z - bf_env_getpoly(lp)->vertex[0].z;
							
							// get the cross product
							Nx = (ssl_y * ssr_z) - (ssl_z * ssr_y);
							Ny = (ssl_z * ssr_x) - (ssl_x * ssr_z);
							Nz = (ssl_x * ssr_y) - (ssl_y * ssr_x);
							// calculate d
							Nd = (-Nx * bf_env_getpoly(lp)->vertex[0].x) + (-Ny * bf_env_getpoly(lp)->vertex[0].y) + (-Nz * bf_env_getpoly(lp)->vertex[0].z);

							//printf("N = < %0.3f, %0.3f, %0.3f, %0.3f >\n",Nx,Ny,Nz,Nd);

							// t = -(L dot S) / (L dot V)
							// (calculate denominator seperately to prevent div by zero and to check if orthogonal)

							t_bot = (Nx * v_pt.x) + (Ny * v_pt.y) + (Nz * v_pt.z);

							if(t_bot != 0.0f) {
								t = -((Nx * start_pt.x) + (Ny * start_pt.y) + (Nz * start_pt.z) + Nd) / t_bot;
								//zlogthis("t = %0.4f\n",t);
							} else {
								t = 0.0f;
								//zlogthis("t Does Not Exist. no collision exists!\n");
								continue;
							}

							Px = start_pt.x + (t * v_pt.x);
							Py = start_pt.y + (t * v_pt.y);
							Pz = start_pt.z + (t * v_pt.z);

							//zlogthis("P(t) = < %0.3f, %0.3f, %0.3f >\n", Px,Py,Pz);

							// calculate barycentric coordinates (w)
							R1x = (Px - bf_env_getpoly(lp)->vertex[0].x);
							R1y = (Py - bf_env_getpoly(lp)->vertex[0].y);
							R1z = (Pz - bf_env_getpoly(lp)->vertex[0].z);
							
							Q1x = (bf_env_getpoly(lp)->vertex[1].x - bf_env_getpoly(lp)->vertex[0].x);
							Q1y = (bf_env_getpoly(lp)->vertex[1].y - bf_env_getpoly(lp)->vertex[0].y);
							Q1z = (bf_env_getpoly(lp)->vertex[1].z - bf_env_getpoly(lp)->vertex[0].z);

							Q2x = (bf_env_getpoly(lp)->vertex[2].x - bf_env_getpoly(lp)->vertex[0].x);
							Q2y = (bf_env_getpoly(lp)->vertex[2].y - bf_env_getpoly(lp)->vertex[0].y);
							Q2z = (bf_env_getpoly(lp)->vertex[2].z - bf_env_getpoly(lp)->vertex[0].z);

							//zlogthis("R = < %0.3f, %0.3f, %0.3f >\nQ1 = < %0.3f, %0.3f, %0.3f >\nQ2 = < %0.3f, %0.3f, %0.3f >\n",R1x,R1y,R1z,Q1x,Q1y,Q1z,Q2x,Q2y,Q2z);

							Q1_dd = (Q1x * Q1x) + (Q1y * Q1y) + (Q1z * Q1z);
							Q2_dd = (Q2x * Q2x) + (Q2y * Q2y) + (Q2z * Q2z);
							Q12_d = (Q1x * Q2x) + (Q1y * Q2y) + (Q1z * Q2z);

							sc_mul = 1.0f / ((Q1_dd)*(Q2_dd) - (Q12_d * Q12_d));

							RQ1 = (R1x * Q1x) + (R1y * Q1y) + (R1z * Q1z);
							RQ2 = (R1x * Q2x) + (R1y * Q2y) + (R1z * Q2z);

							w1 = sc_mul * ((Q2_dd * RQ1) + (-Q12_d * RQ2));
							w2 = sc_mul * ((-Q12_d * RQ1) + (Q1_dd * RQ2));
							w0 = 1.0f - w1 - w2;
						    
							//zlogthis("w = < %0.3f, %0.3f, %0.3f >\n",w0,w1,w2);

							if(w0 > COL_TOLERANCE && w1 > COL_TOLERANCE && w2 > COL_TOLERANCE) {
								collision = true;
								//zlogthis("Collision occurs! Point of intersection is P(t).\n");
								break;
							} else {
								//zlogthis("Collision does not occur!\n");
							}

						}


						// env poly collision detection
						/*
						for(lp = 0; lp < bf_env_count(); lp++) {
							p_d    = -(bf_env_getpoly(lp)->normal.x * bf_env_getpoly(lp)->vertex[0].x + bf_env_getpoly(lp)->normal.y * bf_env_getpoly(lp)->vertex[0].y + bf_env_getpoly(lp)->normal.z * bf_env_getpoly(lp)->vertex[0].z);
							p_org  = (((bf_env_getpoly(lp)->normal.x * Lx) + (bf_env_getpoly(lp)->normal.y * Ly) + (bf_env_getpoly(lp)->normal.z * Lz) + p_d) > 0.0f) ? 1 : -1;
							p_dest = (((bf_env_getpoly(lp)->normal.x * Wx) + (bf_env_getpoly(lp)->normal.y * Wy) + (bf_env_getpoly(lp)->normal.z * Wz) + p_d) > 0.0f) ? 1 : -1;

							// check to see if the begin and end points are on different sides of the plane
							if(p_org + p_dest == 0.0f) {
								// okay, it crosses the plane... determine the location on the plane
								
								// get the delta vector
								r_x = Lx - Wx;
								r_y = Ly - Wy;
								r_z = Lz - Wz;
								
								p_t_d = (bf_env_getpoly(lp)->normal.x * r_x) + (bf_env_getpoly(lp)->normal.y * r_y) + (bf_env_getpoly(lp)->normal.z * r_z);
								
								// if p_t_d is zero, then delta vector is orthogonal to our plane and intersection is impossible...
								if(p_t_d != 0.0f) {

									p_t = -((bf_env_getpoly(lp)->normal.x * Lx) + (bf_env_getpoly(lp)->normal.y * Ly) + (bf_env_getpoly(lp)->normal.z * Lz) + p_d) / p_t_d;

									p_x = Lx + (r_x * p_t);
									p_y = Ly + (r_y * p_t);
									p_z = Lz + (r_z * p_t);

									
								}
							}
						}
						*/

						// squared-distance falloff
						/*
						if(bf_getlight(li)->lumens > 0.0f) { // prevent divide-by-zero
							fallout = bf_getlight(li)->a - ((d*d) / float(bf_getlight(li)->lumens));
							if(fallout < 0.0f) fallout = 0.0f; // not within radius of light
						} else {
							fallout = 0.0f;
						}
						*/
						
						
						// linear falloff
						/*
						if(bf_getlight(li)->lumens > 0.0f) { // prevent divide-by-zero
							fallout = bf_getlight(li)->a - (d / (40.0f));
							if(fallout < 0.0f) fallout = 0.0f; // not within radius of light
						} else {
							fallout = 0.0f;
						}
						*/

						// Lambert falloff
						
						if(!collision) {
							if(d > 0.0f) { // prevent divide-by-zero
								lv_x = Lx - Wx;
								lv_y = Ly - Wy;
								lv_z = Lz - Wz;
								lv_x /= d;
								lv_y /= d;
								lv_z /= d;
								anglex = ((lv_x * triangle->normal.x) + (lv_y * triangle->normal.y) + (lv_z * triangle->normal.z));
								fallout = (float(bf_getlight(li)->lumens) * -anglex) / d;
								//zlogthis("L(%0.02f,%0.02f,%0.02f) W(%0.02f,%0.02f,%0.02f), Tnorm(%0.02f,%0.02f,%0.02f), lv(%0.02f,%0.02f,%0.02f), anglex = %0.02f, fallout = %0.03f, d = %0.01f\n",Lx,Ly,Lz,Wx,Wy,Wz,triangle->normal.x,triangle->normal.y,triangle->normal.z,lv_x,lv_y,lv_z,anglex,fallout,d);
								if(fallout < 0.0f) fallout = 0.0f; // not within radius of light
							} else {
								fallout = 1.0f;
							}
						} else {
							fallout = 0.0f;
						}
					} else if(bf_getlight(li)->l_type == BF_LIGHT_AMBIENT) {
							fallout = bf_getlight(li)->a;
                    }

					//zlogthis("lmpp(%i,%i)li[%i], wp(%0.03f,%0.03f,%0.03f), d=%0.03f, i/d = %0.03f\n",blix,bliy,li,Wx,Wy,Wz,d,fallout);					

					lumR += bf_getlight(li)->r * fallout;
					lumG += bf_getlight(li)->g * fallout;
					lumB += bf_getlight(li)->b * fallout;
					//zlogthis("lightRGB(%0.02f,%0.02f,%0.02f), lumRGB(%0.02f,%0.02f,%0.02f)\n",bf_getlight(li)->r,bf_getlight(li)->g,bf_getlight(li)->b,lumR,lumG,lumB);
				}
			}
			if(lumR > 1.0f) lumR = 1.0f;
			if(lumG > 1.0f) lumG = 1.0f;
			if(lumB > 1.0f) lumB = 1.0f;

			// create a buffer zone on the left and right (drastically improves edge-quality)
			if(blix == 0) {
				lumels[((bliy + 1) * int(Tw) * 3)] = int(lumR * 255.0f);
				lumels[((bliy + 1) * int(Tw) * 3) + 1] = int(lumG * 255.0f);
				lumels[((bliy + 1) * int(Tw) * 3) + 2] = int(lumB * 255.0f);
			} else if(blix == (Tw2 - 1)) {
				lumels[((bliy + 1) * int(Tw) * 3) + ((blix + 2) * 3)] = int(lumR * 255.0f);
				lumels[((bliy + 1) * int(Tw) * 3) + ((blix + 2) * 3) + 1] = int(lumG * 255.0f);
				lumels[((bliy + 1) * int(Tw) * 3) + ((blix + 2) * 3) + 2] = int(lumB * 255.0f);
			}

			lumels[((bliy + 1) * int(Tw) * 3) + ((blix + 1) * 3)] = int(lumR * 255.0f);
			lumels[((bliy + 1) * int(Tw) * 3) + ((blix + 1) * 3) + 1] = int(lumG * 255.0f);
			lumels[((bliy + 1) * int(Tw) * 3) + ((blix + 1) * 3) + 2] = int(lumB * 255.0f);
		}
		//if(bliy == 0) break; //<<<<<<<<<<<<<<<<< REMOVE!!!!!
	}

	// create a buffer zone on the top and bottom
	memcpy(lumels,(lumels + (int(Tw) * 3)),(int(Tw) * 3));
	memcpy((lumels + (int(Th - 1) * int(Tw) * 3)),(lumels + (int(Th - 2) * int(Tw) * 3)),(int(Tw) * 3));

	#ifdef DUMP_RAW_LMAPS
	FILE *dumpy;
	char filenamex[256];

	sprintf(filenamex,"lmap%03i.raw",ident);

	if((dumpy = fopen(filenamex,"wb")) == NULL) {
		zlogthis("error opening dumpfile \"%s\".\n",filenamex);
	} else {
		fwrite(lumels,1,Tw * Th * 3, dumpy);
		fclose(dumpy);
	}
	#endif

	esm_picture pic_data;
	unsigned int new_tex;

	pic_data.data_ptr = lumels;
	pic_data.depth = 24; // 24bpp (RGB)
	pic_data.height = Tw;
	pic_data.width = Th;
	pic_data.type = 3; // RGB
	
	bf_bind_texture(&pic_data, &new_tex);

	triangle->lm_res = Tw;	
	triangle->lm_texdex = new_tex;
	if(!(triangle->flags & BF_HAS_LMAP)) triangle->flags += BF_HAS_LMAP;

	//zlogthis(">>>>>>>>>>>> lm_texdex = %i\n",new_tex);

	free(lumels);	// free lightmap

	END_FUNC
	return 0;
}