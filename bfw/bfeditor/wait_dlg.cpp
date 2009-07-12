/*

  Bluefire Editor: "Working" (Wait) Dialog functions and such

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

// functions
void waitdlg_settext(char *intext);
void waitdlg_setprog(int prog);
void waitdlg_init(char *caption);
void waitdlg_kill();
BOOL CALLBACK WaitDlg(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

HWND waitdlg;

void waitdlg_settext(char *intext) {
	SetDlgItemText(waitdlg, IDC_TEXTSTUFF, intext);
	RedrawWindow(waitdlg,NULL,NULL,RDW_INTERNALPAINT | RDW_UPDATENOW);
	return;
}

// set progress
// range = [0,100]
void waitdlg_setprog(int prog) {
	SendDlgItemMessage(waitdlg, IDC_PROGRESSX, PBM_SETPOS, prog, 0);	
	//RedrawWindow(GetDlgItem(waitdlg,IDC_PROGRESSX),NULL,NULL,RDW_INTERNALPAINT | RDW_UPDATENOW);
	return;
}

void waitdlg_kill() {	
	EndDialog(waitdlg,TRUE);
	return;
}

void waitdlg_init(char *caption) {
	waitdlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_WORKING), bigwin, WaitDlg);
	SetWindowText(waitdlg,caption);
	return;
}

BOOL CALLBACK WaitDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch( msg ) {

		case WM_INITDIALOG:									
			ShowWindow(hDlg, SW_SHOW);
			SetFocus(hDlg);
			ShowWindow(GetDlgItem(hDlg,IDC_PROGRESSX),SW_SHOW);
			zlogthis("PackViewDlg: WM_INITDIALOG\n");

			break;
		
		case WM_DESTROY:
			EndDialog(hDlg, TRUE);			
            break;

		case WM_COMMAND:

			switch (LOWORD(wParam))
			{
				case IDABORT:
				case IDCANCEL:
				case IDOK:
					// haha, too bad, you can't abort just yet...
					//ShowWindow(hDlg,SW_HIDE);
					break;
			}
			break;
		default:
			return FALSE;
    }

	return TRUE;
}

