/*


  Bluefire: bfi - Bluefire-Python High-level Script Interface

  (c) Copyright 2006-2009 Jacob Hipps


  Status: WIP, Level 1
  ** This module is not yet ready for general use.

  Implements the scripting engine using Python.

  To do:
	***

  Contains:
	Python to C bf*.* wrapper functions
	C to Python wrapper functions

*/

#define NR_MEMREAD    // this is for the python header file to
                      // use our custom memory-load functions

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "python/Python.h"
#include "bluefire.h"

// Globals
BF_SCRIPT *scripts[MAX_SCRIPTS];
int bf_script_count;
extern void (__cdecl * go_down)();		// go_down: point to shutdown procedure defined by client program

int fcache_sz;

struct {
	char txt[512];
	int r, g, b, a; // font color & opacity
	int x, y;		// location on screen
	int fontdex;	// font index
	float op_time;	// Opacity duration
	float fade_time;// Fade duration
	float btime;	// timer
} fcache[FCACHE_SIZE];


// wrapper functions
PyObject* core_render_clrframe(PyObject *self);
PyObject* core_render_endframe(PyObject *self);
PyObject* core_render_initortho(PyObject *self);
PyObject* core_render_exitortho(PyObject *self);
PyObject* core_render_drawpic(PyObject *self, PyObject *args);
PyObject* core_render_drawpicx(PyObject *self, PyObject *args);
PyObject* core_render_drawobj(PyObject *self, PyObject *args);
PyObject* core_render_setcolor(PyObject *self, PyObject *args);
PyObject* core_render_ortho_ex(PyObject *self, PyObject *args);
PyObject* core_render_draw_line(PyObject *self, PyObject *args);
PyObject* core_render_drawfont(PyObject *self, PyObject *args);

PyObject* core_obj_count(PyObject *self);

PyObject* core_version(PyObject *self);
PyObject* core_logthis(PyObject *self, PyObject *args);



PyMethodDef bfCoreMethods[] = {
    {"version", (PyCFunction)core_version, METH_NOARGS, ""},
	{"logthis", (PyCFunction)core_logthis, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

PyMethodDef bfObjMethods[] = {
	{"count", (PyCFunction)core_obj_count, METH_NOARGS, "Returns the number of objects."},	
    {NULL, NULL, 0, NULL}
};

PyMethodDef bfImageMethods[] = {
    //{"count", (PyCFunction)core_image_count, METH_NOARGS, "Returns the number of images."},
    {NULL, NULL, 0, NULL}
};

PyMethodDef bfRenderMethods[] = {
    {"clrframe", (PyCFunction)core_render_clrframe, METH_NOARGS, "Clears the frame."},
	{"endframe", (PyCFunction)core_render_endframe, METH_NOARGS, ""},
	{"init_ortho", (PyCFunction)core_render_initortho, METH_NOARGS, ""},
	{"exit_ortho", (PyCFunction)core_render_exitortho, METH_NOARGS, ""},
	{"draw_pic", (PyCFunction)core_render_drawpic, METH_VARARGS, ""},
	{"draw_picx", (PyCFunction)core_render_drawpicx, METH_VARARGS, ""},
	{"draw_obj", (PyCFunction)core_render_drawobj, METH_VARARGS, ""},
	{"setcolor", (PyCFunction)core_render_setcolor, METH_VARARGS, ""},
	{"ortho_ex", (PyCFunction)core_render_ortho_ex, METH_VARARGS, ""},
	{"draw_line", (PyCFunction)core_render_draw_line, METH_VARARGS, ""},
	{"drawfont", (PyCFunction)core_render_drawfont, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

PyMethodDef bfLightMethods[] = {
    //{"count", (PyCFunction)core_light_count, METH_NOARGS, "Returns the number of images."},
    {NULL, NULL, 0, NULL}
};

PyMethodDef bfEnvMethods[] = {
    //{"count", (PyCFunction)core_env_count, METH_NOARGS, "Returns the number of images."},
    {NULL, NULL, 0, NULL}
};

int bfi_init() {
	BEGIN_FUNC("bfi_init")

    Py_Initialize();
	//zlogthisx("Python version %i.%i.%i initialized.",PY_MAJOR_VERSION,PY_MINOR_VERSION,PY_MICRO_VERSION);
	zlogthisx("Python version %s initialized.",PY_VERSION);
	Py_InitModule("bfcore", bfCoreMethods);
	zlogthisx("bfcore class routines loaded");
	Py_InitModule("bfobjs", bfObjMethods);
	zlogthisx("bfobjs class routines loaded");
	Py_InitModule("bfimages", bfImageMethods);
	zlogthisx("bfimages class routines loaded");
	Py_InitModule("bfrender", bfRenderMethods);
	zlogthisx("bfrender class routines loaded");
	Py_InitModule("bflights", bfLightMethods);
	zlogthisx("bflights class routines loaded");
	Py_InitModule("bfenv", bfEnvMethods);
	zlogthisx("bfenv class routines loaded");


    //PySys_SetObject("stdout", Py_BuildValue("s","core.errpassthru"));

	bf_script_count = 0;

	END_FUNC
	return 0;
}

int bfi_kill() {
	BEGIN_FUNC("bfi_kill")

	Py_Finalize();
	zlogthisx("Python unloaded");

	END_FUNC
	return 0;
}

int bfi_exec_script(int nummy, char* funcx) {
	BEGIN_FUNC("bfi_exec_script")
	char scbuffer[8192];

	BF_SCRIPT* thisexec = bf_script_getptr(nummy);
	zlogthisx("thisexec = 0x%08x, nummy = %i",thisexec,nummy);

	zlogthisx("executing \"%s\" in script %s",funcx,thisexec->res_id);

	PyObject *pRet, *pDict, *pErr;
		
	pDict = PyDict_New();	
	PyDict_SetItemString(pDict, "__builtins__", PyEval_GetBuiltins());

	//PyRun_SimpleString("import bfcore, bfrender, bfobjs, bfimages, bflights, bfenv\n");
	//sprintf(scbuffer,"%s\n\n%s\n",thisexec->segs[0]->seg_data,funcx);
	
	//strcpy(scbuffer,thisexec->segs[0]->seg_data);

	
	sprintf(scbuffer,			"import bfcore, bfrender, bfobjs, bfimages, bflights, bfenv\n"            "\n"			"def onProxy():\n"			"   bfrender.drawfont(0,200,200,255,255,255,128,\'this is a test from python ;)\')\n"            "   bfcore.logthis(\'drawfont() called!! :)\')\n"			"   return 13\n"
			"\n"
			);
	

	PyObject *xresult;
	PyObject    *main_module, *global_dict, *expression, *main_dict;
	main_module = PyImport_AddModule("__main__");
	zlogthis("check 1, main_module = 0x%08x\n");
	main_dict = PyModule_GetDict(main_module);
	zlogthis("check 2, main_dict = 0x%08x\n");
	xresult = PyRun_String(scbuffer,Py_file_input,main_dict,main_dict);
    //xresult = PyRun_Mem(scbuffer, sizeof(scbuffer), "xcore.py", main_dict, main_dict);
	//xresult = Py_CompileString(scbuffer, "<xcore.py>", Py_file_input);
	//PyErr_Print();
	zlogthis("check 3, xresult = 0x%08x\n");

    // Extract a reference to the function "func_name"
    // from the global dictionary
	expression = PyDict_GetItemString(main_dict, funcx);
	zlogthis("check 4, expression = 0x%08X\n",expression);

	if(expression) {
		PyObject_CallObject(expression, NULL);
	} else {
		zlogthis("Error: Function not found within loaded modules!\n");
	}

	zlogthis("check 5\n");

	END_FUNC
	return 0;
}


// this is usually called from bfz_loadexec(), but can also be
// called from other functions as needed.

int bfi_load_script(char *script_txt, int script_sz, char* res_id) {
	BEGIN_FUNC("bfi_load_script")

	int scdex = bf_script_add();
	BF_SCRIPT* thisexec = bf_script_getptr(scdex);
	strcpy(thisexec->res_id,res_id);
	
    if((thisexec->segs[0] = (BF_SCRIPT_SEG*)malloc(sizeof(BF_SCRIPT_SEG))) == NULL) {
		zlogthisx("Memory allocation error!");
		go_down();
	}

	thisexec->num_segs = 1;

	strcpy(thisexec->segs[0]->seg_name,"UNIFIED_TXT");
	thisexec->segs[0]->seg_sz = script_sz;
	thisexec->segs[0]->seg_type = BFI_SCRIPT_T_UNIFIED_TXT;

    if((thisexec->segs[0]->seg_data = (char*)malloc(script_sz)) == NULL) {
		zlogthisx("Memory allocation error!");
		go_down();
	}

	memcpy(thisexec->segs[0]->seg_data,script_txt,script_sz);
	thisexec->segs[0]->seg_data[script_sz] = 0;

	zlogthisx("seg_data = \n%s\n",thisexec->segs[0]->seg_data);

	END_FUNC
	return scdex;
}



// Script-pool accounting routines

int bf_script_add() {
	BEGIN_FUNC("bf_script_add")

	int nummy = bf_script_count;
	bf_script_count++;


	if((scripts[nummy] = (BF_SCRIPT*)malloc(sizeof(BF_SCRIPT))) == NULL) {
		zlogthisx("Memory allocation error!");
		go_down();
	}

	memset(scripts[nummy],0,sizeof(BF_SCRIPT)); // zero-out the memory space

	END_FUNC
	return nummy;
}


BF_SCRIPT* bf_script_getptr(int nummy) {	
	BEGIN_FUNC("bf_script_getptr")
	END_FUNC
	return scripts[nummy];
}


int bf_script_getnum(char script_name[]) {
	BEGIN_FUNC("bf_script_getnum")

	int nummy = -1;

	for(int i = 0; i < bf_script_count; i++) {
		if(!strcmp(script_name,scripts[i]->res_id)) {
			nummy = i;
			break;
		}
	}

	END_FUNC
	return nummy;
}


int bf_script_cnt() {
	BEGIN_FUNC("bf_script_cnt")
	END_FUNC
	return bf_script_count;
}

void bf_script_free(int nummy) {
	BEGIN_FUNC("bf_script_free")

	BF_SCRIPT *swimtime = scripts[nummy];

	// free the script

	int x;

	if(swimtime) {		
		for(int i = 0; i < swimtime->num_segs; i++) {
			if(swimtime->segs[i]->seg_data) {
				free(swimtime->segs[i]->seg_data);
			}
			free(swimtime->segs[i]);
		}
		free(swimtime);
	}
	

	// if the object we freed isn't the last one in queue, then
	// put the last object in queue in its place to prevent "holes" from forming
	if(nummy != (bf_script_count - 1)) {
		swimtime = scripts[bf_script_count];
	}

	bf_script_count--;

	END_FUNC
	return;
}

void bf_script_flush() {
	BEGIN_FUNC("bf_script_flush")
	
	for(int i = 0; i < bf_script_count; i++) {
		bf_script_free(i);
	}

	zlogthisx("script cache purged.");

	END_FUNC
	return;
}


// Other helper functions for font cache

int bf_script_ft_clear() {
	BEGIN_FUNC("bf_script_ft_clear")
	fcache_sz = 0;
	END_FUNC
	return 0;
}

int bf_script_ft_render() {
	BEGIN_FUNC("bf_script_ft_render")
	
	bf_enable_textures();

	float passed;
	float nowtime;
	float ccalpha;
	float p2time;
	float fadefrag;

	nowtime = bf_get_secs();

	for(int i = 0; i < fcache_sz; i++) {
		passed = nowtime - fcache[i].btime;

		
		if(passed >= fcache[i].op_time) {
			p2time = passed - fcache[i].op_time;
			if(p2time >= fcache[i].fade_time) {
				ccalpha = 0.0f;
				// <<< add code for auto-deletion
			} else {
				fadefrag = (fcache[i].fade_time - p2time) / fcache[i].fade_time;
                ccalpha = fadefrag * fcache[i].a;
			}
		} else {
			ccalpha = fcache[i].a;
		}
		bf_setcolor(float(fcache[i].r)/256.0f, float(fcache[i].g)/256.0f, float(fcache[i].b)/256.0f, float(ccalpha)/256.0f);
		bf_ft_print(fcache[i].fontdex, fcache[i].x, fcache[i].y, fcache[i].txt);
	}

	END_FUNC
	return 0;
}

int bf_script_ft_write(int fontdex, int r, int g, int b, int a, int x, int y, char *fmt, ...) {
	BEGIN_FUNC("bf_script_ft_write")
    char text[512]; // maximum of 512 bytes of characters

    va_list ap;
    va_start(ap, fmt);	
    vsprintf(text, fmt, ap);
    va_end(ap);

	strcpy(fcache[fcache_sz].txt,text);
	fcache[fcache_sz].fontdex = fontdex;
	fcache[fcache_sz].r = r;
	fcache[fcache_sz].g = g;
	fcache[fcache_sz].b = b;
	fcache[fcache_sz].a = a;
	fcache[fcache_sz].x = x;
	fcache[fcache_sz].y = y;
	fcache[fcache_sz].fontdex = fontdex;
	fcache[fcache_sz].op_time = 5.0f;
	fcache[fcache_sz].btime = bf_get_secs();
	fcache[fcache_sz].fade_time = 2.0f;
	fcache_sz++;

	END_FUNC
	return 0;
}

/*

Wrapper functions

*/

// bfcore.*
PyObject* core_logthis(PyObject *self, PyObject *args) {

	char *textout;

	if (!PyArg_ParseTuple(args, "s", &textout)) return NULL;

	zlogthisx("%s",textout);

    return Py_BuildValue("i", 0);    
}


PyObject* core_version(PyObject *self) {
	return	Py_BuildValue("s","Bluefire build "__DATE__" "__TIME__);
}


// bfrender.*

PyObject* core_render_clrframe(PyObject *self) {    
	bf_clrframe();	
    return Py_BuildValue("i", 0);
}

PyObject* core_render_endframe(PyObject *self) {    
	bf_endframe();	
    return Py_BuildValue("i", 0);
}

PyObject* core_render_initortho(PyObject *self) {    
	bf_init_ortho();	
    return Py_BuildValue("i", 0);
}

PyObject* core_render_exitortho(PyObject *self) {    
	bf_exit_ortho();	
    return Py_BuildValue("i", 0);
}

PyObject* core_render_drawpic(PyObject *self, PyObject *args) {    
	float x, y;
	int num;

	if (!PyArg_ParseTuple(args, "ffi", &x, &y, &num)) {
		zlogthis("SCRIPT ERROR: incorrect number of arguments to bfrender.draw_pic()\n");
		return NULL;
	}

	draw_pic(x,y,num);

    return Py_BuildValue("i", 0);
}

PyObject* core_render_drawpicx(PyObject *self, PyObject *args) {    
	float x, y, width, height, stretch;
	int num;

	if (!PyArg_ParseTuple(args, "fffffi", &x, &y, &width, &height, &stretch, &num)) {
		zlogthis("SCRIPT ERROR: incorrect number of arguments to bfrender.draw_picx()\n");
		return NULL;
	}

	draw_picx(x,y,height,width,stretch,num);

    return Py_BuildValue("i", 0);
}

PyObject* core_render_drawobj(PyObject *self, PyObject *args) {	
	int num;

	if (!PyArg_ParseTuple(args, "i", &num)) {
		zlogthis("SCRIPT ERROR: incorrect number of arguments to bfrender.draw_obj()\n");
		return NULL;
	}

	draw_objx(num);

    return Py_BuildValue("i", 0);
}

PyObject* core_render_setcolor(PyObject *self, PyObject *args) {	
	float r,g,b,a;

	if (!PyArg_ParseTuple(args, "ffff", &r, &g, &b, &a)) {
		zlogthis("SCRIPT ERROR: incorrect number of arguments to bfrender.setcolor()\n");
		return NULL;
	}

	bf_setcolor(r,g,b,a);

    return Py_BuildValue("i", 0);
}

PyObject* core_render_ortho_ex(PyObject *self, PyObject *args) {	
	float negx,posx,negy,posy;

	if (!PyArg_ParseTuple(args, "ffff", &negx,&posx,&negy,&posy)) {
		zlogthis("SCRIPT ERROR: incorrect number of arguments to bfrender.init_ortho_ex()\n");
		return NULL;
	}

	bf_init_ortho_ex(negx,posx,negy,posy);

    return Py_BuildValue("i", 0);
}

PyObject* core_render_draw_line(PyObject *self, PyObject *args) {	
	float x1,y1,x2,y2;

	if (!PyArg_ParseTuple(args, "(ff)(ff)", &x1,&y1,&x2,&y2)) {
		zlogthis("SCRIPT ERROR: incorrect number of arguments to bfrender.draw_line()\n");
		return NULL;
	}

	bf_draw_line(x1,y1,x2,y2);

    return Py_BuildValue("i", 0);
}

/*
PyObject* core_render_tprintf(PyObject *self, PyObject *args) {	
	int x,y;
	char *msg;

	if (!PyArg_ParseTuple(args, "(ff)s", &x,&y,&msg)) {
		zlogthis("SCRIPT ERROR: incorrect number of arguments to bfrender.draw_line()\n");
		return NULL;
	}

	bf_tprintf(x,y, BF_FT_FONT *font_ptr,msg);

    return Py_BuildValue("i", 0);
}
*/


PyObject* core_render_drawfont(PyObject *self, PyObject *args) {	
	int x,y;
	int ddx;
	int r,g,b,a;
	char *msg;

	if (!PyArg_ParseTuple(args, "iiiiiiis", &ddx,&r,&g,&b,&a,&x,&y,&msg)) {
		zlogthis("SCRIPT ERROR: incorrect number of arguments to bfrender.drawfont()\n");
		return NULL;
	}

	bf_script_ft_write(ddx, r, g, b, a, x, y, msg);

    return Py_BuildValue("i", 0);
}

// bfobjs.*
PyObject* core_obj_count(PyObject *self) {
    return Py_BuildValue("i", bf_obj_count());
}

