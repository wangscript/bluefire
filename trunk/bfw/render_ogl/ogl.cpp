/*

  Bluefire: OpenGL Rendering Extention: Initialization/destruction routines

  (c) Copyright 2004 Jacob Hipps

  Bluefire MDL Core: Winchester

*/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bluefire.h"
#include "ogl.h"

// imported
extern void setup_shaders();

// Local functions
bool checkExt (char *string, const char *search);
int bfr_init_opengl();

int already_shown = 0;

// OpenGL only
int bfr_init_opengl() {	
	BEGIN_FUNC("bfr_init_opengl")

	static char dsetty[] = "<n/a>";

	if(already_shown) { 
		END_FUNC
		return 0;
	}

	already_shown = 1;

	zlogthis("render_ogl: Bluefire is written in compliance with OpenGL 1.4\n");

	zlogthis("render_ogl: Rendering subsystem information...\n\n"
			 "Renderer: %s\n"
			 "Vendor: %s\n"
			 "OpenGL Version: %s\n\n"
			 ,glGetString(GL_RENDERER),glGetString(GL_VENDOR),glGetString(GL_VERSION)
			);

	char *extensions;
	extensions=strdup((char *) glGetString(GL_EXTENSIONS)); // Fetch Extension String
	int len = strlen(extensions);
	if(extensions) {
		for (int i=0; i<len; i++) // Separate It By Newline Instead Of Blank
		if (extensions[i]==' ') extensions[i]='\n';
	} else {

	}
		
	zlogthis("render_ogl: Supported OpenGL extensions...\n\n%s\n", extensions);
	zlogthis("render_ogl: Checking for required extensions...\n");

	int maxTexelUnits;

	zlogthis( "\tGL_ARB_multitexture... ");
	if(checkExt(extensions,"GL_ARB_multitexture")) {	
		//CHECKPOINT;
		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&maxTexelUnits);

	   #ifndef GL_GLEXT_PROTOTYPES
		glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC) wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
		glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");		
	   #endif

		zlogthis("Ok. Your card has %i texture units!\n",maxTexelUnits);
		ext_ARB_multitexture = true;
	} else {
		zlogthis("NOT SUPPORTED\n");
		zlogthis("render_ogl: warning: Bluefire cannot utilize single-pass multi-texturing operations!\n"
				 "            Therefore, lightmaps and shadowing has been disabled.\n"
				);
		ext_ARB_multitexture = false;
	}
	

	zlogthis("\tGL_ARB_vertex_shader/GL_ARB_fragment_shader... ");
	if(checkExt(extensions,"GL_ARB_vertex_shader") && checkExt(extensions,"GL_ARB_fragment_shader")) {
	   
	   #ifndef GL_GLEXT_PROTOTYPES
		glAttachShaderARB = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShaderARB");
		glCompileShaderARB = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShaderARB");
		glCreateProgramARB = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgramARB");
		glCreateShaderARB = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShaderARB");
		glDeleteProgramARB = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgramARB");
		glDeleteShaderARB = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShaderARB");
		glDetachShaderARB = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShaderARB");
		glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArrayARB");
		glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArrayARB");
		glGetActiveAttribARB = (PFNGLGETACTIVEATTRIBPROC)wglGetProcAddress("glGetActiveAttribARB");
		glGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress("glGetActiveUniformARB");
		glGetAttachedShadersARB = (PFNGLGETATTACHEDSHADERSPROC)wglGetProcAddress("glGetAttachedShadersARB");
		glGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocationARB");
		glGetProgramivARB = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramivARB");
		glProgramInfoLogARB = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glProgramInfoLogARB");
		glGetShaderivARB = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderivARB");
		glGetShaderInfoLogARB = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLogARB");
		glGetShaderSourceARB = (PFNGLGETSHADERSOURCEPROC)wglGetProcAddress("glGetShaderSourceARB");
		glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocationARB");
		glGetUniformfvARB = (PFNGLGETUNIFORMFVPROC)wglGetProcAddress("glGetUniformfvARB");
		glGetUniformivARB = (PFNGLGETUNIFORMIVPROC)wglGetProcAddress("glGetUniformivARB");
		glGetVertexAttribdvARB = (PFNGLGETVERTEXATTRIBDVPROC)wglGetProcAddress("glGetVertexAttribdvARB");
		glGetVertexAttribfvARB = (PFNGLGETVERTEXATTRIBFVPROC)wglGetProcAddress("glGetVertexAttribfvARB");
		glGetVertexAttribivARB = (PFNGLGETVERTEXATTRIBIVPROC)wglGetProcAddress("glGetVertexAttribivARB");
		glGetVertexAttribPointervARB = (PFNGLGETVERTEXATTRIBPOINTERVPROC)wglGetProcAddress("glGetVertexAttribPointervARB");
		glIsProgramARB = (PFNGLISPROGRAMPROC)wglGetProcAddress("glIsProgramARB");
		glIsShaderARB = (PFNGLISSHADERPROC)wglGetProcAddress("glIsShaderARB");
		glLinkProgramARB = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgramARB");
		glShaderSourceARB = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSourceARB");
		glUseProgramARB = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgramARB");
		glUniform1fpARB = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1fpARB");
		glUniform2fpARB = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2fpARB");
		glUniform3fpARB = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3fpARB");
		glUniform4fpARB = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4fpARB");
		glUniform1iARB = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1iARB");
		glUniform2iARB = (PFNGLUNIFORM2IPROC)wglGetProcAddress("glUniform2iARB");
		glUniform3iARB = (PFNGLUNIFORM3IPROC)wglGetProcAddress("glUniform3iARB");
		glUniform4iARB = (PFNGLUNIFORM4IPROC)wglGetProcAddress("glUniform4iARB");
		glUniform1fvARB = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fvARB");
		glUniform2fvARB = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fvARB");
		glUniform3fvARB = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fvARB");
		glUniform4fvARB = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fvARB");
		glUniform1ivARB = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1ivARB");
		glUniform2ivARB = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2ivARB");
		glUniform3ivARB = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3ivARB");
		glUniform4ivARB = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4ivARB");
		glUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVPROC)wglGetProcAddress("glUniformMatrix2fvARB");
		glUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress("glUniformMatrix3fvARB");
		glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fvARB");
		glValidateProgramARB = (PFNGLVALIDATEPROGRAMPROC)wglGetProcAddress("glValidateProgramARB");
		glVertexAttrib1dARB = (PFNGLVERTEXATTRIB1DPROC)wglGetProcAddress("glVertexAttrib1dARB");
		glVertexAttrib1dvARB = (PFNGLVERTEXATTRIB1DVPROC)wglGetProcAddress("glVertexAttrib1dvARB");
		glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1fARB");
		glVertexAttrib1fvARB = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fvARB");
		glVertexAttrib1sARB = (PFNGLVERTEXATTRIB1SPROC)wglGetProcAddress("glVertexAttrib1sARB");
		glVertexAttrib1svARB = (PFNGLVERTEXATTRIB1SVPROC)wglGetProcAddress("glVertexAttrib1svARB");
		glVertexAttrib2dARB = (PFNGLVERTEXATTRIB2DPROC)wglGetProcAddress("glVertexAttrib2dARB");
		glVertexAttrib2dvARB = (PFNGLVERTEXATTRIB2DVPROC)wglGetProcAddress("glVertexAttrib2dvARB");
		glVertexAttrib2fARB = (PFNGLVERTEXATTRIB2FPROC)wglGetProcAddress("glVertexAttrib2fARB");
		glVertexAttrib2fvARB = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fvARB");
		glVertexAttrib2sARB = (PFNGLVERTEXATTRIB2SPROC)wglGetProcAddress("glVertexAttrib2sARB");
		glVertexAttrib2svARB = (PFNGLVERTEXATTRIB2SVPROC)wglGetProcAddress("glVertexAttrib2svARB");
		glVertexAttrib3dARB = (PFNGLVERTEXATTRIB3DPROC)wglGetProcAddress("glVertexAttrib3dARB");
		glVertexAttrib3dvARB = (PFNGLVERTEXATTRIB3DVPROC)wglGetProcAddress("glVertexAttrib3dvARB");
		glVertexAttrib3fARB = (PFNGLVERTEXATTRIB3FPROC)wglGetProcAddress("glVertexAttrib3fARB");
		glVertexAttrib3fvARB = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fvARB");
		glVertexAttrib3sARB = (PFNGLVERTEXATTRIB3SPROC)wglGetProcAddress("glVertexAttrib3sARB");
		glVertexAttrib3svARB = (PFNGLVERTEXATTRIB3SVPROC)wglGetProcAddress("glVertexAttrib3svARB");
		glVertexAttrib4nbvARB = (PFNGLVERTEXATTRIB4NBVPROC)wglGetProcAddress("glVertexAttrib4nbvARB");
		glVertexAttrib4nivARB = (PFNGLVERTEXATTRIB4NIVPROC)wglGetProcAddress("glVertexAttrib4nivARB");
		glVertexAttrib4nsvARB = (PFNGLVERTEXATTRIB4NSVPROC)wglGetProcAddress("glVertexAttrib4nsvARB");
		glVertexAttrib4nubARB = (PFNGLVERTEXATTRIB4NUBPROC)wglGetProcAddress("glVertexAttrib4nubARB");
		glVertexAttrib4nubvARB = (PFNGLVERTEXATTRIB4NUBVPROC)wglGetProcAddress("glVertexAttrib4nubvARB");
		glVertexAttrib4nuivARB = (PFNGLVERTEXATTRIB4NUIVPROC)wglGetProcAddress("glVertexAttrib4nuivARB");
		glVertexAttrib4nusvARB = (PFNGLVERTEXATTRIB4NUSVPROC)wglGetProcAddress("glVertexAttrib4nusvARB");
		glVertexAttrib4bvARB = (PFNGLVERTEXATTRIB4BVPROC)wglGetProcAddress("glVertexAttrib4bvARB");
		glVertexAttrib4dARB = (PFNGLVERTEXATTRIB4DPROC)wglGetProcAddress("glVertexAttrib4dARB");
		glVertexAttrib4dvARB = (PFNGLVERTEXATTRIB4DVPROC)wglGetProcAddress("glVertexAttrib4dvARB");
		glVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FPROC)wglGetProcAddress("glVertexAttrib4fARB");
		glVertexAttrib4fvARB = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fvARB");
		glVertexAttrib4ivARB = (PFNGLVERTEXATTRIB4IVPROC)wglGetProcAddress("glVertexAttrib4ivARB");
		glVertexAttrib4sARB = (PFNGLVERTEXATTRIB4SPROC)wglGetProcAddress("glVertexAttrib4sARB");
		glVertexAttrib4svARB = (PFNGLVERTEXATTRIB4SVPROC)wglGetProcAddress("glVertexAttrib4svARB");
		glVertexAttrib4ubvARB = (PFNGLVERTEXATTRIB4UBVPROC)wglGetProcAddress("glVertexAttrib4ubvARB");
		glVertexAttrib4uivARB = (PFNGLVERTEXATTRIB4UIVPROC)wglGetProcAddress("glVertexAttrib4uivARB");
		glVertexAttrib4usvARB = (PFNGLVERTEXATTRIB4USVPROC)wglGetProcAddress("glVertexAttrib4usvARB");
		glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointerARB");

		glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
		glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
		glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
		glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
		glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
		glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
	   #endif

		zlogthis("Ok.\n");
		setup_shaders();
		ext_ARB_shaders = true;
	} else {
		zlogthis("NOT SUPPORTED\n");
		zlogthis("render_ogl: warning: Bluefire cannot utilize real-time shading and lighting operations!\n");
		ext_ARB_shaders = false; 
	}


	int max_lights;
	int max_tex_sz;

	glGetIntegerv(GL_MAX_LIGHTS, &max_lights);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_sz);

	zlogthis("render_ogl: Implementation limitations...\n"
			 "\tMax Hardware Lights: %i\n"
			 "\tMax Texture size: %ix%i\n"
			 ,max_lights,max_tex_sz,max_tex_sz
	);

	END_FUNC
	return 0;
}



int bf_init_3d (HWND hwndz) {
	BEGIN_FUNC("bf_init_3d")

    GLuint PixelFormat;

	PIXELFORMATDESCRIPTOR pfd2;
	int formatz;

	static PIXELFORMATDESCRIPTOR pfd= // pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR), // Size Of This Pixel Format Descriptor
		1, // Version Number
		//PFD_DRAW_TO_WINDOW | // Format Must Support Window */
		PFD_SUPPORT_OPENGL | // Format Must Support OpenGL
		PFD_DOUBLEBUFFER, // Must Support Double Buffering
		PFD_TYPE_RGBA, // Request An RGBA Format
		0, // Select Our Color Depth
		0, 0, 0, 0, 0, 0, // Color Bits Ignored
		0, // No Alpha Buffer
		0, // Shift Bit Ignored
		0, // No Accumulation Buffer
		0, 0, 0, 0, // Accumulation Bits Ignored
		0, // 16Bit Z-Buffer (Depth Buffer)
		0, // No Stencil Buffer
		0, // No Auxiliary Buffer
		PFD_MAIN_PLANE, // Main Drawing Layer
		0, // Reserved
		0, 0, 0 // Layer Masks Ignored
	};

	if (!(renderx->hDC=GetDC(hwndz))) // Did We Get A Device Context?
	{
		bf_kill_3d(hwndz); // Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
		zlogthis( "error: unable to create OGL device context.\n");
		return FALSE; // Return FALSE
	}

	// enum pixel formats
	char flgstr[128];
	pfd2.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	formatz = DescribePixelFormat(renderx->hDC,1,0,NULL);
	
#ifdef SHOW_PIXEL_FORMATS_IN_LOG
	zlogthis("Supported PixelFormats... \n");
	for(int i = 1; i <= formatz; i++) {
		flgstr[0] = 0;
		DescribePixelFormat(renderx->hDC,i,pfd2.nSize,&pfd2);

		if(pfd2.dwFlags & PFD_DOUBLEBUFFER) strcat(flgstr," DBLBUF");
		if(pfd2.dwFlags & PFD_STEREO) strcat(flgstr," Stereo");
		if(pfd2.dwFlags & PFD_DRAW_TO_WINDOW) strcat(flgstr," WINDOW");
		if(pfd2.dwFlags & PFD_DRAW_TO_BITMAP) strcat(flgstr," BITMAP");
		if(pfd2.dwFlags & PFD_SUPPORT_GDI) strcat(flgstr," GDI");
		if(pfd2.dwFlags & PFD_SUPPORT_OPENGL) strcat(flgstr," OpenGL");

		if(pfd2.dwFlags & PFD_GENERIC_FORMAT) strcat(flgstr," GENERIC");
		if(pfd2.dwFlags & PFD_NEED_PALETTE) strcat(flgstr," PAL");
		if(pfd2.dwFlags & PFD_NEED_SYSTEM_PALETTE) strcat(flgstr," SysPAL");
		if(pfd2.dwFlags & PFD_SWAP_EXCHANGE) strcat(flgstr," SwapExchg");
		if(pfd2.dwFlags & PFD_SWAP_COPY) strcat(flgstr," SwapCOPY");
		if(pfd2.dwFlags & PFD_SWAP_LAYER_BUFFERS) strcat(flgstr," SwapLAYBUF");
		if(pfd2.dwFlags & PFD_GENERIC_ACCELERATED) strcat(flgstr," ACCEL");
		if(pfd2.dwFlags & PFD_SUPPORT_DIRECTDRAW) strcat(flgstr," DDraw");

		
		zlogthis("   #%03i %s %ibpp [%s ]\n",i,(pfd2.iPixelType ? "RGBA" : "Indx"),pfd2.cDepthBits,flgstr);
	}
#endif

	if (!(PixelFormat=ChoosePixelFormat(renderx->hDC,&pfd))) // Did Windows Find A Matching Pixel Format?
	{ 
		bf_kill_3d(hwndz); // Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
		zlogthis( "error: unable to find suitable pixel format.\n");
		return FALSE; // Return FALSE
	}

	zlogthis("AUTO Selected: PixelFormat = %i\n",PixelFormat);

	if(!SetPixelFormat(renderx->hDC,PixelFormat,&pfd)) // Are We Able To Set The Pixel Format?
	{
		bf_kill_3d(hwndz); // Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
		zlogthis( "error: unable to set pixel format.\n");
		return FALSE; // Return FALSE
	}

	if (!(renderx->hRC=wglCreateContext(renderx->hDC))) // Are We Able To Get A Rendering Context?
	{
		bf_kill_3d(hwndz); // Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
		zlogthis( "error: unable to create OGL rendering context.\n");
		return FALSE; // Return FALSE
	}

	if(!wglMakeCurrent(renderx->hDC,renderx->hRC)) // Try To Activate The Rendering Context
	{
		bf_kill_3d(hwndz); // Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
		zlogthis( "error: unable to active OGL rendering context.\n");
		return FALSE; // Return FALSE
	}

	bf_set_ready(TRUE);

	RECT wirect;
	GetClientRect(hwndz,&wirect);
	renderx->width = wirect.right;
	renderx->height = wirect.bottom;
	bf_resize_disp(wirect.right, wirect.bottom, hwndz); // Set up the screen

	// setup other stuff

  	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH); // Enables Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black Background
	glClearDepth(1.0f); // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST); // Don't forget this, eh!!
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective Calculations
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Set Line Antialiasing
	glEnable(GL_BLEND); // Turn Blending On
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // enable alpha blending
	//glEnable(GL_CULL_FACE);

	//glBlendFunc(GL_SRC_ALPHA_SATURATE,GL_ONE);

	//glEnable(GL_POLYGON_SMOOTH);
	//glEnable(GL_LINE_SMOOTH);

	renderx->raster_mode = BF_FILL;

	bfr_init_opengl();
	zlogthis("bf_init_3d: ok. OpenGL ready.\n");
	
	END_FUNC
	return 0;
}


void bf_set_share_rc(ESM_RENDERZ* rcontext) {
	BEGIN_FUNC("bf_set_share_rc")

	wglShareLists(renderx->hRC, rcontext->hRC);
	wglCopyContext(rcontext->hRC, renderx->hRC, GL_ALL_ATTRIB_BITS);

	END_FUNC
}

bool checkExt (char *string, const char *search) {
	BEGIN_FUNC("checkExt")

	if(strstr(string,search)) {
		END_FUNC
		return true;
	} else {
		END_FUNC
		return false; // Sorry, Not Found!
	}
}

void bf_enable_lighting() {
	BEGIN_FUNC("bf_enable_lighting")
	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // a hack
	END_FUNC
}

void bf_disable_lighting() {
	BEGIN_FUNC("bf_disable_lighting")
	glDisable(GL_LIGHTING);
	END_FUNC
}

void bf_resize_disp(int width, int height, HWND hwndz) {
	BEGIN_FUNC("bf_resize_disp")

	if (!bf_ready()) return;

	if (height==0) // Prevent A Divide By Zero
	{
		height=1;
	}

	renderx->width = width;
	renderx->height = height;

	glViewport(0, 0, width, height); // Reset The Current Viewport

	glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
	glLoadIdentity(); // Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,1000.0f); // Perspective view
    //glOrtho(0.0f,width,height,0.0f,-1.0f,1.0f); // Ortho View
	glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
	glLoadIdentity(); // Reset The Modelview Matrix	

    //zlogthis("info: OpenGL viewport successfully resized to %ix%i\n",width,height);
	
	END_FUNC
}


void bf_set_viewport(int vp1, int vp2, int vp3, int vp4, HWND hwndz) {
	BEGIN_FUNC("bf_set_viewport")

	if (!bf_ready()) return;

	if (vp4 == 0) // Prevent A Divide By Zero
	{
		vp4++;
	}

	glViewport(vp1, vp2, vp3, vp4); // Reset The Current Viewport

	glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
	glLoadIdentity(); // Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	
	gluPerspective(45.0f,(GLfloat)vp3/(GLfloat)vp4,0.1f,1000.0f); // Perspective view
	glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
	glLoadIdentity(); // Reset The Modelview Matrix	
	
	END_FUNC
}


void bf_kill_3d(HWND winhand) {
	BEGIN_FUNC("bf_kill_3d")

	if (renderx->hRC) // Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL)) // Are We Able To Release The DC And RC Contexts?
		{
			//MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			zlogthis("warning: unable to release hDC and hRC class!\n");
		}

		if (!wglDeleteContext(renderx->hRC)) // Are We Able To Delete The RC?
		{
			//MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			zlogthis("warning: unable to release hRC!\n");
		}

		renderx->hRC = NULL; // Set RC To NULL
	}

	if (renderx->hDC && !ReleaseDC(winhand,renderx->hDC)) // Are We Able To Release The DC
	{
		//MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		zlogthis("warning: unable to release hDC!\n");
		renderx->hDC=NULL; // Set DC To NULL
	}

	END_FUNC
}


void bf_set_raster_mode(int mode) {

	renderx->raster_mode = mode;

	switch(mode) {
		case BF_FILL:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case BF_POINT:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;		
		case BF_WIREFRAME:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
	}
	
}

int bf_bind_texture(esm_picture *picdat, unsigned int *picptr) {
	BEGIN_FUNC("bf_bind_texture")

    glGenTextures(1, picptr);
	glBindTexture(GL_TEXTURE_2D, (*picptr));

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
	
	unsigned int tizzle;

	if(picdat->depth == 32) {
		tizzle = GL_RGBA;
	} else if (picdat->depth == 24) {
		tizzle = GL_RGB;
	} else {
		zlogthis("bf_bind_texture: warning: undefined depth. assuming 32bpp (RGBA).\n");
		tizzle = GL_RGBA;
	}

    glTexImage2D(GL_TEXTURE_2D, 0, tizzle, picdat->width, picdat->height, 0, tizzle, GL_UNSIGNED_BYTE, picdat->data_ptr);

	END_FUNC
	return 0;
}

int bf_activate_context(ESM_RENDERZ *rendery) {
	BEGIN_FUNC("bf_activate_context")

	renderx = rendery;

	wglMakeCurrent(renderx->hDC, renderx->hRC);
	
	END_FUNC
	return 0;
}

int bf_activate_context_x(ESM_RENDERZ *rendery) {
	BEGIN_FUNC("bf_activate_context_x")

	renderx = rendery;

	END_FUNC
	return 0;
}


int bf_create_texture(int x, int y, int num) {
	BEGIN_FUNC("bf_create_texture")

	zlogthis("bf_create_texture: attempting to create texture #%i (%ix%i)\n",num,x,y);
	
	// check to make sure we have a valid texture size
	/*
	if(esm_find2com(x) != x || esm_find2com(y) != y) {
		logthis("error: create_texture: invalid texture size: %ix%i. texture size must be a power of two when using this function.\n");
		go_down();
	}
	*/

	BF_TEXNODE *radtex = bf_getimg(num);

	radtex->height = x;
	radtex->width  = y;

    glGenTextures(1, &(radtex->tex_id));
	glBindTexture(GL_TEXTURE_2D, radtex->tex_id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering

	zlogthis("bf_create_texture: texture created successfully! assigned tex_id %i\n",radtex->tex_id);

	END_FUNC
	return 0;
}


int bf_update_lighting() {
	
	GLfloat ambient_light[] = { 0.5f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat pos_of_light[] = { 0.0f, 5.0f, -3.0f, 1.0f};

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_light); // Setup The Ambient Light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light); // Setup The Diffuse Light
	glLightfv(GL_LIGHT1, GL_POSITION, pos_of_light); // Position The Light

	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);

	return 0;
}