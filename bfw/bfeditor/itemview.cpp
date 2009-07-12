/*

  Bluefire Editor: Item Viewer dialog functions

  (c) Copyright 2006 Jacob Hipps
  http://thefro.net/~jacob/

  http://neoretro.net/

*/

#define MAX_ITEMS	1024
#define BLUEFIRE_CLNT

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource.h"
#include "../bluefire.h"
#include "commctrl.h"		// ms common control header

typedef struct {
	int index;
	char resid[64];
	long cdata_sz;
	long ddata_sz;
	char datatype;
} BFZ_ITEMS;


int repopulatex(HWND hDlg);

// imported superglobals
extern HINSTANCE hInst;
extern HWND bigwin;
extern int sel_polys[];
extern int num_selected;
extern int zzsel_mode;
extern HWND surfprop;
extern HWND objprop;
extern HWND editdlg;
extern void deselect_all();
extern void sp_chg_selection();
extern void obj_chgsel();
extern void lite_chgsel();
extern void toggle_selmode(int zzmode);

extern HWND imghwnd;
extern int fsd_mode;

//HIMAGELIST imglist;				// image list handle
TC_ITEM tabbx[5];				// tab information
//BFZ_ITEMS items[MAX_ITEMS];		// item cache
HMENU glory;					// popup menu

HWND itemdlg;

int item_cache_szx;				// item cache bounds

BOOL CALLBACK ItemViewDlg(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

int viewmode;

int init_itemview() {
	
	item_cache_szx = 0;

	memset(tabbx,0,sizeof(TC_ITEM) * 5);

	viewmode = 0;	

	tabbx[0].mask = TCIF_TEXT;
	tabbx[0].iImage = -1;
	tabbx[0].pszText = (char*)"Objects\0";
	tabbx[1].mask = TCIF_TEXT;
	tabbx[1].iImage = -1;
	tabbx[1].pszText = (char*)"Lights\0";
	tabbx[2].mask = TCIF_TEXT;
	tabbx[2].iImage = -1;
	tabbx[2].pszText = (char*)"Scripts\0";
	tabbx[3].mask = TCIF_TEXT;
	tabbx[3].iImage = -1;
	tabbx[3].pszText = (char*)"Audio\0";
	tabbx[4].mask = TCIF_TEXT;
	tabbx[4].iImage = -1;
	tabbx[4].pszText = (char*)"All\0";

	//glory = LoadMenu(hInst,MAKEINTRESOURCE(IDR_BFZPOPUP));
	itemdlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_ITEMVIEW), bigwin, ItemViewDlg);
	
	repopulatex(itemdlg);

	return 0;
}


BOOL CALLBACK ItemViewDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {

	char tstr[10];
	char bigbuff[256];
	char thisitem[256];
	POINT pointy;
	BFZ_FILE loady;
	LV_HITTESTINFO lvhti;
	int dwpos;
	int clicky;

	//zlogthis("msg = 0x%04x, wParam = %x, lParam = %x\n",msg,wParam,lParam);

	int nummy;
	int i;

	switch( msg ) {

		case WM_INITDIALOG:			
						
			SendDlgItemMessage(hDlg, IDC_TABBER, TCM_INSERTITEM, 0, (long)&tabbx[0]);
			SendDlgItemMessage(hDlg, IDC_TABBER, TCM_INSERTITEM, 1, (long)&tabbx[1]);
			SendDlgItemMessage(hDlg, IDC_TABBER, TCM_INSERTITEM, 2, (long)&tabbx[2]);
			SendDlgItemMessage(hDlg, IDC_TABBER, TCM_INSERTITEM, 3, (long)&tabbx[3]);
			SendDlgItemMessage(hDlg, IDC_TABBER, TCM_INSERTITEM, 4, (long)&tabbx[4]);

			ShowWindow(hDlg, SW_SHOW);
			SetFocus(hDlg);

			zlogthisx("INIT_DIALOG\n");

			break;
		
		case WM_DESTROY:

			//ImageList_Destroy(imglist);
			zlogthisx("WM_DESTROY\n");		
			EndDialog(hDlg, TRUE);			
            break;

		case WM_KEYDOWN:

			if(wParam == VK_RBUTTON) {
				GetCursorPos(&pointy);
				TrackPopupMenuEx(glory,0,pointy.x,pointy.y,hDlg,NULL);
			}

			break;
		case WM_CONTEXTMENU:

			TrackPopupMenuEx(glory,0,LOWORD(lParam),HIWORD(lParam),hDlg,NULL);

			break;
		case WM_COMMAND:

			switch (LOWORD(wParam))
			{
				case IDABORT:
				case IDCANCEL:
				case IDOK:
					/*
					ImageList_Destroy(imglist);
					zlogthis("PackViewDlg: EndDialog\n");
					EndDialog(hDlg, TRUE);
					*/
					ShowWindow(hDlg,SW_HIDE);
					break;
			}
			break;
		case WM_NOTIFY:
			switch(LOWORD(wParam)) {
				case IDC_TABBER:
					if(LPNMHDR(lParam)->code == TCN_SELCHANGE) {
						viewmode = SendDlgItemMessage(hDlg, IDC_TABBER, TCM_GETCURSEL, 0, 0);
						repopulatex(hDlg);
					}
				break;
				case IDC_ITEMSX:
					if(LPNM_LISTVIEW(lParam)->hdr.code == LVN_ITEMCHANGED) {
						// nothing here yet
					} else if(LPNM_LISTVIEW(lParam)->hdr.code == NM_DBLCLK) {

						dwpos = GetMessagePos();
						lvhti.pt.x = LOWORD(dwpos);
						lvhti.pt.y = HIWORD(dwpos);

						MapWindowPoints(HWND_DESKTOP, GetDlgItem(hDlg,IDC_ITEMSX), &lvhti.pt, 1);
						clicky = ListView_HitTest(GetDlgItem(hDlg,IDC_ITEMSX), &lvhti);

						if(clicky != -1) {							

							zlogthisx(">>> item clicked. clicky = %i, viewmode = %i, zzsel_mode = %i\n",clicky,viewmode,zzsel_mode);

							if(viewmode == 0) {
								// objects mode
								//toggle_selmode(BFE_SELMODE_OBJ);
								toggle_selmode(BFE_SELMODE_OBJ);
								num_selected = 1;
								sel_polys[0] = clicky; //ListView_GetNextItem(GetDlgItem(itemdlg,IDC_ITEMSX), -1, LVNI_ALL | LVNI_FOCUSED);
								obj_chgsel();
							} else if(viewmode == 1) {
								// lights mode
								toggle_selmode(BFE_SELMODE_LIGHTS);
								num_selected = 1;
								sel_polys[0] = clicky; //ListView_GetNextItem(GetDlgItem(itemdlg,IDC_ITEMSX), -1, LVNI_ALL | LVNI_FOCUSED);
								lite_chgsel();
							}
						}
					}
					break;
			}
		default:
			return FALSE;
    }

	return TRUE;
}


int repopulatex(HWND hDlg) {
		
	char tmpstr[256];
	char tmpfile[256];	
	LV_ITEM bizzle;
	int donuts;

	ShowWindow(GetDlgItem(hDlg,IDC_ITEMSX),SW_HIDE);
	SendDlgItemMessage(hDlg, IDC_ITEMSX, LVM_DELETEALLITEMS, 0, 0);

	//imglist = ImageList_Create(ILIST_SZ,ILIST_SZ,ILC_COLOR24,donuts,1);

	//SendDlgItemMessage(hDlg, IDC_ITEMSX, LVM_SETIMAGELIST, LVSIL_NORMAL, (long)imglist);

	int i, zz;

	zz = 0;

	//HBITMAP pbitty;
	HDC hDCx;
	//BITMAPINFO binfo;
	char fizile[256];
	FILE *dumpfile;

	char *bitvals;

	HWND hHandy;
	int chotzke = 0;

	hHandy = GetDlgItem(hDlg,IDC_ITEMSX);
		
	HBITMAP oldbitty;
	
	switch(viewmode) {
		case 0:		// object mode			
			for(i = 0; i < bf_obj_count(); i++) {

				bizzle.mask = LVIF_TEXT | LVIF_PARAM;
				bizzle.iItem = i;
				bizzle.iSubItem = 0;
				bizzle.pszText = bf_obj_getptr(i)->name;					
				bizzle.lParam = 0;

				SendDlgItemMessage(hDlg, IDC_ITEMSX, LVM_INSERTITEM, zz, (long)&bizzle);				
			}

			break;
		case 1:		// light mode
			for(i = 0; i < bf_light_cnt(); i++) {				
				bizzle.mask = LVIF_TEXT | LVIF_PARAM;
				bizzle.iItem = i;
				bizzle.iSubItem = 0;
				bizzle.pszText = bf_getlight(i)->resid;
				bizzle.lParam = 0;
				SendDlgItemMessage(hDlg, IDC_ITEMSX, LVM_INSERTITEM, zz, (long)&bizzle);
				zz++;
			}
			break;
		case 2:		// scripts mode
			/*
			for(i = 0; i < donuts; i++) {
				if(items[i].datatype == BFC_GEO) {
					bizzle.mask = LVIF_TEXT | LVIF_PARAM;
					bizzle.iItem = i;
					bizzle.iSubItem = 0;
					bizzle.pszText = items[i].resid;
					bizzle.lParam = 0;
					SendDlgItemMessage(hDlg, IDC_ITEMSX, LVM_INSERTITEM, zz, (long)&bizzle);
					zz++;
				}
			}
			*/
			break;
		case 3:		// sound/audio mode
			/*
			for(i = 0; i < donuts; i++) {
				if(items[i].datatype == BFC_AUDIO) {
					bizzle.mask = LVIF_TEXT | LVIF_PARAM;
					bizzle.iItem = i;
					bizzle.iSubItem = 0;
					bizzle.pszText = items[i].resid;
					bizzle.lParam = 0;
					SendDlgItemMessage(hDlg, IDC_ITEMSX, LVM_INSERTITEM, zz, (long)&bizzle);
					zz++;
				}				
			}
			*/
			break;
		case 4:		// everything mode
			/*
			for(i = 0; i < donuts; i++) {
				bizzle.mask = LVIF_TEXT | LVIF_PARAM;
				bizzle.iItem = i;
				bizzle.iSubItem = 0;
				bizzle.pszText = items[i].resid;
				bizzle.lParam = 0;
				SendDlgItemMessage(hDlg, IDC_ITEMSX, LVM_INSERTITEM, 0, (long)&bizzle);
			}
			*/
			break;
	}
	
	//ReleaseDC(imghwnd,hDCx);

	ShowWindow(GetDlgItem(hDlg,IDC_ITEMSX),SW_SHOW);
	return 0;
}

