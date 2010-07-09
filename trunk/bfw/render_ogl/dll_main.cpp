/*

  Bluefire: OpenGL Rendering Extention: DLL Entrypoint/Main

  (c) Copyright 2004 Jacob Hipps

  Bluefire MDL Core: Winchester

*/

#define _BF_MAIN
#define BF_MIN_REQ_BUILD 20100407

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "../bluefire.h"
#include "ogl.h"


int dll_init(BF_DLL_EXPORT *setz) {

	char xtxt[1024];

	if(setz->build_num >= BF_MIN_REQ_BUILD) {
		// ok, it's fine
	} else {
		sprintf(xtxt,"ERROR: This version of render_ogl.dll (OpenGL Renderer) is incompatible\n"
					 "with the current version of bluefire.dll (BFW Core)!\n\n"
					 "OGL Renderer requires build # %i (or higher)\n"
					 "BFW Core build # %i\n\n"
					 "OGL Renderer requires a certain build of BFW Core due to periodical\n"
					 "changes to the way the two modules communicate. Whenever changes to\n"
					 "BFW Core's DLL module binding scheme occurs or shared data structrues\n"
					 "are changed, this required build number increases to prevent unending\n"
					 "amounts of problems! -- tetrisfrog  ;)"
					 ,BF_MIN_REQ_BUILD,setz->build_num);

		MessageBox(NULL,xtxt,"bfw dreamcatcher - Incompatible Builds!",0);
		return 1;
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
	bf_exception = setz->bf_exception;
	
	char v1[128];
	char v2[128];
	int v3;

	rdx_get_version(v1);
	rdx_get_buildtime(v2);
	v3 = rdx_get_buildi();

	zlogthis("\n*******************************************************************************\n");
	zlogthis("Bluefire OpenGL Rendering Module.\n");
	zlogthis("Version %s - build %i - compiled %s\n",v1,v3,v2);
	zlogthis("Copyright (c) 2004-2010 Jacob Hipps, all rights reserved.\n");
	zlogthis("*******************************************************************************\n\n");

	zlogthis("dll_init: Setting up exception handling... ");

	// use bfw core's exception handler
	signal(SIGFPE,bf_exception);
	signal(SIGILL,bf_exception);
	signal(SIGSEGV,bf_exception);

	zlogthis("Ok\n");

	zlogthis("dll_init: Rendering DLL initialized successfully.\n");

#ifdef BF_WITH_SHADERS
	zlogthis("dll_init: BF_WITH_SHADERS defined. Shader support enabled for this build!\n");
	zlogthis("          OpenGL fixed functionality bypassed!\n");
#else
	zlogthis("dll_init: BF_WITH_SHADERS not defined. Shader support will NOT be available!\n");
#endif

	return 0;
}