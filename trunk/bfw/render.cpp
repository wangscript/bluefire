/*

	Bluefire: Render Module Linking Routines
	(c) Copyright 2009 Jacob Hipps/Neo-Retro Group
	http://bluefire.neoretro.net/spec/bfw/render


	Implements:
		bf_bind_render_dll()- Binds to the Rendering DLL specified in bfconfig
		bfGetProcAddress()	- Checks for valid function calling vectors

*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bluefire.h"

extern void (__cdecl * go_down)();
extern BF_EXPORTZ z_config mainconfig;
extern BF_EXPORTZ ESM_RENDERZ mainscr;

// Globals
HINSTANCE hRenderDLL = 0;	// Render DLL instance

// Local functions
FARPROC bfGetProcAddress(HMODULE hModule, LPCSTR lpProcName); // wrapper function for GetProcAddress
															  // so we can check the address' validity
															  // and prevent Illegal Access/Segfaults =]

/***

bf_bind_render_dll()

	Description:
		Loads and binds to the Rendering module

	Arguments:
		char *dllname		Name of the rendering DLL to bind with

	Return:
		0 on success, non-zero on error

	Status:
		Working, Level A
	To-do:
		?

***/
int bf_bind_render_dll(char *dllname) {
	BEGIN_FUNC("bf_bind_render_dll")

	char dll_name[128];
	strcpy(dll_name,bf_config_str("RenderDLL"));

	zlogthisx("Binding to %s...",dllname);

	if((hRenderDLL = LoadLibrary(dll_name)) == NULL) {
		zlogthisx("critical: Unable to bind to DLL! Going down now!");		
		char bizziez[128];
		sprintf(bizziez,"Unable to bind to %s\nBluefire must now exit.",dll_name);
		MessageBox(NULL,bizziez,"BFW Critical Error",0);
		FreeLibrary((HMODULE)hRenderDLL);
		exit(55);		// DO NOT USE go_down() to exit!!!
	}

	zlogthisx("Ok, DLL loaded. Linking to procedures.");

	dll_init = (BF_DLL_INIT) bfGetProcAddress((HMODULE)hRenderDLL, "dll_init");

	//bf_delete_texture = (BF_1INT_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_delete_texture");
	bf_bind_texture   = (BF_BIND_TEX) bfGetProcAddress((HMODULE)hRenderDLL, "bf_bind_texture");
	bf_resize_disp    = (BF_RSIZE_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_resize_disp");

	bf_init_3d = (BF_HWND_INT) bfGetProcAddress((HMODULE)hRenderDLL, "bf_init_3d");	
	bf_kill_3d = (BF_HWND_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_kill_3d");

	bf_create_texture = (BF_3INTS_INT) bfGetProcAddress((HMODULE)hRenderDLL, "bf_create_texture");

	bf_clrframe = (BF_NO_ARGS_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_clrframe");
	bf_endframe = (BF_NO_ARGS_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_endframe");
	bf_renderq = (BF_RENDERQ) bfGetProcAddress((HMODULE)hRenderDLL, "bf_renderq");

	bf_init_ortho = (BF_NO_ARGS_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_init_ortho");
	bf_exit_ortho = (BF_NO_ARGS_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_exit_ortho");

	draw_pic = (BF_DRAWPIC) bfGetProcAddress((HMODULE)hRenderDLL, "draw_pic");
	draw_picx = (BF_DRAWPICX) bfGetProcAddress((HMODULE)hRenderDLL, "draw_picx");
	draw_obj = (BF_1INT_INT) bfGetProcAddress((HMODULE)hRenderDLL, "draw_obj");
	draw_objx = (BF_1INT_INT) bfGetProcAddress((HMODULE)hRenderDLL, "draw_objx");
	bf_rad_blur_pass1 = (BF_2INTS_VOID)bfGetProcAddress((HMODULE)hRenderDLL, "bf_rad_blur_pass1");
	bf_rad_blur_pass2 = (BF_1INT_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_rad_blur_pass2");
	bf_init_radblur   = (BF_1INT_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_init_radblur");

	//create_ds_obj = (BF_2INTS_INT) GetProcAddress((HMODULE)hRenderDLL, "create_ds_obj");
	//show_ds = (BF_1INT_VOID) GetProcAddress((HMODULE)hRenderDLL, "show_ds");

	bf_setcolor = (BF_4F_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_setcolor");
	bf_enable_textures = (BF_NO_ARGS_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_enable_textures");
	bf_disable_textures = (BF_NO_ARGS_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_disable_textures");

	bf_enable_lighting = (BF_NO_ARGS_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_enable_lighting");
	bf_disable_lighting = (BF_NO_ARGS_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_disable_lighting");

	bf_activate_context = (BF_ACT_CTXT) bfGetProcAddress((HMODULE)hRenderDLL, "bf_activate_context");
	bf_set_raster_mode = (BF_1INT_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_set_raster_mode");
	bf_set_share_rc = (BF_ACT_CTXT) bfGetProcAddress((HMODULE)hRenderDLL, "bf_set_share_rc");

	bf_set_radblur = (BF_1INT_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_set_radblur");
	bf_set_shownormals = (BF_1INT_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_set_shownormals");	

	bf_init_ortho_ex = (BF_4F_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_init_ortho_ex");
	bf_draw_line = (BF_4F_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_draw_line");

	bf_query_state = (BF_NO_ARGS_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_query_state");
	bf_draw_env = (BF_RENDERQ) bfGetProcAddress((HMODULE)hRenderDLL, "bf_draw_env");

	bf_init_sel_mode = (BF_3INTS_INT) bfGetProcAddress((HMODULE)hRenderDLL, "bf_init_sel_mode");
	bf_init_sel_mode_ortho = (BF_SEL_ORTHO) bfGetProcAddress((HMODULE)hRenderDLL, "bf_init_sel_mode_ortho");
	bf_exit_sel_mode = (BF_INTPP_INT) bfGetProcAddress((HMODULE)hRenderDLL, "bf_exit_sel_mode");

	bf_ft_dlist = (BF_FT_DLIST) bfGetProcAddress((HMODULE)hRenderDLL, "bf_ft_dlist");
	bf_tprintf  = (BF_TPRINTF) bfGetProcAddress((HMODULE)hRenderDLL, "bf_tprintf");
	bf_tprintf3d  = (BF_TPRINTF3D) bfGetProcAddress((HMODULE)hRenderDLL, "bf_tprintf3d");
	bf_ttf_font = (BF_TTF_FONT) bfGetProcAddress((HMODULE)hRenderDLL, "bf_ttf_font");

	bf_apply_lights = (BF_NO_ARGS_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_apply_lights");
	bf_update_light = (BF_LIGHT_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_update_light");

	bf_draw_billboard = (BF_BILLBOARD) bfGetProcAddress((HMODULE)hRenderDLL, "bf_draw_billboard");

	bf_activate_context_x = (BF_ACT_CTXT) bfGetProcAddress((HMODULE)hRenderDLL, "bf_activate_context_x");
	bf_set_viewport = (BF_VIEWPORT) bfGetProcAddress((HMODULE)hRenderDLL, "bf_set_viewport");

	bf_render_actor = (BF_ACTOREX) bfGetProcAddress((HMODULE)hRenderDLL, "bf_render_actor");

	bf_ft_init = (BF_NO_ARGS_INT) bfGetProcAddress((HMODULE)hRenderDLL, "bf_ft_init");
	bf_ft_kill = (BF_NO_ARGS_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_ft_kill");
	bf_ft_addfont = (BF_FTADDFONT) bfGetProcAddress((HMODULE)hRenderDLL, "bf_ft_addfont");
	bf_ft_print = (BF_FTPRINT) bfGetProcAddress((HMODULE)hRenderDLL, "bf_ft_print");
	bf_ft_font_remove = (BF_NO_ARGS_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_ft_font_remove");
    
	bf_reload_shaders = (BF_NO_ARGS_VOID) bfGetProcAddress((HMODULE)hRenderDLL, "bf_reload_shaders");

	zlogthisx("Done. Initializing DLL. dll_init = 0x%x\n",dll_init);
	

	// sends the addresses of the functions that are exported to the rendering DLL
	// see bluefire.h and the definition of BF_DLL_EXPORT type for more info.
	// Possibly make it to where the rendering DLL uses GetProcAddress instead of doing this??
	BF_DLL_EXPORT skulzy = {
		
		BLUEFIRE_BUILD,
		&mainscr,

		// exported functions
		begin_func_trace,
		end_func_trace,
		zlogfile_wrt,
		bf_create_window,
		bf_get_timer_ptr,
		bf_get_secs,
		bf_getobject,
		bf_getobj_num,
		bf_obj_count,
		bf_getimg,
		bf_getimg_num,
		bf_img_count,
		bf_getlight,
		bf_getlight_num,
		bf_light_cnt,
		bf_ready,
		bf_set_ready,
		bf_getpoly,
		bf_poly_cnt,
		go_down
	};

	dll_init(&skulzy);		// initialize it, nig

	zlogthisx("Bound to %s",dll_name);

	END_FUNC
	return 0;
}


/***

bfGetProcAddress()

	Description:
		A wrapper for the Win32 GetProcAddress() function. It checks
		whether or not the address returned by GetProcAddress() is
		valid or not.
		For more info on the GetProcAddress() function, refer to the
		Win32 SDK or MSDN.

	Arguments:
		HMODULE	hModule		DLL module handle
		LPCSTR lpProcName	Name of the function to get the address of

	Return:
		address of specified function, if found (FARPROC)		

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
FARPROC bfGetProcAddress(HMODULE hModule, LPCSTR lpProcName) {

	FARPROC bizzie;
	bizzie = GetProcAddress(hModule, lpProcName);

	if(!bizzie) {
		zlogthis("error: fatal: Unable to get address for procedure \"%s\".\n",lpProcName);
		MessageBox(NULL,"GetProcAddress error!","BFW Critical Error",0);
		exit(255);
	}

	return bizzie;
}

