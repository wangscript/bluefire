/*

  Bluefire: OpenGL Rendering Extention: DLL Entrypoint/Main

  (c) Copyright 2004 Jacob Hipps

  Bluefire MDL Core: Winchester

*/

#define _BF_MAIN
#define BF_MIN_REQ_BUILD	20040827

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bluefire.h"
#include "ogl.h"



int dll_init(BF_DLL_EXPORT *setz) {

	if(setz->build_num >= BF_MIN_REQ_BUILD && setz->build_num < 20080000) {
		// ok, it's fine
	} else {
		return -1;
	}

	renderx = setz->renderx;

	begin_func_trace = setz->begin_func_trace;
	end_func_trace = setz->end_func_trace;
	zlogfile_wrt = setz->zlogfile_wrt;
	bf_create_window = setz->bf_create_window;
	bf_get_timer_ptr = setz->bf_get_timer_ptr;
	bf_get_secs = setz->bf_get_secs;
	bf_getobject = setz->bf_getobject;
	bf_getobj_num = setz->bf_getobj_num;
	bf_obj_count = setz->bf_obj_count;
	bf_getimg = setz->bf_getimg;
	bf_getimg_num = setz->bf_getimg_num;
	bf_img_count = setz->bf_img_count;
	bf_getlight = setz->bf_getlight;
	bf_getlight_num = setz->bf_getlight_num;
	bf_light_cnt = setz->bf_light_cnt;
	bf_ready = setz->bf_ready;
	bf_set_ready = setz->bf_set_ready;
	bf_getpoly = setz->bf_getpoly;
	bf_poly_cnt = setz->bf_poly_cnt;
	go_down = setz->go_down;
	
	zlogthis("\nBluefire OpenGL Rendering Module.\n");
	zlogthis("Copyright (c) 2004-2009 Jacob Hipps.\n");
	zlogthis("[test build - %s @ %s]\n",__DATE__,__TIME__);
	zlogthis("DLL initialized successfully.\n");

#ifdef BF_WITH_SHADERS
	zlogthis("BF_WITH_SHADERS defined. Shader support enabled for this build.\n");
#endif

	return 0;
}