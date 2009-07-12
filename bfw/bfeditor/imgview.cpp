/*

  Bluefire Editor: Image Viewer dialog functions

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
int bfx_blit(char *dest, esm_picture *src);

// imported superglobals
extern HINSTANCE hInst;
extern HWND bigwin;
extern char packs[256][256];

extern HBITMAP bittyk;
extern HWND packdlg;
extern FILE *fathy;

extern HIMAGELIST imglist;				// image list handle
extern BFZ_ITEMS items[MAX_ITEMS];		// item cache
extern esm_picture images[MAX_ITEMS];	// image/preview cache

LRESULT WINAPI ImgViewProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int show_fullsize();
int imgview_show_256win();
int timer_reset();

HWND imghwnd;
WNDCLASSEX classy_img;
int fsd_mode;
esm_picture bigimage;
int diffx, diffy;			// area that the window frame occupies

HBITMAP bigbitty;			// fullsize bitmap handle

#define TIMEY	1286		// our timer ID


int init_imgview() {
	
    // The Window structure
	char classyimg[]    = "imgview";
    classy_img.hInstance		= hInst;
	classy_img.lpszClassName = classyimg;
    classy_img.lpfnWndProc	= ImgViewProc;
    classy_img.style			= CS_OWNDC;
    classy_img.cbSize		= sizeof(WNDCLASSEX);

    // Use default icon and mouse-pointer
    classy_img.hIcon   = NULL;
    classy_img.hIconSm = NULL;
    classy_img.hCursor = LoadCursor(hInst, IDC_ARROW);
    classy_img.lpszMenuName = 0;
    classy_img.cbClsExtra = 0;                      // No extra bytes after the window class
    classy_img.cbWndExtra = 0;                      // structure or the window instance
    classy_img.hbrBackground = (HBRUSH) COLOR_BACKGROUND; //GetStockObject(WHITE_BRUSH);		// black BG
	
    // Register the window class; if fail, quit the program
    if(!RegisterClassEx(&classy_img)) {
		MessageBox(NULL,"Window class registration failed!","BFW Critical Error",0);
		return 255;
	}

	//WS_EX_TOOLWINDOW
	imghwnd = CreateWindowEx(0,classy_img.lpszClassName,"Image Viewer", WS_OVERLAPPED |  WS_VISIBLE, 0, 0, 300, 300, bigwin, NULL, hInst, NULL );

	return 0;
}


LRESULT WINAPI ImgViewProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	HDC hDC, hDCx;
	PAINTSTRUCT ps;
	HBITMAP oldbitty;
	int selected_img;
	RECT rizzle, fiddle;	

	switch( message ) {

		case WM_CREATE:
			// make the window the correct size so that the client area
			// is exactly 256x256
			GetWindowRect(hwnd,&rizzle);
			GetClientRect(hwnd,&fiddle);
			diffx = (rizzle.right - rizzle.left) - (fiddle.right - fiddle.left);
			diffy = -((rizzle.top - rizzle.bottom) - (fiddle.top - fiddle.bottom));
			zlogthis("diffx = %i, diffy = %i\n",diffx,diffy);
			MoveWindow(hwnd,rizzle.left,rizzle.top, 256 + diffx, 256 + diffy, FALSE);

			bigbitty = 0;
			fsd_mode = 0;

			zlogthis("ImgViewProc: WM_CREATE\n");

		break;

		case WM_PAINT:
/*			
			if(imglist && !fsd_mode) {				
				DeleteObject(bigbitty);
				selected_img = ListView_GetNextItem(GetDlgItem(packdlg,IDC_LIZZIE), -1, LVNI_ALL | LVNI_FOCUSED);
				hDC = BeginPaint(imghwnd,&ps);
				hDCx = CreateCompatibleDC(hDC);
				ImageList_Draw(imglist,selected_img,hDC,0,0,ILD_NORMAL);
				DeleteDC(hDCx);	
				EndPaint(hwnd,&ps);
				SelectObject(hDCx,oldbitty);			
			}
*/
			if(bigbitty && fsd_mode) {
				hDC = BeginPaint(imghwnd,&ps);
				hDCx = CreateCompatibleDC(hDC);
				oldbitty = (HBITMAP)SelectObject(hDCx,bigbitty);
				BitBlt(hDC,0,0,bigimage.width,bigimage.height,hDCx,0,0,SRCCOPY);
				DeleteDC(hDCx);
				EndPaint(hwnd,&ps);
				SelectObject(hDCx,oldbitty);
			}

			return 0;
		
		case WM_TIMER:

			zlogthis("TIMER event recieved!");

			show_fullsize();			
			KillTimer(hwnd,wParam);

			return 0;
		case WM_DESTROY:
			
			zlogthis("ImgViewProc: WM_DESTROY\n");

            break;

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
    }

	return 0;
}


int show_fullsize() {

	BITMAPINFO binfo;	
	HDC hDCx;
	char *bitvals;
	int curitem;

	DeleteObject(bigbitty);

	curitem = ListView_GetNextItem(GetDlgItem(packdlg,IDC_LIZZIE), -1, LVNI_ALL | LVNI_FOCUSED);

	bfz_img_read(fathy,items[curitem].resid,&bigimage);

	memset(&binfo,0,sizeof(BITMAPINFO));
	binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	binfo.bmiHeader.biWidth = bigimage.width;
	binfo.bmiHeader.biHeight = bigimage.height;
	binfo.bmiHeader.biBitCount = 24;	// 24-bit bitmap
	binfo.bmiHeader.biPlanes = 1; // Must be set to one
	binfo.bmiHeader.biCompression = BI_RGB;	
	
	hDCx = GetDC(imghwnd);

	bigbitty = CreateDIBSection(hDCx,&binfo,DIB_RGB_COLORS,(void**)&bitvals,NULL,NULL);

	bfx_blit(bitvals,&bigimage);

	free(bigimage.data_ptr);	// free image data
	ReleaseDC(imghwnd,hDCx);

	// resize the window to occupy the image
	RECT rizzle;

	fsd_mode = 1;

	GetWindowRect(imghwnd,&rizzle);
	MoveWindow(imghwnd,rizzle.left,rizzle.top, bigimage.width + diffx, bigimage.height + diffy, FALSE);

	InvalidateRect(imghwnd,NULL,TRUE);
	PostMessage(imghwnd,WM_NCPAINT,0,0);
	RedrawWindow(imghwnd,NULL,NULL,RDW_INTERNALPAINT | RDW_UPDATENOW);

	return 0;
}

int imgview_show_256win() {

	RECT rizzle;

	GetWindowRect(imghwnd,&rizzle);
	MoveWindow(imghwnd,rizzle.left,rizzle.top, 256 + diffx, 256 + diffy, FALSE);

	return 0;
}

int timer_reset() {
	KillTimer(imghwnd,TIMEY);
	SetTimer(imghwnd,TIMEY,1000,NULL);	
	return 0;
}