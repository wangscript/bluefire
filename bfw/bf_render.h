/*

  Bluefire: Render DLL Header file

  (c) Copyright 2004-2010 Jacob Hipps

*/


// Macros and Defines
#define bfr_get_mode()	bfr_set_mode(BF_RMODE_GET_STATE)
#define bfr_get_blending()  bfr_set_blending(BF_AMODE_GET_STATE)


#ifdef _BF_MAIN
 #ifdef BLUEFIRE_CLNT
  #define BF_EXTERNZ __declspec(dllimport)
 #else
  #ifdef BLUEFIRE_CORE
   #define BF_EXTERNZ __declspec(dllexport)
  #else
   #define BF_EXTERNZ
  #endif
 #endif
#else
 #define BF_EXTERNZ extern
#endif

#ifdef BLUEFIRE_CORE
 #undef _USRDLL
#endif

#ifdef _USRDLL

// Imports (for DLLs)

// Active Rendering context
BF_EXTERNZ ESM_RENDERZ *renderx;

// Bluefire core imports
BF_EXTERNZ void (*begin_func_trace)(char func_trace[], int line, char source[]);
BF_EXTERNZ void (*end_func_trace)();
BF_EXTERNZ int (*zlogfile_wrt)(const char *fmt, ...);
BF_EXTERNZ HWND (*bf_create_window)(char title[], int xpos, int ypos, int fullscreen, int width, int height, WNDCLASSEX *wincl, HWND parent);
BF_EXTERNZ BF_TIMER_INFO* (*bf_get_timer_ptr)();
BF_EXTERNZ float (*bf_get_secs)();
BF_EXTERNZ BF_RNODE* (*bf_getobject)(int nummy);
BF_EXTERNZ int (*bf_getobj_num)(char obj_name[]);
BF_EXTERNZ int (*bf_obj_count)();
BF_EXTERNZ BF_TEXNODE* (*bf_getimg)(int nummy);
BF_EXTERNZ int (*bf_getimg_num)(char img_name[]);
BF_EXTERNZ int (*bf_img_count)();
BF_EXTERNZ BF_LIGHT* (*bf_getlight)(int nummy);
BF_EXTERNZ int (*bf_getlight_num)(char light_name[]);
BF_EXTERNZ int (*bf_light_cnt)();
BF_EXTERNZ int (*bf_ready)();
BF_EXTERNZ void (*bf_set_ready)(int readz);
BF_EXTERNZ BF_TRIANGLE_EX* (*bf_getpoly)(int num);
BF_EXTERNZ int (*bf_poly_cnt)();
BF_EXTERNZ void (*go_down)();
BF_EXTERNZ void (*bf_exception)(int sig);

// Exports (for DLLs)

#define BFEXPORT extern "C" __declspec(dllexport)

// dll entry and init (dll_main.cpp)

BFEXPORT int dll_init(BF_DLL_EXPORT *setz);

// font (font.cpp)

BFEXPORT void bf_ft_dlist(BF_FT_FONT *font_ptr, int start, int span, int dlist);
BFEXPORT int bf_tprintf(int x, int y, BF_FT_FONT *font_ptr, const char *fmt, ... );
BFEXPORT int bf_tprintf3d(float x, float y, float z, BF_FT_FONT *font_ptr, const char *fmt, ... );
BFEXPORT int bf_ttf_font(char *fontname, int start_glyph, int num_glyphs, int size, BF_FT_FONT* font_ptr);

// initialization and load (ogl.cpp/d3d.cpp)

BFEXPORT int  bf_init_3d(HWND winhand);
BFEXPORT void bf_resize_disp(int width, int height, HWND winhand);
BFEXPORT void bf_kill_3d(HWND winhand);

BFEXPORT int  bf_create_texture(int x, int y, int num);
BFEXPORT int  bf_delete_texture(int nummy);
BFEXPORT int bf_bind_texture(esm_picture *picdat, unsigned int *picptr);

// render (render.cpp)

BFEXPORT void bf_clrframe();
BFEXPORT void bf_endframe();
BFEXPORT int  bf_renderq(float x, float y, float z, float or, float oy, float op);

BFEXPORT void bf_init_ortho();
BFEXPORT void bf_exit_ortho();

BFEXPORT void draw_pic(float x, float y, int num);
BFEXPORT void draw_picx(float x, float y, float width, float height, float stretch, int num);
BFEXPORT int  draw_obj(int num);
BFEXPORT int  draw_objx(int nummy);

BFEXPORT void bf_rad_blur_pass1(int obj_num, int tex_num);
BFEXPORT void bf_rad_blur_pass2(int tex_num);
BFEXPORT void bf_init_radblur(int img_num);

BFEXPORT int create_ds_obj (int num, int listnum);
BFEXPORT void show_ds (int listnum);
BFEXPORT void delete_ds (int listnum, int glyphs);

BFEXPORT void bf_setcolor(float r, float g, float b, float a);

BFEXPORT void bf_enable_textures();
BFEXPORT void bf_disable_textures();

BFEXPORT int  bf_activate_context(ESM_RENDERZ *rendery);

BFEXPORT void bf_enable_lighting();
BFEXPORT void bf_disable_lighting();

BFEXPORT void bf_set_radblur(int state);
BFEXPORT void bf_set_shownormals(int state);

BFEXPORT void bf_set_raster_mode(int mode);
BFEXPORT void bf_set_share_rc(ESM_RENDERZ* rcontext);

BFEXPORT void bf_init_ortho_ex(float neg_x, float pos_x, float neg_y, float pos_y);
BFEXPORT void bf_draw_line(float x1, float y1, float x2, float y2);
BFEXPORT void bf_query_state();

BFEXPORT int bf_draw_env(float x, float y, float z, float or, float oy, float op);

BFEXPORT int bf_init_sel_mode(int x, int y, int rad);
BFEXPORT int bf_init_sel_mode_ortho(int x, int y, int rad, float neg_x, float pos_x, float neg_y, float pos_y);
BFEXPORT int bf_exit_sel_mode(int **buffy);

BFEXPORT void bf_apply_lights();
BFEXPORT void bf_update_light(BF_LIGHT *light);

BFEXPORT void bf_draw_billboard(BF_BLBRD *billy, BF_WPD *wpdx);

BFEXPORT int  bf_activate_context_x(ESM_RENDERZ *rendery);
BFEXPORT void bf_set_viewport(int vp1, int vp2, int vp3, int vp4, HWND hwndz);

BFEXPORT void bf_render_actor(BF_ACTOR_DATA *actorx);

// FreeType2 routines (font_ft.cpp)

BFEXPORT int  bf_ft_init();
BFEXPORT void bf_ft_kill();
BFEXPORT void bf_ft_addfont(const char * fname, unsigned int h);
BFEXPORT void bf_ft_print(int font_id, float x, float y, const char *fmt, ...);
BFEXPORT void bf_ft_font_remove();

// GLSL shader handling (shaders.cpp)

//BFEXPORT void bf_load_shader(char *filename);
//BFEXPORT void bf_unload_shaders();
BFEXPORT void bf_reload_shaders();
BFEXPORT int bfr_set_mode(int modectrl);
BFEXPORT int bfr_set_blending(int modectrl);

// Version & Build info (version.cpp)

BFEXPORT void  rdx_get_version(char *ver);
BFEXPORT void  rdx_get_buildtime(char *bt);
BFEXPORT void  rdx_get_build(char *bt);
BFEXPORT int   rdx_get_buildi();

#else


// Imports (for BF Core)

// Typedefs!! (this is so fucking confusing... TODO: write complicated macro)

#define BOOBOO __cdecl

typedef void (BOOBOO * BF_STR_VOID)(char *str1);
typedef void (BOOBOO * BF_NO_ARGS_VOID)();
typedef int  (BOOBOO * BF_NO_ARGS_INT)();

typedef void (BOOBOO * BF_1INT_VOID)(int int1);
typedef int  (BOOBOO * BF_1INT_INT)(int int1);
typedef void (BOOBOO * BF_2INTS_VOID)(int int1, int int2);
typedef int  (BOOBOO * BF_2INTS_INT)(int int1, int int2);
typedef int  (BOOBOO * BF_3INTS_INT)(int int1, int int2, int int3);
typedef int  (BOOBOO * BF_STRINT_INT)(char *str1, int int1);
typedef int  (BOOBOO * BF_STR2INTS_INT)(char *str1, int int1, int int2);
typedef int  (BOOBOO * BF_HWND_INT)(HWND hwnd);
typedef void (BOOBOO * BF_HWND_VOID)(HWND hwnd);
typedef void (BOOBOO * BF_4F_VOID)(float fl1, float fl2, float fl3, float fl4);

typedef int  (BOOBOO * BF_SHOWTEXTB)(int x, int y, int spacing, int font);
typedef int  (BOOBOO * BF_ADDTEXTB)(const char *textbuf, ...);
typedef int  (BOOBOO * BF_DRAWPIC)(float x, float y, int num);
typedef int  (BOOBOO * BF_DRAWPICX)(float x, float y, float width, float height, float stretch, int num);
typedef int  (BOOBOO * BF_RENDERQ)(float x, float y, float z, float or, float oy, float op);
typedef int  (BOOBOO * BF_EPRINTF)(int x, int y, int num, const char *fmt, ... );
typedef int  (BOOBOO * BF_LOADTTFONT)(char fontname[], int start_glyph, int num_glyphs, int size, int num);
typedef int  (BOOBOO * BF_LOADPNGFONT)(int texnum, int x_size, int y_size, int ascii_start, int num);
typedef int  (BOOBOO * BF_DLL_INIT)(BF_DLL_EXPORT *setz);

typedef void (BOOBOO * BF_RSIZE_VOID)(int int1, int int2, HWND winhand);
typedef int  (BOOBOO * BF_BIND_TEX)(esm_picture *srcptr, unsigned int *destptr);

typedef int  (BOOBOO * BF_ACT_CTXT)(ESM_RENDERZ *rendery);

typedef int  (BOOBOO * BF_SEL_ORTHO)(int x, int y, int rad, float neg_x, float pos_x, float neg_y, float pos_y);
typedef int  (BOOBOO * BF_INTPP_INT)(int** intpp);

typedef void (BOOBOO * BF_FT_DLIST)(BF_FT_FONT *font_ptr, int start, int span, int dlist);
typedef int  (BOOBOO * BF_TPRINTF)(int x, int y, BF_FT_FONT *font_ptr, const char *fmt, ... );
typedef int  (BOOBOO * BF_TPRINTF3D)(float x, float y, float z, BF_FT_FONT *font_ptr, const char *fmt, ... );
typedef int  (BOOBOO * BF_TTF_FONT)(char *fontname, int start_glyph, int num_glyphs, int size, BF_FT_FONT* font_ptr);

typedef void (BOOBOO * BF_LIGHT_VOID)(BF_LIGHT* light_ptr);

typedef void (BOOBOO * BF_BILLBOARD)(BF_BLBRD *billy, BF_WPD *wpdx);
typedef void (BOOBOO * BF_VIEWPORT)(int vp1, int vp2, int vp3, int vp4, HWND hwndz);

typedef void (BOOBOO * BF_ACTOREX)(BF_ACTOR_DATA *actorx);

typedef void (BOOBOO * BF_FTADDFONT)(const char * fname, unsigned int h);
typedef void (BOOBOO * BF_FTPRINT)(int font_id, float x, float y, const char *fmt, ...);


BF_EXTERNZ BF_DLL_INIT		dll_init;

BF_EXTERNZ BF_EPRINTF		eprintf;
BF_EXTERNZ BF_LOADTTFONT	load_ttfont;
BF_EXTERNZ BF_LOADPNGFONT	load_pngfont;

BF_EXTERNZ BF_HWND_INT		bf_init_3d;
BF_EXTERNZ BF_RSIZE_VOID	bf_resize_disp;
BF_EXTERNZ BF_HWND_VOID		bf_kill_3d;

BF_EXTERNZ BF_3INTS_INT		bf_create_texture;
BF_EXTERNZ BF_1INT_VOID		bf_delete_texture;
BF_EXTERNZ BF_BIND_TEX		bf_bind_texture;

BF_EXTERNZ BF_NO_ARGS_VOID	bf_clrframe;
BF_EXTERNZ BF_NO_ARGS_VOID	bf_endframe;
BF_EXTERNZ BF_RENDERQ		bf_renderq;

BF_EXTERNZ BF_NO_ARGS_VOID	bf_init_ortho;
BF_EXTERNZ BF_NO_ARGS_VOID	bf_exit_ortho;

BF_EXTERNZ BF_DRAWPIC		draw_pic;
BF_EXTERNZ BF_DRAWPICX		draw_picx;
BF_EXTERNZ BF_1INT_INT		draw_obj;
BF_EXTERNZ BF_1INT_INT		draw_objx;

BF_EXTERNZ BF_1INT_VOID		bf_init_radblur;
BF_EXTERNZ BF_2INTS_VOID	bf_rad_blur_pass1;
BF_EXTERNZ BF_1INT_VOID		bf_rad_blur_pass2;

BF_EXTERNZ BF_2INTS_INT		create_ds_obj;
BF_EXTERNZ BF_1INT_VOID		show_ds;
BF_EXTERNZ BF_2INTS_VOID	delete_ds;

BF_EXTERNZ BF_4F_VOID		bf_setcolor;

BF_EXTERNZ BF_NO_ARGS_VOID	bf_enable_textures;
BF_EXTERNZ BF_NO_ARGS_VOID	bf_disable_textures;

BF_EXTERNZ BF_ACT_CTXT		bf_activate_context;

BF_EXTERNZ BF_NO_ARGS_VOID	bf_disable_lighting;
BF_EXTERNZ BF_NO_ARGS_VOID	bf_enable_lighting;

BF_EXTERNZ BF_1INT_VOID		bf_set_raster_mode;
BF_EXTERNZ BF_1INT_VOID		bf_set_radblur;
BF_EXTERNZ BF_1INT_VOID		bf_set_shownormals;

BF_EXTERNZ BF_ACT_CTXT		bf_set_share_rc;

BF_EXTERNZ BF_4F_VOID		bf_init_ortho_ex;
BF_EXTERNZ BF_4F_VOID		bf_draw_line;

BF_EXTERNZ BF_NO_ARGS_VOID	bf_query_state;
BF_EXTERNZ BF_RENDERQ		bf_draw_env;

BF_EXTERNZ BF_3INTS_INT		bf_init_sel_mode;
BF_EXTERNZ BF_SEL_ORTHO		bf_init_sel_mode_ortho;
BF_EXTERNZ BF_INTPP_INT		bf_exit_sel_mode;

BF_EXTERNZ BF_FT_DLIST		bf_ft_dlist;
BF_EXTERNZ BF_TPRINTF		bf_tprintf;
BF_EXTERNZ BF_TPRINTF3D		bf_tprintf3d;
BF_EXTERNZ BF_TTF_FONT		bf_ttf_font;

BF_EXTERNZ BF_NO_ARGS_VOID	bf_apply_lights;
BF_EXTERNZ BF_LIGHT_VOID	bf_update_light;

BF_EXTERNZ BF_BILLBOARD		bf_draw_billboard;

BF_EXTERNZ BF_ACT_CTXT		bf_activate_context_x;
BF_EXTERNZ BF_VIEWPORT		bf_set_viewport;
BF_EXTERNZ BF_ACTOREX		bf_render_actor;

BF_EXTERNZ BF_NO_ARGS_INT	bf_ft_init;
BF_EXTERNZ BF_NO_ARGS_VOID	bf_ft_kill;
BF_EXTERNZ BF_FTADDFONT		bf_ft_addfont;
BF_EXTERNZ BF_FTPRINT		bf_ft_print;
BF_EXTERNZ BF_NO_ARGS_VOID	bf_ft_font_remove;

BF_EXTERNZ BF_NO_ARGS_VOID	bf_reload_shaders;
BF_EXTERNZ BF_1INT_INT		bfr_set_mode;
BF_EXTERNZ BF_1INT_INT		bfr_set_blending;

BF_EXTERNZ BF_STR_VOID		rdx_get_version;
BF_EXTERNZ BF_STR_VOID		rdx_get_buildtime;
BF_EXTERNZ BF_STR_VOID		rdx_get_build;
BF_EXTERNZ BF_NO_ARGS_INT	rdx_get_buildi;

#endif