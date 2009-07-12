/*

	Bluefire: Window Routines (Win32)
	(c) Copyright 2009 Jacob Hipps/Neo-Retro Group
	http://bluefire.neoretro.net/spec/bfw/window_win32

	Win32 API Version
	Linux/X Window API soon to come ;)

	Implements:
		bf_create_window()	- Creates a window
		bf_kill_window()	- Closes a window created by bf_create_window()

*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bluefire.h"

extern void (__cdecl * go_down)();
extern BF_EXPORTZ z_config mainconfig;
extern BF_EXPORTZ ESM_RENDERZ mainscr;

/***

bf_create_window()

	Description:
		Creates a window with the specified parameters.

	Arguments:
		char *title		Title/Caption of window
		int xpos			X position of top-left corner of the window
		int ypos			Y position of ...
		int width			Width of window
		int height			Height of window
		WNDCLASSEX *winclx	Pointer to window class description
		HWND parent			Handle of parent window (if applies)

	Return:
		handle of the newly created window (HWND)

	Status:
		Working, Level A
	To-do:
		Make platform independent... add routines for X (linux and macOS)

***/
HWND bf_create_window(char title[], int xpos, int ypos, int fullscreen, int width, int height, WNDCLASSEX *winclx, HWND parent) {	
	BEGIN_FUNC("bf_create_window")

	HWND bowlman;

	if(fullscreen) {
		// Fullscreen		
		bowlman =	CreateWindowEx(
				WS_EX_APPWINDOW,      /* use this, eh? */
				winclx->lpszClassName,         /* Classname */
				title,					/* Title Text */
				WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, /*  WS_OVERLAPPEDWINDOW is normal, this is for fullscreen */
				CW_USEDEFAULT,       /* position dosen't matter, it's fullscreen */
				CW_USEDEFAULT,
				width, 
				height,
				HWND_DESKTOP,        /* The window is a child to desktop */
				NULL,                /* No menu */
				winclx->hInstance,	 /* Program Instance handler */
				NULL                 /* No Window Creation data */
				);

		DEVMODE dmScreenSettings; // Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings)); // Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings); // Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = width; // Selected Screen Width
		dmScreenSettings.dmPelsHeight = height; // Selected Screen Height
		dmScreenSettings.dmBitsPerPel = mainconfig.bpp; // Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings, 0); // put us in fullscreen!
	} else {
		// Windowed
		DWORD valz;

		if(parent == NULL || parent == GetDesktopWindow() || parent == HWND_DESKTOP) {
			zlogthis("Creating Parent Window!\n");
			valz = WS_OVERLAPPEDWINDOW;
		} else {
			zlogthis("Creating Child Window!\n");
			valz = WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CHILD | WS_VISIBLE | WS_CAPTION;
		}

		if((bowlman = CreateWindow(winclx->lpszClassName,title,valz, xpos, ypos, width, height, parent, NULL, winclx->hInstance, NULL )) == NULL) {
			char fifoz[32];
			sprintf(fifoz, "CreateWindow error: ERR #%i",GetLastError());
			MessageBox(NULL,fifoz,"error",MB_SYSTEMMODAL);
			exit(GetLastError());
		}
	}

	END_FUNC
	return bowlman;
}



/***

bf_kill_window()

	Description:
		Closes a window created by bf_create_window()

	Arguments:
		HWND winz			Handle of window
		WNDCLASSEX *winclz	Window class information used to create the window

	Return:
		none

	Status:
		Working, Level A
	To-do:
		Add support for X

***/
void bf_kill_window(HWND winz, WNDCLASSEX *winclz) {
	BEGIN_FUNC("bf_kill_window")

	//DestroyWindow(winz);
	UnregisterClass(winclz->lpszClassName, (HINSTANCE)winclz->hInstance);

	END_FUNC
	return;
}
