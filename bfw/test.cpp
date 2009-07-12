/*


  Bluefire: Test

  (c) Copyright 2004 Jacob Hipps

  Bluefire MDL Core: Winchester

*/

#ifdef BLUEFIRE_TEST

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bluefire.h"


// Imported superglobals

extern ESM_RENDERZ mainscr;


// Globals

float main_yaw, main_pit, main_rot, main_x, main_y, main_z;
bool keys[256];


// Local functions

void demo_main();
void render_main();


int demo_init() {
	BEGIN_FUNC("demo_init")

	zlogthis("init: Loading resources...\n");

	BFZ_FILE bfzdat;
	
	bfz_open("testdata.bfz",&bfzdat);

	bfz_loadobj(&bfzdat, "nrlogo", bf_addobject());
	bfz_loadimg(&bfzdat, "bfw_floater", bf_addimg());
	bfz_loadobj(&bfzdat, "main_bldg", bf_addobject());

	bfz_close(&bfzdat);

	zlogthis("init: Finished loading resources.\n");

	END_FUNC
	return 0;
}


LRESULT WINAPI WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
  		    case WM_SIZE:		// resize the window
			{
				bf_resize_disp(LOWORD(lParam),HIWORD(lParam), hwnd);
				mainscr.height = HIWORD(lParam);
				mainscr.width = LOWORD(lParam);
				return 0; // Jump Back
			}
           case WM_PAINT:		// we are ready to be redrawn
			    demo_main();                
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

void demo_main() {
	BEGIN_FUNC("demo_main")

	if (!(keys[VK_SHIFT] || keys[VK_CONTROL])) {
		if (keys[VK_UP]) main_z++;		// forward
		if (keys[VK_DOWN]) main_z--;	// backward
		/*
		if (keys[VK_UP]) {				// forward
			main_z -= (float)cos(main_rot*convfact) * 0.05f;
			main_x -= (float)sin(main_rot*convfact) * 0.05f;
		}
		if (keys[VK_DOWN]) {			// backward
			main_z += (float)cos(main_rot*convfact) * 0.05f;
			main_x += (float)sin(main_rot*convfact) * 0.05f;
		}
		*/
		if (keys[VK_LEFT]) main_x++;	// lateral left
		if (keys[VK_RIGHT]) main_x--;	// lateral right
	}
	if(keys[VK_SHIFT] && !keys[VK_CONTROL]) {
		if (keys[VK_UP]) main_y--;		// upwards
		if (keys[VK_DOWN]) main_y++;	// downwards
		if (keys[VK_LEFT]) main_rot--;	// turn left
		if (keys[VK_RIGHT]) main_rot++;	// turn right
	}
	if(keys[VK_CONTROL] && !keys[VK_SHIFT]) {
		if (keys[VK_UP]) main_pit--;	// turn up
		if (keys[VK_DOWN]) main_pit++;	// turn down
		if (keys[VK_LEFT]) main_yaw--;	// roll left
		if (keys[VK_RIGHT]) main_yaw++;	// roll right
	}
	
	if (keys[VK_ESCAPE]) go_down();

	render_main();

	END_FUNC
}

void render_main() {
	BEGIN_FUNC("render_main")

	// it starts...
	bf_clrframe();	// clear the frame

	// render the objects
	bf_renderq(main_x,main_y,main_z,main_rot,main_yaw,main_pit); // render the objects

	bf_init_ortho();
	bf_setcolor (1.0f, 1.0f, 1.0f, 0.75f);
	bf_enable_textures();
	draw_pic(mainscr.width - 512, mainscr.height - 128, bf_getimg_num("testdata:bfw_floater"));
	bf_disable_textures();
	bf_exit_ortho();
    bf_endframe();						// finished. show the frame. (swaps the pages)

	END_FUNC
}


#endif