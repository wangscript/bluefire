/*

  Bluefire: OpenGL Rendering Extention: Debug and state query functions

  (c) Copyright 2004 Jacob Hipps

  Bluefire MDL Core: Winchester

  added 09.27.2004

*/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bluefire.h"
#include "ogl.h"

void bf_query_state() {

	char enabled[] = "Enabled";
	char disabled[] = "Disabled";

	int viewport[4];
	float depth_range[2];
	float ccolor[4];
	float p_size, l_size;

	glGetIntegerv(GL_VIEWPORT, (int*)&viewport);
	glGetFloatv(GL_DEPTH_RANGE, (float*)&depth_range);
	glGetFloatv(GL_CURRENT_COLOR, (float*)&ccolor);
	glGetFloatv(GL_POINT_SIZE, (float*)&p_size);
	glGetFloatv(GL_LINE_WIDTH, (float*)&l_size);

	zlogthis("OpenGL render state:\n"
			 "\tLighting: %s\n"
			 "\tViewport: %i x %i x %i x %i\n"
			 "\tDepth range: [%.1f,%.1f]\n"			 
			 "\tCurrent color: %.2f,%.2f,%.2f,%.2f\n"
			 "\tPoint antialiasing: %s\n"
			 "\tPoint size: %2.2f\n"
			 "\tLine antialiasing: %s\n"
			 "\tLine width: %2.2f\n"
			 "\tCulling: %s\n"
			 "\tPolygon antialiasing: %s\n"
			 "\tTextures (2D): %s\n"
			 "\tBlending: %s\n"
			 "\tDepth testing: %s\n"
			 "\n"

			 ,glIsEnabled(GL_LIGHTING) ? enabled : disabled
			 ,viewport[0],viewport[1],viewport[2],viewport[3]
			 ,depth_range[0],depth_range[1]			 
			 ,ccolor[0],ccolor[1],ccolor[2],ccolor[3]
			 ,glIsEnabled(GL_POINT_SMOOTH) ? enabled : disabled
			 ,p_size
			 ,glIsEnabled(GL_LINE_SMOOTH) ? enabled : disabled
			 ,l_size
			 ,glIsEnabled(GL_CULL_FACE) ? enabled : disabled
			 ,glIsEnabled(GL_POLYGON_SMOOTH) ? enabled : disabled
			 ,glIsEnabled(GL_TEXTURE_2D) ? enabled : disabled
			 ,glIsEnabled(GL_BLEND) ? enabled : disabled
			 ,glIsEnabled(GL_DEPTH_TEST) ? enabled : disabled
	);

	zlogthis("Bluefire render state:\n"
			 "\trenderx = %08x\n"
			 "\thRC     = %08x\n"
			 "\thDC     = %08x\n"
			 "\tviewport= %ix%i\n"
			 "\tBG color= %.2f,%.2f,%.2f\n"
			 "\traster  = %i\n"
			 "\n"
			 ,renderx,renderx->hRC,renderx->hDC
			 ,renderx->width,renderx->height
			 ,renderx->bgR,renderx->bgG,renderx->bgB
			 ,renderx->raster_mode
	);

	return;
}

