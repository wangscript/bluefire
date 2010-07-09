/*

  Bluefire: OpenGL Renderer: Real-time Shader (GLSL)

  (c) Copyright 2006-2010 Jacob Hipps  

*/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../bluefire.h"
#include "ogl.h"

// functions
void setup_shaders();
void printInfoLog(GLhandleARB obj);
int  bfr_init_uniforms();
int  load_shaders();

// globals
GLhandleARB p = NULL; // program handle
GLhandleARB f = NULL; // fragment shader handle
GLhandleARB v = NULL; // vertex shader handle

char compile_log[16000]; // compilation log

// Uniform indicies
struct { 
	int baseTex;	// base texture
	int secTex;		// secondary texture
	int bumpTex;	// bumpmap texture
	int rmode;		// render mode control
	int amode;		// alpha/blend mode control
} bf_uniforms;

// Uniform values					
int	  u_rendermode;	// render mode control
int   u_alphamode;	// alpha/blend mode control

// Attributes
int   atr_bfstate;	// render state attribute index
short bfstate;		// render state bitmask
					// 1	= Lighting enabled
					// 2	= Texturing enabled
			


#define SHADER_VERT_FILE "shaders\\shade.vert"
#define SHADER_FRAG_FILE "shaders\\shade.frag"

void setup_shaders() {
	BEGIN_FUNC("setup_shaders")

#ifdef BF_WITH_SHADERS

	v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	p = glCreateProgramObjectARB();

	load_shaders();

	zlogthis("setup_shaders: Setting up uniforms...\n");
	bfr_init_uniforms();

	zlogthis("setup_shaders: Ready!\n");

#endif

	END_FUNC
	return;
}

int load_shaders() {
	BEGIN_FUNC("load_shaders")

	int fsizer_f, fsizer_v;
	char vs[16000];		// <------- make this dynamic later ****
	char fs[16000]; 
	FILE *shadeyfile;
	int linkstatus;
	int cchoice;


	zlogthis("load_shaders: Loading vertex shader from file [%s]... ",SHADER_VERT_FILE);

	if((shadeyfile = fopen(SHADER_VERT_FILE,"rb")) == NULL) {
		zlogthis("Error: unable to open file!\n");
		go_down();
	}

	// get file size
	fseek(shadeyfile,0,SEEK_END);
	fsizer_v = ftell(shadeyfile);
	fseek(shadeyfile,0,0);

	zlogthis("Ok (%i bytes)\n",fsizer_v);

	// copy file to string
	fread(vs,fsizer_v,1,shadeyfile);
	fclose(shadeyfile);
	vs[fsizer_v] = 0;		// ensure last byte is NULL

	
	zlogthis("load_shaders: Loading fragment shader from file [%s]... ",SHADER_FRAG_FILE);

	if((shadeyfile = fopen(SHADER_FRAG_FILE,"rb")) == NULL) {
		zlogthis("Error: unable to open file!\n");
		go_down();
	}
	
	// get file size
	fseek(shadeyfile,0,SEEK_END);
	fsizer_f = ftell(shadeyfile);
	fseek(shadeyfile,0,0);

	zlogthis("Ok (%i bytes)\n",fsizer_f);

	// copy file to string
	fread(fs,fsizer_f,1,shadeyfile);
	fclose(shadeyfile);
	fs[fsizer_f] = 0;		// ensure last byte is NULL


	const char *vv = vs;
	const char *ff = fs;

	zlogthis("load_shaders: Binding to ShaderSource objects...\n");

	glShaderSourceARB(v, 1, &vv, NULL);
	glShaderSourceARB(f, 1, &ff, NULL);

	zlogthis("load_shaders: Preparing to compile on GPU hardware...\n");

	// clear compile log
	compile_log[0] = NULL;

	zlogthis("load_shaders: Compiling Vertex Shader...\n");
	sprintf(compile_log,"/// Vertex Shader...\n\n");
	glCompileShaderARB(v);
	printInfoLog(v);

	zlogthis("load_shaders: Compiling Fragment Shader...\n");
	sprintf(compile_log,"%s\n/// Fragment Shader...\n\n",compile_log);
	glCompileShaderARB(f);
	printInfoLog(f);	

	glAttachObjectARB(p,v);
	glAttachObjectARB(p,f);

	zlogthis("load_shaders: Linking...\n");
	sprintf(compile_log,"%s\n/// Linking...\n\n",compile_log);
	glLinkProgramARB(p);

	glGetProgramivARB(p,GL_LINK_STATUS,&linkstatus);
	printInfoLog(p);

	if(linkstatus == GL_FALSE) {
		cchoice = MessageBox(NULL,"Shader program(s) failed to compile/link properly. See logfile.\n\nContinue?","bfw dreamcatcher - GLSL Shader Error",MB_YESNO|MB_DEFBUTTON2|MB_ICONHAND);
		if(cchoice == IDNO) go_down();
	}
	zlogthis("load_shaders: Activating program on hardware...\n");
	glUseProgramObjectARB(p);

	zlogthis("load_shaders: Shaders successfully loaded!\n");

	END_FUNC
	return 0;
}

void bf_reload_shaders() {
	BEGIN_FUNC("bf_reload_shaders")

	// disable bluefire ready flag (disables rendering)
	bf_set_ready(false);

	/*
	zlogthis("bf_reload_shaders: Removing shader programs from hardware...\n");
	// remove current shader program from hardware GPU
	zlogthis("reverting to fixed functionality.\n");
	glUseProgramARB(0);
	zlogthis("delete vertex\n");
	glDeleteShaderARB(v);
	zlogthis("delete fragment\n");
	glDeleteShaderARB(f);
	zlogthis("detach vertex\n");
	glDetachShaderARB(p,v);
	zlogthis("detach fragment\n");
	glDetachShaderARB(p,f);
	zlogthis("delete program\n");
	glDeleteProgramARB(p);
	*/

	zlogthis("bf_reload_shaders: Reloading shaders...\n");
	// reload shader program
	load_shaders();

	zlogthis("bf_reload_shaders: Setting up uniforms...\n");
	bfr_init_uniforms();

	// set ready flag
	bf_set_ready(true);
	zlogthis("bf_reload_shaders: Shaders reloaded!\n");

	END_FUNC
}

int bfr_init_uniforms() {
	BEGIN_FUNC("bfr_init_uniforms")

	// get uniform locations
	bf_uniforms.baseTex = glGetUniformLocationARB(p,"tex0");
	bf_uniforms.secTex = glGetUniformLocationARB(p,"tex1");
	bf_uniforms.bumpTex = glGetUniformLocationARB(p,"tex2");	
	bf_uniforms.rmode = glGetUniformLocationARB(p,"rmode");	

	// set texture unit values
    glUniform1iARB(bf_uniforms.baseTex,0);
	glUniform1iARB(bf_uniforms.secTex,1);
	glUniform1iARB(bf_uniforms.bumpTex,2);

	// initialize state information with the defaults
	glUniform1iARB(bf_uniforms.rmode,BF_RMODE_FULL);	// render mode
	glUniform1iARB(bf_uniforms.amode,BF_AMODE_NORMAL);	// alpha blending mode

	// set up attributes
	atr_bfstate = glGetAttribLocationARB(p,"bfstate");

	END_FUNC
	return 0;
}

// bfr_get_mode() is a macro that calls bfr_set_mode(BF_RMODE_GET_STATE)
//                and returns the current render mode

int bfr_set_mode(int modectrl) {
	// modectrl can be set to BF_RMODE_GET_STATE (-1) to return
	// the current rendermode state without making a change
	if(modectrl != BF_RMODE_GET_STATE) {
		u_rendermode = modectrl;
		glUniform1iARB(bf_uniforms.rmode,u_rendermode);
	} 
	return u_rendermode;
}

// bfr_get_blending() is a macro that calls bfr_set_blending(BF_AMODE_GET_STATE)
//                    and returns the current alpha blending mode

int bfr_set_blending(int modectrl) {

	if(modectrl != BF_AMODE_GET_STATE) {
		u_alphamode = modectrl;
		glUniform1iARB(bf_uniforms.amode,u_alphamode);
	} 
	return u_alphamode;
}

void bfr_update_state() {
	BEGIN_FUNC("bfr_update_state")



	END_FUNC	
}

/*
void bfr_load_light(int lightnum) {

	int zloc = glGetUniformLocationARB(p,"bfLights[0]");
	//glUniform3fv(zloc,

	return;
}
*/

void printInfoLog(GLhandleARB obj) {
	BEGIN_FUNC("printInfoLog")

	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);

	zlogthis("\n");

	if (infologLength > 0) {
		infoLog = (char *)malloc(infologLength);
		glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
		zlogthis("%s\n",infoLog);
		strcat(compile_log,infoLog);
		strcat(compile_log,"\n");
		free(infoLog);
	}

	END_FUNC
	return;
}
