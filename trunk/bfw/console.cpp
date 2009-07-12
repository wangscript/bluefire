/*

  Bluefire: Console routines

  (c) Copyright 2004 Jacob Hipps

*/

#define BLUEFIRE_CORE

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bluefire.h"
#include "resource.h"

extern HINSTANCE hBluefireDLL;

HWND consolehand;
HFONT blahfont;
HBITMAP hbitty;

BOOL CALLBACK ConsoleDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

#define MAX_BUFF	8192

char conbuff[MAX_BUFF];
char cmdbuff[256];
int  buffill;
extern void (__cdecl * go_down)();	// go_down

int init_console() {
	BEGIN_FUNC("init_console")

	buffill = 0;		
	blahfont = CreateFont(16, 0 /*width*/, 0, 0, FW_NORMAL /*font weight*/, FALSE /*italic, underline, strikeout*/, FALSE, FALSE, ANSI_CHARSET /*charset*/, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE /*family*/ | DEFAULT_PITCH /*pitch*/, "Courier New");
	consolehand = CreateDialog(hBluefireDLL, MAKEINTRESOURCE(IDD_CONSOLEX), NULL, ConsoleDlg);

	END_FUNC
	return 0;
}

int kill_console() {
	BEGIN_FUNC("kill_console")

	EndDialog(consolehand,TRUE);
	DeleteObject(blahfont);

	END_FUNC
	return 0;
}

void add_text(char *txtx) {
	BEGIN_FUNC("add_text")

	buffill += strlen(txtx);
	if(buffill > 8192) {
		conbuff[0] = 0;
		buffill = strlen(txtx);
	}
	int zig = strlen(txtx);

	char text[4096];
	char tempbuff[4096];
	strcpy(text,txtx);

	// replace newlines with CR+NL for stupid Windows
	for(int i = 0; i < zig; i++) {
		if(text[i] == '\n') {
			text[i] = '\r';			
			memcpy(tempbuff, text + i + 1, strlen(text) - i);			
			memcpy(text + i + 2, tempbuff, strlen(text) - i);
			text[i+1] = '\n';
			buffill++;
			i++;
			zig++;
		}
	}
	
	strcat(conbuff,text);

	SetDlgItemText(consolehand,IDC_TXTLOG,conbuff);

	//SendDlgItemMessage(consolehand,IDC_TXTLOG,EM_SETSEL,strlen(conbuff),strlen(conbuff));

	int lines = SendDlgItemMessage(consolehand,IDC_TXTLOG,EM_GETLINECOUNT,0,0);

	SendDlgItemMessage(consolehand,IDC_TXTLOG,EM_LINESCROLL,0,lines);

	RedrawWindow(GetDlgItem(consolehand,IDC_TXTLOG),NULL,NULL,RDW_UPDATENOW);

	END_FUNC
	return;
}


BOOL CALLBACK ConsoleDlg(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {

	RECT blah;	
	HDC hDC, hDCx;		

	switch(msg) {
		case WM_INITDIALOG:

			ShowWindow(hDlg, SW_SHOW);
			SetFocus(hDlg);			

			SendDlgItemMessage(hDlg,IDC_TXTLOG,WM_SETFONT,(WPARAM)blahfont,FALSE);
			SendMessage(hDlg,DM_SETDEFID,IDOK,0);
			zlogthis("ConsoleDlg: WM_INITDIALOG\n");
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDOK:
					GetDlgItemText(hDlg,IDC_COMMANDTXT,cmdbuff,256);
					execute_command(cmdbuff);
					cmdbuff[0] = 0;
					SetDlgItemText(hDlg,IDC_COMMANDTXT,cmdbuff);					
					SetFocus(GetDlgItem(hDlg,IDC_COMMANDTXT));
					break;
				case IDCLOSE:
					EndDialog(hDlg,TRUE);
					break;
			}
			break;
		case WM_DESTROY:
			zlogthis("ConsoleDlg: WM_DESTROY\n");
			EndDialog(hDlg,TRUE);
			break;
		case WM_CLOSE:
			// do nothing
			break;
		case WM_SIZE:
			GetClientRect(hDlg,&blah);
			blah.bottom -= 10;

			// log			
			MoveWindow(GetDlgItem(hDlg,IDC_TXTLOG), 7, 7, blah.right - 7, blah.bottom - 33, TRUE);
			// cmd
			MoveWindow(GetDlgItem(hDlg,IDC_COMMANDTXT), 7, blah.bottom - 20, blah.right - 78, 23, TRUE);
			// execute
			MoveWindow(GetDlgItem(hDlg,IDOK), blah.right - 57, blah.bottom - 21, 50, 24, TRUE);
			break;
		default:
			return FALSE;
	}


	return TRUE;
}

int execute_command(char *cmdtxt) {
	BEGIN_FUNC("execute_command")

	char filenamex[256];
	char filenamez[256];
	extern HMODULE hRenderDLL;

	int x, z;

	if(!strcmp(cmdtxt,"info")) {
		GetModuleFileName(hRenderDLL,filenamex,256);
		GetModuleFileName(NULL,filenamez,256);
		zlogthis("-----------------------------------------------\n"
				 "Bluefire Engine. [Winchester/MDL Core]\n"
				 "Build time %s @ %s\n"
				 "Copyright (c) 2003-2009 Jacob Hipps\n"
				 "\n"
				 "Render DLL: %s\n"
				 "Host Process: %s\n"
				 "\n"
				 ,__DATE__,__TIME__,filenamex,filenamez
				);
	} else if (!strcmp(cmdtxt,"crash")) {
		zlogthis("--------------------\n"
				 "Doing an illegal memory access to test exception handler.\n\n"
				 );
		z = (int)0x0FFFEcafe;
		memcpy((char*)x,(char*)z,1024);
		x = (int)hRenderDLL;
	} else if (!strcmp(cmdtxt,"exit")) {
		go_down();
	} else {
		zlogthis("Command not recognized. \"%s\"\n",cmdtxt);
	}

	END_FUNC
	return 0;
}