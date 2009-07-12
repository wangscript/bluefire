/*

  Bluefire Editor: Object Properties dialog functions

  (c) Copyright 2004 Jacob Hipps
  http://thefro.net/~jacob/

  http://neoretro.net/

*/

#define BLUEFIRE_CLNT

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource.h"
#include "../bluefire.h"
#include "commctrl.h"		// ms common control header

// imported superglobals
extern HINSTANCE hInst;
extern HWND bigwin;

extern int sel_polys[1024];
extern int num_selected;

BF_RNODE *coptr;
HWND objprop;

BOOL CALLBACK EditObjDlg(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
void obj_chgsel();
void objprop_enable(bool endis);


int init_objprop() {

	objprop = CreateDialog(hInst, MAKEINTRESOURCE(IDD_EDOBJ), bigwin, EditObjDlg);	
	SetFocus(objprop);

	objprop_enable(FALSE);

	return 0;
}


BOOL CALLBACK EditObjDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {

	char tstr[10];
	char bigbuff[256];

	switch( msg ) {

		case WM_INITDIALOG:			
			
			ShowWindow(hDlg, SW_SHOW);
			SetFocus(hDlg);

			break;
		case WM_DESTROY:

			EndDialog(hDlg, TRUE);
            break;

		case WM_HSCROLL:
			switch(GetDlgCtrlID((HWND)lParam)) {
				case IDC_OPACITY:
					coptr->opacity = float(SendDlgItemMessage(hDlg, IDC_OPACITY, TBM_GETPOS, 0, 0)) / 100.0f;
					sprintf(bigbuff,"%i%%",int(coptr->opacity * 100.0f));
					SetDlgItemText(hDlg, IDC_TRANSP, bigbuff);					
					break;
			}
			break;

		case WM_COMMAND:

			switch (LOWORD(wParam))
			{				
				case IDC_POSX:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_POSX,bigbuff,255);
						coptr->pos.x = atof(bigbuff);
						sprintf(tstr,"%.2f",coptr->pos.x);
						SetDlgItemText(hDlg,IDC_POSX,tstr);
					}
					break;
				case IDC_POSY:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_POSY,bigbuff,255);
						coptr->pos.y = atof(bigbuff);
						sprintf(tstr,"%.2f",coptr->pos.y);
						SetDlgItemText(hDlg,IDC_POSY,tstr);
					}
					break;
				case IDC_POSZ:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_POSZ,bigbuff,255);
						coptr->pos.z = atof(bigbuff);
						sprintf(tstr,"%.2f",coptr->pos.z);
						SetDlgItemText(hDlg,IDC_POSZ,tstr);
					}
					break;

				case IDC_NORMX:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_NORMX,bigbuff,255);
						coptr->orient.p = atof(bigbuff);
						sprintf(tstr,"%.2f",coptr->orient.p);
						SetDlgItemText(hDlg,IDC_NORMX,tstr);
					}
					break;
				case IDC_NORMY:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_NORMY,bigbuff,255);
						coptr->orient.r = atof(bigbuff);
						sprintf(tstr,"%.2f",coptr->orient.r);
						SetDlgItemText(hDlg,IDC_NORMY,tstr);
					}
					break;
				case IDC_NORMZ:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_NORMZ,bigbuff,255);
						coptr->orient.y = atof(bigbuff);
						sprintf(tstr,"%.2f",coptr->orient.y);
						SetDlgItemText(hDlg,IDC_NORMZ,tstr);
					}
					break;

				case IDC_SCX:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_SCX,bigbuff,255);
						coptr->scale.x = atof(bigbuff);
						sprintf(tstr,"%.2f",coptr->scale.x);
						SetDlgItemText(hDlg,IDC_SCX,tstr);
					}
					break;
				case IDC_SCY:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_SCY,bigbuff,255);
						coptr->scale.y = atof(bigbuff);
						sprintf(tstr,"%.2f",coptr->scale.y);
						SetDlgItemText(hDlg,IDC_SCY,tstr);
					}
					break;
				case IDC_SCZ:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_SCZ,bigbuff,255);
						coptr->scale.z = atof(bigbuff);
						sprintf(tstr,"%.2f",coptr->scale.z);
						SetDlgItemText(hDlg,IDC_SCZ,tstr);
					}
					break;

				case IDABORT:
				case IDCANCEL:
				case IDOK:
					zlogthis("EditLightDlg: EndDialog\n");
					EndDialog(hDlg, TRUE);								
					break;
			}
			break;

		default:
			return FALSE;
    }

	return TRUE;
}

void obj_chgsel() {

	BF_RNODE *thisobj;
	int i, seltd;
	
	if(num_selected == 0) {
		objprop_enable(0);
		coptr = (BF_RNODE*)0x0CafeCafe;
	} else {
		thisobj = bf_getobject(sel_polys[0]);
		objprop_enable(1);
		SetDlgItemText(objprop,IDC_LNAME,thisobj->name);
		if(thisobj->bDuplicate) {
			SetDlgItemText(objprop,IDC_MASTER,thisobj->master);
			CheckDlgButton(objprop,IDC_RTTRANS,BST_CHECKED);
		} else {
			SetDlgItemText(objprop,IDC_MASTER,"(none)");
			CheckDlgButton(objprop,IDC_RTTRANS,BST_UNCHECKED);
		}


		seltd = -1;

		for(i = 0; i < bf_obj_count(); i++) {			
			SendDlgItemMessage(objprop, IDC_PARENT, CB_INSERTSTRING, i, (long)bf_getobject(i)->name);
			if(!strcmp(bf_getobject(i)->name,thisobj->parent)) seltd = i;
		}

		SendDlgItemMessage(objprop, IDC_PARENT, CB_INSERTSTRING, i, (long)"(none)");

		if(seltd == -1)	SendDlgItemMessage(objprop, IDC_PARENT, CB_SETCURSEL, i, 0);
		else			SendDlgItemMessage(objprop, IDC_PARENT, CB_SETCURSEL, seltd, 0);

		
		CheckDlgButton(objprop,IDC_ENABLED,thisobj->bEnabled ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(objprop,IDC_VISIBLE,thisobj->bVisible ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(objprop,IDC_DYNAMIC,thisobj->bDynamic ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(objprop,IDC_NOLIGHT,thisobj->bNoLight ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(objprop,IDC_DS,thisobj->bNoCull ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(objprop,IDC_INDRB,thisobj->bRadBlur ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(objprop,IDC_ACTIVE,thisobj->bActive ? BST_CHECKED : BST_UNCHECKED);		

		coptr = thisobj;
	}

	return;
}

void objprop_enable(bool endis) {
	BEGIN_FUNC("objprop_enable")

	char bleh[64];

	if(endis) {
		//sprintf(bleh,"%i Surfaces Selected",num_selected);
		sprintf(bleh,"Object Selected");
		SetWindowText(objprop,bleh);
	} else {
		SetWindowText(objprop,"No Object Selected");
	}

	EnableWindow(GetDlgItem(objprop,IDC_ST01), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST02), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST03), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST04), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST05), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST06), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST07), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST08), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST09), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST10), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST11), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST12), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST13), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST14), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST15), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST16), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ST17), endis);
	
	EnableWindow(GetDlgItem(objprop,IDC_LNAME), endis);

	EnableWindow(GetDlgItem(objprop,IDC_POSX), endis);
	EnableWindow(GetDlgItem(objprop,IDC_POSY), endis);
	EnableWindow(GetDlgItem(objprop,IDC_POSZ), endis);

	EnableWindow(GetDlgItem(objprop,IDC_NORMX), endis);
	EnableWindow(GetDlgItem(objprop,IDC_NORMY), endis);
	EnableWindow(GetDlgItem(objprop,IDC_NORMZ), endis);

	EnableWindow(GetDlgItem(objprop,IDC_SCX), endis);
	EnableWindow(GetDlgItem(objprop,IDC_SCY), endis);
	EnableWindow(GetDlgItem(objprop,IDC_SCZ), endis);

	EnableWindow(GetDlgItem(objprop,IDC_ALPHA), endis);
	EnableWindow(GetDlgItem(objprop,IDC_TRANSP), endis);
	EnableWindow(GetDlgItem(objprop,IDC_RTTRANS), endis);

	EnableWindow(GetDlgItem(objprop,IDC_SCRIPT), endis);
	EnableWindow(GetDlgItem(objprop,IDC_EDITSCRIPT), endis);
	EnableWindow(GetDlgItem(objprop,IDC_PARENT), endis);	

	EnableWindow(GetDlgItem(objprop,IDC_ENABLED), endis);
	EnableWindow(GetDlgItem(objprop,IDC_VISIBLE), endis);
	EnableWindow(GetDlgItem(objprop,IDC_DYNAMIC), endis);
	EnableWindow(GetDlgItem(objprop,IDC_NOLIGHT), endis);
	EnableWindow(GetDlgItem(objprop,IDC_DYNAMIC), endis);
	EnableWindow(GetDlgItem(objprop,IDC_DS), endis);
	EnableWindow(GetDlgItem(objprop,IDC_ACTIVE), endis);
	EnableWindow(GetDlgItem(objprop,IDC_INDRB), endis);
	

	END_FUNC
	return;
}


/*
void edo_chg_flag(int flag_val, int state) {
	BEGIN_FUNC("edo_chg_flag")
	
	int i;
	BF_RNODE *thispoly;	

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

void edo_chg_alpha(int percent) {
	BEGIN_FUNC("edo_chg_alpha")
	
	int i;
	BF_RNODE *thispoly;	

	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getobject(sel_polys[i]);
		thispoly->opacity = float(percent) / 100.0f;
	}

	END_FUNC
	return;
}

void edo_sp_chg_selection() {
	BEGIN_FUNC("edo_sp_chg_selection")

	char bleh[256];
	int def_alpha = 0;
	int c_alpha;
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
*/