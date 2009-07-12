/*

  Bluefire Editor: Main functions

  (c) Copyright 2004-2006 Jacob Hipps

  http://bluefire.neoretro.net/

*/

// disable 'integer-float conversion: possible loss of data' warning
#pragma warning(disable:4244)

#define BLUEFIRE_CLNT
#define _BF_MAIN

#define BUFFER_ZONE		1	// offset from center of main window to allow
							// for the partition bars


#include <windows.h>
#include "commctrl.h"		// ms common controls
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "resource.h"
#include "../bluefire.h"
#include <signal.h>


// globals
HWND bigwin;				// main window handle
WNDCLASSEX wincl;			// window class info
bool keys[256];
char packs[256][256];
char xtext[256];

int focal, winsok;
HCURSOR pcursor;			// old cursor
HACCEL master_accel;
HINSTANCE hInst;

BF_FT_FONT fizont;			// default font
BF_FT_FONT suckfont;

HCURSOR move_cursor, sel_cursor, latrot_cursor;

typedef struct {
	HWND hwnd;
	ESM_RENDERZ renderz;
	WNDCLASSEX wincl;
	int x_init, x_final, y_init, y_final;
	float t_init;
	float main_yaw, main_pit, main_rot, main_x, main_y, main_z;
	int raster_mode;
	int last_x, last_y;
	unsigned int  vp1, vp2, vp3, vp4;
	unsigned long tid;
	bool isOrtho;
} BF_WN_CLNT;

BF_WN_CLNT winclients[4];


POINT mc_x;

int npmove;					// set this to 1 when the app explicitly moves the cursor.
int cx_var, cy_var;			// center x,y offset

int sel_polys[1024];
int num_selected;

int shift_down;

int sz_breakout, fizzie;

int testimg; //debug
int zzsel_mode = 0;

int enable_lmaps;
int enable_lights;
int enable_dsort;
int enable_normals;
int enable_radblur;

extern HWND packdlg;
extern HWND surfprop;
extern HWND objprop;
extern HWND itemdlg;
extern HWND editdlg;

// external funcs
extern int add_light_source();		// lighting.cpp
extern int init_liteprop();			// lighting.cpp
extern void lite_chgsel();			// lighting.cpp
extern int init_packview();			// packview.cpp
extern int init_imgview();			// imgview.cpp
extern int init_surfprop();			// surfprop.cpp
extern void sp_chg_selection();		// surfprop.cpp
extern int init_itemview();			// itemview.cpp
extern int init_objprop();			// objedit_dlg.cpp
extern void obj_chgsel();			// objedit_dlg.cpp
extern int brush_import(char *filez, char *resid);			// brush.cpp
extern int bfe_fix_normals(int num);						// util.cpp
extern int bfe_fix_vtex_rot(int num);						// util.cpp
extern int repopulatex(HWND hDlg);	// itemview.cpp
extern int tex_align();				// surfprop.cpp
extern int tex_autofit();			// surfprop.cpp
extern int tex_sel_coplanar();		// surfprop.cpp


// wait dialog stuffs
extern void waitdlg_settext(char *intext);
extern void waitdlg_setprog(int prog);
extern void waitdlg_init(char *caption);
extern void waitdlg_kill();

int fs_mode = -1;
int xdown = -1;

// local functions
void build_viewports();
void go_down();
int demo_init();
void demo_main(int dex);
void render_main(BF_WN_CLNT *winclientx);
LRESULT WINAPI WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int hit_test(int dex, int sel_mode);
void deselect_all();
void flip_sel_normals();
void rotz_sel_normals();

const float piover180 = 3.14159f / 180.0f;		// approximation of pi/180

void stupid(int sig);
void draw_actors();
void toggle_selmode(int zzmode);


// exception handler
void stupid(int sig) {	

	// call BFW's exception handler
	exception(sig);

	return;
}

// context menu stuffs
HMENU hPopup;
HMENU hPopupTrack;


void activate_viewport(int num) {
	
	RECT katkat;

	if(fs_mode != -1) {
		GetClientRect(bigwin,&katkat);
		bf_activate_context_x(&winclients[num].renderz);
		//bf_resize_disp(katkat.left,katkat.bottom,bigwin);
		bf_set_viewport(0,0,katkat.right,katkat.bottom, bigwin);
	} else {
		bf_activate_context_x(&winclients[num].renderz);
		bf_set_viewport(winclients[num].vp1,winclients[num].vp2,winclients[num].vp3,winclients[num].vp4, bigwin);	
	}

	return;
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	hInst = hInstance;

	bf_init(go_down);			// initialize Bluefire

	// setup exception handling
	signal(SIGINT,stupid);
	signal(SIGILL,stupid);
	signal(SIGFPE,stupid);
	signal(SIGSEGV,stupid);
	signal(SIGTERM,stupid);
	signal(SIGBREAK,stupid);
	signal(SIGABRT,stupid);

	BEGIN_FUNC("WinMain")

	focal = 0;
	npmove = 0;
	winsok = 0;
	sz_breakout = 0;
	cx_var = 0;
	cy_var = 0;
	fizzie = 0;
	num_selected = 0;

	enable_lights = 0;
	enable_lmaps = 0;
	enable_dsort = 0;
	enable_radblur = 0;
	enable_normals = 0;

	mainconfig.fullscreen = 0;	// enable fullscreen by default?
	mainconfig.bpp = 32;		// bits per pixel
	mainconfig.width = 1024;
	mainconfig.height = 768;
	mainconfig.blursteps = 10;	// number of steps in radial blur
								// raising this above 25 severly hinders performance


	master_accel = LoadAccelerators(hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// Cursors

	move_cursor = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_MOVECUR));
	sel_cursor = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_SELCUR));
	latrot_cursor = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_LATROTCUR));


    // The Window structure
	char classnomen[] =  "BFEDITOR";
    wincl.hInstance		= hInstance;
	wincl.lpszClassName = classnomen;
    wincl.lpfnWndProc	= WindowProcedure;
    wincl.style			= CS_OWNDC;
    wincl.cbSize		= sizeof(WNDCLASSEX);

    // Use default icon and mouse-pointer
    wincl.hIcon   = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wincl.hIconSm = NULL;
    wincl.hCursor = LoadCursor(hInstance, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    wincl.cbClsExtra = 0;                      // No extra bytes after the window class
    wincl.cbWndExtra = 0;                      // structure or the window instance
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND; //GetStockObject(WHITE_BRUSH);		// black BG
	
    // Register the window class; if fail, quit the program
    if(!RegisterClassEx(&wincl)) {
		MessageBox(NULL,"Window class registration failed!","BFW Critical Error",0);
		return 255;
	}

	bf_bind_render_dll("render_ogl\\render_ogl.dll"); // bind to the Render DLL

	zlogthisx("Binding to MSCommCtrl DLL... ");
	InitCommonControls();
	zlogthisx("ok.");

	// create the window now!
	char windowtitle[128];
	sprintf(windowtitle,"Bluefire Editor [test build %s]",__DATE__);
	
	bigwin = bf_create_window(windowtitle, 100, 100, 0, mainconfig.width, mainconfig.height, &wincl, GetDesktopWindow());

    zlogthisx("window created successfully. fullscreen = %i, %ix%ix%ibpp",mainconfig.fullscreen,mainconfig.width,mainconfig.height,mainconfig.bpp);

	// build the client windows

	fs_mode = 0; // <--- set default viewing mode as Single DL

	winclients[0].raster_mode = BF_FILL;	
	winclients[0].main_x = 0.8f;
	winclients[0].main_y = 5.54f;
	winclients[0].main_z = -11.28f;
	winclients[0].main_rot = -41.6f;
	winclients[0].isOrtho = false;

	winclients[2].raster_mode = BF_WIREFRAME;
	winclients[2].main_y = -12.0f;
	winclients[2].main_rot = 180.0f;
	winclients[2].main_pit = 0.0f;
	winclients[2].main_yaw = 0.0f;
	winclients[2].isOrtho = true;

	winclients[1].raster_mode = BF_WIREFRAME;
	winclients[1].main_y = -12.0f;
	winclients[1].main_y = -12.0f;
	winclients[1].main_rot = 180.0f;
	winclients[1].main_pit = 90.0f;
	winclients[1].main_yaw = 0.0f;
	winclients[1].isOrtho = true;

	winclients[3].raster_mode = BF_FILL;
	winclients[3].isOrtho = false;
	
	ShowWindow(bigwin,SW_HIDE); // hide the window until initialization is complete!
	
	

	// write some info to the logfile...
    zlogthis("Bluefire Editor. [test build %s @ %s]\n"
			 "Copyright (c) 2004-2009 Jacob Hipps, all rights reserved.\n"
			 ,__DATE__, __TIME__);


	//demo_init();

	bf_activate_context(&winclients[0].renderz);
	build_viewports();
	bf_init_3d(bigwin);
	
	winclients[1].renderz.hDC = winclients[0].renderz.hDC;
	winclients[2].renderz.hDC = winclients[0].renderz.hDC;
	winclients[3].renderz.hDC = winclients[0].renderz.hDC;


	init_packview();
	//init_imgview();
	init_surfprop();
	init_objprop();
	init_itemview();
	init_liteprop();

	ShowWindow(packdlg,SW_HIDE);
	ShowWindow(surfprop,SW_HIDE);
	ShowWindow(objprop,SW_HIDE);
	ShowWindow(itemdlg,SW_HIDE);
	ShowWindow(editdlg,SW_HIDE);

	bf_script_ft_clear();

	BFZ_FILE bfzfile;	
	
	bfz_open("testdata.bfz",&bfzfile);
	//bfz_loadobj(&bfzfile,"ttg_logo",bf_addobject());
	bfz_loadobj(&bfzfile,"nr_logo",bf_addobject());
	//bfz_loadimg(&bfzfile,"dero",bf_addimg());
	bfz_loadexec(&bfzfile,"xcore");
	bfz_close(&bfzfile);

	

    // test script functions...
	zlogthis("TESTING SCRIPT FUNCTIONALITY...\n");
	bfi_exec_script(bf_script_getnum("testdata:xcore"), "onProxy");
	bfi_kill();
    zlogthis("---- END TEST ----\n");


	// load the environment from object data
	brush_import("testdata.bfz", "main_bldg");
	//brush_import("city.bfz","city");

	//bfz_open("city.bfz",&bfzfile);
	//bfz_loadobj(&bfzfile,"city",bf_addobject());
	//bfz_close(&bfzfile);

	bf_set_radblur(0); // <<<<<<<< disable radial blur
	bf_getobject(bf_getobj_num("testdata:nr_logo"))->bEnabled = true;
	
	// initialize radial blur capabilities
	bf_init_radblur(bf_addimg());
	
	// enable radial blur for neo-retro logo
	/*
	bf_getobject(bf_getobj_num("testdata:nr_logo"))->bRadBlur = true;
	bf_getobject(bf_getobj_num("testdata:nr_logo"))->surfdex[0].cr = 0.2f;
	bf_getobject(bf_getobj_num("testdata:nr_logo"))->surfdex[0].cg = 0.2f;
	bf_getobject(bf_getobj_num("testdata:nr_logo"))->surfdex[0].cb = 0.2f;
	bf_getobject(bf_getobj_num("testdata:nr_logo"))->surfdex[0].trans = 0.8f;
	*/

	// send default options to rendering plugin
	bf_set_radblur(enable_radblur);
	bf_set_shownormals(enable_normals);

	// font stuff

	
	bf_ft_init();
	//bf_ft_open("c:\\windows\\fonts\\gothic.ttf", &fizont);
	//bf_ft_rasterize(&fizont, 32);
	bf_ft_addfont("bennb.ttf", 32);
	//bf_ft_dlist(&fizont, 32, 128, 0);	// generate dlist
	

	bf_ttf_font("Arial", 32, 96, 16, &fizont);

	/*
	testimg = bf_addimg();

	bf_getimg(testimg)->tex_id = fizont.tex_num;
	*/

	// main point light
	/*
	BF_LIGHT *mylite = bf_getlight(bf_addlight());
	mylite->lumens = 30;			// radius
	mylite->r = 1.00f;				// color
	mylite->g = 0.60f;
	mylite->b = 0.15f;
	mylite->a = 0.80f;				// intensity
	mylite->l_type = BF_LIGHT_POINT; // point light
	mylite->x = -10.0f;				// world position
	mylite->y = 50.0f;
	mylite->z = -30.0f;
	mylite->inf = 1.0f;				// finite
	mylite->bEnabled = true;
	strcpy(mylite->resid,"main_light");
	bf_update_light(mylite);	// use this as the current OpenGL hardware light
	*/

	// main point light
	BF_LIGHT *mylite = bf_getlight(bf_addlight());
	mylite->lumens = 10;			// radius
	mylite->r = 1.00f;				// color
	mylite->g = 1.00f;
	mylite->b = 1.00f;
	mylite->a = 0.50f;				// intensity
	mylite->l_type = BF_LIGHT_POINT; // point light
	mylite->x = 0.0f;				// world position
	mylite->y = 0.0f;
	mylite->z = 0.0f;
	mylite->inf = 1.0f;				// finite
	mylite->bEnabled = true;
	strcpy(mylite->resid,"main_light");
	bf_update_light(mylite);	// use this as the current OpenGL hardware light
	

	// global ambient light
	mylite = bf_getlight(bf_addlight());
	mylite->r = 1.00f;	// color
	mylite->g = 1.00f;
	mylite->b = 1.00f;
	mylite->a = 0.10f;	// global intensity
	mylite->l_type = BF_LIGHT_AMBIENT;
	mylite->inf = 0.0f; // infinite
	mylite->bEnabled = true;
	strcpy(mylite->resid,"ambient_light");
	
	// set up popup context menu
	hPopup = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MAINPOP));
	hPopupTrack = GetSubMenu(hPopup, 0); 

	// initialize timer
	bf_init_timer();
	zlogthisx("Timer ready.");

	ShowWindow(bigwin,SW_SHOW); // show the window	
	SetCursor(sel_cursor);

	/*
	zlogthis("caclulating lightmaps...\n");
	waitdlg_init("Calculating Lightmaps...");
	char temptext[256];
	for(int i = 0; i < bf_poly_cnt(); i++) {
		waitdlg_setprog(int((float(i+1)/float(bf_poly_cnt())) * 100.0f));
		sprintf(temptext,"Raytracing polygon %i/%i...",i+1,bf_poly_cnt());
		waitdlg_settext(temptext);
		bf_gen_lmap(bf_getpoly(i), i);
	}
	waitdlg_kill();
	zlogthis("lightmap generation complete.\n");
	if(!(winclients[0].renderz.flags & BF_RZ_LMAPS_ENABLED)) {
		winclients[0].renderz.flags += BF_RZ_LMAPS_ENABLED;
		enable_lmaps = 1;
	}
	*/

	// We are ready to render, nig
	bf_set_ready(1);
	repopulatex(editdlg); // refresh the actor browser

	zlogthis("We ready! Going into main loop.\n");
	//sprintf(xtext,"Bluefire Rendering Engine - Version %s, build %s (%s)",bf_get_version(),bf_get_build(),bf_get_buildtime());
	sprintf(xtext,"Bluefire Rendering Engine. (c) Copyright 2003-2009 Jacob Hipps");


	// Go into the main message dispatcher loop
    MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    while( msg.message != WM_QUIT ) {                      
		   TranslateAccelerator(bigwin,master_accel, &msg);
           if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {                            
              TranslateMessage( &msg );
              DispatchMessage( &msg );
           } else {
				//Sleep(2000);
		   } 
    }

	zlogthisx("Shutdown via WinMain.");
    go_down();

    return msg.wParam;
}


LRESULT WINAPI WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	int i, posx, posy, wln, wht;
	RECT rectizzle;
	HWND waitdlg;
	PAINTSTRUCT painter;
	HDC hDC, hDCx;
	char temptext[256];
	int xcoord, ycoord;
	int zonetouch = -1;
	int ztmode = -1;
	char zazzy[512];

	RECT wirect, rectz, kirect;
	POINT mc_x_old;

	// determine which viewport the cursor is over
	
	if(message == WM_RBUTTONDOWN || message == WM_LBUTTONDOWN || message == WM_RBUTTONUP || message == WM_LBUTTONUP || (message == WM_MOUSEMOVE && xdown != -1)) {		
		memcpy(&mc_x_old,&mc_x,sizeof(POINT));
		GetWindowRect(hwnd,&wirect);
		GetClientRect(hwnd,&rectz);
		GetCursorPos((POINT*)&mc_x);
		xcoord = mc_x.x - wirect.left - (((wirect.right - wirect.left) - rectz.right) / 2);
		ycoord = mc_x.y - wirect.top - (((wirect.bottom - wirect.top) - rectz.bottom) / 2);

		if(fs_mode != -1) {
			zonetouch = fs_mode;
			ztmode = winclients[fs_mode].raster_mode;
		} else {
			for(i = 0; i < 4; i++) {
				if(xcoord > winclients[i].vp1 && xcoord < (winclients[i].vp3 - winclients[i].vp1) && ycoord > winclients[i].vp2 && ycoord < (winclients[i].vp4 - winclients[i].vp2)) {
					zonetouch = i;
					ztmode = winclients[i].raster_mode;
					break;
				}
			}
		}
		
		//zlogthis("zonetouch = %i, ztmode = %i, fs_mode = %i\n",zonetouch, ztmode, fs_mode);

		memcpy(&mc_x,&mc_x_old,sizeof(POINT));

	}


	if(xdown != -1) {
		zonetouch = xdown;
		ztmode = winclients[zonetouch].raster_mode;
	}

	//<<<<<<<<<<<<<<
	zonetouch = 0;
	ztmode = BF_FILL;

	if(ztmode == BF_FILL) {

		// Perspective

		switch (message)
		{
		   case WM_RBUTTONDOWN:

				GetWindowRect(hwnd,&wirect);
				GetClientRect(hwnd,&rectz);

				if(!(wParam & MK_CONTROL)) {
					winclients[zonetouch].x_init = LOWORD(lParam);
					winclients[zonetouch].y_init = HIWORD(lParam);
					SetCapture(hwnd);
					GetCursorPos((POINT*)&mc_x);
					/*
					if(fs_mode != -1) {
						kirect.top = wirect.top + winclients[zonetouch].vp2 + 30;
						kirect.bottom = wirect.top + winclients[zonetouch].vp2 + winclients[zonetouch].vp4 - 30;
						kirect.left = wirect.left + winclients[zonetouch].vp1 + 30;
						kirect.right = wirect.left + winclients[zonetouch].vp1 + winclients[zonetouch].vp3 - 30;
					} else {
						kirect.top = wirect.top + 30;
						kirect.bottom = wirect.bottom - 30;
						kirect.left = wirect.left + 30;
						kirect.right = wirect.right - 30;						
					}
					*/
					kirect.top = wirect.top + 50;
					kirect.bottom = wirect.bottom - 30;
					kirect.left = wirect.left + 30;
					kirect.right = wirect.right - 30;

					ClipCursor(&kirect);
					//pcursor = SetCursor(NULL);
					pcursor = SetCursor(latrot_cursor);

				}

				xdown = zonetouch;
				return 0;
		   case WM_LBUTTONDOWN:

				GetWindowRect(hwnd,&wirect);
				GetClientRect(hwnd,&rectz);

				if(wParam & MK_CONTROL) {
					winclients[zonetouch].last_x = mc_x.x - wirect.left - (((wirect.right - wirect.left) - rectz.right) / 2);
					winclients[zonetouch].last_y = mc_x.y - wirect.top - (((wirect.bottom - wirect.top) - rectz.bottom) / 2);
					hit_test(zonetouch, zzsel_mode);
					if(zzsel_mode == 0)	sp_chg_selection();
					else				obj_chgsel();
				} else {
					winclients[zonetouch].x_init = LOWORD(lParam);
					winclients[zonetouch].y_init = HIWORD(lParam);
					SetCapture(hwnd);
					GetCursorPos((POINT*)&mc_x);
					/*
					if(fs_mode != -1) {
						kirect.top = wirect.top + winclients[zonetouch].vp2 + 30;
						kirect.bottom = wirect.top + winclients[zonetouch].vp2 + winclients[zonetouch].vp4 - 30;
						kirect.left = wirect.left + winclients[zonetouch].vp1 + 30;
						kirect.right = wirect.left + winclients[zonetouch].vp1 + winclients[zonetouch].vp3 - 30;
					} else {
						kirect.top = wirect.top + 30;
						kirect.bottom = wirect.bottom - 30;
						kirect.left = wirect.left + 30;
						kirect.right = wirect.right - 30;						
					}
					*/
					kirect.top = wirect.top + 50;
					kirect.bottom = wirect.bottom - 30;
					kirect.left = wirect.left + 30;
					kirect.right = wirect.right - 30;

					ClipCursor(&kirect);
					//pcursor = SetCursor(NULL);
					pcursor = SetCursor(move_cursor);

					//sprintf(zazzy,"mc_x = (%i,%i)\nkirect(TBLR) = [%i,%i,%i,%i]",mc_x.x,mc_x.y,kirect.top,kirect.bottom,kirect.left,kirect.right);
					//MessageBox(NULL,zazzy,"nig",0);

				}

				xdown = zonetouch;

				return 0;
		   case WM_MOUSEMOVE:
				if(winclients[zonetouch].x_init >= 0 && !npmove) {
					winclients[zonetouch].x_final = LOWORD(lParam);
					winclients[zonetouch].y_final = HIWORD(lParam);
					// L button (fwd/back, lateral left/rt)
					if((wParam & MK_LBUTTON) && !(wParam & MK_RBUTTON) && !(wParam & MK_SHIFT)) {
						//winclients[zonetouch].main_z += cos(winclients[zonetouch].main_rot * piover180) * float(winclients[zonetouch].y_final - winclients[zonetouch].y_init) / (float(winclients[zonetouch].renderz.height) / 10.0f);
						//winclients[zonetouch].main_x += sin(winclients[zonetouch].main_rot * piover180) * float(winclients[zonetouch].x_final - winclients[zonetouch].x_init) / (float(winclients[zonetouch].renderz.width)  / 10.0f);
						winclients[zonetouch].main_z += cos(winclients[zonetouch].main_rot * piover180) * (float(winclients[zonetouch].y_final - winclients[zonetouch].y_init) / (float(winclients[zonetouch].renderz.height) * 0.4f));
						winclients[zonetouch].main_x -= sin(winclients[zonetouch].main_rot * piover180) * (float(winclients[zonetouch].y_final - winclients[zonetouch].y_init) / (float(winclients[zonetouch].renderz.height) * 0.4f));
						winclients[zonetouch].main_z -= cos((winclients[zonetouch].main_rot * piover180) + 1.570795f) * (float(winclients[zonetouch].x_final - winclients[zonetouch].x_init) / (float(winclients[zonetouch].renderz.width) * 0.4f));
						winclients[zonetouch].main_x += sin((winclients[zonetouch].main_rot * piover180) + 1.570795f) * (float(winclients[zonetouch].x_final - winclients[zonetouch].x_init) / (float(winclients[zonetouch].renderz.width) * 0.4f));
					// R button (rotation, pitch)
					} else if ((wParam & MK_RBUTTON) && !(wParam & MK_LBUTTON)) {
						if(!winclients[zonetouch].isOrtho) {
							//winclients[dex].main_yaw -= abs((4 * winclients[dex].main_rot) / 360) * float(winclients[dex].y_final - winclients[dex].y_init) / (float(winclients[dex].renderz.height) / 10.0f);
							//winclients[dex].main_pit += abs((4 * (winclients[dex].main_rot - 90)) / 360) * float(winclients[dex].y_final - winclients[dex].y_init) / (float(winclients[dex].renderz.height) / 10.0f);

							winclients[zonetouch].main_rot += float(winclients[zonetouch].x_final - winclients[zonetouch].x_init) / (float(winclients[zonetouch].renderz.width)  / 12.0f);
						}
					// L button+shift (up/down vertical)
					} else if ((wParam & MK_LBUTTON) && (wParam & MK_SHIFT)) {
						winclients[zonetouch].main_y -= float(winclients[zonetouch].y_final - winclients[zonetouch].y_init) / (float(winclients[zonetouch].renderz.height) / 4.0f);
					}

					npmove = 1;
					//sprintf(zazzy,"mc_x = (%i,%i)\nkirect(TBLR) = [%i,%i,%i,%i]",mc_x.x,mc_x.y,wirect.top,wirect.bottom,wirect.left,wirect.right);
					//MessageBox(NULL,zazzy,"nig",0);
					//SetCursorPos(mc_x.x,mc_x.y);
					//sprintf(zazzy,"zonetouch = %i, (x_init = %i, y_init = %i), (x_final = %i, y_final = %i)\n",zonetouch,winclients[zonetouch].x_init,winclients[zonetouch].y_init,winclients[zonetouch].x_final,winclients[zonetouch].y_final);
					//MessageBox(NULL,zazzy,"nig",0);

					//winclients[zonetouch].x_init = LOWORD(lParam);
					//winclients[zonetouch].y_init = HIWORD(lParam);

					demo_main(zonetouch);
				} else {
					npmove = 0;
				}
				return 0;

		   case WM_RBUTTONUP:
		   //case WM_CONTEXTMENU:
				if((wParam & MK_CONTROL)) {
					GetCursorPos(&mc_x);
					TrackPopupMenu(hPopupTrack, TPM_LEFTALIGN | TPM_RIGHTBUTTON, mc_x.x, mc_x.y, 0, bigwin, NULL);
				}
		   case WM_LBUTTONUP:
			    if(winclients[zonetouch].x_init != -1) {
					ReleaseCapture();
					winclients[zonetouch].x_init = -1; // signal that we are finished
					//SetCursorPos(mc_x.x,mc_x.y);
					kirect.left = winclients[zonetouch].vp1;
					kirect.top = winclients[zonetouch].vp1;
					kirect.right = winclients[zonetouch].vp1;
					kirect.bottom = winclients[zonetouch].vp1;
					ClipCursor(NULL);
					SetCursor(pcursor);
				}
				xdown = -1;
				return 0;
		   default:
			   xdown = -1;
			   break;
		}


	} else if (ztmode == BF_WIREFRAME) {

		// Ortho

		switch(message)
		{
  		   case WM_RBUTTONDOWN:		   				
		   case WM_LBUTTONDOWN:

				GetWindowRect(hwnd,&wirect);
				GetClientRect(hwnd,&rectz);

				if(wParam & MK_CONTROL) {
					winclients[zonetouch].last_x = mc_x.x - wirect.left - (((wirect.right - wirect.left) - rectz.right) / 2);
					winclients[zonetouch].last_y = mc_x.y - wirect.top - (((wirect.bottom - wirect.top) - rectz.bottom) / 2);
					hit_test(zonetouch, zzsel_mode);
				} else {
					winclients[zonetouch].x_init = LOWORD(lParam);
					winclients[zonetouch].y_init = HIWORD(lParam);
					SetCapture(hwnd);
					GetCursorPos(&mc_x);
					if(fs_mode != -1) {
						kirect.top = wirect.top + winclients[zonetouch].vp2 + 30;
						kirect.bottom = wirect.top + winclients[zonetouch].vp2 + winclients[zonetouch].vp4 - 30;
						kirect.left = wirect.left + winclients[zonetouch].vp1 + 30;
						kirect.right = wirect.left + winclients[zonetouch].vp1 + winclients[zonetouch].vp3 - 30;
					} else {
						kirect.top = wirect.top + 30;
						kirect.bottom = wirect.bottom - 30;
						kirect.left = wirect.left + 30;
						kirect.right = wirect.right - 30;
					}
					ClipCursor(&kirect);
					pcursor = SetCursor(NULL);
				}
				xdown = zonetouch;
				return 0;
		   case WM_MOUSEMOVE:
				if(winclients[zonetouch].x_init >= 0 && !npmove) {
					winclients[zonetouch].x_final = LOWORD(lParam);
					winclients[zonetouch].y_final = HIWORD(lParam);
					// L button (fwd/back, lateral left/rt)
					if((wParam & MK_LBUTTON) && !(wParam & MK_RBUTTON)) {				
						winclients[zonetouch].main_z += float(winclients[zonetouch].y_final - winclients[zonetouch].y_init) / (float(winclients[zonetouch].renderz.height) / 10.0f);
						winclients[zonetouch].main_x += float(winclients[zonetouch].x_final - winclients[zonetouch].x_init) / (float(winclients[zonetouch].renderz.width)  / 10.0f);						
					// L+R buttons (up/down vertical)
					} else if ((wParam & MK_LBUTTON) && (wParam & MK_RBUTTON)) {
						winclients[zonetouch].main_y += float(winclients[zonetouch].y_final - winclients[zonetouch].y_init) / (float(winclients[zonetouch].renderz.height) / 10.0f);
					}					
					npmove = 1;
					SetCursorPos(mc_x.x,mc_x.y);
					demo_main(zonetouch);
				} else {
					npmove = 0;
				}
				return 0;
		   case WM_RBUTTONUP:
		   case WM_LBUTTONUP:
				if(winclients[zonetouch].x_init != -1) {
					ReleaseCapture();
					SetCursorPos(mc_x.x,mc_x.y);
					ClipCursor(NULL);
					SetCursor(pcursor);
					winclients[zonetouch].x_init = -1; // signal that we are finished
				}
				xdown = -1;
				return 0;
		   default:
			   xdown = -1;
			   break;
		}
	}


    switch (message)
    {
			case WM_RBUTTONDOWN:		   				
			case WM_LBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_LBUTTONUP:
			case WM_MOUSEMOVE:
				// we've already processed these if they've occured
				return 0;
  		    case WM_SIZE:		// resize the window
			{
				bf_resize_disp(LOWORD(lParam),HIWORD(lParam),hwnd);

				if(winsok) {
					if(fs_mode != -1) {
						// obsolete
					} else {
						for(i = 0; i < 4; i++) {
							GetClientRect(bigwin,&rectizzle);

							if(i > 1) posx = (rectizzle.right / 2) + cx_var;
							else      posx = 0;
							if(i % 2) posy = 0;
							else      posy = (rectizzle.bottom / 2) + cy_var;

							if(posx == 0) {
								wln = (rectizzle.right / 2) + cx_var;
							} else {
								wln = (rectizzle.right / 2) - cx_var;
							}

							if(posy == 0) {
								wht = (rectizzle.bottom / 2) + cy_var;
							} else {
								wht = (rectizzle.bottom / 2) - cy_var;
							}

							winclients[i].vp1 = posx;
							winclients[i].vp2 = posy;
							winclients[i].vp3 = wln - BUFFER_ZONE;
							winclients[i].vp4 = wht - BUFFER_ZONE;
						}
					}
				}

				return 0; // Jump Back
			}
		   case WM_ERASEBKGND:
		   	return 0;
		  /*
		   case WM_NCPAINT:
   				GetClientRect(bigwin,&rectizzle);

				hDC = BeginPaint(hwnd, &painter);
				MoveToEx(hDC,(rectizzle.right / 2) + cx_var,0,NULL);
				LineTo(hDC,(rectizzle.right / 2) + cx_var,rectizzle.bottom);
				MoveToEx(hDC,0,(rectizzle.bottom / 2) + cy_var,NULL);
				LineTo(hDC,rectizzle.right,(rectizzle.bottom / 2) + cy_var);
				EndPaint(hwnd, &painter);
				DefWindowProc(hwnd,message,wParam,lParam);
				return 0;
			*/
           case WM_PAINT:		// we are ready to be redrawn

				bf_clrframe();	// clear the frame

				if(fs_mode != -1) {
					// single viewport mode
					activate_viewport(fs_mode);
					demo_main(fs_mode);
				} else {
					// quad/multiple viewport mode
					for(i = 0; i < 4; i++) {
						activate_viewport(i);
						demo_main(i);
					}
				}

				bf_endframe();	// finished. show the frame. (swaps the pages)

                return 0;
           case WM_DESTROY:
                go_down();
                break;
		   case WM_COMMAND:
			   switch(LOWORD(wParam)) {
					case ID_FILE_QUIT:
						PostQuitMessage(0);
						break;
					case ID_EDIT_DESELECTALL:
						deselect_all();		
						if(zzsel_mode == 0)	sp_chg_selection();
						else				obj_chgsel();
						break;
					case ID_LIGHTING_ADDLIGHTSOURCE:
						add_light_source();
						break;
					case ID_VIEW_PACKAGEVIEWER:
						ShowWindow(packdlg,SW_SHOW);
						break;
					case ID_VIEW_SURFPROP:
						if(zzsel_mode == BFE_SELMODE_ENVPOLY)	ShowWindow(surfprop,SW_SHOW);
						else if(zzsel_mode == BFE_SELMODE_OBJ)	ShowWindow(objprop,SW_SHOW);
						else if(zzsel_mode == BFE_SELMODE_LIGHTS) ShowWindow(editdlg,SW_SHOW);
						break;
					case ID_VIEW_ACTORBROWSER:
						ShowWindow(itemdlg,SW_SHOW);
						break;
					case ID_BRUSH_IMPORTBRUSH:						
						bf_enable_textures();
						brush_import(NULL,NULL);
						break;
					case ID_EDIT_FLIPPOLYGONNORMAL:
						flip_sel_normals();
						break;
					case ID_EDIT_ROTPOLYGONNORMAL:
						rotz_sel_normals();
						break;
					case ID_VIEW_SINGLEVIEW_BL:
						fs_mode = 0;
						SendMessage(bigwin,WM_SIZE,0,0);
						break;
					case ID_VIEW_SINGLEVIEW_BR:
						fs_mode = 2;
						SendMessage(bigwin,WM_SIZE,0,0);
						break;
					case ID_VIEW_SINGLEVIEW_TL:
						fs_mode = 1;
						SendMessage(bigwin,WM_SIZE,0,0);
						break;
					case ID_VIEW_SINGLEVIEW_TR:
						fs_mode = 3;
						SendMessage(bigwin,WM_SIZE,0,0);
						break;
					case ID_VIEW_QUADVIEW:
						fs_mode = -1;						
						SendMessage(bigwin,WM_SIZE,0,0);
						break;
					case ID_EDIT_FIXENVNORMALS:
						bfe_fix_normals(-1);
						break;
					case ID_EDIT_FIXOBJNORMALS:
						bfe_fix_normals(0);		// <<<<<<<<<<<< change this later!!
						break;
					case ID_EDIT_FIXENVVROT:
						bfe_fix_vtex_rot(-1);
						break;
					case ID_EDIT_FIXOBJVROT:
						bfe_fix_vtex_rot(0);		// <<<<<<<<<<<< change this later!!
						break;
					case ID_EDIT_ALIGNTEXTURES:
						tex_align();
						break;
					case ID_LIGHTING_RECALCULATELIGHTMAPS:
						zlogthis("caclulating lightmaps...\n");
						waitdlg_init("Calculating Lightmaps...");
						for(i = 0; i < bf_poly_cnt(); i++) {
							waitdlg_setprog(int((float(i+1)/float(bf_poly_cnt())) * 100.0f));
							sprintf(temptext,"Raytracing polygon %i/%i...",i+1,bf_poly_cnt());
							waitdlg_settext(temptext);
							zlogthis("\tpolygon %02i... ",i);
							bf_gen_lmap(bf_getpoly(i), i);
							zlogthis("ok\n");
						}
						waitdlg_kill();
						zlogthis("lightmap generation complete.\n");
						sprintf(xtext,"Lightmaps generated.");

						if(!(winclients[0].renderz.flags & BF_RZ_LMAPS_ENABLED)) {
							winclients[0].renderz.flags += BF_RZ_LMAPS_ENABLED;
							enable_lmaps = 1;
						}

					// context menu items ~~~~~~~~

					case ID_CONTEXT_PROPERTIES:
						if(zzsel_mode == BFE_SELMODE_ENVPOLY)	ShowWindow(surfprop,SW_SHOW);
						else if(zzsel_mode == BFE_SELMODE_OBJ)	ShowWindow(objprop,SW_SHOW);
						else if(zzsel_mode == BFE_SELMODE_LIGHTS) ShowWindow(editdlg,SW_SHOW);
						break;
					case ID_TEXTURES_ALIGN:
						tex_align();
						break;
					case ID_TEXTURES_AUTO:
						tex_autofit();
						break;
					case ID_CONTEXT_DESELECTALL:
						deselect_all();		
						if(zzsel_mode == 0)	sp_chg_selection();
						else				obj_chgsel();
						break;
					case ID_SELECTION_COPLANAR:
						tex_sel_coplanar();
						break;
					default:
						break;
			   }

			   return 0;
           case WM_KEYDOWN:		// someone has pressed a key
                keys[wParam] = true;	// mark the key as pressed

				/* keys that toggle stuff are handled here (or else if you held down
				   the tab key, it would toggle text_enable (etc.) really fast and
				   be quite annoying)
				*/
				if(keys[VK_F12]) {
					zlogthis("viewport state information: =============\n");
					for(i = 0; i < 4; i++) {
						zlogthis("%i: xyz = (%.2f,%.2f,%.2f), ypr = (%.2f, %.2f, %.2f)\n",i,winclients[i].main_x,winclients[i].main_y,winclients[i].main_z,winclients[i].main_rot,winclients[i].main_pit,winclients[i].main_yaw);
					}
					zlogthis("=========================================\n");
				} else if(keys[VK_SHIFT] && keys[VK_F7]) {
					// rebuild/link shader programs
					
					
					zlogthis("INVOKE_DEBUG: Reloading shaders.\n");
					waitdlg_init("Reloading shaders...");
					
					waitdlg_settext("Attempting to load shader program into hardware...");
					bf_reload_shaders();

					waitdlg_setprog(100);
					waitdlg_kill();

					sprintf(xtext,"Rendering context reloaded. Shaders compiled and linked.");
				} else if(keys[VK_F11]) {
					bf_query_state();
				} else if(keys[VK_F5]) {
					if(fizzie == 0) fizzie = 1;
					else fizzie = 0;
					zlogthis("fizzie = %i\n",fizzie);
				} else if(keys[VK_F6]) {
					if(enable_dsort == 0) enable_dsort = 1;
					else enable_dsort = 0;
				} else if(keys[VK_F7]) {
					toggle_selmode(-1);
				} else if(keys['/']) {
					deselect_all();
					if(zzsel_mode == 0)	sp_chg_selection();
					else				obj_chgsel();
				} else if(keys[VK_F8]) {
					if(enable_lights == 0) enable_lights = 1;
					else enable_lights = 0;
				} else if(keys[VK_F9]) {
					if(enable_lmaps == 0) {
						enable_lmaps = 1;
						winclients[0].renderz.flags += BF_RZ_LMAPS_ENABLED;
					} else {
						enable_lmaps = 0;
						winclients[0].renderz.flags -= BF_RZ_LMAPS_ENABLED;
					}
				} else if(keys['Q']) {
					if(enable_radblur == 0) {
						enable_radblur = 1;						
					} else {
						enable_radblur = 0;
					}
					bf_set_radblur(enable_radblur);
				} else if(keys['W']) {
					if(enable_normals == 0) {
						enable_normals = 1;						
					} else {
						enable_normals = 0;
					}
					bf_set_shownormals(enable_normals);
				}

                return 0;
           case WM_KEYUP:			// someone has let off of the key
                keys[wParam] = false;	// mark it as unpressed
                return 0;
           default:		
			    // for messages that we don't deal with...
				// send them to the "Default Window Procedure"...
				return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;

}

void demo_main(int dex) {
	BEGIN_FUNC("demo_main")

	if(keys['C']) {
		bfi_exec_script(bf_script_getnum("testdata:xcore"), "onProxy");
	}

	if (!(keys[VK_SHIFT] || keys[VK_CONTROL])) {
		if (keys[VK_UP]) { // forward +
			winclients[dex].main_z += cos(winclients[dex].main_rot * piover180) * 0.3f;
			winclients[dex].main_x -= sin(winclients[dex].main_rot * piover180) * 0.3f;
		}
		if (keys[VK_DOWN]) { // backward -
			winclients[dex].main_z -= cos(winclients[dex].main_rot * piover180) * 0.3f;
			winclients[dex].main_x += sin(winclients[dex].main_rot * piover180) * 0.3f;
		}
		if (keys[VK_LEFT]) { // lateral left
			winclients[dex].main_z -= cos((winclients[dex].main_rot * piover180) + 1.570795f) * 0.3f;
			winclients[dex].main_x += sin((winclients[dex].main_rot * piover180) + 1.570795f) * 0.3f;
		}
		if (keys[VK_RIGHT]) { // lateral right
			winclients[dex].main_z -= cos((winclients[dex].main_rot * piover180) - 1.570795f) * 0.3f;
			winclients[dex].main_x += sin((winclients[dex].main_rot * piover180) - 1.570795f) * 0.3f;
		}
	}
	if(keys[VK_SHIFT] && !keys[VK_CONTROL] && !winclients[dex].isOrtho) {
		if (keys[VK_UP]) winclients[dex].main_y -= 0.3f;		// upwards
		if (keys[VK_DOWN]) winclients[dex].main_y += 0.3f;	// downwards
		if (keys[VK_LEFT]) winclients[dex].main_rot--;	// turn left
		if (keys[VK_RIGHT]) winclients[dex].main_rot++;	// turn right
	}
	/*
	if(keys[VK_CONTROL] && !keys[VK_SHIFT]) {
		if (keys[VK_UP]) winclients[dex].main_pit--;	// turn up
		if (keys[VK_DOWN]) winclients[dex].main_pit++;	// turn down
		if (keys[VK_LEFT]) winclients[dex].main_yaw--;	// roll left
		if (keys[VK_RIGHT]) winclients[dex].main_yaw++;	// roll right
	}
	*/
	if(keys[VK_CONTROL] && !keys[VK_SHIFT] && !winclients[dex].isOrtho) {
		if(zzsel_mode == BFE_SELMODE_OBJ && num_selected == 1) {
			if (keys[VK_UP]) bf_obj_getptr(sel_polys[0])->pos.z -= 0.10f;
			if (keys[VK_DOWN]) bf_obj_getptr(sel_polys[0])->pos.z += 0.10f;
			if (keys[VK_LEFT])  bf_obj_getptr(sel_polys[0])->pos.x -= 0.10f;
			if (keys[VK_RIGHT])  bf_obj_getptr(sel_polys[0])->pos.x += 0.10f;
			obj_chgsel();			
		}
		if(zzsel_mode == BFE_SELMODE_LIGHTS && num_selected == 1) {
			if (keys[VK_UP]) bf_getlight(sel_polys[0])->z -= 0.10f;
			if (keys[VK_DOWN]) bf_getlight(sel_polys[0])->z += 0.10f;
			if (keys[VK_LEFT])  bf_getlight(sel_polys[0])->x -= 0.10f;
			if (keys[VK_RIGHT])  bf_getlight(sel_polys[0])->x += 0.10f;
			lite_chgsel();
		}
	}

	if(keys[VK_CONTROL] && keys[VK_SHIFT]) {
		if(zzsel_mode == BFE_SELMODE_OBJ && num_selected == 1) {
			if (keys[VK_UP]) bf_obj_getptr(sel_polys[0])->pos.y += 0.10f;
			if (keys[VK_DOWN]) bf_obj_getptr(sel_polys[0])->pos.y -= 0.10f;
			if (keys[VK_LEFT])  bf_obj_getptr(sel_polys[0])->orient.r -= 1.00f;
			if (keys[VK_RIGHT])  bf_obj_getptr(sel_polys[0])->orient.r += 1.00f;
			obj_chgsel();
		}
		if(zzsel_mode == BFE_SELMODE_LIGHTS && num_selected == 1) {
			if (keys[VK_UP]) bf_getlight(sel_polys[0])->y += 0.10f;
			if (keys[VK_DOWN]) bf_getlight(sel_polys[0])->y -= 0.10f;
			//if (keys[VK_LEFT])  bf_getlight(sel_polys[0])->rot -= 1.00f;
			//if (keys[VK_RIGHT])  bf_getlight(sel_polys[0])->rot += 1.00f;
			lite_chgsel();
		}
	}

	//if (keys[VK_ESCAPE]) go_down();
	
	render_main(&winclients[dex]);

	END_FUNC
}


void render_main(BF_WN_CLNT *winclientx) {
	BEGIN_FUNC("render_main")
	
	static float lasttime = 0.0f;
	static unsigned int frames = 0;
	float timedelta;
	float thistime;
	static float aveFPS = 0.0f;
	static float avefps_tdelta = 0.0f;

	// reset frames count after set limit
	if(frames > 6000) frames = 0;

	// inc frame count
	frames++;

	// get current time
	thistime = bf_get_secs();

	// calculate time delta and set lasttime
	if(lasttime > 0.0f) {
		timedelta = thistime - lasttime;
		lasttime = thistime;
	} else {
		timedelta = 0.0f;
		lasttime = thistime;
	}

	// calculate average FPS every 300 frames
	if(!(frames % 60)) {
		aveFPS = 60.0f / avefps_tdelta;
		avefps_tdelta = 0.0f;
	} else {
		avefps_tdelta += timedelta;
	}


	// sort polygons
	if(enable_dsort) bf_dsort_env(winclientx->main_x,winclientx->main_y,winclientx->main_z);

	//if(fizzie == 1 && winclientx->raster_mode == BF_FILL) return;

	// it starts...	

	float pos_x, pos_y;
	float ix, cvale, delta_vx;	

	BF_BLBRD kathy, dero, dero2;
	BF_WPD   master_wpd;

	master_wpd.x  = winclientx->main_x;
	master_wpd.y  = winclientx->main_y;
	master_wpd.z  = winclientx->main_z;
	master_wpd.nr = winclientx->main_rot;
	master_wpd.ny = winclientx->main_yaw;
	master_wpd.np = winclientx->main_pit;

	kathy.width = 6.0f;
	kathy.height = 6.0f;
	kathy.x = 0.0f;
	kathy.y = -7.0f;
	kathy.z = 0.0f;
	kathy.texid = bf_getimg(bf_getimg_num("testdata:dero"))->tex_id;

	/*
	dero.width = 6.0f;
	dero.height = 6.0f;
	dero.x = 6.0f;
	dero.y = -7.0f;
	dero.z = 6.0f;
	dero.texid = bf_getimg(bf_getimg_num("testdata:dero"))->tex_id;

	dero2.width = 6.0f;
	dero2.height = 6.0f;
	dero2.x = -6.0f;
	dero2.y = -7.0f;
	dero2.z = -6.0f;
	dero2.texid = bf_getimg(bf_getimg_num("testdata:dero"))->tex_id;
	*/

	// spin the NR logo
	float rxv = bf_getobject(bf_getobj_num("testdata:nr_logo"))->orient.r;
	rxv += timedelta * 180.0f;
	if(rxv >= 360.0f) rxv = 0.0f;
	bf_getobject(bf_getobj_num("testdata:nr_logo"))->orient.r = rxv;


	// render the objects
	if(winclientx->raster_mode != BF_FILL) {
		bf_disable_lighting();
		bf_disable_textures();
		bf_set_raster_mode(BF_WIREFRAME);

		if(winclientx->main_y >= 0.0f) winclientx->main_y = -0.1f;

		pos_x = ((float(winclientx->renderz.width) / 2.0f) / winclientx->main_y);
		pos_y = ((float(winclientx->renderz.height) / 2.0f) / winclientx->main_y);

		bf_init_ortho_ex(-pos_x + float(winclientx->main_x),pos_x + float(winclientx->main_x),-pos_y + float(winclientx->main_z),pos_y + float(winclientx->main_z));

		// draw 2D grid
		bf_setcolor(1.0f,0.0f,0.0f,1.0f);

		bf_draw_line(-pos_x + float(winclientx->main_x), 0, pos_x + float(winclientx->main_x), 0);
		bf_draw_line(0, -pos_y + float(winclientx->main_z), 0, pos_y + float(winclientx->main_z));

		bf_setcolor(1.0f,0.0f,0.0f,0.8f);
		
		delta_vx = abs((pos_x + float(winclientx->main_x)) - (-pos_x + float(winclientx->main_x)));

		cvale = float(int((delta_vx / winclientx->renderz.width) * 16.0f));

		if(cvale < 1.0f) cvale = 1.0f;

		for(ix = -pos_y; ix > pos_y; ix -= cvale) {
			bf_draw_line(-pos_x + float(winclientx->main_x), int(ix + float(winclientx->main_z)), pos_x + float(winclientx->main_x), int(ix + float(winclientx->main_z)));
		}

		for(ix = -pos_x; ix > pos_x; ix -= cvale) {
			bf_draw_line(int(ix + float(winclientx->main_x)), -pos_y + float(winclientx->main_z), int(ix + float(winclientx->main_x)), pos_y + float(winclientx->main_z));
		}
		
		bf_setcolor(1.0f,1.0f,1.0f,1.0f);
		bf_draw_env(0.0f,0.0f,16.0f,winclientx->main_rot,winclientx->main_yaw,winclientx->main_pit);
		bf_renderq(0.0f,0.0f,16.0f,winclientx->main_rot,winclientx->main_yaw,winclientx->main_pit);
		bf_exit_ortho();
	} else {
		bf_set_raster_mode(BF_FILL);

		if(enable_lights == 1) {
			bf_enable_lighting();
			bf_update_light(bf_getlight(0));
		} else {
			bf_disable_lighting();
		}

		bf_enable_textures();
		
		bf_setcolor(0.5f,0.5f,0.5f,1.0f);
		bf_renderq(winclientx->main_x,winclientx->main_y,winclientx->main_z,winclientx->main_rot,winclientx->main_yaw,winclientx->main_pit);
		if(enable_lights == 1) {
			bf_enable_lighting();
			bf_update_light(bf_getlight(0));
		} else {
			bf_disable_lighting();
		}
		bf_draw_env(winclientx->main_x,winclientx->main_y,winclientx->main_z,winclientx->main_rot,winclientx->main_yaw,winclientx->main_pit);
		draw_actors();

		if(enable_radblur) bf_rad_blur_pass2(bf_getimg_num("core:radtex"));

		
		//bf_set_raster_mode(BF_WIREFRAME);		
		//bf_disable_lighting();
		//bf_disable_textures();
		//bf_setcolor(1.0f,0.0f,0.0f,0.8f);
		//bf_draw_env(winclientx->main_x,winclientx->main_y,winclientx->main_z,winclientx->main_rot,winclientx->main_yaw,winclientx->main_pit);
		//bf_renderq(winclientx->main_x,winclientx->main_y,winclientx->main_z,winclientx->main_rot,winclientx->main_yaw,winclientx->main_pit);

	}	


	bf_disable_lighting();
	bf_setcolor(1.0f,1.0f,1.0f,1.0f);
	//bf_draw_billboard(&kathy, &master_wpd);
	//bf_draw_billboard(&dero, &master_wpd);
	//bf_draw_billboard(&dero2, &master_wpd);

	bf_init_ortho();	
	bf_disable_lighting();
	bf_disable_textures();
	
	bf_setcolor(0.0f, 1.0f, 0.0f, 1.0f);
	bf_tprintf(10, 20, &fizont, "%s", xtext);
	
	char zzmode_txt[16];

	switch(zzsel_mode) {
		case BFE_SELMODE_ENVPOLY: sprintf(zzmode_txt,"Env Polys"); break;
		case BFE_SELMODE_OBJ: sprintf(zzmode_txt,"Objects"); break;
		case BFE_SELMODE_LIGHTS: sprintf(zzmode_txt,"Lights"); break;
		case BFE_SELMODE_ACTORS: sprintf(zzmode_txt,"Actors"); break;
	}

	//bf_setcolor(1.0f, 0.0f, 0.0f, 1.0f);
	//bf_tprintf(10, winclientx->renderz.height - 28, &fizont, "Env Poly D-Sort (F6) [%s], Radial Blur (q) [%s], Poly Normal Vectors (w) [%s]", enable_dsort ? "Enabled" : "Disabled", enable_radblur ? "Enabled" : "Disabled", enable_normals ? "Shown" : "Hidden");
	//bf_tprintf(10, winclientx->renderz.height - 12, &fizont, "Lighting (F8) [%s], Lightmaps (F9) [%s], Selection Mode (F7) [%s]", enable_lights ? "Enabled" : "Disabled", enable_lmaps ? "Enabled" : "Disabled", zzmode_txt );
	
	bf_setcolor(1.0f, 0.0f, 0.0f, 0.7f);
	bf_tprintf(10, winclientx->renderz.height - 28, &fizont, "Rebuild Shaders (Shift+F7)");
	bf_setcolor(1.0f, 1.0f, 1.0f, 0.7f);
	bf_tprintf(10, winclientx->renderz.height - 12, &fizont, "timedelta = %01.04f / fps = %3.01f / fps(ave) = %3.01f / polys = %i", timedelta, 1.0f/timedelta, aveFPS, bf_env_count());
	

	bf_setcolor(113.0f/256.0f, 178.0f/256.0f, 219.0f/256.0f, 1.0f);
	bf_enable_textures();
	//bf_ft_print(0, 200, 200, "This is a test :)");
	bf_script_ft_render();

	/*
	int sz = 2;		// size of half the box
	bf_draw_line(winclientx->last_x - sz, winclientx->last_y + sz, winclientx->last_x + sz, winclientx->last_y + sz);
	bf_draw_line(winclientx->last_x + sz, winclientx->last_y + sz, winclientx->last_x + sz, winclientx->last_y - sz);
	bf_draw_line(winclientx->last_x + sz, winclientx->last_y - sz, winclientx->last_x - sz, winclientx->last_y - sz);
	bf_draw_line(winclientx->last_x - sz, winclientx->last_y - sz, winclientx->last_x - sz, winclientx->last_y + sz);
	bf_draw_line(winclientx->last_x - sz, winclientx->last_y - sz, winclientx->last_x + sz, winclientx->last_y + sz);
	bf_draw_line(winclientx->last_x - sz, winclientx->last_y + sz, winclientx->last_x + sz, winclientx->last_y - sz);
	*/

	bf_enable_textures();
	bf_exit_ortho();

	END_FUNC
}


// sel_mode = 0, Env Poly mode
//         <> 0, Object mode
int hit_test(int dex, int sel_mode) {
	BEGIN_FUNC("hit_test")

	// Get hits for environmental polys	

	RECT rectz, rectw;
	POINT mousey;
	int *hitptr;
	int hits;

	GetClientRect(bigwin,&rectz);
	GetWindowRect(bigwin,&rectw);
	GetCursorPos(&mousey);	
	
	int x = mousey.x - rectw.left - (((rectw.right - rectw.left) - rectz.right) / 2);
	int y = mousey.y - rectw.top - (((rectw.bottom - rectw.top) - rectz.bottom) / 2);
	int rad = 2;	// sensitivity

	// ortho stuff
	float pos_x = ((float(winclients[dex].renderz.width) / 2.0f) / winclients[dex].main_y);
	float pos_y = ((float(winclients[dex].renderz.height) / 2.0f) / winclients[dex].main_y);

	int cnum = 0;
	int cdep = -1;
	int dmin, dmax;
	int hrec, poly_id;
	int i = 0;

	if(sel_mode == 0) {
		if(winclients[dex].raster_mode == BF_FILL) {
			bf_init_sel_mode(x, y, rad);
			bf_draw_env(winclients[dex].main_x,winclients[dex].main_y,winclients[dex].main_z,winclients[dex].main_rot,winclients[dex].main_yaw,winclients[dex].main_pit);
		} else {
			bf_init_sel_mode_ortho(x, y, rad, -pos_x + float(winclients[dex].main_x),pos_x + float(winclients[dex].main_x),-pos_y + float(winclients[dex].main_z),pos_y + float(winclients[dex].main_z));
			bf_draw_env(0.0f,0.0f,-16.0f,winclients[dex].main_rot,winclients[dex].main_yaw,winclients[dex].main_pit);
		}
		hits = bf_exit_sel_mode(&hitptr);
	
		poly_id = -1;

		while(i < (hits * 4)) {
			hrec = hitptr[i];			i++;
			dmin = hitptr[i];			i++;
			dmax = hitptr[i];			i++;
	
			if(hrec) {			
				poly_id = hitptr[i];	i++;
			}
				
			if((dmax <= cdep) || cdep == -1) {
				cnum = i;
				cdep = dmax;
			}
		}

		poly_id = hitptr[cnum - 1];

		int thisone;

		BF_TRIANGLE_EX *thispoly;

		if(poly_id != -1) {
			thispoly = bf_getpoly(poly_id);
			if(thispoly->flags & BF_TRI_SELECTED) {
				// if it's already selected, unselect it
				thispoly->flags -= BF_TRI_SELECTED;			
				i = 0;
				while(i < num_selected) {
					if(sel_polys[i] == poly_id) {
						sel_polys[i] = sel_polys[num_selected];
						num_selected--;
						break;
					}
					i++;
				}
			} else {
				thispoly->flags |= BF_TRI_SELECTED;
				sel_polys[num_selected] = poly_id;
				num_selected++;
			}
		}

		sprintf(xtext,"Selected Env Poly #%i",poly_id);

	} else {
	

		// get hits for objects

		if(winclients[dex].raster_mode == BF_FILL) {
			bf_init_sel_mode(x, y, rad);
			bf_renderq(winclients[dex].main_x,winclients[dex].main_y,winclients[dex].main_z,winclients[dex].main_rot,winclients[dex].main_yaw,winclients[dex].main_pit);
		} else {
			bf_init_sel_mode_ortho(x, y, rad, -pos_x + float(winclients[dex].main_x),pos_x + float(winclients[dex].main_x),-pos_y + float(winclients[dex].main_z),pos_y + float(winclients[dex].main_z));
			bf_renderq(0.0f,0.0f,-16.0f,winclients[dex].main_rot,winclients[dex].main_yaw,winclients[dex].main_pit);
		}

		hits = bf_exit_sel_mode(&hitptr);

		poly_id = -1;

		while(i < (hits * 4)) {
			hrec = hitptr[i];			i++;
			dmin = hitptr[i];			i++;
			dmax = hitptr[i];			i++;

			if(hrec) {			
				poly_id = hitptr[i];	i++;
			}
				
			if((dmax <= cdep) || cdep == -1) {
				cnum = i;
				cdep = dmax;
			}
		}

		poly_id = hitptr[cnum - 1];

		sprintf(xtext,"Selected Obj #%i",poly_id);
	}


	END_FUNC
	return poly_id;
}


// this flips the order of the verticies, but does not change the value of the normal

void flip_sel_normals() {
	int i;

	BF_TRIANGLE_EX *thispoly;
	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		thispoly->normal.x = -thispoly->normal.x;
		thispoly->normal.y = -thispoly->normal.y;
		thispoly->normal.z = -thispoly->normal.z;
	}

	return;
}

// rotate sel verticies order
void rotz_sel_normals() {
	int i;
	float tx, ty, tz, tu, tv;

	BF_TRIANGLE_EX *thispoly;
	for(i = 0; i < num_selected; i++) {
		thispoly = bf_getpoly(sel_polys[i]);
		tx = thispoly->vertex[2].x;
		ty = thispoly->vertex[2].y;
		tz = thispoly->vertex[2].z;
		tu = thispoly->tex[2].u;
		tv = thispoly->tex[2].v;		
		thispoly->vertex[2].x = thispoly->vertex[0].x;
		thispoly->vertex[2].y = thispoly->vertex[0].y;
		thispoly->vertex[2].z = thispoly->vertex[0].z;
		thispoly->tex[2].u = thispoly->tex[0].u;
		thispoly->tex[2].v = thispoly->tex[0].v;
		thispoly->vertex[0].x = thispoly->vertex[1].x;
		thispoly->vertex[0].y = thispoly->vertex[1].y;
		thispoly->vertex[0].z = thispoly->vertex[1].z;
		thispoly->tex[0].u = thispoly->tex[1].u;
		thispoly->tex[0].v = thispoly->tex[1].v;
		thispoly->vertex[1].x = tx;
		thispoly->vertex[1].y = ty;
		thispoly->vertex[1].z = tz;
		thispoly->tex[1].u = tu;
		thispoly->tex[1].v = tv;
	}

	return;
}

void deselect_all() {

	int i;
	int dsl = 0;

	if(zzsel_mode == BFE_SELMODE_ENVPOLY) {
		for(i = 0; i < bf_env_count(); i++) {
			//zlogthis(">> bf_getpoly(i)->flags = %i (%b)\n",bf_getpoly(i)->flags,bf_getpoly(i)->flags);
			if(bf_getpoly(i)->flags & BF_TRI_SELECTED) {
				bf_getpoly(i)->flags -= BF_TRI_SELECTED;
				dsl++;
			}			
		}
		//zlogthis(">>> deselected %i polygons.\n",dsl);

		/*
		while(i < num_selected) {
			//if(bf_getpoly(sel_polys[i])->flags & BF_TRI_SELECTED)
			bf_getpoly(sel_polys[i])->flags -= BF_TRI_SELECTED;
			i++;
		}
		*/
	}

	num_selected = 0;

	return;
}

void build_viewports() {
	BEGIN_FUNC("build_viewports")		

	RECT rectizzle;
	int posx, posy, wht, wln;
	GetClientRect(bigwin,&rectizzle);

    for(int i = 0; i < 4; i++) {

		/*

		window positions:

			0	2

			1	3

		*/

		if(i > 1) posx = (rectizzle.right / 2) + cx_var;
		else      posx = 0;
		if(i % 2) posy = 0;
		else      posy = (rectizzle.bottom / 2) + cy_var;

		if(posx == 0) {
			wln = (rectizzle.right / 2) + cx_var;
		} else {
			wln = (rectizzle.right / 2) - cx_var;
		}

		if(posy == 0) {
			wht = (rectizzle.bottom / 2) + cy_var;
		} else {
			wht = (rectizzle.bottom / 2) - cy_var;
		}

		winclients[i].vp1 = posx;
		winclients[i].vp2 = posy;
		winclients[i].vp3 = cx_var - BUFFER_ZONE;
		winclients[i].vp4 = cy_var - BUFFER_ZONE;
		winclients[i].renderz.width = wln;
		winclients[i].renderz.height = wht;
		winclients[i].renderz.bgR = 0.0f;
		winclients[i].renderz.bgG = 0.0f;
		winclients[i].renderz.bgB = 0.0f;

		bf_set_raster_mode(winclients[i].raster_mode);

		winclients[i].x_init = -1;

	}

	winsok = 1;

	END_FUNC
}


void go_down() {

	zlogthis("Shutting down rendering subsystem...\n");
	bf_kill_3d(bigwin);	

	//bf_ft_kill_font(&fizont);
	bf_ft_font_remove();			/// << this needs to be fixed later :(

	EndDialog(packdlg, TRUE);
	bf_kill_window(bigwin, &wincl);

	// rid the context menu
	DestroyMenu(hPopup);

	bf_shutdown();

	exit(0);
}

void draw_actors() {

	BF_ACTOR_DATA thespis;

	// lights

	for(int i = 0; i < bf_light_cnt(); i++) {
		if(bf_getlight(i)->l_type == BF_LIGHT_AMBIENT) continue;

		thespis.x = bf_getlight(i)->x;
		thespis.y = bf_getlight(i)->y;
		thespis.z = bf_getlight(i)->z;

		thespis.r = bf_getlight(i)->r;
		thespis.g = bf_getlight(i)->g;
		thespis.b = bf_getlight(i)->b;
		thespis.a = 1.0f;

		if(zzsel_mode == BFE_SELMODE_LIGHTS && sel_polys[0] == i && num_selected == 1) thespis.bSelected = true;
		else thespis.bSelected = false;

		bf_render_actor(&thespis);

	}

	return;
}

void toggle_selmode(int zzmode) {

	int oldwinvis = 0;

	if(IsWindowVisible(surfprop)) oldwinvis++;
	if(IsWindowVisible(objprop)) oldwinvis++;
	if(IsWindowVisible(editdlg)) oldwinvis++;

	if(zzmode == -1) {
		zzsel_mode++;
		if(zzsel_mode > BFE_SELMODE_ACTORS) zzsel_mode = BFE_SELMODE_ENVPOLY;
	} else {
		zzsel_mode = zzmode;
	}
	if(zzsel_mode > 3 || zzsel_mode < 0) zzsel_mode = 0;
	
	if(zzsel_mode == BFE_SELMODE_ENVPOLY) {
		if(oldwinvis) ShowWindow(surfprop,SW_SHOW);
	} else {
		ShowWindow(surfprop,SW_HIDE);
	}

	if(zzsel_mode == BFE_SELMODE_OBJ) {
		if(oldwinvis) ShowWindow(objprop,SW_SHOW);
	} else {
		ShowWindow(objprop,SW_HIDE);
	}

	if(zzsel_mode == BFE_SELMODE_LIGHTS) {
		if(oldwinvis) ShowWindow(editdlg,SW_SHOW);
	} else {
		ShowWindow(editdlg,SW_HIDE);
	}

	// not implemented yet
	if(zzsel_mode == BFE_SELMODE_ACTORS) {
		//if(oldwinvis) ShowWindow(objprop,SW_SHOW);
	} else {
		//ShowWindow(objprop,SW_HIDE);
	}

	deselect_all();
	sp_chg_selection();
	obj_chgsel();
	lite_chgsel();

	return;
}
