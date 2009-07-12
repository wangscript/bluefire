/*

  Bluefire: OpenGL Rendering Extention: Selection mode routines

  (c) Copyright 2004 Jacob Hipps

  Bluefire MDL Core: Winchester

*/


#define SEL_BUFFER_SIZE		2048

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bluefire.h"
#include "ogl.h"

unsigned int selbuffer[SEL_BUFFER_SIZE];

// enter selection mode using perspective projection
int bf_init_sel_mode(int x, int y, int rad) {

	glSelectBuffer(SEL_BUFFER_SIZE,(unsigned int*)&selbuffer);
	glRenderMode(GL_SELECT);
	glInitNames();			// initialize name stack
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	int vport[4];
	glGetIntegerv(GL_VIEWPORT,(int*)&vport);

	gluPickMatrix(x,renderx->height - y,rad,rad,(int*)&vport);

	gluPerspective(45.0f,(GLfloat)renderx->width/(GLfloat)renderx->height,0.1f,100.0f); // Perspective view

	glMatrixMode(GL_MODELVIEW);
	return 0;
}

// Enter selection mode using orthogonal projection
int bf_init_sel_mode_ortho(int x, int y, int rad, float neg_x, float pos_x, float neg_y, float pos_y) {
	
	glInitNames();	// initialize name stack
	glSelectBuffer(SEL_BUFFER_SIZE,(unsigned int*)&selbuffer);
	glRenderMode(GL_SELECT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	int vport[4];
	glGetIntegerv(GL_VIEWPORT,(int*)&vport);

	gluPickMatrix(x,renderx->height - y,rad,rad,(int*)&vport);

	glOrtho(neg_x,pos_x,pos_y,neg_y,-1.0f,100.0f); // Ortho View

	glMatrixMode(GL_MODELVIEW);
	return 0;
}

// return to framebuffer mode
int bf_exit_sel_mode(int **buffy) {

	int hits;

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();

	hits = glRenderMode(GL_RENDER);

	(*buffy) = (int*)&selbuffer;

	return hits;
}
