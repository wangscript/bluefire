/*

  Bluefire: OpenGL Rendering Extention: Header file

  (c) Copyright 2004-2010 Jacob Hipps

*/

//#define GL_GLEXT_PROTOTYPES
#include <gl\gl.h>
#include <gl\glu.h>
#include "glext.h"


#ifdef GL_GLEXT_PROTOTYPES
	#define glMultiTexCoord2fARB		glMultiTexCoord2f
	#define glActiveTextureARB			glActiveTexture
	#define glGetInfoLogARB				glGetInfoLog
	#define glGetObjectParameterivARB	glGetObjectParameteriv
	#define gl
#endif


#ifndef GL_GLEXT_PROTOTYPES
#ifdef _BF_MAIN

// ARB_multitexture extension

PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB = NULL;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB = NULL;
PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB = NULL;
PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB = NULL;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB= NULL;
bool ext_ARB_multitexture = false;

// ARB_*_shader extentions

PFNGLATTACHSHADERPROC glAttachShaderARB = NULL;
PFNGLCOMPILESHADERPROC glCompileShaderARB = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgramARB = NULL;
PFNGLCREATESHADERPROC glCreateShaderARB = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgramARB = NULL;
PFNGLDELETESHADERPROC glDeleteShaderARB = NULL;
PFNGLDETACHSHADERPROC glDetachShaderARB = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArrayARB = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArrayARB = NULL;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttribARB = NULL;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniformARB = NULL;
PFNGLGETATTACHEDSHADERSPROC glGetAttachedShadersARB = NULL;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocationARB = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramivARB = NULL;
PFNGLGETPROGRAMINFOLOGPROC glProgramInfoLogARB = NULL;
PFNGLGETSHADERIVPROC glGetShaderivARB = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLogARB = NULL;
PFNGLGETSHADERSOURCEPROC glGetShaderSourceARB = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocationARB = NULL;
PFNGLGETUNIFORMFVPROC glGetUniformfvARB = NULL;
PFNGLGETUNIFORMIVPROC glGetUniformivARB = NULL;
PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdvARB = NULL;
PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfvARB = NULL;
PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribivARB = NULL;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointervARB = NULL;
PFNGLISPROGRAMPROC glIsProgramARB = NULL;
PFNGLISSHADERPROC glIsShaderARB = NULL;
PFNGLLINKPROGRAMPROC glLinkProgramARB = NULL;
PFNGLSHADERSOURCEPROC glShaderSourceARB = NULL;
PFNGLUSEPROGRAMPROC glUseProgramARB = NULL;
PFNGLUNIFORM1FPROC glUniform1fpARB = NULL;
PFNGLUNIFORM2FPROC glUniform2fpARB = NULL;
PFNGLUNIFORM3FPROC glUniform3fpARB = NULL;
PFNGLUNIFORM4FPROC glUniform4fpARB = NULL;
PFNGLUNIFORM1IPROC glUniform1iARB = NULL;
PFNGLUNIFORM2IPROC glUniform2iARB = NULL;
PFNGLUNIFORM3IPROC glUniform3iARB = NULL;
PFNGLUNIFORM4IPROC glUniform4iARB = NULL;
PFNGLUNIFORM1FVPROC glUniform1fvARB = NULL;
PFNGLUNIFORM2FVPROC glUniform2fvARB = NULL;
PFNGLUNIFORM3FVPROC glUniform3fvARB = NULL;
PFNGLUNIFORM4FVPROC glUniform4fvARB = NULL;
PFNGLUNIFORM1IVPROC glUniform1ivARB = NULL;
PFNGLUNIFORM2IVPROC glUniform2ivARB = NULL;
PFNGLUNIFORM3IVPROC glUniform3ivARB = NULL;
PFNGLUNIFORM4IVPROC glUniform4ivARB = NULL;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fvARB = NULL;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fvARB = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fvARB = NULL;
PFNGLVALIDATEPROGRAMPROC glValidateProgramARB = NULL;
PFNGLVERTEXATTRIB1DPROC glVertexAttrib1dARB = NULL;
PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dvARB = NULL;
PFNGLVERTEXATTRIB1FPROC glVertexAttrib1fARB = NULL;
PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fvARB = NULL;
PFNGLVERTEXATTRIB1SPROC glVertexAttrib1sARB = NULL;
PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1svARB = NULL;
PFNGLVERTEXATTRIB2DPROC glVertexAttrib2dARB = NULL;
PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dvARB = NULL;
PFNGLVERTEXATTRIB2FPROC glVertexAttrib2fARB = NULL;
PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fvARB = NULL;
PFNGLVERTEXATTRIB2SPROC glVertexAttrib2sARB = NULL;
PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2svARB = NULL;
PFNGLVERTEXATTRIB3DPROC glVertexAttrib3dARB = NULL;
PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dvARB = NULL;
PFNGLVERTEXATTRIB3FPROC glVertexAttrib3fARB = NULL;
PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fvARB = NULL;
PFNGLVERTEXATTRIB3SPROC glVertexAttrib3sARB = NULL;
PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3svARB = NULL;
PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4nbvARB = NULL;
PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4nivARB = NULL;
PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4nsvARB = NULL;
PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4nubARB = NULL;
PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4nubvARB = NULL;
PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4nuivARB = NULL;
PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4nusvARB = NULL;
PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bvARB = NULL;
PFNGLVERTEXATTRIB4DPROC glVertexAttrib4dARB = NULL;
PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dvARB = NULL;
PFNGLVERTEXATTRIB4FPROC glVertexAttrib4fARB = NULL;
PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fvARB = NULL;
PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4ivARB = NULL;
PFNGLVERTEXATTRIB4SPROC glVertexAttrib4sARB = NULL;
PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4svARB = NULL;
PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubvARB = NULL;
PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uivARB = NULL;
PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usvARB = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointerARB = NULL;

PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;
PFNGLGETINFOLOGARBPROC glGetInfoLogARB = NULL;

//typedef void (APIENTRYP PFNGLDELETEOBJECTARBPROC) (GLhandleARB obj);
//typedef GLhandleARB (APIENTRYP PFNGLGETHANDLEARBPROC) (GLenum pname);
//typedef void (APIENTRYP PFNGLDETACHOBJECTARBPROC) (GLhandleARB containerObj, GLhandleARB attachedObj);
//typedef void (APIENTRYP PFNGLSHADERSOURCEARBPROC) (GLhandleARB shaderObj, GLsizei count, const GLcharARB* *string, const GLint *length);
//typedef void (APIENTRYP PFNGLCOMPILESHADERARBPROC) (GLhandleARB shaderObj);
//typedef void (APIENTRYP PFNGLLINKPROGRAMARBPROC) (GLhandleARB programObj);
//typedef void (APIENTRYP PFNGLVALIDATEPROGRAMARBPROC) (GLhandleARB programObj);

bool ext_ARB_shaders = false;

#else

extern PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB;
extern PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB;
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
extern bool ext_ARB_multitexture;

// ARB_*_shader extentions

extern PFNGLATTACHSHADERPROC glAttachShaderARB;
extern PFNGLCOMPILESHADERPROC glCompileShaderARB;
extern PFNGLCREATEPROGRAMPROC glCreateProgramARB;
extern PFNGLCREATESHADERPROC glCreateShaderARB;
extern PFNGLDELETEPROGRAMPROC glDeleteProgramARB;
extern PFNGLDELETESHADERPROC glDeleteShaderARB;
extern PFNGLDETACHSHADERPROC glDetachShaderARB;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArrayARB;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArrayARB;
extern PFNGLGETACTIVEATTRIBPROC glGetActiveAttribARB;
extern PFNGLGETACTIVEUNIFORMPROC glGetActiveUniformARB;
extern PFNGLGETATTACHEDSHADERSPROC glGetAttachedShadersARB;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocationARB;
extern PFNGLGETPROGRAMIVPROC glGetProgramivARB;
extern PFNGLGETPROGRAMINFOLOGPROC glProgramInfoLogARB;
extern PFNGLGETSHADERIVPROC glGetShaderivARB;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLogARB;
extern PFNGLGETSHADERSOURCEPROC glGetShaderSourceARB;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocationARB;
extern PFNGLGETUNIFORMFVPROC glGetUniformfvARB;
extern PFNGLGETUNIFORMIVPROC glGetUniformivARB;
extern PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdvARB;
extern PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfvARB;
extern PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribivARB;
extern PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointervARB;
extern PFNGLISPROGRAMPROC glIsProgramARB;
extern PFNGLISSHADERPROC glIsShaderARB;
extern PFNGLLINKPROGRAMPROC glLinkProgramARB;
extern PFNGLSHADERSOURCEPROC glShaderSourceARB;
extern PFNGLUSEPROGRAMPROC glUseProgramARB;
extern PFNGLUNIFORM1FPROC glUniform1fpARB;
extern PFNGLUNIFORM2FPROC glUniform2fpARB;
extern PFNGLUNIFORM3FPROC glUniform3fpARB;
extern PFNGLUNIFORM4FPROC glUniform4fpARB;
extern PFNGLUNIFORM1IPROC glUniform1iARB;
extern PFNGLUNIFORM2IPROC glUniform2iARB;
extern PFNGLUNIFORM3IPROC glUniform3iARB;
extern PFNGLUNIFORM4IPROC glUniform4iARB;
extern PFNGLUNIFORM1FVPROC glUniform1fvARB;
extern PFNGLUNIFORM2FVPROC glUniform2fvARB;
extern PFNGLUNIFORM3FVPROC glUniform3fvARB;
extern PFNGLUNIFORM4FVPROC glUniform4fvARB;
extern PFNGLUNIFORM1IVPROC glUniform1ivARB;
extern PFNGLUNIFORM2IVPROC glUniform2ivARB;
extern PFNGLUNIFORM3IVPROC glUniform3ivARB;
extern PFNGLUNIFORM4IVPROC glUniform4ivARB;
extern PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fvARB;
extern PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fvARB;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fvARB;
extern PFNGLVALIDATEPROGRAMPROC glValidateProgramARB;
extern PFNGLVERTEXATTRIB1DPROC glVertexAttrib1dARB;
extern PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dvARB;
extern PFNGLVERTEXATTRIB1FPROC glVertexAttrib1fARB;
extern PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fvARB;
extern PFNGLVERTEXATTRIB1SPROC glVertexAttrib1sARB;
extern PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1svARB;
extern PFNGLVERTEXATTRIB2DPROC glVertexAttrib2dARB;
extern PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dvARB;
extern PFNGLVERTEXATTRIB2FPROC glVertexAttrib2fARB;
extern PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fvARB;
extern PFNGLVERTEXATTRIB2SPROC glVertexAttrib2sARB;
extern PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2svARB;
extern PFNGLVERTEXATTRIB3DPROC glVertexAttrib3dARB;
extern PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dvARB;
extern PFNGLVERTEXATTRIB3FPROC glVertexAttrib3fARB;
extern PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fvARB;
extern PFNGLVERTEXATTRIB3SPROC glVertexAttrib3sARB;
extern PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3svARB;
extern PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4nbvARB;
extern PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4nivARB;
extern PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4nsvARB;
extern PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4nubARB;
extern PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4nubvARB;
extern PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4nuivARB;
extern PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4nusvARB;
extern PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bvARB;
extern PFNGLVERTEXATTRIB4DPROC glVertexAttrib4dARB;
extern PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dvARB;
extern PFNGLVERTEXATTRIB4FPROC glVertexAttrib4fARB;
extern PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fvARB;
extern PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4ivARB;
extern PFNGLVERTEXATTRIB4SPROC glVertexAttrib4sARB;
extern PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4svARB;
extern PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubvARB;
extern PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uivARB;
extern PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usvARB;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointerARB;

extern PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
extern PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;
extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
extern PFNGLGETINFOLOGARBPROC glGetInfoLogARB;

extern bool ext_ARB_shaders;

#endif

#endif // GL_EXT_PROTOTYPES
