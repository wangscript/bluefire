/*

  Bluefire: Version & Build Information

  (c) Copyright 2004-2009 Jacob Hipps

  Status: Working, Level A

  To-do:
		???

  Implements:
		Version and build querying routines
*/

/***************************************************************************/

#define BF_CORE_VERSION	"0.10 WIP"	// version number

/***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Version Globals
char monthlist[][8] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
char bfCoreBuild[32];
char bfCoreCompiled[] = __DATE__ " @ " __TIME__;
char bfCoreVersion[] = BF_CORE_VERSION;

char* bf_get_version() {
	return bfCoreVersion;
}
char* bf_get_buildtime() {
	//return __DATE__ " @ " __TIME__;
	return bfCoreCompiled;
}

char* bf_get_build() {
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

	sprintf(bfCoreBuild,"%04i%02i%02i",yearx,monthy,dayx);

	return bfCoreBuild;
}

int bf_get_buildi() {
	return atoi(bf_get_build());
}