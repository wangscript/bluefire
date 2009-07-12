/*

  Bluefire Editor: Surface Properties dialog functions

  (c) Copyright 2004 Jacob Hipps
  http://thefro.net/~jacob/

  http://neoretro.net/

*/

#define BLUEFIRE_CLNT

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "resource.h"
#include "../bluefire.h"
#include "commctrl.h"		// ms common control header

// imported superglobals
extern HINSTANCE hInst;
extern HWND bigwin;
extern char packs[256][256];

extern bool keys[256];

extern HWND imghwnd;
extern int fsd_mode;

extern int sel_polys[1024];
extern int num_selected;

HWND surfprop;
char kybdstate[256];

// local functions
BOOL CALLBACK SurfPropDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void rotatex(float delta);
void scaleu(float delta);
void scalev(float delta);
void upan(float delta);
void vpan(float delta);
void trot(float delta);
void surfprop_enable(bool endis);
void chg_alpha(int percent);
void chg_flag(int flag_val, int state);
void sp_chg_selection();
void calc_bbox(BBOX *boundbox);
int tex_autofit();
int tex_align();

int init_surfprop() {

	surfprop = CreateDialog(hInst, MAKEINTRESOURCE(IDD_SURFPROP), bigwin, SurfPropDlg);	
	SetFocus(surfprop);

	surfprop_enable(FALSE);

	return 0;
}

BOOL CALLBACK SurfPropDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {

	int nummy;
	int i;
	float mult;
	float yy;
	char bigbuff[256];

	switch( msg ) {

		case WM_INITDIALOG:			

			SendDlgItemMessage(hDlg, IDC_ALPHA, TBM_SETRANGE, FALSE, MAKELONG(0,100));
			SendDlgItemMessage(hDlg, IDC_ALPHA, TBM_SETPOS, TRUE, 100.0f);
			SetDlgItemText(hDlg, IDC_TRANSP,"100%");
			SetDlgItemText(hDlg, IDC_TEXNAME,"none");
			ShowWindow(hDlg, SW_SHOW);
			SetFocus(hDlg);

			zlogthis("SurfPropDlg: WM_INITDIALOG\n");

			break;
		
		case WM_DESTROY:

			zlogthis("SurfPropDlg: WM_DESTROY\n");
			EndDialog(hDlg, TRUE);			
            break;

		case WM_HSCROLL:
			switch(GetDlgCtrlID((HWND)lParam)) {
				case IDC_ALPHA:
					i = SendDlgItemMessage(hDlg, IDC_ALPHA, TBM_GETPOS, 0, 0);
					sprintf(bigbuff,"%i%%",i);
					SetDlgItemText(hDlg, IDC_TRANSP, bigbuff);

					chg_alpha(i);

					break;
			}
			break;

		case WM_COMMAND:
						
			GetAsyncKeyState(VK_SHIFT);	// flush keyb buffer

			if(GetAsyncKeyState(VK_SHIFT)) mult = -1.0f;
			else mult = 1.0f;

			switch (LOWORD(wParam))
			{
				case IDC_UNLIT:
					chg_flag(BF_TRI_UNLIT, IsDlgButtonChecked(hDlg,IDC_RTTRANS));
					break;
				case IDC_RTTRANS:
					chg_flag(BF_TRI_CALCTRANS, IsDlgButtonChecked(hDlg,IDC_RTTRANS));
					break;
				case IDC_AUTOSZ:
					tex_autofit();
					break;
				case IDC_FLIPU:
					scaleu(-1.0f);
					break;
				case IDC_FLIPV:
					scalev(-1.0f);
					break;
				case IDC_SCLAPPLY:
					bigbuff[0] = 0;
					yy = 0.0f;
					GetDlgItemText(hDlg,IDC_USCL,bigbuff,255);
					yy = atof(bigbuff);
					scaleu(yy);
					bigbuff[0] = 0;
					yy = 0.0f;
					GetDlgItemText(hDlg,IDC_VSCL,bigbuff,255);
					yy = atof(bigbuff);
					scalev(yy);
					break;
				case IDC_UPAN1:
					upan(1.0f/256.0f * mult);
					break;
				case IDC_UPAN8:
					upan(8.0f/256.0f * mult);
					break;
				case IDC_UPAN32:
					upan(32.0f/256.0f * mult);
					break;
				case IDC_UPAN64:
					upan(64.0f/256.0f * mult);
					break;
				case IDC_VPAN1:
					vpan(1.0f/256.0f * mult);
					break;
				case IDC_VPAN8:
					vpan(8.0f/256.0f * mult);
					break;
				case IDC_VPAN32:
					vpan(32.0f/256.0f * mult);
					break;
				case IDC_VPAN64:
					vpan(64.0f/256.0f * mult);
					break;
				case IDC_ROT90:
					rotatex(90.0f * mult);
					break;
				case IDC_ROT45:
					rotatex(45.0f * mult);
					break;
				case IDC_ROT5:
					rotatex(5.0f * mult);
					break;
				case IDC_ROT1:
					rotatex(1.0f * mult);
					break;				
				case IDABORT:
				case IDCANCEL:
				case IDOK:
					ShowWindow(hDlg,SW_HIDE);
					break;
			}
			break;

		case WM_NOTIFY:
			switch(LOWORD(wParam)) {
				// bleh.
			}
		default:
			return FALSE;
    }

	return TRUE;
}

void scaleu(float delta) {
	BEGIN_FUNC("scaleu")
	
	int i;
	BF_TRIANGLE_EX *thispoly;

	if(delta == 0.0f) {
		END_FUNC
		return;
	}

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		thispoly->tex[0].u *= delta;
		//if(thispoly->tex[0].u > 1.0f) thispoly->tex[0].u -= 1.0f;
		thispoly->tex[1].u *= delta;
		//if(thispoly->tex[1].u > 1.0f) thispoly->tex[1].u -= 1.0f;
		thispoly->tex[2].u *= delta;
		//if(thispoly->tex[2].u > 1.0f) thispoly->tex[2].u -= 1.0f;
	}

	END_FUNC
	return;
}

/*
void rotatex(float delta) {	
	BEGIN_FUNC("rotatex")
	int i;
	BF_TRIANGLE_EX *thispoly;

	float cu, cv, m;
	float x1, x2, x3, x4;
	float y1, y2, y3, y4;
	float b1, b2, a1, a2;
	float ba1, bb1, ba2, bb2;

	float gu[3];
	float gv[3];

	float theta_prm;

	const float pi = 3.14159f;
	const float piover180 = 3.14159f / 180.0f;

	int z;

	if(delta == 0.0f) {
		END_FUNC
		return;
	}

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);;

		// find the center of the triangle

		x1 = thispoly->tex[0].u;
		y1 = thispoly->tex[0].v;		
		x2 = (thispoly->tex[1].u + thispoly->tex[2].u) / 2.0f;
		y2 = (thispoly->tex[1].v + thispoly->tex[2].v) / 2.0f;
		x3 = thispoly->tex[1].u;
		y3 = thispoly->tex[1].v;
		x4 = (thispoly->tex[0].u + thispoly->tex[2].u) / 2.0f;
		y4 = (thispoly->tex[0].v + thispoly->tex[2].v) / 2.0f;

		ba1 = y2 - y1;
		bb1 = x2 - x1;
		ba2 = y4 - y3;
		bb2 = x4 - x3;

		if(ba2 == 0.0f) {	// horizontal line (line 1)
			cv = y3;
			m = (y1 - y2) / (x1 - x2);
			cu = (y3 - (y1 - (m * x1))) / m;
		} else if(bb2 == 0.0f) {	// vertical line (line 1)
			cu = x3;
			m = (y1 - y2) / (x1 - x2);
			cv = (m * x3) + (y1 - (m * x1));
		} else if(ba1 == 0.0f) {	// horizontal line (line 2)
			cv = y1;
			m = (y3 - y4) / (x3 - x4);
			cu = (y1 - (y3 - (m * x3))) / m;
		} else if(bb1 == 0.0f) {	// vertical line (line 2)
			cu = x3;
			m = (y3 - y4) / (x3 - x4);
			cv = (m * x1) + (y3 - (m * x3));
		} else {				// diagonal line
			b1 = ba1 / bb1;
			b2 = ba2 / bb2;
			a1 = y1-b1*x1;
			a2 = y3-b2*x3;
			cu = -(a1-a2)/(b1-b2);
			cv = a1+b1*cu;
		}		
		
		// center on origin
		for(z = 0; z < 3; z++) {			
			gu[z] = thispoly->tex[z].u - cu;
			gv[z] = thispoly->tex[z].v - cv;						
		}

		// rotate and translate back to center!!
		for(z = 0; z < 3; z++) {
			theta_prm = delta * piover180;		
			thispoly->tex[z].u = (gu[z] * cos(theta_prm)) + (gv[z] * (-sin(theta_prm))) + cu;
			thispoly->tex[z].v = (gu[z] * sin(theta_prm)) + (gv[z] * cos(theta_prm)) + cv;			
		}

	}


	END_FUNC
	return;
}
*/


void rotatex(float delta) {	
	BEGIN_FUNC("rotatex")
	int i;
	BF_TRIANGLE_EX *thispoly;

	float cu, cv, m;
	float x1, x2, x3, x4;
	float y1, y2, y3, y4;
	float b1, b2, a1, a2;
	float ba1, bb1, ba2, bb2;

	float gu[3];
	float gv[3];

	float theta_prm;

	const float pi = 3.14159f;
	const float piover180 = 3.14159f / 180.0f;

	int z;

	if(delta == 0.0f) {
		END_FUNC
		return;
	}

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);;

		for(z = 0; z < 3; z++) {			
			gu[z] = thispoly->tex[z].u;
			gv[z] = thispoly->tex[z].v;						
		}

		// rotate and translate back to center!!
		for(z = 0; z < 3; z++) {
			theta_prm = delta * piover180;
			thispoly->tex[z].u = (gu[z] * cos(theta_prm)) + (gv[z] * (-sin(theta_prm)));
			thispoly->tex[z].v = (gu[z] * sin(theta_prm)) + (gv[z] * cos(theta_prm));
		}

	}


	END_FUNC
	return;
}


void calc_bbox(BBOX *bound) {
	BEGIN_FUNC("calc_bbox")

	BF_TRIANGLE_EX *thispoly;
    
	bound->max_xx = -900000.00f; // maxes
	bound->max_xy = -900000.00f;
	bound->max_xz = -900000.00f;
	bound->max_yx = -900000.00f;
	bound->max_yy = -900000.00f;
	bound->max_yz = -900000.00f;
	bound->max_zx = -900000.00f;
	bound->max_zy = -900000.00f;
	bound->max_zz = -900000.00f;
	bound->min_xx = 900000.00f; // mins
	bound->min_xy = 900000.00f;
	bound->min_xz = 900000.00f;
	bound->min_yx = 900000.00f;
	bound->min_yy = 900000.00f;
	bound->min_yz = 900000.00f;
	bound->min_zx = 900000.00f;
	bound->min_zy = 900000.00f;
	bound->min_zz = 900000.00f;
	
	int i;
	for(int hh = 0; hh < num_selected; hh++) {
	  thispoly = bf_getpoly(sel_polys[hh]);
	  for(i = 0; i < 3; i++) {
		// maxes
		if(thispoly->vertex[i].x > bound->max_xx) {
			bound->max_xx = thispoly->vertex[i].x;
			bound->max_xy = thispoly->vertex[i].y;
			bound->max_xz = thispoly->vertex[i].z;
		}
		if(thispoly->vertex[i].y > bound->max_yy) {
			bound->max_yx = thispoly->vertex[i].x;
			bound->max_yy = thispoly->vertex[i].y;
			bound->max_yz = thispoly->vertex[i].z;
		}
		if(thispoly->vertex[i].z > bound->max_zz) {
			bound->max_zx = thispoly->vertex[i].x;
			bound->max_zy = thispoly->vertex[i].y;
			bound->max_zz = thispoly->vertex[i].z;
		}
		// mins
		if(thispoly->vertex[i].x < bound->min_xx) {
			bound->min_xx = thispoly->vertex[i].x;
			bound->min_xy = thispoly->vertex[i].y;
			bound->min_xz = thispoly->vertex[i].z;
		}
		if(thispoly->vertex[i].y < bound->min_yy) {
			bound->min_yx = thispoly->vertex[i].x;
			bound->min_yy = thispoly->vertex[i].y;
			bound->min_yz = thispoly->vertex[i].z;
		}
		if(thispoly->vertex[i].z < bound->min_zz) {
			bound->min_zx = thispoly->vertex[i].x;
			bound->min_zy = thispoly->vertex[i].y;
			bound->min_zz = thispoly->vertex[i].z;
		}
	  }
	}

	END_FUNC
	return;
}

/*

verify_coplanar()

Test to ensure all of the selected polygons are indeed coplanar

*/
/*
int verify_coplanar() {
	BEGIN_FUNC("verify_coplanar")
	
	BF_TRIANGLE_EX *thispoly;	
    float normA, normB, normC, plane;

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		if(i == 0) {
			normA = thispoly->normal.x;
			normB = thispoly->normal.y;
			normC = thispoly->normal.z;
			plane = 
		}
	}

	END_FUNC
	return 1; // everything OK =]
}
*/

int tex_align() {
	BEGIN_FUNC("tex_align")

	int i, hh;
	BBOX boxybrown;
	float norm_x, norm_y, norm_z;
	BF_TRIANGLE_EX *thispoly;

	calc_bbox(&boxybrown);

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		norm_x = thispoly->normal.x;
		norm_y = thispoly->normal.y;
		norm_z = thispoly->normal.z;
		for(hh = 0; hh < 3; hh++) {
			thispoly->tex[hh].u = ((norm_z+norm_y) * thispoly->vertex[hh].x) + (norm_x * thispoly->vertex[hh].z);
			thispoly->tex[hh].v = ((norm_z+norm_x) * thispoly->vertex[hh].y) + (norm_y * thispoly->vertex[hh].z);
			zlogthis("vertex(%0.02f,%0.02f,%0.02f), norm(%0.02f,%0.02f,%0.02f),  new: tex(%0.02f,%0.02f)\n",thispoly->vertex[hh].x,thispoly->vertex[hh].y,thispoly->vertex[hh].z,norm_x,norm_y,norm_z,thispoly->tex[hh].u,thispoly->tex[hh].v);
		}
	}

	END_FUNC
	return 0;
}

int tex_sel_coplanar() {
	BEGIN_FUNC("tex_sel_coplanar")

	BF_TRIANGLE_EX *thispoly;
	float norm_x, norm_y, norm_z, d, d_prime;
	int i;

	if(num_selected == 0) return -1;

	thispoly = bf_getpoly(sel_polys[0]);
	norm_x = thispoly->normal.x;
	norm_y = thispoly->normal.y;
	norm_z = thispoly->normal.z;
	d = -((norm_x * thispoly->vertex[0].x) + (norm_y * thispoly->vertex[0].y) + (norm_z * thispoly->vertex[0].z));

	for(i = 0; i < bf_env_count(); i++) {
		thispoly = bf_getpoly(i);
		if(norm_x == thispoly->normal.x && norm_y == thispoly->normal.y && norm_z == thispoly->normal.z) {
			if((((norm_x * thispoly->vertex[0].x) + (norm_y * thispoly->vertex[0].y) + (norm_z * thispoly->vertex[0].z)) + d) == 0.0f) {
				if(!(thispoly->flags & BF_TRI_SELECTED)) {
					sel_polys[num_selected] = i;
					num_selected++;
					thispoly->flags += BF_TRI_SELECTED;
				}
			}
		}
	}

	END_FUNC
	return 0;
}

int tex_autofit() {
	BEGIN_FUNC("tex_autofit")

	int i, hh;
	BBOX boxybrown;
	float norm_x, norm_y, norm_z;
	float up1, up2;

	BF_TRIANGLE_EX *thispoly;

	calc_bbox(&boxybrown);
	float abs_x = boxybrown.max_xx - boxybrown.min_xx;
	float abs_y = boxybrown.max_yy - boxybrown.min_yy;
	float abs_z = boxybrown.max_zz - boxybrown.min_zz;

	zlogthis("!autofit! >> abs(%0.02f,%0.02f,%0.02f)\n",abs_x,abs_y,abs_z);

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		norm_x = thispoly->normal.x;
		norm_y = thispoly->normal.y;
		norm_z = thispoly->normal.z;
		for(hh = 0; hh < 3; hh++) {
			// u, part 1
			if(abs_x < -0.001f || abs_x > 0.001f) up1 = ((norm_z+norm_y) * ((thispoly->vertex[hh].x - boxybrown.min_xx) / abs_x));
			else up1 = 0.0f;
			// u, part 2
			if(abs_z < -0.001f || abs_z > 0.001f) up2 = (norm_x * ((thispoly->vertex[hh].z - boxybrown.min_zz) / abs_z));
			else up2 = 0.0f;
			// u
			thispoly->tex[hh].u = up1 + up2;

			// v, part 1
			if(abs_y < -0.001f || abs_y > 0.001f) up1 = ((norm_z+norm_x) * ((thispoly->vertex[hh].y - boxybrown.min_yy) / abs_y));
			else up1 = 0.0f;
			// v, part 2
			if(abs_z < -0.001f || abs_z > 0.001f) up2 = (norm_y * ((thispoly->vertex[hh].z - boxybrown.min_zz) / abs_z));
			else up2 = 0.0f;
			// v
			thispoly->tex[hh].v = up1 + up2;

			zlogthis("!autofit! vertex(%0.02f,%0.02f,%0.02f), norm(%0.02f,%0.02f,%0.02f),  new: tex(%0.02f,%0.02f)\n",thispoly->vertex[hh].x,thispoly->vertex[hh].y,thispoly->vertex[hh].z,norm_x,norm_y,norm_z,thispoly->tex[hh].u,thispoly->tex[hh].v);
		}
	}

	END_FUNC
	return 0;
}

/*
int tex_autofit() {
	BEGIN_FUNC("tex_autofit")
	
	int i, hh;
	float norm_x, norm_y, norm_z;
	float d1, d2, d3, d4;

	BF_TRIANGLE_EX *thispoly;
	BBOX boxybrown;
    
	// get the bounding box (find maxes and mins)
	calc_bbox(&boxybrown);		

	// get the surface normal from the first triangle
	// (since all surfaces are assumed to be coplanar)
	thispoly = bf_getpoly(sel_polys[0]);
	norm_x = thispoly->normal.x;
	norm_y = thispoly->normal.y;
	norm_z = thispoly->normal.z;
    
	// get d values
	if(abs(norm_x) >= abs(norm_z) && abs(norm_x) > abs(norm_y)) {
		// tending towards YZ axis
		d1 = (norm_x * boxybrown.min_yx) + (norm_y * boxybrown.min_yy) + (norm_z * boxybrown.min_yz);
		d2 = (norm_x * boxybrown.max_yx) + (norm_y * boxybrown.max_yy) + (norm_z * boxybrown.max_yz);
		d3 = (norm_x * boxybrown.min_zx) + (norm_y * boxybrown.min_zy) + (norm_z * boxybrown.min_zz);
		d4 = (norm_x * boxybrown.max_zx) + (norm_y * boxybrown.max_zy) + (norm_z * boxybrown.max_zz);
	} else if(abs(norm_z) >= abs(norm_y)){
		// tending towards XY axis
		d1 = (norm_x * boxybrown.min_xx) + (norm_y * boxybrown.min_xy) + (norm_z * boxybrown.min_xz);
		d2 = (norm_x * boxybrown.max_xx) + (norm_y * boxybrown.max_xy) + (norm_z * boxybrown.max_xz);
		d3 = (norm_x * boxybrown.min_yx) + (norm_y * boxybrown.min_yy) + (norm_z * boxybrown.min_yz);
		d4 = (norm_x * boxybrown.max_yx) + (norm_y * boxybrown.max_yy) + (norm_z * boxybrown.max_yz);
	} else {
		// tending towards XZ axis
		d1 = (norm_x * boxybrown.min_xx) + (norm_y * boxybrown.min_xy) + (norm_z * boxybrown.min_xz);
		d2 = (norm_x * boxybrown.max_xx) + (norm_y * boxybrown.max_xy) + (norm_z * boxybrown.max_xz);
		d3 = (norm_x * boxybrown.min_zx) + (norm_y * boxybrown.min_zy) + (norm_z * boxybrown.min_zz);
		d4 = (norm_x * boxybrown.max_zx) + (norm_y * boxybrown.max_zy) + (norm_z * boxybrown.max_zz);
	}

	zlogthis("d1 = %0.04f, d2 = %0.04f, d3 = %0.04f, d4 = %0.04f\n",d1,d2,d3,d4);

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		for(hh = 0; hh < 3; hh++) {
			thispoly->tex[hh].u = ((norm_x * thispoly->vertex[hh].x) + (norm_y * thispoly->vertex[hh].y) + (norm_z * thispoly->vertex[hh].z) - d1) / (d2 - d1);
			thispoly->tex[hh].v = ((norm_x * thispoly->vertex[hh].x) + (norm_y * thispoly->vertex[hh].y) + (norm_z * thispoly->vertex[hh].z) - d3) / (d4 - d3);
			zlogthis("vertex(%0.02f,%0.02f,%0.02f),  new: tex(%0.02f,%0.02f)\n",thispoly->vertex[hh].x,thispoly->vertex[hh].y,thispoly->vertex[hh].z,thispoly->tex[hh].u,thispoly->tex[hh].v);
		}
	}

	END_FUNC
	return 0;
}
*/



void scalev(float delta) {
	BEGIN_FUNC("scalev")
	
	int i;
	BF_TRIANGLE_EX *thispoly;

	if(delta == 0.0f) {
		END_FUNC
		return;
	}

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		thispoly->tex[0].v *= delta;
		//if(thispoly->tex[0].u > 1.0f) thispoly->tex[0].u -= 1.0f;
		thispoly->tex[1].v *= delta;
		//if(thispoly->tex[1].u > 1.0f) thispoly->tex[1].u -= 1.0f;
		thispoly->tex[2].v *= delta;
		//if(thispoly->tex[2].u > 1.0f) thispoly->tex[2].u -= 1.0f;
	}

	END_FUNC
	return;
}

void upan(float delta) {
	BEGIN_FUNC("upan")
	
	int i;
	BF_TRIANGLE_EX *thispoly;

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		thispoly->tex[0].u += delta;
		//if(thispoly->tex[0].u > 1.0f) thispoly->tex[0].u -= 1.0f;
		thispoly->tex[1].u += delta;
		//if(thispoly->tex[1].u > 1.0f) thispoly->tex[1].u -= 1.0f;
		thispoly->tex[2].u += delta;
		//if(thispoly->tex[2].u > 1.0f) thispoly->tex[2].u -= 1.0f;
	}

	END_FUNC
	return;
}

void vpan(float delta) {
	BEGIN_FUNC("vpan")
	
	int i;
	BF_TRIANGLE_EX *thispoly;	

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		thispoly->tex[0].v += delta;
		//if(thispoly->tex[0].v > 1.0f) thispoly->tex[0].v -= 1.0f;
		thispoly->tex[1].v += delta;
		//if(thispoly->tex[1].v > 1.0f) thispoly->tex[1].v -= 1.0f;
		thispoly->tex[2].v += delta;
		//if(thispoly->tex[2].v > 1.0f) thispoly->tex[2].v -= 1.0f;		
	}

	END_FUNC
	return;
}

void trot(float delta) {
	BEGIN_FUNC("trot")

	/*
	int i;
	BF_TRIANGLE_EX *thispoly;	

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		thispoly->tex[0].v += delta;
		thispoly->tex[1].v += delta;
		thispoly->tex[2].v += delta;
	}
	*/

	END_FUNC
	return;
}


void surfprop_enable(bool endis) {
	BEGIN_FUNC("surfprop_enable")

	char bleh[64];

	if(endis) {
		sprintf(bleh,"%i Surfaces Selected",num_selected);
		SetWindowText(surfprop,bleh);
	} else {
		SetWindowText(surfprop,"No Surfaces Selected");
	}

	EnableWindow(GetDlgItem(surfprop,IDC_ST01), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_ST02), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_ST03), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_ST04), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_ST05), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_ST06), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_ST07), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_ST08), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_ST09), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_ST10), endis);

	EnableWindow(GetDlgItem(surfprop,IDC_TEXNAME), endis);

	EnableWindow(GetDlgItem(surfprop,IDC_UPAN1), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_UPAN8), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_UPAN32), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_UPAN64), endis);

	EnableWindow(GetDlgItem(surfprop,IDC_VPAN1), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_VPAN8), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_VPAN32), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_VPAN64), endis);

	EnableWindow(GetDlgItem(surfprop,IDC_ROT90), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_ROT45), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_ROT5), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_ROT1), endis);

	EnableWindow(GetDlgItem(surfprop,IDC_ALPHA), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_TRANSP), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_RTTRANS), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_UNLIT), endis);
	
	EnableWindow(GetDlgItem(surfprop,IDC_USCL), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_VSCL), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_SCLAPPLY), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_SCLRESET), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_FLIPU), endis);
	EnableWindow(GetDlgItem(surfprop,IDC_FLIPV), endis);		

	END_FUNC
	return;
}



void chg_flag(int flag_val, int state) {
	BEGIN_FUNC("chg_flag")
	
	int i;
	BF_TRIANGLE_EX *thispoly;	

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		if(state == BST_UNCHECKED) {
			if(thispoly->flags & flag_val) thispoly->flags -= flag_val;
		} else if(state == BST_CHECKED) {
			if(!(thispoly->flags & flag_val)) thispoly->flags += flag_val;
		}
	}

	END_FUNC
	return;
}

void chg_alpha(int percent) {
	BEGIN_FUNC("chg_alpha")
	
	int i;
	BF_TRIANGLE_EX *thispoly;	

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		thispoly->opacity = float(percent) / 100.0f;
	}

	END_FUNC
	return;
}

void sp_chg_selection() {
	BEGIN_FUNC("sp_chg_selection")

	char bleh[256];
	int def_alpha = 0;
	int def_unlit = 0;
	int def_dtrans = 0;
	int def_tex = 0;
	int c_alpha, c_unlit, c_dtrans, c_tex;
	int i;
	BF_TRIANGLE_EX *thispoly;	

	if(num_selected > 0) surfprop_enable(TRUE);
	else surfprop_enable(FALSE);

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		c_alpha = int(thispoly->opacity * 100.0f);
		c_unlit = (thispoly->flags & BF_TRI_UNLIT) ? 1 : 0;
		c_dtrans = (thispoly->flags & BF_TRI_CALCTRANS) ? 1 : 0;
		c_tex = thispoly->df_texdex;
		if(i == 0) {
			def_alpha = c_alpha;
			def_unlit = c_unlit;
			def_dtrans = c_dtrans;
			def_tex = c_tex;
		}
	}

	SendDlgItemMessage(surfprop, IDC_ALPHA, TBM_SETPOS, TRUE, def_alpha);
	if(c_alpha != def_alpha) {
		SetDlgItemText(surfprop,IDC_TRANSP,"***");
	} else {
		sprintf(bleh,"%i%%",def_alpha);
		SetDlgItemText(surfprop,IDC_TRANSP,bleh);
	}

	if(c_unlit != def_unlit) {
		CheckDlgButton(surfprop,IDC_UNLIT,BST_INDETERMINATE);
	} else {		
		if(def_unlit) CheckDlgButton(surfprop,IDC_UNLIT,BST_CHECKED);
		else CheckDlgButton(surfprop,IDC_UNLIT,BST_UNCHECKED);
	}

	if(c_dtrans != def_dtrans) {
		CheckDlgButton(surfprop,IDC_RTTRANS,BST_INDETERMINATE);
	} else {		
		if(def_dtrans) CheckDlgButton(surfprop,IDC_RTTRANS,BST_CHECKED);
		else CheckDlgButton(surfprop,IDC_RTTRANS,BST_UNCHECKED);
	}

	if(c_tex != def_tex) {
		SetDlgItemText(surfprop,IDC_TEXNAME,"[multiple textures]");
	} else {
		SetDlgItemText(surfprop,IDC_TEXNAME,bf_getimg(def_tex)->resid);
	}

	END_FUNC
	return;
}