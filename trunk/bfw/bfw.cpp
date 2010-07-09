/*


  Bluefire: Main functions

  (c) Copyright 2004-2009 Jacob Hipps

  IMPORTANT: Make sure that BLUEFIRE_CORE is defined!
			 (if compiling w/ Win32, it's already defined in the MSVC project file)

  Status: Working, Level A

  To-do:
		???

  Implements:
		Initialization routines
			
*/

#define _BF_MAIN

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../project_w/libpng/png.h"
#include "../project_w/zlib/zlib.h"
#include "bluefire.h"

// Superglobals

void (__cdecl * go_down)();		// go_down: point to shutdown procedure defined by client program

// Imported Globals
extern BF_TEXNODE *img_pool[];		// Image/texture pool
extern BF_RNODE *obj_pool[];		// Object pool
extern BF_TRIANGLE_EX **env_polydex;// Environmental Polygon pool

extern int bf_imgpool_count;		// image count
extern int objects_otl;				// object count
extern int bf_poly_count;			// environment polygon count
extern int env_capacity;			// environment polygon capacity


int sounds_otl;				// sounds/audio count
int ready;					// are we ready?


// Globals

BF_TIMER_INFO bf_timer;		// timer info
HANDLE wconsole;			// console handle
FILE *logfile;				// logfile pointer
extern HINSTANCE hRenderDLL;// Render DLL instance
HINSTANCE hBluefireDLL = 0;	// this program

// Imports
extern char trace_funcs[][64];
extern char trace_src[][64];
extern int  trace_line[];
extern int  trace_depth;


// Local functions
int bf_bind_render_dll(char *dllname);

/***

bf_init()

	Description:
		Initializes Bluefire.
		Opens the logfile (if compiled-in), the console (if compiled-in),
		sets up exception handling for Bluefire (NOT client program), initializes the function
		trace stack, initializes the resource pools (objects, images, etc.).

	Arguments:
		void (__cdecl *go_downx)		Pointer to shutdown function.
										This function is called by Bluefire:
										 AFTER bf_shutdown() is called (during normal operation)
										     OR
										 If go_down() is called directly (in case of an error)
	Return:
		0 on success, non-zero on error

	Status:
		Working, Level B
	To-do:	
		Nothing right now. =]

***/

int bf_init(void (__cdecl * go_downx)()) {

	// set logfile FILE pointer to NULL (if we do not have write-access in the current directory,
	// logfile will be set to stdin to disable logfile output
	// should this behaviour be possibly changed???
	logfile = (FILE*)NULL;
	
	// check to see if the go_downx function is non-NULL
	if(go_downx == NULL) {
		MessageBox(NULL,"go_downx cannot be NULL!!","Critical Error",0);
		exit(255);
	}

	// whenever go_down() is called, it will call the client's designated function
	go_down = go_downx;
	
	// setup exception and error handling
	bf_catchme();
	reset_func_trace();	
	BEGIN_FUNC("bf_init")

	// initialize values for various pool counts and such
	bf_imgpool_count = 0;
	objects_otl = 0;
	ready = 0;
	bf_poly_count = 0;
	env_capacity = 1;

	// initialize the environment polygon pool with enough room for 1 polygon
	// (it is enlarged when more are added)
	if((env_polydex = (BF_TRIANGLE_EX**)malloc(env_capacity * sizeof(BF_TRIANGLE_EX*))) == NULL) {
		zlogthisx("Memory allocation error!");
		go_down();
	}


	// enable black-box COMMAND-style console logging support
	#ifdef ENABLE_CONSOLE_SUPPORT
	AllocConsole();
	SetConsoleTitle("Bluefire log console");
	wconsole = CreateFile("CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	zlogthisx("console support enabled. (ENABLE_CONSOLE_SUPPORT)");
	#endif

	// open the logfile for output, or else disable it if the filesystem is readonly
    if((logfile = fopen("bfw.log","w")) == NULL) {
                MessageBox(NULL,"Unable to open bfw.log for writing!","Warning",MB_SYSTEMMODAL);
    }

	// get the instance (hInstance) handle of the DLL.
	// note: this REQUIRES the DLL to be named "bluefire.dll" !!
	hBluefireDLL = GetModuleHandle("bluefire.dll");

	// enable new console dialog support (the Dialog template is in rez.rc)
	// the new console allows the user to input commands
	#ifdef ENABLE_NEW_CONSOLE
	init_console();
	zlogthisx("new console support enabled. (ENABLE_NEW_CONSOLE)");
	#endif
//BF_EXPORTZ char* bf_get_buildtime();
//BF_EXPORTZ char* bf_get_build();

	zlogthis("*******************************************************************************\n");
    zlogthis("Bluefire rendering engine.\n");
	zlogthis("Version %s - build %s - compiled %s\n",bf_get_version(), bf_get_build(), bf_get_buildtime());
	zlogthis("Copyright (c) 2003-2010 Jacob Hipps, all rights reserved.\n");
	zlogthis("*******************************************************************************\n\n");

	// load config
	bf_load_config();

	zlogthisx("app instance, base location = 0x%08x",hBluefireDLL);
    zlogthisx("zlib version %s",zlibVersion());
    zlogthisx("libpng version %s",png_libpng_ver);
	bfi_init();

	// these are hacks to test the audio <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// these should be called by the client application when correctly implemented!!
	bf_init_audio();	// << take out later...
	bf_al_test();		// ditto

	END_FUNC
	return 0;
}

/***

bf_init_timer()

	Description:
		Initializes the performance timer.

	Arguments:
		(none)
	Return:
		0 on success, non-zero on error
	Status:
		Working, Level B
	To-do:
		Nothing. =]
***/

int bf_init_timer() {
	BEGIN_FUNC("bf_init_timer")

	// Set up the timer...
	memset(&bf_timer, 0, sizeof(bf_timer));	// clear our timer structure

	// Check To See If A Performance Counter Is Available
	// If One Is Available The Timer Frequency Will Be Updated
	if (!QueryPerformanceFrequency((LARGE_INTEGER *) &bf_timer.frequency))	{
		zlogthis("error: could not initialize the performance counter!\n");
		return 1;
	} else {
		// Performance Counter Is Available, Use It Instead Of The Multimedia Timer
		// Get The Current Time And Store It In performance_timer_start
		QueryPerformanceCounter((LARGE_INTEGER *) &bf_timer.performance_timer_start);
		// Calculate The Timer Resolution Using The Timer Frequency
		// bf_timer.resolution = float(double(1.0f)/double(timer.frequency));
		// Set The Elapsed Time To The Current Time
		bf_timer.performance_timer_elapsed = bf_timer.performance_timer_start;
	} 

	END_FUNC
	return 0;
}

/***

bf_get_secs()

	Description:
		Get the current value of the timer

	Arguments:
		(none)

	Return:
		value of timer (float)
	Status:
		Working, Level B
	To-do:
		Nothing

***/
float bf_get_secs() {
	BEGIN_FUNC("bf_get_secs")

	__int64 clicky;

	QueryPerformanceCounter((LARGE_INTEGER *) &clicky);

	float retval = float(clicky) / float(bf_timer.frequency);

	END_FUNC
	return retval;
}

/***

bf_get_timer_ptr()

	Description:
		Returns the pointer to Bluefire's timer information structure

	Arguments:
		(none)
	Return:
		pointer to Bluefire's bf_timer structure (BF_TIMER_INFO*)
	Status:
		Working, Level B
	To-do:
		Nothing

***/
BF_TIMER_INFO* bf_get_timer_ptr() {
	BEGIN_FUNC("bf_get_timer_ptr")
	END_FUNC
	return &bf_timer;
}



/***

zlogfile_wrt()

	Description:
		Writes information to the logfile and console
		Called using the macro: zlogfile()
		Operates in the same way as printf()
		** zlogfilex() should be used instead of this one in most cases!

	Arguments:
		same as the stdio/C function "printf()"

	Return:
		0 unconditionally

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
int zlogfile_wrt(const char *fmt, ...) {
	BEGIN_FUNC("zlogfile_wrt")

    char text[4096]; // maximum of 4KB of characters

    va_list ap;
    va_start(ap, fmt);	
    vsprintf(text, fmt, ap);
    va_end(ap);

	//if(text[strlen(text) - 1] != '\n') cizz = '\n';
	//else cizz = 0;

	#ifdef LOG_ENABLE
	if(logfile != NULL) {
		fprintf(logfile,"%s",text);
		#ifndef LF_NOFLUSH
		fflush(logfile); // <--- DEBUG ONLY!!
		#endif
	}
	#endif
	#ifdef ENABLE_CONSOLE_SUPPORT
	conprintf("%s",text);
	#endif

	#ifdef ENABLE_NEW_CONSOLE
	add_text(text);
	#endif

	END_FUNC
	return 0;
}

/***

zlogfile_wrtx()

	Description:
		Called using the macro: zlogfilex()
		Same as zlogfile_wrt(), except adds the name of
		the current function in front of the printed information.
		Important: In order for this to work correctly, the calling
		function MUST use BEGIN_FUNC and END_FUNC macros!
		Also, this function adds a newline (\n) after the text to
		be printed.
		

	Arguments:
		same as the stdio/C function "printf()"

	Return:
		0 unconditionally

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
int zlogfile_wrtx(const char *fmt, ...) {
	BEGIN_FUNC("zlogfile_wrtx")

    char text[4096]; // maximum of 4KB of characters

    va_list ap;
    va_start(ap, fmt);	
    vsprintf(text, fmt, ap);
    va_end(ap);

	//if(text[strlen(text) - 1] != '\n') cizz = '\n';
	//else cizz = 0;

	#ifdef LOG_ENABLE
	if(logfile != NULL) {
	
		fprintf(logfile,"%s: %s\n",trace_funcs[trace_depth - 2],text);
		#ifndef LF_NOFLUSH
		fflush(logfile); // <--- DEBUG ONLY!!
		#endif
	}

	#endif
	#ifdef ENABLE_CONSOLE_SUPPORT
	conprintf("%s\n",text);
	#endif

	#ifdef ENABLE_NEW_CONSOLE
	add_text(text);
	#endif

	END_FUNC
	return 0;
}

/***

conprintf()

	Description:
		Prints texts to the console (if enabled).	

	Arguments:
		same as the stdio/C function "printf()"

	Return:
		0 unconditionally

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
int conprintf(const char *fmt, ...) {
	BEGIN_FUNC("conprintf")

	char text[4096]; // maximum of 4K characters

    va_list ap;
    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);

	WriteConsole(wconsole, text, strlen(text), NULL, NULL);	

	END_FUNC
	return 0;
}


/***

bf_shutdown()

	Description:
		Shuts down Bluefire, deallocates memory, releases textures, polygons,
		shuts down the renderer, shuts down the audio system, etc.

	Arguments:
		(none)

	Return:
		none

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
void bf_shutdown() {
	// imports from errors.cpp

	zlogthisx("Shutting down...");
	
	zlogthisx("call trace:");
	zlogthis("\t");
	int i;
	for(i = 0; i < trace_depth; i++) {
		if(i > 0) zlogthis(" -> ");
		zlogthis("%s",trace_funcs[i]);
	}
	zlogthis("\n");
	zlogthis("\t(%s, line %i)\n",trace_src[trace_depth - 1],trace_line[trace_depth - 1]);

	BEGIN_FUNC("bf_shutdown")

	bf_kill_audio();
	bfi_kill();

	zlogthisx("Deallocating memory...");
    
	// deallocate all of this shaz...
	
     for (i = 0; i < bf_imgpool_count; i++) {
         if (img_pool[i]) {
			// bf_delete_texture isn't required...
			//bf_delete_texture(img_pool[i]->tex_id);
			free(img_pool[i]);					// and the data structure as well
         }
     }

     fprintf(logfile,"%i textures freed.\n",bf_imgpool_count);
	 bf_imgpool_count = 0;

	 
	 /*
     fprintf(logfile,"\t\tfont data... ");
     fflush(logfile);
     for (i = 0; i < 5; i++) {
         if (fonts[i]) {
             if (fonts[i]->font_data) {				// if it exists (eg. non-zero pointer)...
                 delete_ds(fonts[i]->font_data, fonts[i]->glyphs);
													// then delete its display list
             }
             free(fonts[i]);						// and free the data structure
         }
     }
     fprintf(logfile,"ok\n");
	 */

     zlogthisx("Freeing object pool...");     
	 bf_freeobjpool();	 

     zlogthisx("Freeing environment data...");
	 bf_flush_env();
	 free(env_polydex);
	
	 zlogthisx("Freeing script data...");
	 bf_script_flush();

     zlogthisx("memory freed.");

	 FreeLibrary((HMODULE)hRenderDLL);

	 zlogthisx("Unbound from %s",bf_config_str("RenderDLL"));

     zlogthisx("restoring display settings...");
	 if(mainconfig.fullscreen) ChangeDisplaySettings(NULL, 0); // puts us back in the normal screen mode
	 if(mainconfig.fullscreen) ChangeDisplaySettings(NULL, 0); // and do it again to ensure things are correct

     zlogthisx("Good-bye!\n");

	 #ifdef ENABLE_NEW_CONSOLE
	 kill_console();
	 #endif

     fclose(logfile);

	 ready = 0;	 

	 return;
}


/***

bf_ready()

	Description:
		Returns the bf_ready flag. The client must set this to 1 or true whenever
		it is ready for Bluefire to commence rendering.

	Arguments:
		(none)

	Return:
		bf_ready flag (int)

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
int bf_ready() {
	BEGIN_FUNC("bf_ready")
	END_FUNC
	return ready;
}

/***

bf_set_ready()

	Description:
		Sets the bf_ready flag. (see bf_ready() function, above)

	Arguments:
		int readz		State of bf_ready flag.
						0 (false) = not ready, 1 (true) = ready.

	Return:
		none

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
void bf_set_ready(int readz) {
	BEGIN_FUNC("bf_set_ready")
	ready = readz;
	END_FUNC
	return;
}
