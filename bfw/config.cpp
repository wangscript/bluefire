/*

Bluefire Settings Parser
(c) Copyright 2009 Jacob Hipps/Neo-Retro Group, all rights reserved.

Started: 2009.01.21

Implements:
	bf_load_config()
	bf_config_*()

*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bluefire.h"


BF_CONFIG_SPEC bfconfig[] = {
	{ "ScreenRes", "1024x768" },	// Screen Resolution (WxH)
	{ "ScreenDepth", "32" },		// Screen Depth (bpp)
	{ "RenderDLL", "render_ogl\\render_ogl.dll" },	// Rendering DLL Module filename
	{ "VertexShader", "shader.vert" } ,	// Vertex shader filename
	{ "FragmentShader", "shader.frag" } ,	// Fragment shader filename
	{ "FullScreen", "0" },			// Fullscreen (bool)
	{ NULL, NULL }	// Last entry must be NULL!
};


/*

Config Value Retrieval Functions

bf_config_str - Returns string value from config array
bf_config_int - Returns converted integer value from config array
bf_config_float - Returns converted float value from config array

*/

char* bf_config_str(char *varname) {
	BEGIN_FUNC("bf_config_str")

	int nummy = 0;

	while(bfconfig[nummy].varname[0]) {				// Cycle through until we reach the NULL key
		if(!strcmp(varname,bfconfig[nummy].varname)) {
			END_FUNC
			return bfconfig[nummy].value;
		}
		nummy++;
	}

	END_FUNC
	return NULL;
}

int bf_config_int(char *varname) {
	BEGIN_FUNC("bf_config_int")

	int nummy = 0;
	int retvali = 0;

	while(bfconfig[nummy].varname[0]) {				// Cycle through until we reach the NULL key
		if(!strcmp(varname,bfconfig[nummy].varname)) {
			sscanf(bfconfig[nummy].value,"%i",&retvali);
			END_FUNC
			return retvali;
		}
		nummy++;
	}

	END_FUNC
	return 0;
}

float bf_config_float(char *varname) {
	BEGIN_FUNC("bf_config_float")

	int nummy = 0;
	float retvalf = 0.0f;

	while(bfconfig[nummy].varname[0]) {				// Cycle through until we reach the NULL key
		if(!strcmp(varname,bfconfig[nummy].varname)) {
			sscanf(bfconfig[nummy].value,"%f",&retvalf);
			END_FUNC
			return retvalf;
		}
		nummy++;
	}

	END_FUNC
	return 0.0f;
}

int bf_load_config() {
	BEGIN_FUNC("bf_load_config")

	FILE *setfile;
    char tkn_key[128];
	char tkn_val[128];
	char thisline[256];
	int nummy;

	// open the file
	if((setfile = fopen(CONFIG_FILENAME,"r")) == NULL) {
		zlogthisx("Unable to open config file %s for reading!",CONFIG_FILENAME);
		END_FUNC
		return 1;
	}

	char *zloc;
	char tchar;
	int zmode;
	int i;
	int ilen;
	int linenum = 0;

	// parse file
	while(!feof(setfile)) {
		
		linenum++;

		tkn_key[0] = 0; // reset tokens
		tkn_val[0] = 0;

		thisline[0] = 0;
		fgets(thisline,255,setfile);

		// tokenizer
		zmode = 0;
		for(i = 0; i < strlen(thisline); i++) {
			tchar = thisline[i];
			
			switch(tchar) {
				case '#':
				case '\n':
				case '\r':
				case 0:
                    zmode = 9;
					break;
				case '\t':
					break;
				case '=':
					if(zmode == 0) zmode = 1;
					else zmode = 9;
					break;
				case '\"':
					if(zmode == 1) zmode = 2;
					else if(zmode == 2) zmode = 9;
					break;
				case ' ':
					if(zmode != 2) break;
				default:
					if(zmode == 0) {
						ilen = strlen(tkn_key);
                        tkn_key[ilen] = tchar;
						tkn_key[ilen+1] = 0;
					} else if(zmode == 1 || zmode == 2) {
						ilen = strlen(tkn_val);
                        tkn_val[ilen] = tchar;
						tkn_val[ilen+1] = 0;
					}
			}
			if(zmode == 9) break;
		}

		if(tkn_key[0] != 0) {

			nummy = 0;
			while(bfconfig[nummy].varname[0]) {				// Cycle through until we reach the NULL key
				if(!strcmp(tkn_key,bfconfig[nummy].varname)) {
					strcpy(bfconfig[nummy].value,tkn_val);	// Copy to bfconfig if keys match
					break;
				}
				nummy++;
			}

			if(bfconfig[nummy].varname[0] == NULL) {
				zlogthisx("Line %i : Unrecognized Identifier : [%s] = \"%s\"",linenum,tkn_key,tkn_val);
			}
		}
	}

	// close config file
	fclose(setfile);

	END_FUNC
	return 0;
}