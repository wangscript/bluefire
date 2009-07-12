/*

  Bluefire: OpenGL Rendering Extention: Real-time Shader (GLSL)

  (c) Copyright 2006 Jacob Hipps

  Bluefire MDL Core: Winchester

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
int bfr_init_uniforms();

// globals
GLhandleARB p, f, v;

struct { 
	int baseTex;	// base texture
	int secTex;		// secondary texture
	int bumpTex;	// bumpmap texture
} bf_uniforms;



void setup_shaders() {
	BEGIN_FUNC("setup_shaders")

#ifdef BF_WITH_SHADERS
	int fsizer_f, fsizer_v;
	char vs[16000];		// <------- make this dynamic later ****
	char fs[16000]; 
	FILE *shadeyfile;

	v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	if((shadeyfile = fopen("shade.vert","rb")) == NULL) {
		zlogthis("unable to open file %s\n","shade.vert");
		go_down();
	}

	fseek(shadeyfile,0,SEEK_END);
	fsizer_v = ftell(shadeyfile);
	fseek(shadeyfile,0,0);

	fread(vs,fsizer_v,1,shadeyfile);

	fclose(shadeyfile);

	vs[fsizer_v] = 0;

	if((shadeyfile = fopen("shade.frag","rb")) == NULL) {
		zlogthis("unable to open file %s\n","shade.frag");
		go_down();
	}

	fseek(shadeyfile,0,SEEK_END);
	fsizer_f = ftell(shadeyfile);
	fseek(shadeyfile,0,0);

	fread(fs,fsizer_f,1,shadeyfile);

	fclose(shadeyfile);

	fs[fsizer_f] = 0;

	//zlogthis("fsizer_v = %i, fsizer_f = %i\n",fsizer_v, fsizer_f);
	//zlogthis("vs = \nvv------vv\n%s\n^^-------^^\n",vs);
	//zlogthis("fs = \nvv------vv\n%s\n^^-------^^\n",fs);

	const char *vv = vs;
	const char *ff = fs;

	zlogthis("fsizer_v = %i, fsizer_f = %i\n",fsizer_v, fsizer_f);
	zlogthis("vv = \nvv------vv\n%s\n^^-------^^\n",vv);
	zlogthis("ff = \nvv------vv\n%s\n^^-------^^\n",ff);

	glShaderSourceARB(v, 1, &vv, NULL);
	glShaderSourceARB(f, 1, &ff, NULL);

	glCompileShaderARB(v);
	printInfoLog(v);
	glCompileShaderARB(f);
	printInfoLog(f);

	p = glCreateProgramObjectARB();

	if(!p) {
		MessageBox(NULL,"Shader program(s) failed to compile/link properly. See logfile.","render_ogl error",0);
	} else {
		glAttachObjectARB(p,v);
		glAttachObjectARB(p,f);

		glLinkProgramARB(p);
		glUseProgramObjectARB(p);
	}
	printInfoLog(p);

#endif

	END_FUNC
	return;
}

void bf_reload_shaders() {
	BEGIN_FUNC("bf_reload_shaders")

	// remove current shader program from hardware GPU
	glDetachShaderARB(p,v);
	glDetachShaderARB(p,f);
	//glDeleteShaderARB(v);
	//glDeleteShaderARB(f);
	glDeleteProgramARB(p);

	// reload shader program
	setup_shaders();
	bfr_init_uniforms();

	END_FUNC
}

void printInfoLog(GLhandleARB obj) {
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);

	zlogthis("infolog:\n\n");

	if (infologLength > 0) {
		infoLog = (char *)malloc(infologLength);
		glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
		zlogthis("%s\n",infoLog);
		free(infoLog);
	}

	return;
}

int bfr_init_uniforms() {

	// get uniform locations
	bf_uniforms.baseTex = glGetUniformLocationARB(p,"baseTex");
	bf_uniforms.secTex = glGetUniformLocationARB(p,"secTex");
	bf_uniforms.bumpTex = glGetUniformLocationARB(p,"bumpTex");

	// set texture unit values
    glUniform1iARB(bf_uniforms.baseTex,0);
	glUniform1iARB(bf_uniforms.secTex,1);
	glUniform1iARB(bf_uniforms.bumpTex,2);

	return 0;
}

/*
void bfr_load_light(int lightnum) {

	int zloc = glGetUniformLocationARB(p,"bfLights[0]");
	//glUniform3fv(zloc,

	return;
}
*/