/*

Bluefire Demo
(c) Copyright 2009 Jacob Hipps
http://bluefire.neoretro.net/


*/


// Bluefire Defines
#define BLUEFIRE_CLNT
#define _BF_MAIN

// Includes
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include "../bluefire.h"

// Consts
const float piover180 = 3.14159f / 180.0f;		// approximation of pi/180

// Globals
HINSTANCE hInst;
HWND bigwin;				// main window handle
WNDCLASSEX wincl;			// window class info
bool keys[256];
//char packs[256][256];		// wtf??
char xtext[256];
BF_FT_FONT fizont;			// default font
BF_FT_FONT suckfont;
POINT mc_x;
int npmove;
int xdown = -1;
int fs_mode = 0;
int cx_var = 0;
int cy_var = 0;
int move_valid = 0;


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

BF_WN_CLNT	winclient;


// Local function prototypes
void sighand(int sig);
void go_down();
LRESULT WINAPI WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void demo_main(int dex);
void render_main(BF_WN_CLNT *winclientx);


// exception handler
void sighand(int sig) {	

	// call BFW's exception handler
	exception(sig);

	return;
}


// Main entry point

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	hInst = hInstance;

	bf_init(go_down);			// initialize Bluefire

	// setup exception handling
	signal(SIGINT,sighand);
	signal(SIGILL,sighand);
	signal(SIGFPE,sighand);
	signal(SIGSEGV,sighand);
	signal(SIGTERM,sighand);
	signal(SIGBREAK,sighand);
	signal(SIGABRT,sighand);

	BEGIN_FUNC("WinMain")

	mainconfig.fullscreen = 1;	// enable fullscreen by default?
	mainconfig.bpp = 32;		// bits per pixel
	mainconfig.width = 1024;
	mainconfig.height = 768;
	mainconfig.blursteps = 10;	// number of steps in radial blur
								// raising this above 25 severly hinders performance

	winclient.raster_mode = BF_FILL;	
	winclient.main_x = 0.8f;
	winclient.main_y = 5.54f;
	winclient.main_z = -11.28f;
	winclient.main_rot = -41.6f;
	winclient.isOrtho = false;

	// The Window structure
	char classnomen[] =  "BFDEMO";
    wincl.hInstance		= hInstance;
	wincl.lpszClassName = classnomen;
    wincl.lpfnWndProc	= WindowProcedure;
    wincl.style			= CS_OWNDC;
    wincl.cbSize		= sizeof(WNDCLASSEX);

    // Use default icon and mouse-pointer
    wincl.hIcon   = NULL; //LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wincl.hIconSm = NULL;
    wincl.hCursor = LoadCursor(hInstance, IDC_ARROW);
    wincl.lpszMenuName = NULL; //MAKEINTRESOURCE(IDR_MENU1);
    wincl.cbClsExtra = 0;                      // No extra bytes after the window class
    wincl.cbWndExtra = 0;                      // structure or the window instance
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND; //GetStockObject(WHITE_BRUSH);		// black BG
	
    // Register the window class; if fail, quit the program
    if(!RegisterClassEx(&wincl)) {
		MessageBox(NULL,"Window class registration failed!","BFW Critical Error",0);
		return 255;
	}

	bf_bind_render_dll("render_ogl\\render_ogl.dll"); // bind to the Render DLL


	// create the window now
	char windowtitle[128];
	sprintf(windowtitle,"Bluefire Demo [test build %s]",__DATE__);
	
	bigwin = bf_create_window(windowtitle, 100, 100, mainconfig.fullscreen, mainconfig.width, mainconfig.height, &wincl, GetDesktopWindow());
    zlogthisx("window created successfully. fullscreen = %i, %ix%ix%ibpp",mainconfig.fullscreen,mainconfig.width,mainconfig.height,mainconfig.bpp);
	ShowWindow(bigwin,SW_HIDE); // hide the window until initialization is complete!
	
	
	// write some info to the logfile...
    zlogthis("Bluefire Demo. [test build %s @ %s]\n"
			 "Copyright (c) 2009 Jacob Hipps, all rights reserved.\n"
			 ,__DATE__, __TIME__);


	bf_activate_context(&winclient.renderz);
	
	bf_init_3d(bigwin);

	bf_script_ft_clear();


	BFZ_FILE bfzfile;	
	
	bfz_open("testdata.bfz",&bfzfile);
	//bfz_loadobj(&bfzfile,"ttg_logo",bf_addobject());
	bfz_loadobj(&bfzfile,"nr_logo",bf_addobject());
	bfz_loadobj(&bfzfile,"main_bldg",bf_addobject());
	//bfz_loadimg(&bfzfile,"dero",bf_addimg());
	//bfz_loadexec(&bfzfile,"xcore");
	bfz_close(&bfzfile);

	bfz_open("demoimg.bfz",&bfzfile);
	bfz_loadimg(&bfzfile,"new_float",bf_addimg());
	bfz_close(&bfzfile);
	

    // test script functions...
	/*
	zlogthis("TESTING SCRIPT FUNCTIONALITY...\n");
	bfi_exec_script(bf_script_getnum("testdata:xcore"), "onProxy");
	bfi_kill();
    zlogthis("---- END TEST ----\n");
	*/

	// load the environment from object data
	
	//brush_import("testdata.bfz", "main_bldg");
	
	//brush_import("city.bfz","city");

	//bfz_open("city.bfz",&bfzfile);
	//bfz_loadobj(&bfzfile,"city",bf_addobject());
	//bfz_close(&bfzfile);

	bf_getobject(bf_getobj_num("testdata:nr_logo"))->bEnabled = true;
	bf_getobject(bf_getobj_num("testdata:nr_logo"))->surfdex[0].cr = 1.0f;
	bf_getobject(bf_getobj_num("testdata:nr_logo"))->surfdex[0].cg = 1.0f;
	bf_getobject(bf_getobj_num("testdata:nr_logo"))->surfdex[0].cb = 1.0f;

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
	bf_set_radblur(false);
	bf_set_shownormals(false);

	// font stuff
	/*
	bf_ft_init();
	//bf_ft_open("c:\\windows\\fonts\\gothic.ttf", &fizont);
	//bf_ft_rasterize(&fizont, 32);
	bf_ft_addfont("bennb.ttf", 32);
	//bf_ft_dlist(&fizont, 32, 128, 0);	// generate dlist
	*/

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


	// initialize timer
	bf_init_timer();
	zlogthisx("Timer ready.");

	ShowWindow(bigwin,SW_SHOW); // show the window	
	SetCursor(NULL);

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

	zlogthis("We ready! Going into main loop.\n");
	//sprintf(xtext,"Bluefire Rendering Engine - Version %s, build %s (%s)",bf_get_version(),bf_get_build(),bf_get_buildtime());
	sprintf(xtext,"Bluefire Rendering Engine. (c) Copyright 2003-2009 Jacob Hipps");


	// Go into the main message dispatcher loop
    MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    while( msg.message != WM_QUIT ) {                      		   
           if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {                            
              TranslateMessage( &msg );
              DispatchMessage( &msg );
           } else {
				//Sleep(2000);
		   } 
    }

	zlogthisx("Shutdown via WinMain.");
    go_down();

    return (int)msg.wParam;
}

void go_down() {

	zlogthis("Shutting down rendering subsystem...\n");

	bf_kill_3d(bigwin);	
	//bf_ft_kill_font(&fizont);
	bf_ft_font_remove();			/// << this needs to be fixed later :(	
	bf_kill_window(bigwin, &wincl);
	bf_shutdown();

	exit(0);
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
	int x_delta, y_delta;

	RECT wirect, rectz, kirect;
	POINT mc_x_old;

	zonetouch = 0;

	switch (message)
	{
		case WM_RBUTTONDOWN:

			GetWindowRect(hwnd,&wirect);
			GetClientRect(hwnd,&rectz);

			if(!(wParam & MK_CONTROL)) {
				winclient.x_init = LOWORD(lParam);
				winclient.y_init = HIWORD(lParam);
				SetCapture(hwnd);
				GetCursorPos((POINT*)&mc_x);
				/*
				if(fs_mode != -1) {
					kirect.top = wirect.top + winclient.vp2 + 30;
					kirect.bottom = wirect.top + winclient.vp2 + winclient.vp4 - 30;
					kirect.left = wirect.left + winclient.vp1 + 30;
					kirect.right = wirect.left + winclient.vp1 + winclient.vp3 - 30;
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
				//pcursor = SetCursor(latrot_cursor);

			}

			xdown = zonetouch;

			return 0;
		case WM_LBUTTONDOWN:

			GetWindowRect(hwnd,&wirect);
			GetClientRect(hwnd,&rectz);

			if(wParam & MK_CONTROL) {
				winclient.last_x = mc_x.x - wirect.left - (((wirect.right - wirect.left) - rectz.right) / 2);
				winclient.last_y = mc_x.y - wirect.top - (((wirect.bottom - wirect.top) - rectz.bottom) / 2);
				//hit_test(zonetouch, zzsel_mode);
			} else {
				winclient.x_init = LOWORD(lParam);
				winclient.y_init = HIWORD(lParam);
				SetCapture(hwnd);
				GetCursorPos((POINT*)&mc_x);
				/*
				if(fs_mode != -1) {
					kirect.top = wirect.top + winclient.vp2 + 30;
					kirect.bottom = wirect.top + winclient.vp2 + winclient.vp4 - 30;
					kirect.left = wirect.left + winclient.vp1 + 30;
					kirect.right = wirect.left + winclient.vp1 + winclient.vp3 - 30;
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
				//pcursor = SetCursor(move_cursor);

				//sprintf(zazzy,"mc_x = (%i,%i)\nkirect(TBLR) = [%i,%i,%i,%i]",mc_x.x,mc_x.y,kirect.top,kirect.bottom,kirect.left,kirect.right);
				//MessageBox(NULL,zazzy,"nig",0);

			}

			xdown = zonetouch;

			return 0;
		case WM_MOUSEMOVE:

				// testing new camera movement 06/26/2009 -jacob
			
				if(LOWORD(lParam) == winclient.renderz.width / 2 && HIWORD(lParam) == winclient.renderz.height / 2) {
					return 0;
				}

				winclient.x_final = LOWORD(lParam);
				winclient.y_final = HIWORD(lParam);

				x_delta = winclient.x_final - winclient.x_init;
				y_delta = winclient.y_final - winclient.y_init;
                
				winclient.main_pit += (float(y_delta) / float(winclient.renderz.height)) * 36.0f;
				winclient.main_rot += (float(x_delta) / float(winclient.renderz.width)) * 36.0f;

                winclient.x_init = winclient.renderz.width / 2;
				winclient.y_init = winclient.renderz.height / 2;

				SetCursorPos(winclient.x_init, winclient.y_init);
				move_valid = 0;
				

			if(winclient.x_init >= 0 && !npmove) {

				// old movement

				/*
				winclient.x_final = LOWORD(lParam);
				winclient.y_final = HIWORD(lParam);

				// L button (fwd/back, lateral left/rt)
				if((wParam & MK_LBUTTON) && !(wParam & MK_RBUTTON) && !(wParam & MK_SHIFT)) {
					//winclient.main_z += cos(winclient.main_rot * piover180) * float(winclient.y_final - winclient.y_init) / (float(winclient.renderz.height) / 10.0f);
					//winclient.main_x += sin(winclient.main_rot * piover180) * float(winclient.x_final - winclient.x_init) / (float(winclient.renderz.width)  / 10.0f);
					winclient.main_z += cos(winclient.main_rot * piover180) * (float(winclient.y_final - winclient.y_init) / (float(winclient.renderz.height) * 0.4f));
					winclient.main_x -= sin(winclient.main_rot * piover180) * (float(winclient.y_final - winclient.y_init) / (float(winclient.renderz.height) * 0.4f));
					winclient.main_z -= cos((winclient.main_rot * piover180) + 1.570795f) * (float(winclient.x_final - winclient.x_init) / (float(winclient.renderz.width) * 0.4f));
					winclient.main_x += sin((winclient.main_rot * piover180) + 1.570795f) * (float(winclient.x_final - winclient.x_init) / (float(winclient.renderz.width) * 0.4f));
				// R button (rotation, pitch)
				} else if ((wParam & MK_RBUTTON) && !(wParam & MK_LBUTTON)) {
					if(!winclient.isOrtho) {
						//winclient.main_yaw -= abs((4 * winclient.main_rot) / 360) * float(winclient.y_final - winclient.y_init) / (float(winclient.renderz.height) / 10.0f);
						//winclient.main_pit += abs((4 * (winclient.main_rot - 90)) / 360) * float(winclient.y_final - winclient.y_init) / (float(winclient.renderz.height) / 10.0f);

						winclient.main_rot += float(winclient.x_final - winclient.x_init) / (float(winclient.renderz.width)  / 12.0f);
					}
				// L button+shift (up/down vertical)
				} else if ((wParam & MK_LBUTTON) && (wParam & MK_SHIFT)) {
					winclient.main_y -= float(winclient.y_final - winclient.y_init) / (float(winclient.renderz.height) / 4.0f);
				}
				*/

				npmove = 1;
				//sprintf(zazzy,"mc_x = (%i,%i)\nkirect(TBLR) = [%i,%i,%i,%i]",mc_x.x,mc_x.y,wirect.top,wirect.bottom,wirect.left,wirect.right);
				//MessageBox(NULL,zazzy,"nig",0);
				//SetCursorPos(mc_x.x,mc_x.y);
				//sprintf(zazzy,"zonetouch = %i, (x_init = %i, y_init = %i), (x_final = %i, y_final = %i)\n",zonetouch,winclient.x_init,winclient.y_init,winclient.x_final,winclient.y_final);
				//MessageBox(NULL,zazzy,"nig",0);

				//winclient.x_init = LOWORD(lParam);
				//winclient.y_init = HIWORD(lParam);

				demo_main(zonetouch); 
			} else {
				npmove = 0;
			}
			return 0;

		case WM_RBUTTONUP:

		case WM_LBUTTONUP:
			if(winclient.x_init != -1) {
				ReleaseCapture();
				winclient.x_init = -1; // signal that we are finished
				//SetCursorPos(mc_x.x,mc_x.y);
				kirect.left = winclient.vp1;
				kirect.top = winclient.vp1;
				kirect.right = winclient.vp1;
				kirect.bottom = winclient.vp1;
				ClipCursor(NULL);
				//SetCursor(pcursor);
			}
			xdown = -1;
			return 0;
		default:
			xdown = -1;
			break;
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

						winclient.vp1 = posx;
						winclient.vp2 = posy;
						winclient.vp3 = wln;
						winclient.vp4 = wht;
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
				demo_main(0);
				bf_endframe();	// finished. show the frame. (swaps the pages)

                return 0;
           case WM_DESTROY:
                go_down();
                break;
           case WM_KEYDOWN:		// someone has pressed a key
                keys[wParam] = true;	// mark the key as pressed

				/* keys that toggle stuff are handled here (or else if you held down
				   the tab key, it would toggle text_enable (etc.) really fast and
				   be quite annoying)
				*/
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

	/*
	if(keys['C']) {
		bfi_exec_script(bf_script_getnum("testdata:xcore"), "onProxy");
	}
	*/

	if (!(keys[VK_SHIFT] || keys[VK_CONTROL])) {
		if (keys[VK_UP] || keys['W']) { // forward +
			winclient.main_z += cos(winclient.main_rot * piover180) * 0.3f;
			winclient.main_x -= sin(winclient.main_rot * piover180) * 0.3f;
		}
		if (keys[VK_DOWN] || keys['S']) { // backward -
			winclient.main_z -= cos(winclient.main_rot * piover180) * 0.3f;
			winclient.main_x += sin(winclient.main_rot * piover180) * 0.3f;
		}
		if (keys[VK_LEFT] || keys['A']) { // lateral left
			winclient.main_z -= cos((winclient.main_rot * piover180) + 1.570795f) * 0.3f;
			winclient.main_x += sin((winclient.main_rot * piover180) + 1.570795f) * 0.3f;
		}
		if (keys[VK_RIGHT] || keys['D']) { // lateral right
			winclient.main_z -= cos((winclient.main_rot * piover180) - 1.570795f) * 0.3f;
			winclient.main_x += sin((winclient.main_rot * piover180) - 1.570795f) * 0.3f;
		}
	}
	if(keys[VK_SHIFT] && !keys[VK_CONTROL] && !winclient.isOrtho) {
		if (keys[VK_UP]) winclient.main_y -= 0.3f;		// upwards
		if (keys[VK_DOWN]) winclient.main_y += 0.3f;	// downwards
		if (keys[VK_LEFT]) winclient.main_rot--;	// turn left
		if (keys[VK_RIGHT]) winclient.main_rot++;	// turn right
	}
	/*
	if(keys[VK_CONTROL] && !keys[VK_SHIFT]) {
		if (keys[VK_UP]) winclient.main_pit--;	// turn up
		if (keys[VK_DOWN]) winclient.main_pit++;	// turn down
		if (keys[VK_LEFT]) winclient.main_yaw--;	// roll left
		if (keys[VK_RIGHT]) winclient.main_yaw++;	// roll right
	}
	*/

	/*
	if(keys[VK_CONTROL] && !keys[VK_SHIFT] && !winclient.isOrtho) {
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
	*/

	if (keys[VK_ESCAPE]) go_down();
	
	render_main(&winclient);

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

	float adj_pitch, adj_yaw;

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
	//if(enable_dsort) bf_dsort_env(winclientx->main_x,winclientx->main_y,winclientx->main_z);

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

		if(false) {		// <<< hack, fix later
			bf_enable_lighting();
			bf_update_light(bf_getlight(0));
		} else {
			bf_disable_lighting();
		}

		bf_enable_textures();
		
		bf_setcolor(0.5f,0.5f,0.5f,1.0f);

		// calculate adjusted pitch/yaw heading values
		// TODO: add a + sin/cos values for adjusted yaw (not used right now)
		adj_pitch = cos(winclientx->main_rot * piover180) * winclientx->main_pit;
		adj_yaw   = sin(winclientx->main_rot * piover180) * winclientx->main_pit;
		//adj_pitch = winclientx->main_pit;
		//adj_yaw = 0.0f;

		bf_renderq(winclientx->main_x,winclientx->main_y,winclientx->main_z,winclientx->main_rot,adj_yaw,adj_pitch);
		
		if(false) {	// <<< hack, fix later
			bf_enable_lighting();
			bf_update_light(bf_getlight(0));
		} else {
			bf_disable_lighting();
		}
		//bf_draw_env(winclientx->main_x,winclientx->main_y,winclientx->main_z,winclientx->main_rot,winclientx->main_yaw,winclientx->main_pit);
		//draw_actors();

		//bf_rad_blur_pass2(bf_getimg_num("core:radtex"));

		
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

	/*
	switch(zzsel_mode) {
		case BFE_SELMODE_ENVPOLY: sprintf(zzmode_txt,"Env Polys"); break;
		case BFE_SELMODE_OBJ: sprintf(zzmode_txt,"Objects"); break;
		case BFE_SELMODE_LIGHTS: sprintf(zzmode_txt,"Lights"); break;
		case BFE_SELMODE_ACTORS: sprintf(zzmode_txt,"Actors"); break;
	}
	*/

	//bf_setcolor(1.0f, 0.0f, 0.0f, 1.0f);
	//bf_tprintf(10, winclientx->renderz.height - 28, &fizont, "Env Poly D-Sort (F6) [%s], Radial Blur (q) [%s], Poly Normal Vectors (w) [%s]", enable_dsort ? "Enabled" : "Disabled", enable_radblur ? "Enabled" : "Disabled", enable_normals ? "Shown" : "Hidden");
	//bf_tprintf(10, winclientx->renderz.height - 12, &fizont, "Lighting (F8) [%s], Lightmaps (F9) [%s], Selection Mode (F7) [%s]", enable_lights ? "Enabled" : "Disabled", enable_lmaps ? "Enabled" : "Disabled", zzmode_txt );
	
	bf_setcolor(1.0f, 1.0f, 0.0f, 0.7f);
	bf_tprintf(10, winclientx->renderz.height - 28, &fizont, "Using GLSL / OpenGL 2.0 / Vertical Sync Enforced / BFdemo built %s @ %s",__DATE__,__TIME__);

	bf_tprintf(10, winclientx->renderz.height - 44, &fizont, "adj_yaw = %0.02f, adj_pitch = %0.02f",adj_yaw,adj_pitch);

	bf_setcolor(1.0f, 1.0f, 1.0f, 0.7f);
	bf_tprintf(10, winclientx->renderz.height - 12, &fizont, "timedelta = %01.04f / fps(ave) = %3.01f / polys = %i", timedelta, aveFPS, bf_env_count());
	
	bf_enable_textures();
	bf_setcolor(1.0f,1.0f,1.0f,0.5f);
	draw_pic(250,420,bf_img_getnum("demoimg:new_float"));


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


