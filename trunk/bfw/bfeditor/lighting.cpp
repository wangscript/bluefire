/*

  Bluefire Editor: Light Editor Dialog functions

  (c) Copyright 2004-2006 Jacob Hipps
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


HWND editdlg;

BOOL CALLBACK EditLightDlg(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

int cledit = -1;		// current light being edited
BF_LIGHT *clptr;

extern int sel_polys[];
extern int num_selected;
extern int zzsel_mode;
extern void deselect_all();
extern void toggle_selmode(int zzmode);

int init_liteprop();
void lite_chgsel();
void liteprop_enable(bool endis);
CHOOSECOLOR ccpicker;
COLORREF reffy;
COLORREF custcolors[16];

char *colorx = (char*)reffy;

int add_light_source() {
	BEGIN_FUNC("add_light_source")

	BF_LIGHT *thislite;
	
	cledit = bf_addlight();
	thislite = bf_getlight(cledit);

	memset(thislite,0,sizeof(BF_LIGHT));	// zero-out the struct
	sprintf(thislite->resid,"Light%i",cledit);
	thislite->lumens = 5;
	thislite->r = 1.0f;
	thislite->g = 1.0f;
	thislite->b = 1.0f;
	thislite->a = 1.0f;	
	thislite->bEnabled = 1;
	thislite->l_type = BF_LIGHT_POINT;

	clptr = thislite;

	toggle_selmode(BFE_SELMODE_LIGHTS);

	zzsel_mode = BFE_SELMODE_LIGHTS;
	sel_polys[0] = cledit;
	num_selected = 1;
	lite_chgsel();

	ShowWindow(editdlg,SW_SHOW);
	SendDlgItemMessage(editdlg, IDC_LNAME, EM_SETSEL, 0, -1);

	END_FUNC
	return 0;
}

int init_liteprop() {
	BEGIN_FUNC("init_liteprop")

	editdlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_EDLIGHT), bigwin, EditLightDlg);

	END_FUNC
	return 0;
}


BOOL CALLBACK EditLightDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	BEGIN_FUNC("EditLightDlg")

	char tstr[10];
	char bigbuff[256];
	
	//zlogthis("msg = 0x%04x, wParam = %x, lParam = %x\n",msg,wParam,lParam);

	switch( msg ) {

		case WM_INITDIALOG:			
			

			SendDlgItemMessage(hDlg, IDC_LTYPE, CB_INSERTSTRING, 0, (long)"Point");
			SendDlgItemMessage(hDlg, IDC_LTYPE, CB_INSERTSTRING, 1, (long)"Directional");
			SendDlgItemMessage(hDlg, IDC_LTYPE, CB_INSERTSTRING, 2, (long)"Spotlight");
			SendDlgItemMessage(hDlg, IDC_LTYPE, CB_INSERTSTRING, 3, (long)"Ambient");
			SendDlgItemMessage(hDlg, IDC_LTYPE, CB_INSERTSTRING, 4, (long)"Area");
			SendDlgItemMessage(hDlg, IDC_LTYPE, CB_INSERTSTRING, 5, (long)"Specular");

			ShowWindow(hDlg, SW_SHOW);
			SetFocus(hDlg);
			// set the focus to the Light Name editbox			
			SetFocus(GetDlgItem(hDlg,IDC_LNAME));
			SendDlgItemMessage(hDlg, IDC_LNAME, EM_SETSEL, 0, -1);

			zlogthisx("WM_INITDIALOG\n");

			break;
		
		case WM_DESTROY:

			zlogthisx("WM_DESTROY\n");
			EndDialog(hDlg, TRUE);
            break;

		case WM_HSCROLL:
			switch(GetDlgCtrlID((HWND)lParam)) {
				case IDC_LUMENS:					
					clptr->lumens = SendDlgItemMessage(hDlg, IDC_LUMENS, TBM_GETPOS, 0, 0);
					sprintf(bigbuff,"%i lm",clptr->lumens);
					SetDlgItemText(hDlg, IDC_FLUX, bigbuff);					
					break;
				case IDC_INTENSITY:
					clptr->a = float(SendDlgItemMessage(hDlg, IDC_INTENSITY, TBM_GETPOS, 0, 0)) / 100.0f;
					sprintf(bigbuff,"%i%%",int(clptr->a * 100.0f));
					SetDlgItemText(hDlg, IDC_DINT, bigbuff);					
					break;
			}
			break;

		case WM_COMMAND:

			switch (LOWORD(wParam))
			{
				case IDC_LTYPE:
					switch(HIWORD(wParam)) {
					  case CBN_SELCHANGE:
						clptr->l_type = SendDlgItemMessage(hDlg, IDC_LTYPE, CB_GETCURSEL, 0, 0);
						lite_chgsel();
						break;
					}
					break;
				case IDC_POSX:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_POSX,bigbuff,255);
						clptr->x = atof(bigbuff);
						sprintf(tstr,"%.2f",clptr->x);
						SetDlgItemText(hDlg,IDC_POSX,tstr);
					}
					break;
				case IDC_POSY:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_POSY,bigbuff,255);
						clptr->y = atof(bigbuff);
						sprintf(tstr,"%.2f",clptr->y);
						SetDlgItemText(hDlg,IDC_POSY,tstr);
					}
					break;
				case IDC_POSZ:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_POSZ,bigbuff,255);
						clptr->z = atof(bigbuff);
						sprintf(tstr,"%.2f",clptr->z);
						SetDlgItemText(hDlg,IDC_POSZ,tstr);
					}
					break;

				case IDC_NORMX:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_NORMX,bigbuff,255);
						clptr->nx = atof(bigbuff);
						sprintf(tstr,"%.2f",clptr->nx);
						SetDlgItemText(hDlg,IDC_NORMX,tstr);
					}
					break;
				case IDC_NORMY:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_NORMY,bigbuff,255);
						clptr->ny = atof(bigbuff);
						sprintf(tstr,"%.2f",clptr->ny);
						SetDlgItemText(hDlg,IDC_NORMY,tstr);
					}
					break;
				case IDC_NORMZ:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_NORMZ,bigbuff,255);
						clptr->nz = atof(bigbuff);
						sprintf(tstr,"%.2f",clptr->nz);
						SetDlgItemText(hDlg,IDC_NORMZ,tstr);
					}
					break;

				case IDC_DIFR:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_DIFR,bigbuff,255);
						clptr->r = atof(bigbuff) / 100.0f;
						if(clptr->r > 1.0f) clptr->r = 1.0f;
						sprintf(tstr,"%03.0f",clptr->r * 100);
						SetDlgItemText(hDlg,IDC_DIFR,tstr);
					}
					break;
				case IDC_DIFG:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_DIFG,bigbuff,255);
						clptr->g = atof(bigbuff) / 100.0f;
						if(clptr->g > 1.0f) clptr->g = 1.0f;
						sprintf(tstr,"%03.0f",clptr->g * 100);
						SetDlgItemText(hDlg,IDC_DIFG,tstr);
					}
					break;
				case IDC_DIFB:
					if(HIWORD(wParam) == EN_KILLFOCUS) {
						GetDlgItemText(hDlg,IDC_DIFB,bigbuff,255);
						clptr->b = atof(bigbuff) / 100.0f;
						if(clptr->b > 1.0f) clptr->b = 1.0f;
						sprintf(tstr,"%03.0f",clptr->b * 100);
						SetDlgItemText(hDlg,IDC_DIFB,tstr);
					}
					break;
				case IDC_USEFG:
					
					reffy = ((int)(clptr->b * 255.0f) << 16) + ((int)(clptr->g * 255.0f) << 8) + (int)(clptr->r * 255.0f);

					ccpicker.lStructSize = sizeof(ccpicker);
					ccpicker.hwndOwner = editdlg;    
					ccpicker.hInstance = NULL;		
					ccpicker.rgbResult = reffy;
					ccpicker.lpCustColors = custcolors;
					ccpicker.Flags = CC_FULLOPEN|CC_RGBINIT;
					ccpicker.lCustData = NULL;
					ccpicker.lpfnHook = NULL;
					ccpicker.lpTemplateName = NULL;
					if(ChooseColor(&ccpicker)) {
						clptr->b = float(GetBValue(ccpicker.rgbResult)) / 255.0f;
						clptr->g = float(GetGValue(ccpicker.rgbResult)) / 255.0f;
						clptr->r = float(GetRValue(ccpicker.rgbResult)) / 255.0f;
						lite_chgsel();
					}
					break;
				case IDABORT:
				case IDCANCEL:
				case IDOK:
					ShowWindow(hDlg,SW_HIDE);
					break;
			}
			break;

		default:
			END_FUNC
			return FALSE;
    }

	END_FUNC
	return TRUE;
}



void lite_chgsel() {
	BEGIN_FUNC("lite_chgsel")

	int i, seltd;
	char bigbuff[256];
	char tstr[10];

	if(num_selected == 0) {
		liteprop_enable(0);
		clptr = (BF_LIGHT*)0x0CafeCafe;
	} else {
		clptr = bf_getlight(sel_polys[0]);
		liteprop_enable(1);
		SetDlgItemText(editdlg, IDC_LNAME, clptr->resid);

		sprintf(tstr,"%.2f",clptr->x);
		SetDlgItemText(editdlg, IDC_POSX, tstr);
		sprintf(tstr,"%.2f",clptr->y);
		SetDlgItemText(editdlg, IDC_POSY, tstr);
		sprintf(tstr,"%.2f",clptr->z);
		SetDlgItemText(editdlg, IDC_POSZ, tstr);

		sprintf(tstr,"%.2f",clptr->nx);
		SetDlgItemText(editdlg, IDC_NORMX, tstr);
		sprintf(tstr,"%.2f",clptr->ny);
		SetDlgItemText(editdlg, IDC_NORMY, tstr);
		sprintf(tstr,"%.2f",clptr->nz);
		SetDlgItemText(editdlg, IDC_NORMZ, tstr);

		sprintf(tstr,"%03.0f",clptr->r * 100);
		SetDlgItemText(editdlg, IDC_DIFR, tstr);
		sprintf(tstr,"%03.0f",clptr->g * 100);
		SetDlgItemText(editdlg, IDC_DIFG, tstr);
		sprintf(tstr,"%03.0f",clptr->b * 100);
		SetDlgItemText(editdlg, IDC_DIFB, tstr);

		CheckDlgButton(editdlg, IDC_ENABLED, clptr->bEnabled ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(editdlg, IDC_NEGATIVE, clptr->bNegative ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(editdlg, IDC_DYNAMIC, clptr->bDynamic ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(editdlg, IDC_NOLMS, clptr->bNoLMaps ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(editdlg, IDC_VERTEXRTL, BST_CHECKED);

		if(clptr->l_type == BF_LIGHT_DIRECTION || clptr->l_type == BF_LIGHT_SPOT) {
			SendDlgItemMessage(editdlg, IDC_POSX, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_POSX, EM_SETREADONLY, 0, 0);
			SendDlgItemMessage(editdlg, IDC_POSY, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_POSY, EM_SETREADONLY, 0, 0);
			SendDlgItemMessage(editdlg, IDC_POSZ, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_POSZ, EM_SETREADONLY, 0, 0);
			SendDlgItemMessage(editdlg, IDC_LUMENS, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_STATNORM0, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_STATNORM1, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_STATNORM2, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_STATNORM3, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_NORMX, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_NORMX, EM_SETREADONLY, 0, 0);
			SendDlgItemMessage(editdlg, IDC_NORMY, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_NORMY, EM_SETREADONLY, 0, 0);
			SendDlgItemMessage(editdlg, IDC_NORMZ, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_NORMZ, EM_SETREADONLY, 0, 0);
		} else if(clptr->l_type == BF_LIGHT_AMBIENT) {
			EnableWindow(GetDlgItem(editdlg,IDC_FLUX), 0);
			SendDlgItemMessage(editdlg, IDC_POSX, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_POSX, EM_SETREADONLY, 1, 1);
			SendDlgItemMessage(editdlg, IDC_POSY, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_POSY, EM_SETREADONLY, 1, 1);
			SendDlgItemMessage(editdlg, IDC_POSZ, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_POSZ, EM_SETREADONLY, 1, 1);
			SendDlgItemMessage(editdlg, IDC_LUMENS, WM_ENABLE, 0, 0);			
			SendDlgItemMessage(editdlg, IDC_STATNORM0, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_STATNORM1, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_STATNORM2, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_STATNORM3, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_NORMX, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_NORMX, EM_SETREADONLY, 1, 1);
			SendDlgItemMessage(editdlg, IDC_NORMY, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_NORMY, EM_SETREADONLY, 1, 1);
			SendDlgItemMessage(editdlg, IDC_NORMZ, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_NORMZ, EM_SETREADONLY, 1, 1);
		} else {
			SendDlgItemMessage(editdlg, IDC_POSX, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_POSX, EM_SETREADONLY, 0, 0);
			SendDlgItemMessage(editdlg, IDC_POSY, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_POSY, EM_SETREADONLY, 0, 0);
			SendDlgItemMessage(editdlg, IDC_POSZ, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_POSZ, EM_SETREADONLY, 0, 0);
			SendDlgItemMessage(editdlg, IDC_LUMENS, WM_ENABLE, 1, 1);
			SendDlgItemMessage(editdlg, IDC_STATNORM0, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_STATNORM1, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_STATNORM2, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_STATNORM3, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_NORMX, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_NORMX, EM_SETREADONLY, 1, 1);
			SendDlgItemMessage(editdlg, IDC_NORMY, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_NORMY, EM_SETREADONLY, 1, 1);
			SendDlgItemMessage(editdlg, IDC_NORMZ, WM_ENABLE, 0, 0);
			SendDlgItemMessage(editdlg, IDC_NORMZ, EM_SETREADONLY, 1, 1);
		}

		sprintf(bigbuff,"%i lm",clptr->lumens);
		SetDlgItemText(editdlg, IDC_FLUX, bigbuff);
		sprintf(bigbuff,"%i%%",int(clptr->a * 100.0f));
		SetDlgItemText(editdlg, IDC_DINT, bigbuff);
		SendDlgItemMessage(editdlg, IDC_LUMENS, TBM_SETRANGE, FALSE, MAKELONG(0,200));
		SendDlgItemMessage(editdlg, IDC_LUMENS, TBM_SETPOS, TRUE, clptr->lumens);
		SendDlgItemMessage(editdlg, IDC_INTENSITY, TBM_SETRANGE, FALSE, MAKELONG(0,100));
		SendDlgItemMessage(editdlg, IDC_INTENSITY, TBM_SETPOS, TRUE, int(clptr->a * 100.0f));
		SendDlgItemMessage(editdlg, IDC_LTYPE, CB_SETCURSEL, clptr->l_type, 0);

	}

	END_FUNC
	return;
}

void liteprop_enable(bool endis) {
	BEGIN_FUNC("liteprop_enable")

	char bleh[64];

	if(endis) {
		//sprintf(bleh,"%i Surfaces Selected",num_selected);
		sprintf(bleh,"Light Selected");
		SetWindowText(editdlg,bleh);
	} else {
		SetWindowText(editdlg,"No Lights Selected");
	}

	EnableWindow(GetDlgItem(editdlg,IDC_LNAME), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_ENABLED), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_NEGATIVE), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_NOLMS), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_VERTEXRTL), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_LUMENS), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_FLUX), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_LTYPE), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_POSX), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_POSY), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_POSZ), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_NORMX), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_NORMY), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_NORMZ), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_DIFR), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_DIFG), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_DIFB), endis);	
	EnableWindow(GetDlgItem(editdlg,IDC_INTENSITY), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_DINT), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_USEFG), endis);
/*
	EnableWindow(GetDlgItem(editdlg,IDC_STATIC1), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_STATIC2), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_STATIC3), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_STATIC4), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_STATIC5), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_STATIC6), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_STATIC7), endis);
	EnableWindow(GetDlgItem(editdlg,IDC_STATIC8), endis);
*/


	END_FUNC
	return;
}
