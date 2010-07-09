/*

  Bluefire: Error/exception/tracing functions

  (c) Copyright 2004 Jacob Hipps

  Bluefire MDL Core: Winchester

*/


#include <windows.h>
#include <stdio.h>
#include <signal.h>
#include "bluefire.h"

extern FILE* logfile;
extern void (__cdecl * go_down)();

extern int ready;

char trace_funcs[32][64];
char trace_src[32][64];
int  trace_line[32];
int  trace_depth;


int  dbg_cur_line;
char dbg_cur_file[512];
char dbg_cur_msg[1024];

CRITICAL_SECTION zCritical;


// function declarations
void exception(int sig);

/*********************************************************************
 IMPORTANT: DO NOT USE BEGIN_FUNC AND END_FUNC WITH THESE FUNCTIONS!!
**********************************************************************/

// setup exception handling
int bf_catchme() {

	#ifndef DISABLE_EXCEPTION_HANDLING
	signal(SIGFPE,bf_exception);
	signal(SIGILL,bf_exception);
	signal(SIGSEGV,bf_exception);
	#endif

	InitializeCriticalSection(&zCritical);

	return 0;
}

// exception handler
void bf_exception(int sig) {	

	char msgstring[512];
	char tracey[256];	

	EnterCriticalSection(&zCritical);

	ready = 0;

	zlogthis("\n*******************************************\nERROR REPORT\n\n");
	zlogthis("critical: Exception caught!\n");
	zlogthis("Call trace:\n");

	tracey[0] = 0;

	for(int i = 0; i < trace_depth; i++) {
		if(i > 0) strcat(tracey," -> ");
		strcat(tracey,trace_funcs[i]);
	}

	zlogthis("%s",tracey); // print call trace info
	zlogthis("\n");

	//if(dbg_cur_file[0] == 0) sprintf(dbg_cur_file,"(unknown)");
	//if(dbg_cur_msg[0] == 0) sprintf(dbg_cur_msg,"(none)");

	sprintf(dbg_cur_file,"(unknown)");
	sprintf(dbg_cur_msg,"(none)");

	zlogthis("dbg_cur_file (mem loc = 0x%08X) = \"%s\". dbg_cur_file[0] = 0x%02X\n",dbg_cur_file,dbg_cur_file,dbg_cur_file[0]);

	zlogthis("(%s, line %i)\n",trace_src[trace_depth - 1],trace_line[trace_depth - 1]);
	
	zlogthis("dbg_cur_file (mem loc = 0x%08X) = \"%s\". dbg_cur_file[0] = 0x%02X\n",dbg_cur_file,dbg_cur_file,dbg_cur_file[0]);

	zlogthis("ztrack info: file \"%s\", line %i\nztrack msg: \"%s\"\n",dbg_cur_file,dbg_cur_line,dbg_cur_msg);

	switch(sig) {
		case SIGFPE:
			zlogthis("Critical: Exiting due to divide by zero or overflow (SIGFPE)!");
			sprintf(msgstring,"Critical: Exiting due to divide by zero or overflow (SIGFPE)\n\nCall trace:\n%s",tracey);
			MessageBox(NULL,msgstring,"bfw dreamcatcher - Fatal Exception!",0);
			LeaveCriticalSection(&zCritical);
			DeleteCriticalSection(&zCritical);
			go_down();
		case SIGILL:
			zlogthis("Critical: Exiting due to illegal function call (SIGILL)!");
			sprintf(msgstring,"Critical: Exiting due to illegal function call (SIGILL)\n\nCall trace:\n%s",tracey);
			MessageBox(NULL,msgstring,"bfw dreamcatcher - Fatal Exception!",0);
			fclose(logfile);
			LeaveCriticalSection(&zCritical);
			DeleteCriticalSection(&zCritical);
			exit(254);
		case SIGSEGV:
			zlogthis("Critical: Exiting due to segmentation fault (SIGSEGV)!");
			sprintf(msgstring,"Critical: Exiting due to segmentation fault (SIGSEGV)\n\nCall trace:\n%s",tracey);
			MessageBox(NULL,msgstring,"bfw dreamcatcher - Fatal Exception!",0);
			fclose(logfile);
			LeaveCriticalSection(&zCritical);
			DeleteCriticalSection(&zCritical);
			exit(253);
		default:
			zlogthis("Critical: Exiting due to unknown fault. (sig = %i)\n",sig);
			sprintf(msgstring,"Critical: Exiting due to unknown fault!\n\nCall trace:\n%s",tracey);
			MessageBox(NULL,msgstring,"bfw dreamcatcher - Fatal Exception!",0);
			fclose(logfile);
			LeaveCriticalSection(&zCritical);
			DeleteCriticalSection(&zCritical);
			exit(250);
	}

	LeaveCriticalSection(&zCritical);
	DeleteCriticalSection(&zCritical);
	return;
}


void begin_func_trace(char func_trace[], int line, char source[]) {
	trace_depth++;
	strcpy(trace_funcs[trace_depth - 1],func_trace);
	strcpy(trace_src[trace_depth - 1],source);
	trace_line[trace_depth - 1] = line;
	return;
}

void end_func_trace() {
	dbg_cur_msg[0] = 0; // invalidate the ztrackme info
	dbg_cur_file[0] = 0;
	dbg_cur_line = 0;
	trace_depth--;
	return;
}

void reset_func_trace() {
	dbg_cur_msg[0] = 0;
	dbg_cur_file[0] = 0;
	dbg_cur_line = 0;
	trace_depth = 0;
	return;
}


void debug_linetrack(char *filename, int linenum, char *msg) {

	dbg_cur_line = linenum;
	strcpy(dbg_cur_file,filename);

	if(msg) strcpy(dbg_cur_msg,msg);
	else dbg_cur_msg[0] = 0;

	return;
}

