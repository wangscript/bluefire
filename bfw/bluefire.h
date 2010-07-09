/*

  Bluefire: Global Header

  (c) Copyright 2004-2010 Jacob Hipps

*/


/******************************************************************************
 Global defines and compile options
*******************************************************************************/

#define MAX_NODES		512			// maximum pool size (obj, img, snd)
#define MAX_LIGHTS		128			// max light sources
#define MAX_SCRIPTS		2048		// max scripts
#define FCACHE_SIZE		256			// size of font cache (queues font writes until post-render)
#define LOG_ENABLE					// enable log support
//#define CHECKPOINTS_ENABLED			// checkpoint support
//#define SHOW_PIXEL_FORMATS_IN_LOG	// print list of supported pixel formats in logfile
#define CONFIG_FILENAME	"bfw.ini"	// filename with config options for Rendering system
#define BF_WITH_SHADERS			// enable shader support
//#define LF_NOFLUSH				// do not flush the logfile after each write
#define ENABLE_CONSOLE_SUPPORT	// enable console logging support (old-style Read-only)

//#define ENABLE_NEW_CONSOLE		// enable new console support
//#define DISABLE_EXCEPTION_HANDLING	// disable exception handling
//#define DISABLE_CALL_TRACE		// disable call tracing and ztrackme() macros
//#define DUMP_RAW_LMAPS			// dump all rendered lightmaps to RAW RGB8 files

/******************************************************************************
 BFZ Type Definitions
*******************************************************************************/

#define BFC_IMG		1
#define BFC_GEO		2
#define BFC_OBJ		3
#define BFC_SND		4
#define BFC_AUDIO	4
#define BFC_MUS		4
#define BFC_MOV		6
#define BFC_ANI		7
#define BFC_ENV		8
#define BFC_EXEC	9

/******************************************************************************
 Rasterization constants
*******************************************************************************/

#define BF_FILL			0
#define BF_POINT		1
#define BF_LINE			2
#define BF_WIREFRAME	2

// Bluefire Projection modes

#define BF_MM_PERSPECTIVE	0
#define BF_MM_ORTHO			1

// Bluefire light types
#define BF_LIGHT_POINT		0
#define BF_LIGHT_DIRECTION	1
#define BF_LIGHT_SPOT		2
#define BF_LIGHT_AMBIENT	3
#define BF_LIGHT_AREA		4
#define BF_LIGHT_SPECULAR	5

// Bluefire Editor Selection Modes
#define BFE_SELMODE_ENVPOLY		0
#define BFE_SELMODE_OBJ			1
#define BFE_SELMODE_LIGHTS		2
#define BFE_SELMODE_ACTORS		3

// State control bit masks
#define BF_STATE_LIGHTS			1
#define BF_STATE_TEXTURE		2

// Render mode control
#define BF_RMODE_GET_STATE		-1	// Not an actual mode. When given to
									// bfr_set_mode() it returns the current
									// render mode w/o making a change
#define BF_RMODE_FULL			0	// Full T&L
#define BF_RMODE_STENCODE		1	// Encodes state information as surface colors
									// R = lighting enabled, G = texturing enabled
#define BF_RMODE_NORMALS		2	// Encodes normal information as surface colors
#define BF_RMODE_NOTL			3	// Disables T&L, uses color only

// Alpha/blending mode control
#define BF_AMODE_GET_STATE		-1	// Like above. Returns current blend mode.
#define BF_AMODE_NORMAL			0	// Normal. Uses texture RGBA channels.
#define BF_AMODE_LUMA			1	// Uses texture alpha channel multiplied by the current color


// Macros

#ifdef CHECKPOINTS_ENABLED
 #define CHECKPOINT		zlogthis(">>>>>>>>>> CHECK THIS! Line %i <<<<<<<<<<\n",__LINE__)
#else
 #define CHECKPOINT		{ __asm{ nop }; }
#endif

#ifndef DISABLE_CALL_TRACE
 #define BEGIN_FUNC(fn)	begin_func_trace(fn,__LINE__,__FILE__);
 #define END_FUNC		end_func_trace();
 #define ztrackme(s)	debug_linetrack(__FILE__,__LINE__,s)
#else
 #define BEGIN_FUNC(fn)	{ __asm{ nop }; }
 #define END_FUNC		{ __asm{ nop }; }
 #define ztrackme(s)	{ __asm{ nop }; }
#endif

#define zlogthis		zlogfile_wrt
#define zlogthisx		zlogfile_wrtx

#define bf_obj_free		bf_freeobject
#define bf_obj_freeall	bf_freeobjpool
#define bf_obj_add		bf_addobject
#define bf_obj_getptr	bf_getobject
#define bf_obj_getnum	bf_getobj_num
//bf_obj_count
#define bf_img_add		bf_addimg
#define bf_img_getptr	bf_getimg
#define bf_img_getnum	bf_getimg_num
//bf_img_count	
#define bf_env_addpoly	bf_add_env_poly
#define bf_env_getpoly	bf_getpoly
#define bf_env_count	bf_poly_cnt
#define bf_env_flush	bf_flush_env

// External Dependencies (includes)

// FreeType2
#include <ft2build.h>
#include FT_FREETYPE_H



// Types

// Configuration spec
typedef struct {
	char varname[32];
	char value[128];
} BF_CONFIG_SPEC;

typedef struct {
	char	signature[3];
	char	datatype;
	long	cdata_sz;
	long	ddata_sz;
	long	crc32;
	char	resid[64];
} BFZ_CHUNK_HEADER;


typedef struct {
	char	filez[128];
	FILE	*fptrx;
} BFZ_FILE;

typedef struct {
	float x, y, z;
	float r, g, b, a;
	bool bSelected;
} BF_ACTOR_DATA;

typedef struct {
	unsigned short vectors[3];
	unsigned short flags;
	float u, v;		// texture coordinates
} POLYINFO;

typedef struct {
	float x, y, z;	// vector
} VTEXINFO;

typedef struct {
	float x, y, z;		// vector
	float u, v;			// texture coordinates
} VTEXINFOX;

typedef struct {
	float max_xx, max_xy, max_xz; // max x
	float min_xx, min_xy, min_xz; // min x
	float max_yx, max_yy, max_yz; // max y
	float min_yx, min_yy, min_yz; // min y
	float max_zx, max_zy, max_zz; // max z
	float min_zx, min_zy, min_zz; // min z
} BBOX;

typedef struct {	
	char name[128];			// surface name	
	float trans;			// transparency/alpha
	float spec;				// specularity
	float gloss;			// glossiness
	float diff;				// diffusion
	float refl;				// reflection
	float cr, cg, cb;		// color RGB
	struct {				
		float x, y, z;
	} rota;					// all textures: rotation
	struct {				
		float x, y, z;
	} size;					// all textures: scaling
	struct {				
		float x, y, z;
	} cntr;					// all textures: rotation
	int  scount;			// number of polygons associated with the surface
	char filename[256];		// color texture filename
	int  snum;				// surface number
} SURFACEINFO;

typedef struct {
	unsigned short vectors[3];	// vectors (index number to coordinate triad)
	float nx, ny, nz;			// surface normal
	float tv, th;				// texture coordinates
	unsigned short surface;		// surface number
	unsigned short id;			// original polygon number (before tripling)	
} POLYINFOX;

typedef struct {
        int fullscreen;         // this should usually be set to true
								// (true = fullscreen, false = windowed)
        int width, height, bpp; // screen parameters
        int lod;				// level of detail
		int blursteps;			// number of steps in radial blur
} z_config;


typedef struct {
	#ifdef BF_DIRECTX
	LPDIRECT3DDEVICE9 g_pD3DDevice;
	#else
	HGLRC	hRC;									// Permanent Rendering Context
	#endif
	HDC		hDC;									// Private GDI Device Context
	int 	width, height;							// Viewpoint size
	float	bgR, bgG, bgB;							// Background color
	int		raster_mode;
	int		flags;									// flags
} ESM_RENDERZ;


// flag definitions

#define BF_RZ_LMAPS_ENABLED		1


typedef struct {
	float x, y, z;
	float width, height;
	unsigned int texid; // texture id
} BF_BLBRD;

typedef struct {
	float x, y, z;
	float ny, np, nr;
} BF_WPD;

typedef struct {
	FT_Face face;
	int sz;
	int glyph_sz;
	unsigned int tex_num;
	int dlist;
	int start;	
	int kern[256];
	int ybear[256];
	char *facedex;
} BF_FT_FONT;

typedef struct {
	struct {
		float x, y, z;
	} vertex[3];
	struct {
		float x, y, z;
	} normal;
	struct {
		float u, v;			// texture coordinates
	} tex[3];
} BF_TRIANGLE;

typedef struct {
	struct {
		float x, y, z;		// verticies
	} vertex[3];
	struct {
		float x, y, z;		// normal vector
	} normal;
	struct {
		float u, v;			// texture coordinates
	} tex[3];
	struct {
		float u, v;			// lightmap texture coordinates
	} lm_tex[3];
	float opacity;			// opacity
	unsigned short lm_res;	// lightmap resolution
	unsigned short lm_ratio;// lightmap coordiates/world coordinates ratio
	int df_texdex;			// diffuse texture index
	int lm_texdex;			// lightmap texture index
	int flags;				// flags
} BF_TRIANGLE_EX;

// BF_TRIANGLE_EX flags

#define BF_TRI_SELECTED		1	// polygon selected
#define BF_TRI_UNLIT		2	// unlit: always show at full intensity
#define BF_TRI_NONSOLID		4	// partial non-solid: does not cause object or sound collision								
#define BF_TRI_FULLNS		8	// fully non-solid: does not cause any type of collision								
#define BF_TRI_CALCTRANS	16	// raytrace through this polygon and take into account
								// its transparency
#define BF_TRI_SEMISOLID	32	// semi-solid: does not cause BSP collision
#define BF_HAS_LMAP			64	// has a lightmap (lm_texdex is valid)
								

typedef struct {
	int  index;				// index
	char datatype;			// data type (BFC_*)
	char res_id[128];		// texture identifier "package_name:res_id"
} BF_RESDATA_EX;

typedef struct {
	int  index;				// index
	char res_id[128];		// texture identifier "package_name:res_id"
	float x, y, z;			// position
	float nx, ny, nz;		// normalized rotation vector
	char prv_data[128];		// private data 
} BF_OBJDATA_EX;

typedef struct {
	int  version;			// version identifier
	char id[128];			// environment descriptor
	int  textures;			// number of textures in RESDATA_EX array
	int  objects;			// number of objects in OBJDATA_EX array
	int  scripts;			// number of scripts in RESDATA_EX array
	int  polycount;			// number of polygons in TRIANGLE_EX array
	int  flags;				// flags (see below)
	int  sz_custom;			// size of custom data (bytes)
} BF_ENVIRON_DATA;

// BF_ENVIRON_DATA flags

#define	BF_ENV_MUSIC		1	// environment utilizes background music
								// found in RESDATA_EX[0]. If bAutomated is true,
								// the automation script must provide the engine
								// with the music resource. This flag is mutually-exculsive
								// with BF_ENV_AUTOMATED
#define BF_ENV_AUTOMATED	2	// environment is self-automated and non-interactive
								// (eg. a demo/intro). Automation script is contained
								// in RESDATA_EX[0]



typedef struct {						// timer information structure
	__int64 frequency;					// Timer Frequency
	__int64 performance_timer_start;	// Performance Timer Start Value
	__int64 performance_timer_elapsed;	// Performance Timer Elapsed Time
} BF_TIMER_INFO;


typedef struct {
	char seg_name[32];
	int seg_type;
	int seg_sz;
	char *seg_data;
} BF_SCRIPT_SEG;

typedef struct {
	char res_id[256];			// resource id (fully qualified, ie. "srcfile:resname")
	int num_segs;				// number of segments
	BF_SCRIPT_SEG *segs[128];	// segment index
} BF_SCRIPT;

// BF_SCRIPT_SEG types

#define BFI_SCRIPT_T_UNIFIED_TXT		1		// unified text
#define BFI_SCRIPT_T_UNIFIED_BC			2		// unified bytecode
#define BFI_SCRIPT_T_MODULAR_TXT		3		// modular text (specific function)
#define BFI_SCRIPT_T_MODULAR_BC			4		// modular bytecode (specific function)
#define BFI_SCRIPT_T_DUNI_TXT			5		// discrete unified, text component
#define BFI_SCRIPT_T_DUNI_BC			6		// discrete unified, bytecode component
#define BFI_SCRIPT_T_DMOD_TXT			7		// modular discrete, text component
#define BFI_SCRIPT_T_DMOD_BC			8		// modular discrete, bytecode component



typedef struct {
	struct {
		float x, y, z;		// position
	} pos;
	struct {
		float r, p, y;		// orientation
	} orient;
	struct {
		float x, y, z;		// scaling
	} scale;
	bool bEnabled;		// consider object?
	bool bVisible;		// visible?
	bool bHighPriority; // object is important
	bool bLowPriority;	// object is expendable
	bool bRenderLast;	// render object at top of z-order
	bool bRenderFirst;	// render object at bottom of z-order
	bool bDynamic;		// object data can be changed dynamically; do not
						// compile a display list on object load
	bool bNoCull;		// do not cull backsides; object has dual-sided polygons
	bool bNoLight;		// object is not affected by lighting (eg. unlit)
	bool bActive;		// object is animate or has scripts that must be
						// executed
	bool bSoundBarrier;	// object blocks sound
	bool bMirror;		// object is fully reflective (same as lighting.refl = 1.0f)
	bool bNoBSP;		// object does not affect BSP tree! (CAUTION: this will cause
						// the object to be drawn ALWAYS, even if it is not visible!)
	bool bLight;		// object emittes light
	bool bRadBlur;		// object utilizes independent radial blur

	// debug flags
	bool bZiggy;		// special group 1
	bool bAllieCat;		// special group 2 (render b-box)
	bool bYummy;		// special group 3
	bool bIcey;			// special group 4 
	bool bTofu;			// special group 5

	// ObjX
	bool bObjX;			// is it in ObjX format? >>> OBSOLETE. ALL objects should be in ObjX format!!
	int surfaces;		// number of surfaces (different textures) in object
	bool bDuplicate;	// uses same vertex and surface data as another object
	char master[64];	// master, if bDuplicate is set
	char parent[64];	// parent, NULL if top-level
	float opacity;

	SURFACEINFO *surfdex;		// surface data
	BF_TRIANGLE *vtexinfo[64];	// vertex information						
	
	// Lightmaps
	int lm_format;		// lightmap format
						// 0 = No lightmaps
						// 1 = RGB8 lightmaps [24-bits/px] (default)
						// 2 = RGBA4 lightmaps [16-bits/px]
						// 3 = CY8 lightmaps (greyscale) [8-bits/px]
						// 4 = CY4 lightmaps (greyscale) [4-bits/px]
	char **lightmaps;	// pointer to lightmap array
	int lm_res;			// lightmap resolution (pixels/meter)
						// (4, 8, 16, 32, 64, 128, 256, 512, 1024)
	int obj_num;		// object number (for vertex/texture data)
	char name[64];		// object unique identifier
} BF_RNODE;

typedef struct {	
	int lumens;			// luminous flux (lumens)
	float r, g, b, a;	// color/alpha
	float x, y, z, inf;	// position
	float nx,ny,nz;		// vector normal (for spot lights)
	int l_type;			// 0 = point
						// 1 = direction
						// 2 = spot
						// 3 = ambient
						// 4 = area
						// 5 = specular
	bool bEnabled;		// enabled?
	bool bDynamic;		// affect dynamic shadowing/lighting?
	bool bNegative;		// is it a subtractive/negative light?
	bool bNoLMaps;		// does not effect lightmaps?
	char resid[64];		// resource id/name
} BF_LIGHT;

typedef struct {
	char resid[64];		// resource id
	char respkg[64];	// resource pkg. filename
	union {
		unsigned int tex_id;			// texture identifier (OpenGL)
		#ifdef __DIRECT3D
		LPDIRECT3DTEXTURE9 *ppTexture;	// texture pointer (Direct3D)
		#endif
	};
	int width;			// width
	int height;			// height
} BF_TEXNODE;

typedef struct {
        long height, width, planes, depth;
        int type;
        void *data_ptr;
} esm_picture;


typedef struct {

	// bluefire build (eg. 20040816)
	int build_num;

	// primary rendering context
	ESM_RENDERZ *renderx;


	// exported core functions
	void (*begin_func_trace)(char func_trace[], int line, char source[]);
	void (*end_func_trace)();
	int (*zlogfile_wrt)(const char *fmt, ...);
	HWND (*bf_create_window)(char title[], int xpos, int ypos, int fullscreen, int width, int height, WNDCLASSEX *wincl, HWND parent);
	BF_TIMER_INFO* (*bf_get_timer_ptr)();
	float (*bf_get_secs)();
	BF_RNODE* (*bf_getobject)(int nummy);
	int (*bf_getobj_num)(char obj_name[]);
	int (*bf_obj_count)();
	BF_TEXNODE* (*bf_getimg)(int nummy);
	int (*bf_getimg_num)(char img_name[]);
	int (*bf_img_count)();
	BF_LIGHT* (*bf_getlight)(int nummy);
	int (*bf_getlight_num)(char light_name[]);
	int (*bf_light_cnt)();
	int (*bf_ready)();
	void (*bf_set_ready)(int readz);
	BF_TRIANGLE_EX* (*bf_getpoly)(int num);
	int (*bf_poly_cnt)();
	void (*go_down)();
	void (*bf_exception)(int sig);

} BF_DLL_EXPORT;

/*
 Bluefire Superglobal exports
*/

/****************************************************
 Core functions
*****************************************************/

#if defined(BLUEFIRE_CORE) || defined(BLUEFIRE_CLNT)

#ifdef BLUEFIRE_CORE
#define BF_EXPORTZ __declspec(dllexport)
#else
#define BF_EXPORTZ __declspec(dllimport)
#endif

/*
  BF Core-only functions
*/
#ifdef _BF_MAIN

BF_EXPORTZ z_config mainconfig;
BF_EXPORTZ ESM_RENDERZ mainscr;

#endif

/*

  console.cpp
  Console upkeep functions

*/

#ifdef BLUEFIRE_CORE

int init_console();
int kill_console();
void add_text(char *text);
int execute_command(char *cmdtxt);

#endif

/*

  errors.cpp
  Function trace, Error, and Exception handling routines

*/

/*

  void reset_func_trace()

  Resets the function-calltrace stack
  return: nothing

*/
BF_EXPORTZ void reset_func_trace();

/*

  void begin_func_trace(char func_trace[])

  Adds a function to the function-calltrace stack
  arguments:
	char func_trace[]		Pointer to name of function
	int  line				Line number
	char source[]			Pointer to source filename
  return: nothing

  Note: This function should be invoked through the BEGIN_FUNC macro
*/
BF_EXPORTZ void begin_func_trace(char func_trace[], int line, char source[]);

/*

  void end_func_trace()

  Deletes the last function in the function-calltrace stack
  return: nothing

  Note: This function should be invoked through the END_FUNC macro

*/
BF_EXPORTZ void end_func_trace();

/*

  void debug_linetrack(char *filename, int linenum, char *msg)

  Sets a debug point for tracking trouble spots in source code.
	char *filename		Filename of source file
	int linenum			Line number of this tracker
	char *msg			Custom message
  return: nothing

  Note: This function should be invoked through the ztrackme() macro
        so that the filename and line number are automatically inserted.

*/
BF_EXPORTZ void debug_linetrack(char *filename, int linenum, char *msg);

/*

  int bf_catchme()

  Enable Bluefire's exception handler
  return: zero on success, non-zero on error

*/
BF_EXPORTZ int bf_catchme();


/*

  bfw.cpp
  Initialization, destruction, timer, and window functions

*/

/*

  int zlogfile_wrt(const char *fmt, ...)

  Similar to printf, except outputs text to logfile and/or the console
  return: 0

*/
BF_EXPORTZ int zlogfile_wrt(const char *fmt, ...);
BF_EXPORTZ int zlogfile_wrtx(const char *fmt, ...);

/*

  int conprintf(const char *fmt, ...)

  Similar to printf, except outputs text to the console
  return: 0

*/
BF_EXPORTZ int conprintf(const char *fmt, ...);

/*

  int bf_init(void (__cdecl * go_downx)())

  Initialize Bluefire
  arguments:
	go_downx		Shutdown function callback
  return: zero on success, non-zero on error

*/
BF_EXPORTZ int bf_init(void (__cdecl * go_downx)());

/*

  void bf_shutdown()

  Shutdown Bluefire
  return: nothing

*/
BF_EXPORTZ void bf_shutdown();

/*

  int bf_bind_render_dll(char *dllname);

  Binds to the 3D rendering DLL
  arguments:
	dllname		Pointer to null-terminated ASCII string containing the
				name of the rendering DLL to bind to
  return: zero on success, non-zero on error

*/
BF_EXPORTZ int bf_bind_render_dll(char *dllname);


/*

  HWND bf_create_window(char title[], int xpos, int ypos, int fullscreen, int width, int height, WNDCLASSEX *wincl)

  Create a new window.
  arugments:
	title		Pointer to window title
	xpos		X Position
	ypos		Y Position
	fullscreen	zero = Windowed, non-zero = Fullscreen
	width		Width
	height		Height
	wincl		Pointer to window class information
	parent		Handle of parent window or GetDesktopWindow() or HWND_DESKTOP
  return: handle to new window

*/
BF_EXPORTZ HWND bf_create_window(char title[], int xpos, int ypos, int fullscreen, int width, int height, WNDCLASSEX *winclx, HWND parent);

/*

  void bf_kill_window(HWND winz, WNDCLASSEX *winclz)

  Destroys the specified window and unregisters its associated class
  arguments:
	HWND winz			Handle of window to destroy
	WNDCLASSEX *winclz	Pointer to class information for window
  return: nothing

*/
BF_EXPORTZ void bf_kill_window(HWND winz, WNDCLASSEX *winclz);

/*

  int bf_init_timer()

  Initialize the timer
  return: zero on success, non-zero on error

*/
BF_EXPORTZ int bf_init_timer();

/*

  float bf_get_secs()

  Get the current timer offset in seconds
  return: offset in seconds (fractional)

*/
BF_EXPORTZ float bf_get_secs();

/*

  BF_TIMER_INFO* bf_get_timer_ptr()

  Return pointer to timer structure
  return: pointer to timer struct

*/
BF_EXPORTZ BF_TIMER_INFO* bf_get_timer_ptr();


/*
  Object Pool functions
*/

/*

  int bf_format_objx(char *delerium, int obj_num)

  Reads object information (ObjX) from the data stream specified and
  puts it into an object in the object pool
  return: zero on success, non-zero on error

*/
BF_EXPORTZ int bf_format_objx(char *delerium, int obj_num);

/*

  void bf_freeobject(int nummy)

  Frees an object from the object pool
  return: none

*/
BF_EXPORTZ void bf_freeobject(int nummy);

/*

  void bf_freeobjpool()

  Flushes the object pool (frees all objects)
  return: none

*/
BF_EXPORTZ void bf_freeobjpool();

/*

  int bf_addobject()

  Allocates memory for an object in the object pool
  return: index number of the object added

*/
BF_EXPORTZ int bf_addobject();


/*

  BF_RNODE* bf_getobject(int nummy)

  arguments:
	int nummy			Index of node
  return: pointer to object node at specified index (nummy)

*/
BF_EXPORTZ BF_RNODE* bf_getobject(int nummy);

/*

  int bf_getobj_num(char obj_name[])

  Returns the number of the object with the specified name.
  arguments:
	char obj_name[]		Name of object
  return: index of object on success, -1 on error (object not found)

*/
BF_EXPORTZ int bf_getobj_num(char obj_name[]);

/*

  int bf_obj_count()

  return: number of objects in the object pool

*/
BF_EXPORTZ int bf_obj_count();

/*

  int bf_addimg()

  Allocates memory for an image/texture in the image pool
  return: index number of the image added

*/
BF_EXPORTZ int bf_addimg();

/*

  BF_TEXNODE* bf_getimg(int nummy)

  arguments:
	int nummy			Index of node
  return: pointer to image node at specified index (nummy)

*/
BF_EXPORTZ BF_TEXNODE* bf_getimg(int nummy);

/*

  int bf_getimg_num(char img_name[])

  Returns the number of the image/texture with the
  specified name.
  arguments:
	char img_name[]		Name of image/texture
  return: index of image on success, -1 on error (image not found)

*/
BF_EXPORTZ int bf_getimg_num(char img_name[]);

/*

  int bf_img_count()
  
  return: number of images in the image/texture pool

*/
BF_EXPORTZ int bf_img_count();

/*

  int bf_ready()
  
  return: 1 on ready, 0 on not ready

*/
BF_EXPORTZ int bf_ready();

/*

  void bf_set_ready(int readz)
  
  arguments:
	int readz		bf_ready value
	return: nothing

*/
BF_EXPORTZ void bf_set_ready(int readz);

/*

  bfz_read.cpp

  BFZ resource loading functions

*/

/*

  int bfz_open(char filename[], BFZ_FILE *bfzp)

  Opens a file for use with the BFZ functions
  arguments:    
	char filename[]		Pointer to name of file
	BFZ_FILE *bfzp		Pointer to BFZ file struct
  return: zero on success, non-zero on error

*/
BF_EXPORTZ int bfz_open(char filename[], BFZ_FILE *bfzp);

/*

  void bfz_close(BFZ_FILE *bfz_ptr)

  Closes a file used by the BFZ routines.
  arguments:
    BFZ_FILE *bfz_ptr	File pointer for BFZ file
  return: nothing

*/
BF_EXPORTZ void bfz_close(BFZ_FILE *bfzp);

/*

  int bfz_loadimg(BFZ_FILE *bfz_ptr, char res_name[], int nummy)

  Loads an image resource from the specified file.
  arguments:
    BFZ_FILE *bfz_ptr	File pointer for BFZ file
	char res_name[]		Pointer to name of resource ID
	int nummy			Texture pool index number
  return: index of new texture on success, -1 on error

*/
BF_EXPORTZ int bfz_loadimg(BFZ_FILE *bfzp, char res_name[], int nummy);

/*

  int bfz_loadobj(BFZ_FILE *bfz_ptr, char res_name[], int nummy)

  Loads an object resource from the specified file.
  arguments:
    BFZ_FILE *bfz_ptr	File pointer for BFZ file
	char res_name[]		Pointer to name of resource ID
	int nummy			Object pool index number
  return: index of new object on success, -1 on error

*/
BF_EXPORTZ int bfz_loadobj(BFZ_FILE *bfzp, char res_name[], int nummy);


/*

  int bfz_loadexec(BFZ_FILE *bfz_ptr, char res_name[])

  Loads a script resource from the specified file.
  arguments:
    BFZ_FILE *bfz_ptr	File pointer for BFZ file
	char res_name[]		Pointer to name of resource ID	
  return: index of new object on success, -1 on error

*/
BF_EXPORTZ int bfz_loadexec(BFZ_FILE *bfzp, char res_name[]);

/*

  Light pool functions
  lights.cpp

*/

/*

  int bf_addlight()

  return: index of added light

*/
BF_EXPORTZ int bf_addlight();


/*

  BF_LIGHT* bf_getlight(int nummy)

  arguments:
	int nummy			Index of node
  return: pointer to light node at specified index (nummy)

*/
BF_EXPORTZ BF_LIGHT* bf_getlight(int nummy);

/*

  int bf_getlight_num(char light_name[])

  Returns the number of the light with the
  specified name.
  arguments:
	char light_name[]		Name of image/texture
  return: index of light on success, -1 on error (light not found)

*/
BF_EXPORTZ int bf_getlight_num(char light_name[]);

/*

  int bf_light_cnt()
  
  return: number of lights in the light pool

*/
BF_EXPORTZ int bf_light_cnt();



/*

	Environmental Polygon functions

*/


/*

  int bf_add_env_poly(BF_TRIANGLE_EX *polyptr)

  Allocates memory for another polygon and copies info
  to the env_polydex. If polyptr is NULL, then it is
  zeroed.
  arguments:
	BF_TRIANGLE_EX *polyptr		Pointer to polygon information to initialize with.
  return: index of new polygon on success, -1 on error

*/
BF_EXPORTZ int bf_add_env_poly(BF_TRIANGLE_EX *polyptr);

/*

  BF_TRIANGLE_EX* bf_getpoly(int num)

  Returns a pointer to the specified environment polygon  
  arguments:
	int num		Index of polygon
  return: pointer to polygon on success, NULL on error

*/
BF_EXPORTZ BF_TRIANGLE_EX* bf_getpoly(int num);


/*

  int bf_flush_env()

  Flushes all polygons from env_polydex and resizes the
  cache to accomodate only one pointer entry.

  return: nothing
*/
BF_EXPORTZ void bf_flush_env();

/*

  int bf_poly_cnt()

  return: number of environment polygons
*/
BF_EXPORTZ int bf_poly_cnt();

/*

  void bf_dsort_env(float x, float y, float z)

  Sort environment polygons from back to front.
  arguments:
	float	x		World x coordinate
	float	y		World y coordinate
	float	z		World z coordinate
  return: nothing
*/
BF_EXPORTZ void bf_dsort_env(float x, float y, float z);


//BF_EXPORTZ int bf_ft_init();
//BF_EXPORTZ int bf_ft_open(char* filename, BF_FT_FONT* font_ptr);
//BF_EXPORTZ int bf_ft_rasterize(BF_FT_FONT* font_ptr, int sz);
//BF_EXPORTZ void bf_ft_kill_font(BF_FT_FONT *font_ptr);


BF_EXPORTZ int bf_gen_lmap(BF_TRIANGLE_EX *triangle, int ident);

/*

Script functions
(script.cpp)

*/

BF_EXPORTZ int bfi_init();
BF_EXPORTZ int bfi_kill();
BF_EXPORTZ int bfi_load_script(char *script_txt, int script_sz, char* res_id);
BF_EXPORTZ int bfi_exec_script(int nummy, char* funcx);

BF_EXPORTZ int bf_script_add();
BF_EXPORTZ BF_SCRIPT* bf_script_getptr(int nummy);
BF_EXPORTZ int bf_script_getnum(char script_name[]);
BF_EXPORTZ int bf_script_cnt();
BF_EXPORTZ void bf_script_free(int nummy);
BF_EXPORTZ void bf_script_flush();

BF_EXPORTZ int bf_script_ft_write(int fontdex, int r, int g, int b, int a, int x, int y, char *fmt, ...);
BF_EXPORTZ int bf_script_ft_clear();
BF_EXPORTZ int bf_script_ft_render();

/*

Settings File Functions
(config.cpp)

*/

BF_EXPORTZ int bf_load_config();
BF_EXPORTZ char* bf_config_str(char *varname);
BF_EXPORTZ int bf_config_int(char *varname);
BF_EXPORTZ float bf_config_float(char *varname);


/*

Audio
(audio.cpp)

*/
BF_EXPORTZ int bf_init_audio();
BF_EXPORTZ int bf_kill_audio();
BF_EXPORTZ int bf_al_test();


/*

Version/Build info
(version.cpp)

*/
BF_EXPORTZ char* bf_get_buildtime();
BF_EXPORTZ char* bf_get_version();
BF_EXPORTZ char* bf_get_build();
BF_EXPORTZ int   bf_get_buildi();

// BFW exception handler

BF_EXPORTZ void bf_exception(int sig);

#endif

#include "bf_render.h"

