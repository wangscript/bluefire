/*

  Bluefire Editor: Brush functions

  (c) Copyright 2004 Jacob Hipps

  http://neoretro.net/

*/

#define BLUEFIRE_CLNT
#define _BF_MAIN		// <-- ???

#include <windows.h>
#include "commctrl.h"		// ms common controls
#include "commdlg.h"		// ms common dialog
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource.h"

#include "../bluefire.h"

typedef struct {
	int index;
	char resid[64];
	long cdata_sz;
	long ddata_sz;
	char datatype;
} BFZ_ITEMS;

// imported superglobals
extern HWND bigwin;
extern char packs[256][256];
extern HINSTANCE hInst;

// external functinos
extern int bfz_read(FILE *fin, BFZ_ITEMS *items, int maxsize);

// globals
char filter_string[] = "BFZ Object (*.bfz)\0*.bfz\0\0";
char filename[512];
BFZ_ITEMS listing[256];
char resname[64];

// wait dialog stuffs
extern void waitdlg_settext(char *intext);
extern void waitdlg_setprog(int prog);
extern void waitdlg_init(char *caption);
extern void waitdlg_kill();

// local functions
BOOL CALLBACK ImpBrushDlg(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

int brush_import(char *filez, char *resid) {
	BEGIN_FUNC("brush_import")

	OPENFILENAME impfile;	
	filename[0] = 0;
	BFZ_FILE ifile;

	if(!filez) {
		ZeroMemory(&impfile, sizeof(OPENFILENAME));
		impfile.lStructSize = sizeof(OPENFILENAME);
		impfile.hwndOwner = bigwin;
		impfile.lpstrFile = filename;
		impfile.nMaxFile = 512;
		impfile.lpstrFilter = filter_string;
		impfile.lpstrTitle = (char*)"Import Brush...";
		impfile.nFilterIndex = 1;
		impfile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;


		if(!GetOpenFileName(&impfile)) {
			// cancelled
			END_FUNC
			return 1;
		}	

		if(!DialogBox(hInst,MAKEINTRESOURCE(IDD_IMPBRUSH),bigwin,ImpBrushDlg)) {
			END_FUNC
			return 1;
		}

	} else {
		strcpy(filename,filez);
		strcpy(resname,resid);
	}

	//bf_query_state(); // <-- DEBUG -----------

	char temptext[256];
	waitdlg_init("Loading object as environment...");
	
	sprintf(temptext,"Opening file and loading object...");
	waitdlg_settext(temptext);

	bfz_open(filename,&ifile);

	int obj_num = bfz_loadobj(&ifile, resname, bf_addobject());

	BF_RNODE *thisobj = bf_getobject(obj_num);

	BF_TRIANGLE_EX thispoly;

	int s, i;
	int surfaces = thisobj->surfaces;
	int polys;

	//zlogthis("--== Surfaces = %i\n",surfaces);

	for(s = 0; s < surfaces; s++) {
		thispoly.df_texdex = thisobj->surfdex[s].snum;
		thispoly.opacity = 1 - (thisobj->surfdex[s].trans); // transparency -> opacity
		sprintf(temptext,"Surface %i/%i...",s+1,surfaces);
		waitdlg_settext(temptext);
		
		polys = thisobj->surfdex[s].scount;

		//zlogthisx(" +== Surface %i has %i polys.",s,polys);

		for(i = 0; i < polys; i++) {
			thispoly.normal.x = thisobj->vtexinfo[s][i].normal.x;
			thispoly.normal.y = thisobj->vtexinfo[s][i].normal.y;
			thispoly.normal.z = thisobj->vtexinfo[s][i].normal.z;
			//zlogthisx("s=%i,i=%i, thispoly.vertex = 0x%08x, thispoly.tex = 0x%08x",s,i,&thispoly.vertex,&thispoly.tex);
			//zlogthisx("thisobj->vtexinfo[s][i] = 0x%08x",&thisobj->vtexinfo[s][i]);
			memcpy(&thispoly.vertex,thisobj->vtexinfo[s][i].vertex,sizeof(float) * 9);
			memcpy(&thispoly.tex,thisobj->vtexinfo[s][i].tex,sizeof(float) * 6);			
			thispoly.normal.z = thisobj->vtexinfo[s][i].normal.z;
			thispoly.flags = 0;
			//zlogthisx("memcopys OK!");

			if(((thispoly.vertex[0].x + thispoly.vertex[0].y + thispoly.vertex[0].z) == (thispoly.vertex[1].x + thispoly.vertex[1].y + thispoly.vertex[1].z)) || ((thispoly.vertex[2].x + thispoly.vertex[2].y + thispoly.vertex[2].z) == (thispoly.vertex[1].x + thispoly.vertex[1].y + thispoly.vertex[1].z)) || ((thispoly.vertex[0].x + thispoly.vertex[0].y + thispoly.vertex[0].z) == (thispoly.vertex[2].x + thispoly.vertex[2].y + thispoly.vertex[2].z))) {
				//zlogthisx("THIS POLYGON IS NOT A TRIANGLE! >> POLYGON CULLED <<--------------!!!");
			} else {
				bf_add_env_poly(&thispoly);
				//zlogthisx("poly added OK!");

				waitdlg_setprog(int((float(i+1)/float(polys)) * 100.0f));
				
				//zlogthisx(" + Added env poly #%i, df_tex = %i",bf_poly_cnt() - 1,thispoly.df_texdex);
				/*
				zlogthis("Poly info:\n"
					 "vtex: (%.2f,%.2f,%.2f),(%.2f,%.2f,%.2f),(%.2f,%.2f,%.2f)\n"
					 "tex:  (%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)\n"
					 "normal: (%.2f,%.2f,%.2f)\n"
					 "opacity: %.2f\n"
					 "texture: %i\n"
					 "\n"
					,thispoly.vertex[0].x,thispoly.vertex[0].y,thispoly.vertex[0].z
					,thispoly.vertex[1].x,thispoly.vertex[1].y,thispoly.vertex[1].z
					,thispoly.vertex[2].x,thispoly.vertex[2].y,thispoly.vertex[2].z
					,thispoly.tex[0].u,thispoly.tex[0].v
					,thispoly.tex[1].u,thispoly.tex[1].v
					,thispoly.tex[2].u,thispoly.tex[2].v
					,thispoly.normal.x,thispoly.normal.y,thispoly.normal.z
					,thispoly.opacity
					,thispoly.df_texdex
				);
			
				if((thispoly.normal.x + thispoly.normal.y + thispoly.normal.z) == 0.0f) {
					zlogthis("THIS POLYGON CONTAINS INVALID NORMAL VECTOR!---------------!!!\nnormal set to (0.00,0.00,1.00)\n");
					thispoly.normal.z = 1.0f;
				}
				*/
			}
		}
	}

	zlogthisx("Ok. Env polys loaded. Cleaning up...");

	sprintf(temptext,"Freeing object...");
	waitdlg_settext(temptext);	
	bf_freeobject(obj_num);

	sprintf(temptext,"Closing file...");
	waitdlg_settext(temptext);
	bfz_close(&ifile);

	sprintf(temptext,"Done.");
	waitdlg_settext(temptext);
	waitdlg_kill();	

	END_FUNC
	return 0;
}


BOOL CALLBACK ImpBrushDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {

	char tstr[10];
	char bigbuff[256];
	int itemz, i, x;
	FILE *fathy;

	//zlogthis("msg = 0x%04x, wParam = %x, lParam = %x\n",msg,wParam,lParam);

	switch( msg ) {

		case WM_INITDIALOG:			
			
			if((fathy = fopen(filename,"rb")) == NULL) {
				MessageBox(bigwin,"Unable to open file.","Error",MB_APPLMODAL);
				EndDialog(hDlg,FALSE);
			}

			itemz = bfz_read(fathy,listing,256);
			
			x = 0;

			for(i = 0; i < itemz; i++) {
				if(listing[i].datatype == BFC_OBJ || listing[i].datatype == BFC_GEO) {
					SendDlgItemMessage(hDlg, IDC_OBJID, CB_INSERTSTRING, x, (long)listing[i].resid);
					x++;
				}
			}

			if(x == 0) {
				MessageBox(bigwin,"This package does not contain any objects or geometry data structures.","Import error",MB_APPLMODAL);
				EndDialog(hDlg,FALSE);
			}

			ShowWindow(hDlg, SW_SHOW);
			SetFocus(hDlg);

			fclose(fathy);

			break;
		
		case WM_DESTROY:
			
			EndDialog(hDlg, TRUE);
            break;

		case WM_COMMAND:

			switch (LOWORD(wParam))
			{				
				case IDOK:					
					SendDlgItemMessage(hDlg, IDC_OBJID, CB_GETLBTEXT, SendDlgItemMessage(hDlg, IDC_OBJID, CB_GETCURSEL, 0, 0), (long)resname);
					EndDialog(hDlg, TRUE);
					break;
				case IDABORT:
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
			}
			break;

		default:
			return FALSE;
    }

	return TRUE;
}
