/*

  Bluefire: OpenGL Renderer: Version & Build Information

  (c) Copyright 2004-2010 Jacob Hipps

  Status: Working, Level A

  To-do:
		???

  Implements:
		Version and build querying routines
*/

/***************************************************************************/

#define BF_RENDERER_VERSION	"0.10 WIP"	// version number

/***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "../bluefire.h"

// Version Globals
char monthlist[][8] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
char rdxCoreBuild[32];
char rdxCoreCompiled[] = __DATE__ " @ " __TIME__;
char rdxCoreVersion[] = BF_RENDERER_VERSION;

void rdx_get_version(char *ver) {
	if(!ver) return;
	strcpy(ver,rdxCoreVersion);	
}

void rdx_get_buildtime(char *bt) {
	//return __DATE__ " @ " __TIME__;
	if(bt)	strcpy(bt,rdxCoreCompiled);
}

void rdx_get_build(char *bt) {	
	char ddizzle[] = __DATE__;
	char monthx[16];
	int dayx, yearx, monthy;

	// parse build time and create serial build string
	sscanf(ddizzle,"%s %i %i",monthx,&dayx,&yearx);
	for(int i = 0; i < 12; i++) {
		if(!strcmp(monthx,monthlist[i])) {
			monthy = i + 1;
			break;
		}
	}

	sprintf(rdxCoreBuild,"%04i%02i%02i",yearx,monthy,dayx);

	if(bt) strcpy(bt,rdxCoreBuild);
}

int rdx_get_buildi() {	
	rdx_get_build(NULL);
	return atoi(rdxCoreBuild);
}
