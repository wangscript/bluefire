/*

  Bluefire Editor: Package Viewer dialog functions

  (c) Copyright 2004 Jacob Hipps
  http://thefro.net/~jacob/

  http://neoretro.net/

*/

#define PACKBASE	"."		// base dir of BFZ packages
#define MAX_ITEMS	1024
#define ILIST_SZ	256		// image list square size (ea. image)
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

// BFZ functions (local: bfz.cpp)
int bfz_read(FILE *fin, BFZ_ITEMS *items, int maxsize);
int bfz_img_read(FILE* bfz_ptr, char *res_name, esm_picture *pic_data);

int imgview_show_256win();
int timer_reset();
int show_fullsize();

int repopulate(HWND hDlg);
int bfx_blit(char *dest, esm_picture *src);
int bfx_rszblit(char *dest, esm_picture *src);
int bfx_blit256(char *dest, esm_picture *src);


// imported superglobals
extern HINSTANCE hInst;
extern HWND bigwin;
extern char packs[256][256];

extern HWND imghwnd;
extern int fsd_mode;

extern int sel_polys[1024];
extern int num_selected;

HBITMAP bittyk;

HIMAGELIST imglist;				// image list handle
TC_ITEM tabbies[5];				// tab information
BFZ_ITEMS items[MAX_ITEMS];		// item cache
esm_picture images[MAX_ITEMS];	// image/preview cache
FILE *fathy;					// currently opened file
HMENU poppy;					// popup menu
char packfile[512];				// current package filename

HWND packdlg;

int img_cache_sz;				// image cache bounds
int item_cache_sz;				// item cache bounds
int loaded_packs;				// number of packages currently loaded

BOOL CALLBACK PackViewDlg(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
int enum_packs();

int packmode;

int init_packview() {
	

	enum_packs();

	img_cache_sz = 0;
	item_cache_sz = 0;

	memset(tabbies,0,sizeof(TC_ITEM) * 5);

	packmode = 0;
	fathy = NULL;

	tabbies[0].mask = TCIF_TEXT;
	tabbies[0].iImage = -1;
	tabbies[0].pszText = (char*)"Textures\0";
	tabbies[1].mask = TCIF_TEXT;
	tabbies[1].iImage = -1;
	tabbies[1].pszText = (char*)"Objects\0";
	tabbies[2].mask = TCIF_TEXT;
	tabbies[2].iImage = -1;
	tabbies[2].pszText = (char*)"Brushes\0";
	tabbies[3].mask = TCIF_TEXT;
	tabbies[3].iImage = -1;
	tabbies[3].pszText = (char*)"Audio\0";
	tabbies[4].mask = TCIF_TEXT;
	tabbies[4].iImage = -1;
	tabbies[4].pszText = (char*)"All\0";

	poppy = LoadMenu(hInst,MAKEINTRESOURCE(IDR_BFZPOPUP));
	packdlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_PACKVIEW), bigwin, PackViewDlg);
	
	repopulate(packdlg);

	return 0;
}


BOOL CALLBACK PackViewDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {

	char tstr[10];
	char bigbuff[256];
	char thisitem[256];
	POINT pointy;
	BFZ_FILE loady;

	//zlogthis("msg = 0x%04x, wParam = %x, lParam = %x\n",msg,wParam,lParam);

	int nummy;
	int i;

	switch( msg ) {

		case WM_INITDIALOG:			
						
			SendDlgItemMessage(hDlg, IDC_TABBY, TCM_INSERTITEM, 0, (long)&tabbies[0]);
			SendDlgItemMessage(hDlg, IDC_TABBY, TCM_INSERTITEM, 1, (long)&tabbies[1]);
			SendDlgItemMessage(hDlg, IDC_TABBY, TCM_INSERTITEM, 2, (long)&tabbies[2]);
			SendDlgItemMessage(hDlg, IDC_TABBY, TCM_INSERTITEM, 3, (long)&tabbies[3]);
			SendDlgItemMessage(hDlg, IDC_TABBY, TCM_INSERTITEM, 4, (long)&tabbies[4]);

			// enumerate packages
			for(i = 0; i < loaded_packs; i++) {
				SendDlgItemMessage(hDlg, IDC_PACKLIST, CB_INSERTSTRING, i, (long)packs[i]);
			}

			ShowWindow(hDlg, SW_SHOW);
			SetFocus(hDlg);

			zlogthis("PackViewDlg: WM_INITDIALOG\n");

			break;
		
		case WM_DESTROY:

			ImageList_Destroy(imglist);
			zlogthis("PackViewDlg: WM_DESTROY\n");
			fclose(fathy);
			EndDialog(hDlg, TRUE);			
            break;

		case WM_KEYDOWN:

			if(wParam == VK_RBUTTON) {
				GetCursorPos(&pointy);
				TrackPopupMenuEx(poppy,0,pointy.x,pointy.y,hDlg,NULL);
			}

			break;
		case WM_CONTEXTMENU:

			TrackPopupMenuEx(poppy,0,LOWORD(lParam),HIWORD(lParam),hDlg,NULL);

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
				case IDC_PACKLIST:					
					if(HIWORD(wParam) == CBN_SELCHANGE) {
						repopulate(hDlg);
					}
					break;
			}
			break;
		case WM_NOTIFY:
			switch(LOWORD(wParam)) {
				case IDC_TABBY:
					if(LPNMHDR(lParam)->code == TCN_SELCHANGE) {
						packmode = SendDlgItemMessage(hDlg, IDC_TABBY, TCM_GETCURSEL, 0, 0);
						repopulate(hDlg);
					}
				break;
				case IDC_LIZZIE:
					if(LPNM_LISTVIEW(lParam)->hdr.code == LVN_ITEMCHANGED) {
						fsd_mode = 0;	// disable the fullsize bitmap in image viewer
						imgview_show_256win();
						if(imghwnd) {
							InvalidateRect(imghwnd,NULL,TRUE);
							RedrawWindow(imghwnd,NULL,NULL,RDW_INTERNALPAINT | RDW_UPDATENOW);
						}
					} else if(((LV_KEYDOWN*)lParam)->hdr.code == LVN_KEYDOWN) {
						/*
						if(imghwnd) {
							show_fullsize();
						}
						*/
						ListView_GetItemText(GetDlgItem(packdlg,IDC_LIZZIE),ListView_GetNextItem(GetDlgItem(packdlg,IDC_LIZZIE), -1, LVNI_ALL | LVNI_FOCUSED),0,thisitem,255);
			
						sprintf(bigbuff,"%s:%s",packfile,thisitem);
						zlogthis("checking for availability of %s... ",bigbuff);

						if(packmode == 0) {
							nummy = bf_getimg_num(bigbuff);

							zlogthis("%i\n",nummy);

							if(nummy == -1) {
								sprintf(bigbuff,"%s\\%s.bfz",PACKBASE,packfile);
								bfz_open(bigbuff, &loady);
								nummy = bf_addimg();
								bfz_loadimg(&loady,thisitem,nummy);
								bfz_close(&loady);
							}
		
							i = 0;
							//zlogthis("num_selected = %i, nummy = %i, bf_getimg(nummy)->tex_id = %i\n",num_selected, nummy, bf_getimg(nummy)->tex_id);
							while(i < num_selected) {
								//zlogthis("i = %i, sel_polys[i] = %i, bf_getimg(nummy) ptr = 0x%08x, bf_getpoly(sel_polys[i]) ptr = %08x\n",i, sel_polys[i],bf_getimg(nummy),bf_getpoly(sel_polys[i]));
								bf_getpoly(sel_polys[i])->df_texdex = nummy;
								i++;
							}
							zlogthis("Textures changed!\n");
						}						
					}
					break;
			}
		default:
			return FALSE;
    }

	return TRUE;
}


int repopulate(HWND hDlg) {
		
	char tmpstr[256];
	char tmpfile[256];
	int cfile;		// current file (index of packs[])
	LV_ITEM bizzle;
	int donuts;

	ShowWindow(GetDlgItem(hDlg,IDC_PROGRESS),SW_SHOW);
	ShowWindow(GetDlgItem(hDlg,IDC_IMPHELP),SW_HIDE);
	ShowWindow(GetDlgItem(hDlg,IDC_LIZZIE),SW_HIDE);
	ShowWindow(GetDlgItem(hDlg,IDC_WAITTEXT),SW_SHOW);
	RedrawWindow(GetDlgItem(hDlg,IDC_WAITTEXT),NULL,NULL,RDW_INTERNALPAINT | RDW_UPDATENOW);
	SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETPOS, 0, 0);

	cfile = SendDlgItemMessage(hDlg, IDC_PACKLIST, CB_GETCURSEL, 0, 0);
	if(cfile == CB_ERR) {
		//zlogthis("repopulate: invalid selection.\n");
		SendDlgItemMessage(hDlg, IDC_PACKLIST, CB_SETCURSEL, 0, 0);
		cfile = 0;
	}
	

	SendDlgItemMessage(hDlg, IDC_PACKLIST, CB_GETLBTEXT, cfile, (long)&tmpfile);
	
	SendDlgItemMessage(hDlg, IDC_LIZZIE, LVM_DELETEALLITEMS, 0, 0);

	sprintf(tmpstr,"%s\\%s.bfz",PACKBASE,tmpfile);

	strcpy(packfile,tmpfile);

	if(fathy) fclose(fathy);
	if((fathy = fopen(tmpstr,"rb")) == NULL) {
		zlogthis("repopulate: Could not open package \"%s\".\n",tmpstr);
		MessageBox(bigwin,"Error opening package.","Bluefire Editor",MB_APPLMODAL);
		return -1;
	}

	donuts = bfz_read(fathy,items,MAX_ITEMS);	

	//zlogthis("repopulate: package \"%s\" has %i items.\n",tmpfile,donuts);
	
	ImageList_Destroy(imglist);

	imglist = ImageList_Create(ILIST_SZ,ILIST_SZ,ILC_COLOR24,donuts,1);

	SendDlgItemMessage(hDlg, IDC_LIZZIE, LVM_SETIMAGELIST, LVSIL_NORMAL, (long)imglist);

	int i, zz;

	zz = 0;

	HBITMAP pbitty;
	HDC hDCx;
	BITMAPINFO binfo;
	char fizile[256];
	FILE *dumpfile;

	char *bitvals;

	HWND hHandy;
	int chotzke = 0;

	hHandy = GetDlgItem(hDlg,IDC_LIZZIE);
	
	hDCx = GetDC(imghwnd);
	HBITMAP oldbitty;
	
	switch(packmode) {
		case 0:		// image/texture mode
			for(i = 0; i < donuts; i++) {
				if(items[i].datatype == BFC_IMG) {
					bizzle.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
					bizzle.iItem = i;
					bizzle.iSubItem = 0;
					bizzle.pszText = items[i].resid;
					bizzle.lParam = 0;

					bfz_img_read(fathy,items[i].resid,&images[i]);
					
					memset(&binfo,0,sizeof(BITMAPINFO));
					binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					binfo.bmiHeader.biWidth = 256;
					binfo.bmiHeader.biHeight = 256;
					binfo.bmiHeader.biBitCount = 24;	// 24-bit bitmap
					binfo.bmiHeader.biPlanes = 1; // Must be set to one
					binfo.bmiHeader.biCompression = BI_RGB;					
										
					bittyk = CreateDIBSection(hDCx,&binfo,DIB_RGB_COLORS,(void**)&bitvals,NULL,NULL);

					bfx_blit256(bitvals,&images[i]);

					
					bizzle.iImage = ImageList_Add(imglist,bittyk,NULL);
					//zlogthis("repopulate: added image #%i to image list. bittyk = 0x%08x (err %i)\n",bizzle.iImage,bittyk,GetLastError());
					
					SendDlgItemMessage(hDlg, IDC_LIZZIE, LVM_INSERTITEM, zz, (long)&bizzle);

					free(images[i].data_ptr);	// free unused bitmap data
					DeleteObject(bittyk);		// delete the bitmap object
					zz++;

				}
				SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETPOS, int((float(i + 1)/float(donuts)) * 100.0f), 0);
			}

			break;
		case 1:		// object mode
			for(i = 0; i < donuts; i++) {
				if(items[i].datatype == BFC_OBJ) {
					bizzle.mask = LVIF_TEXT | LVIF_PARAM;
					bizzle.iItem = i;
					bizzle.iSubItem = 0;
					bizzle.pszText = items[i].resid;
					bizzle.lParam = 0;
					SendDlgItemMessage(hDlg, IDC_LIZZIE, LVM_INSERTITEM, zz, (long)&bizzle);
					zz++;
				}
				SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETPOS, int((float(i + 1)/float(donuts)) * 100.0f), 0);
			}
			break;
		case 2:		// brush mode
			for(i = 0; i < donuts; i++) {
				if(items[i].datatype == BFC_GEO) {
					bizzle.mask = LVIF_TEXT | LVIF_PARAM;
					bizzle.iItem = i;
					bizzle.iSubItem = 0;
					bizzle.pszText = items[i].resid;
					bizzle.lParam = 0;
					SendDlgItemMessage(hDlg, IDC_LIZZIE, LVM_INSERTITEM, zz, (long)&bizzle);
					zz++;
				}
				SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETPOS, int((float(i + 1)/float(donuts)) * 100.0f), 0);
			}
			break;
		case 3:		// sound/audio mode
			for(i = 0; i < donuts; i++) {
				if(items[i].datatype == BFC_AUDIO) {
					bizzle.mask = LVIF_TEXT | LVIF_PARAM;
					bizzle.iItem = i;
					bizzle.iSubItem = 0;
					bizzle.pszText = items[i].resid;
					bizzle.lParam = 0;
					SendDlgItemMessage(hDlg, IDC_LIZZIE, LVM_INSERTITEM, zz, (long)&bizzle);
					zz++;
				}
				SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETPOS, int((float(i + 1)/float(donuts)) * 100.0f), 0);
			}
			break;
		case 4:		// everything mode

			for(i = 0; i < donuts; i++) {
				bizzle.mask = LVIF_TEXT | LVIF_PARAM;
				bizzle.iItem = i;
				bizzle.iSubItem = 0;
				bizzle.pszText = items[i].resid;
				bizzle.lParam = 0;
				SendDlgItemMessage(hDlg, IDC_LIZZIE, LVM_INSERTITEM, 0, (long)&bizzle);
				SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETPOS, int((float(i + 1)/float(donuts)) * 100.0f), 0);
			}

			break;
	}
	
	//DeleteDC(hDCx);
	ReleaseDC(imghwnd,hDCx);

	ShowWindow(GetDlgItem(hDlg,IDC_PROGRESS),SW_HIDE);	
	ShowWindow(GetDlgItem(hDlg,IDC_WAITTEXT),SW_HIDE);
	ShowWindow(GetDlgItem(hDlg,IDC_IMPHELP),SW_SHOW);
	ShowWindow(GetDlgItem(hDlg,IDC_LIZZIE),SW_SHOW);
	return 0;
}





/*

  Does conversion of bitmap from top-down to bottom-up
  and swaps the RGB values to Intel ordering from
  Network/Motorola ordering. (RGB -> BGR).
  Windows GDI sucks.

  Only works with 24-bit uncompressed bitmaps.
  (8-bits per channel, RGB)

*/

int bfx_blit(char *dest, esm_picture *src) {

	int i, z;	

	int x_width = src->width;
	int y_height = src->height;
	int rowspan = src->width;

	// start at the bottom-left corner
	for(i = (y_height - 1); i >= 0; i--) {
		for(z = 0; z < x_width; z++) {
			//zlogthis("%i + (%i * %i * 3) + (%i * 3) = %i\n",dest,i,x_width,z,dest + (i * x_width * 3) + (z * 3));
			// copy G, swap R and B (heh, have fun figuring out these pointers ;)
			*((char*)((int)dest + (i * x_width * 3) + (z * 3))) = *((char*)((int)src->data_ptr + ((y_height - i) * rowspan * 3) + (z * 3) + 2));
			*((char*)((int)dest + (i * x_width * 3) + (z * 3) + 1)) = *((char*)((int)src->data_ptr + ((y_height - i) * rowspan * 3) + (z * 3) + 1));
			*((char*)((int)dest + (i * x_width * 3) + (z * 3) + 2)) = *((char*)((int)src->data_ptr + ((y_height - i) * rowspan * 3) + (z * 3)));
		}
	}

	return 0;
}

int bfx_blit256(char *dest, esm_picture *src) {

	int i, z;	

	int x_width = src->width;
	int y_height = src->height;
	int rowspan = src->width;

	if(x_width > 256) x_width = 256;
	if(y_height > 256) y_height = 256;

	memset(dest,255,196608);	// clear the dest bitmap to white

	// start at the bottom-left corner
	for(i = (y_height - 1); i >= 0; i--) {
		for(z = 0; z < x_width; z++) {
			*((char*)((int)dest + (i * x_width * 3) + (z * 3))) = *((char*)((int)src->data_ptr + ((y_height - i) * rowspan * 3) + (z * 3) + 2));
			*((char*)((int)dest + (i * x_width * 3) + (z * 3) + 1)) = *((char*)((int)src->data_ptr + ((y_height - i) * rowspan * 3) + (z * 3) + 1));
			*((char*)((int)dest + (i * x_width * 3) + (z * 3) + 2)) = *((char*)((int)src->data_ptr + ((y_height - i) * rowspan * 3) + (z * 3)));
		}
	}

	return 0;
}

/*

  Blit the image and resize it so that it is
  equal to 256x256 pixels

*/
int bfx_rszblit(char *dest, esm_picture *src) {

	//int linespan = (src->width * 3) + ((src->width * 3) % 4); // make sure DWORD-aligned
	int linespan = src->width * 3;

	int skipx = (src->width / 256);
	int skipy = (src->height / 256);
	int i, z, h, v;

	zlogthis("buffer = %i bytes, linespan = %i bytes\n",((src->width * 3) % 4),linespan);
	if(src->width % 256 || src->height % 256) {
		zlogthis("bfx_rszblit: Not evenly divisible by 256! Image will be cropped!\n");
	}

	if(skipx == 0) skipx = 1;
	if(skipy == 0) skipy = 1;

	memset(dest,255,196608);	// clear the dest bitmap to white

	// start at the bottom-left corner
	v = 255;
	for(i = (src->height - 1); i >= 0; i -= skipy) {
		h = 0;
		zlogthis("dest:src V %i <- %i ======\n",v,i);
		for(z = 0; z < src->width; z += skipx) {
			// copy G, swap R and B (heh, have fun figuring out these pointers ;)
			zlogthis("dest:src H %i <- %i\n",h,z);
			*((char*)((int)dest + (v * linespan) + (h * 3))) = *((char*)((int)src->data_ptr + ((src->height - i) * src->width * 3) + (z * 3) + 2));
			*((char*)((int)dest + (v * linespan) + (h * 3) + 1)) = *((char*)((int)src->data_ptr + ((src->height - i) * src->width * 3) + (z * 3) + 1));
			*((char*)((int)dest + (v * linespan) + (h * 3) + 2)) = *((char*)((int)src->data_ptr + ((src->height - i) * src->width * 3) + (z * 3)));			
			h++;
			if(h > 255) break;
		}
		v--;
		if(v < 0) break;
	}

	return 0;
}


int enum_packs() {

	WIN32_FIND_DATA FileData;  
	HANDLE hSearch; 
	char filedesc[128];
	int fFinished = 0;


	hSearch = FindFirstFile(PACKBASE "\\*.bfz", &FileData); 
	if (hSearch == INVALID_HANDLE_VALUE) 
	{ 
		loaded_packs = 0;
		zlogthis("enum_packs: No BFZ packages found.\n");
		return 2;
	} 

	int i;
	loaded_packs = 0;

	while (!fFinished) { 
		
		for(i = strlen(FileData.cFileName); i >= 0; i--) {
			if(FileData.cFileName[i] == '.') {
				break;
			}
		}

		memcpy(filedesc,FileData.cFileName,i);
		filedesc[i] = 0;

		strcpy(packs[loaded_packs],filedesc);
					
		if (!FindNextFile(hSearch, &FileData)) {
			if (GetLastError() == ERROR_NO_MORE_FILES) {
				printf("\nComplete.\n");
				fFinished = TRUE;
			} else {
				zlogthis("Error enumerating files.\r");
				return 2;
			} 
		}

		loaded_packs++;
	} 
 
	FindClose(hSearch);
	
	return 0;
}