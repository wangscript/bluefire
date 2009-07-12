/*

  Bluefire: OpenGL Rendering Extention: Rendering & Rasterization routines

  (c) Copyright 2004-2006 Jacob Hipps

  Bluefire MDL Core: Winchester

*/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../bluefire.h"
#include "ogl.h"

// steps in radial blur
#define BF_RB_BLURSTEPS		10
#define BF_RB_RES			128

BF_FT_FONT *bleh_font;

// locals

int enable_blur = 1;
int show_normals = 0;
int rtx_num;

void bf_set_radblur(int state) {
	enable_blur = state;
	return;
}

void bf_set_shownormals(int state) {
	show_normals = state;
	return;
}

void bf_init_ortho() {
	BEGIN_FUNC("bf_init_ortho")

	// Ortho (2D)
	glMatrixMode(GL_PROJECTION); // Select The projection matrix
	glPushMatrix(); // Store The Projection Matrix
	glLoadIdentity(); // Reset The Projection Matrix
	glOrtho(0.0f,renderx->width - 1,renderx->height - 1,0.0f,-1.0f,1.0f); // Ortho View
	glMatrixMode(GL_MODELVIEW); // select the modelview matrix
	glLoadIdentity(); // reset to origin
	glDisable(GL_LIGHTING);	// disable lighting for ortho stuff (if you don't do this,
							// it looks really odd...)
	glDisable(GL_DEPTH_TEST);	// disable depth testing (you don't want the 3D objects
							// jutting through the interface as if it were a part of the
							// scene!)

	END_FUNC
	return;
}

void bf_init_ortho_ex(float neg_x, float pos_x, float neg_y, float pos_y) {
	BEGIN_FUNC("bf_init_ortho_ex")

	// Ortho (2D)
	glMatrixMode(GL_PROJECTION); // Select The projection matrix
	glPushMatrix(); // Store The Projection Matrix
	glLoadIdentity(); // Reset The Projection Matrix
	glOrtho(neg_x,pos_x,pos_y,neg_y,-1.0f,1000.0f); // Ortho View
	glMatrixMode(GL_MODELVIEW); // select the modelview matrix
	glLoadIdentity(); // reset to origin
	glDisable(GL_LIGHTING);	// disable lighting for ortho stuff (if you don't do this,
							// it looks really odd...)
	glDisable(GL_DEPTH_TEST);	// disable depth testing (you don't want the 3D objects
							// jutting through the interface as if it were a part of the
							// scene!)

	END_FUNC
	return;
}

void bf_render_actor(BF_ACTOR_DATA *actorx) {
	BEGIN_FUNC("bf_render_actor")	

	float x = actorx->x;
	float y = actorx->y;
	float z = actorx->z;

	glColor4f(actorx->r,actorx->g,actorx->b,actorx->a);

	glBegin(GL_LINES);

	glVertex3f(x - 1.0f, y, z);
	glVertex3f(x + 1.0f, y, z);

	glVertex3f(x, y - 1.0f, z);
	glVertex3f(x, y + 1.0f, z);

	glVertex3f(x, y, z - 1.0f);
	glVertex3f(x, y, z + 1.0f);


	glVertex3f(x - 0.5f, y, z);
	glVertex3f(x, y - 0.5f, z);

	glVertex3f(x - 0.5f, y, z);
	glVertex3f(x, y + 0.5f, z);

	glVertex3f(x + 0.5f, y, z);
	glVertex3f(x, y - 0.5f, z);

	glVertex3f(x + 0.5f, y, z);
	glVertex3f(x, y + 0.5f, z);

	glVertex3f(x, y, z + 0.5f);
	glVertex3f(x, y - 0.5f, z);

	glVertex3f(x, y, z + 0.5f);
	glVertex3f(x, y + 0.5f, z);

	glVertex3f(x, y, z - 0.5f);
	glVertex3f(x, y - 0.5f, z);

	glVertex3f(x, y, z - 0.5f);
	glVertex3f(x, y + 0.5f, z);


	glVertex3f(x, y, z + 0.5f);
	glVertex3f(x - 0.5f, y, z);

	glVertex3f(x, y, z + 0.5f);
	glVertex3f(x + 0.5f, y, z);

	glVertex3f(x, y, z - 0.5f);
	glVertex3f(x - 0.5f, y, z);

	glVertex3f(x, y, z - 0.5f);
	glVertex3f(x + 0.5f, y, z);

	if(actorx->bSelected) {

		glColor4f(0.75f,0.75f,1.0f,0.75f);

		glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
		glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);

		glVertex3f(x - 0.5f, y + 0.5f, z - 0.5f);
		glVertex3f(x - 0.5f, y + 0.5f, z + 0.5f);

		glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
		glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);

		glVertex3f(x + 0.5f, y + 0.5f, z - 0.5f);
		glVertex3f(x + 0.5f, y + 0.5f, z + 0.5f);


		glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
		glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);

		glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);
		glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);

		glVertex3f(x - 0.5f, y + 0.5f, z - 0.5f);
		glVertex3f(x + 0.5f, y + 0.5f, z - 0.5f);

		glVertex3f(x - 0.5f, y + 0.5f, z + 0.5f);
		glVertex3f(x + 0.5f, y + 0.5f, z + 0.5f);


		glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
		glVertex3f(x - 0.5f, y + 0.5f, z - 0.5f);

		glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);
		glVertex3f(x - 0.5f, y + 0.5f, z + 0.5f);

		glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
		glVertex3f(x + 0.5f, y + 0.5f, z - 0.5f);

		glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);
		glVertex3f(x + 0.5f, y + 0.5f, z + 0.5f);
	}

	glEnd();

	END_FUNC
	return;
}

void bf_exit_ortho() {
	BEGIN_FUNC("bf_exit_ortho")

	// Restore perspective matrix
	glMatrixMode(GL_PROJECTION); // select projection matrix
	glPopMatrix();				// load the old perspective matrix
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);		// re-enable the lighting
	glEnable(GL_DEPTH_TEST);	// re-enable depth testing

	END_FUNC
	return;
}

// clear frame and reset to origin
void bf_clrframe() {
	BEGIN_FUNC("bf_clrframe")

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear
	glLoadIdentity(); // Reset The Current Modelview Matrix

	END_FUNC
	return;
}

// flip the buffers after frame has been fully drawn
void bf_endframe() {
	BEGIN_FUNC("bf_endframe")

    SwapBuffers(renderx->hDC); // Swap Buffers

	END_FUNC
	return;
}


void bf_draw_billboard(BF_BLBRD *billy, BF_WPD *wpdx) {

	glLoadIdentity();
	glTranslatef(billy->x + wpdx->x, billy->y + wpdx->y, billy->z + wpdx->z);
	

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, billy->texid);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
    glVertex3f(billy->width / -2.0f, billy->height / 2.0f, 0.0f);    
	glTexCoord2f(0.0f, 1.0f);
    glVertex3f(billy->width / -2.0f, billy->height / -2.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);	
    glVertex3f(billy->width / 2.0f, billy->height / -2.0f, 0.0f);	
	glTexCoord2f(1.0f, 0.0f);
    glVertex3f(billy->width / 2.0f, billy->height / 2.0f, 0.0f);

	glEnd();

	return;
}



// render the environment

int bf_draw_env(float x, float y, float z, float or, float oy, float op) {
	BEGIN_FUNC("bf_draw_env")
	
	const float piover180 = 3.14159f / 180.0f;

	// translate/rotate main environment	
	glLoadIdentity();
	float ex, ey, ez;
	ey = y;
	ex = (z * sin(or * piover180)) + (x * cos(or * piover180));
	ez = (z * cos(or * piover180)) - (x * sin(or * piover180));

	glTranslatef(ex,ey,ez);
	glRotatef(or, 0.0f, 1.0f, 0.0f);	// rotation
	glRotatef(oy, 0.0f, 0.0f, 1.0f);	// yaw
	glRotatef(op, 1.0f, 0.0f, 0.0f);	// pitch
	
	int i, ix, polylim, texable, lm_enable;
	

	polylim = bf_poly_cnt();	// get number of polys

	BF_TRIANGLE_EX* thispoly;


	if(renderx->raster_mode == BF_FILL) texable = 1;
	else texable = 0;

	if(texable) {
		glEnable(GL_TEXTURE_2D);
	} else {
		glDisable(GL_TEXTURE_2D);
		//glDisable(GL_LIGHTING);
	}

	int lite_on = 0;


	for(i = 0; i < polylim; i++) {

		thispoly = bf_getpoly(i);
			
		BF_TEXNODE *blehtex = bf_getimg(thispoly->df_texdex);
		if(blehtex != 0) {
			glBindTexture(GL_TEXTURE_2D, blehtex->tex_id);
			glColor4f(1.0f,1.0f,1.0f,thispoly->opacity);
		} else {
			glDisable(GL_TEXTURE_2D);
			glColor4f(0.5f,0.5f,0.7f,thispoly->opacity);
		}


		// for lightmaps
		lm_enable = 0; // disable by default
		//if(ext_ARB_multitexture && (thispoly->flags & BF_HAS_LMAP)) {
		
		if(thispoly->lm_texdex != 0 && (renderx->flags & BF_RZ_LMAPS_ENABLED) && ext_ARB_multitexture) {
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, thispoly->lm_texdex);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);				
			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			lm_enable = 1;
		}
		

		glPushName(i);	// selection mode ID
		glBegin(GL_TRIANGLES);

		for(ix = 0; ix < 3; ix++) {
			if(texable) {				
				glTexCoord2f(-thispoly->tex[ix].u,-thispoly->tex[ix].v);
				//if(lm_enable) glMultiTexCoord2fARB(GL_TEXTURE1_ARB,thispoly->lm_tex[ix].u,thispoly->lm_tex[ix].v);
			}


			glVertex3f(thispoly->vertex[ix].x,thispoly->vertex[ix].y,thispoly->vertex[ix].z);
			glNormal3f(thispoly->normal.x,thispoly->normal.y,thispoly->normal.z);

		}		

		glEnd(); // TRIANGLES

		if(lm_enable) {
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE0_ARB);
		}

		// if the polygon is selected, give it a 33% blue highlight
		

		if(thispoly->flags & BF_TRI_SELECTED) {
			if(texable) {
				glDisable(GL_TEXTURE_2D);
				if(glIsEnabled(GL_LIGHTING)) {
					glDisable(GL_LIGHTING);
					lite_on = 1;
				}
			}


			glBegin(GL_TRIANGLES);
			for(ix = 0; ix < 3; ix++) {
				glColor4f(0.0f,0.0f,1.0f,0.33f);
				glVertex3f(thispoly->vertex[ix].x,thispoly->vertex[ix].y,thispoly->vertex[ix].z);
				glNormal3f(thispoly->normal.x,thispoly->normal.y,thispoly->normal.z);

				// shows 3d space world-coordinates
				//glColor4f(1.0f,1.0f,1.0f,1.0f);
				//bf_tprintf3d(thispoly->vertex[ix].x - 0.5f, thispoly->vertex[ix].y - 0.5f,thispoly->vertex[ix].z - 0.5f,bleh_font,"#%i uv(%.2f,%.2f)",ix,-thispoly->tex[ix].u,-thispoly->tex[ix].v);
			}
			glEnd();

			if(texable) {
				glEnable(GL_TEXTURE_2D);
				if(lite_on) glEnable(GL_LIGHTING);				
			}

		}



		glPopName();

	}		

	//glColor4f(1.0f,1.0f,1.0f,1.0f); // set full opacity

	END_FUNC
	return 0;
}


// bf_renderq: renders all objects in the object pool
// x/y/z position, or/oy/op orientation (rotation/yaw/pitch)
int bf_renderq(float x, float y, float z, float or, float oy, float op) {
	BEGIN_FUNC("bf_renderq")

	const float piover180 = 3.14159f / 180.0f;

	// translate/rotate main environment	
	glLoadIdentity();
	float ex, ey, ez;
	ey = y;
	ex = (z * sin(or * piover180)) + (x * cos(or * piover180));
	ez = (z * cos(or * piover180)) - (x * sin(or * piover180));

	glTranslatef(ex,ey,ez);
	glRotatef(or, 0.0f, 1.0f, 0.0f);	// rotation
	glRotatef(oy, 0.0f, 0.0f, 1.0f);	// yaw
	glRotatef(op, 1.0f, 0.0f, 0.0f);	// pitch

	glColor4f(1.0f,1.0f,1.0f,1.0f); // set full opacity

	BF_RNODE *thisobj;
	int num_objs = bf_obj_count();

	int i;

	// initial pass (for radial blur)
	for(i = 0; i < num_objs; i++) {
		thisobj = bf_getobject(i);
		if(thisobj && thisobj->bEnabled && thisobj->bVisible && thisobj->bRadBlur) {
			if(thisobj->bNoLight) glDisable(GL_LIGHTING);
			else glEnable(GL_LIGHTING);
			glLoadIdentity();
			glTranslatef(ex + thisobj->pos.x, ey + thisobj->pos.y, ez + thisobj->pos.z);
			glRotatef(or + thisobj->orient.r, 0.0f, 1.0f, 0.0f);
			glRotatef(oy + thisobj->orient.y, 0.0f, 0.0f, 1.0f);
			glRotatef(op + thisobj->orient.p, 1.0f, 0.0f, 0.0f);
			if(enable_blur) bf_rad_blur_pass1(i,rtx_num);
		}
	}

	/*
	// objects with bRenderFirst set
	for(i = 0; i < num_objs; i++) {
		thisobj = bf_getobject(i);
		if(thisobj && thisobj->bEnabled && thisobj->bVisible && thisobj->bRenderFirst) {
			if(thisobj->bNoLight) glDisable(GL_LIGHTING);
			else glEnable(GL_LIGHTING);			
			glLoadIdentity();
			glTranslatef(x + thisobj->pos.x, y + thisobj->pos.y, z + thisobj->pos.z);
			glRotatef(or + thisobj->orient.r, 0.0f, 1.0f, 0.0f);
			glRotatef(oy + thisobj->orient.y, 0.0f, 0.0f, 1.0f);
			glRotatef(op + thisobj->orient.p, 1.0f, 0.0f, 0.0f);
			if(thisobj->bObjX)	draw_objx(i);
			else					draw_obj(thisobj->obj_num);
			if(thisobj->bRadBlur && enable_blur) bf_rad_blur_pass2();
		}
	}

	*/

	// main pass
	for(i = 0; i < num_objs; i++) {
		thisobj = bf_getobject(i);
		if(thisobj && thisobj->bEnabled && thisobj->bVisible && !(thisobj->bRenderFirst || thisobj->bRenderLast )) {
			if(thisobj->bNoLight) glDisable(GL_LIGHTING);
			else glEnable(GL_LIGHTING);
			glLoadIdentity();

			
			glTranslatef(ex,ey,ez);
			glTranslatef(thisobj->pos.x, thisobj->pos.y, thisobj->pos.z);
			
			glRotatef(oy + thisobj->orient.y, 0.0f, 0.0f, 1.0f);
			glRotatef(op + thisobj->orient.p, 1.0f, 0.0f, 0.0f);
			glRotatef(or + thisobj->orient.r, 0.0f, 1.0f, 0.0f);

			
			glPushName(i);	// selection mode ID
			if(thisobj->bObjX)	draw_objx(i);
			else				draw_obj(thisobj->obj_num);
			glPopName();	// selection mode -- pop ID
			//if(thisobj->bRadBlur && enable_blur) bf_rad_blur_pass2(rtx_num);
			//bf_rad_blur_pass2(rtx_num);
		}
	}

	/*
	useless

	// objects with bRenderLast set
	for(i = 0; i < num_objs; i++) {
		thisobj = bf_getobject(i);
		if(thisobj && thisobj->bEnabled && thisobj->bVisible && thisobj->bRenderLast) {
			if(thisobj->bNoLight) glDisable(GL_LIGHTING);
			else glEnable(GL_LIGHTING);			
			glLoadIdentity();
			glTranslatef(x + thisobj->pos.x, y + thisobj->pos.y, z + thisobj->pos.z);
			glRotatef(or + thisobj->orient.r, 0.0f, 1.0f, 0.0f);
			glRotatef(oy + thisobj->orient.y, 0.0f, 0.0f, 1.0f);
			glRotatef(op + thisobj->orient.p, 1.0f, 0.0f, 0.0f);
			if(thisobj->bObjX)	draw_objx(i);
			else					draw_obj(thisobj->obj_num);
			if(thisobj->bRadBlur && enable_blur) bf_rad_blur_pass2();
		}
	}
	*/

	END_FUNC
	return 0;
}

void bf_draw_line(float x1, float y1, float x2, float y2) {
	BEGIN_FUNC("bf_draw_line")

	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();

	END_FUNC
}

// draws the specified texture from the texture pool at a location
void draw_pic(float x, float y, int num) {
	BEGIN_FUNC("draw_pic")

	BF_TEXNODE *thistex = bf_getimg(num);

	// a little safegaurd :)
	if(!thistex) {
		//MessageBox(NULL,"Program referenced undefined texture! See logfile for details!","Bluefire: Critical Error",MB_SYSTEMMODAL);
		bf_set_ready(0);
		zlogthis("critical: draw_pic: program referenced undefined texture #%i!\n",num);
		go_down();
	}

    glBindTexture(GL_TEXTURE_2D, thistex->tex_id); // Select texture to use
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2d(x, y);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2d(x + thistex->width, y);    
	glTexCoord2f(1.0f, 1.0f);
    glVertex2d(x + thistex->width, y + thistex->height);
	glTexCoord2f(0.0f, 1.0f);
    glVertex2d(x, y + thistex->height);

    glEnd();

	END_FUNC
}


// draws the specified texture at x,y with size width,height and stretched by stretch
void draw_picx(float x, float y, float width, float height, float stretch, int num) {
	BEGIN_FUNC("draw_picx")

	BF_TEXNODE *thistex = bf_getimg(num);

	// a little safegaurd :)
	if(!thistex) {
		MessageBox(NULL,"Program referenced undefined texture! See logfile for details!","Bluefire: Critical Error",MB_SYSTEMMODAL);
		zlogthis("critical: draw_picx: program referenced undefined texture #%i!\n",num);
		
		go_down();
	}


    glBindTexture(GL_TEXTURE_2D, thistex->tex_id); // Select texture to use
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f+stretch, 0.0f-stretch);
    glVertex2d(x, y);
    glTexCoord2f(1.0f-stretch, 0.0f-stretch);
    glVertex2d(x + width, y);
    glTexCoord2f(1.0f-stretch, 1.0f+stretch);
    glVertex2d(x + width, y + height);
    glTexCoord2f(0.0f+stretch, 1.0f+stretch);
    glVertex2d(x, y + height);

    glEnd();

	END_FUNC
}

int draw_obj(int nummy) {
	// NOT IMPLEMENTED! OBSOLETE!
	return 0;
}

// draws an object from the object pool that is in ObjX format
int draw_objx(int nummy) {
	BEGIN_FUNC("draw_objx")

	int i, x, z;
	int texable = 0;	
	float mval;

	BF_RNODE *swimtime = bf_getobject(nummy);

	BF_TEXNODE *temptex;

	for(i = 0; i < swimtime->surfaces; i++) {

		if(renderx->raster_mode == BF_FILL) {
			glColor4f(swimtime->surfdex[i].cr,swimtime->surfdex[i].cg,swimtime->surfdex[i].cb,swimtime->surfdex[i].trans);

			if(swimtime->surfdex[i].filename[0] && swimtime->surfdex[i].filename[1]) {
				glEnable(GL_TEXTURE_2D);
				unsigned int texy = bf_getimg(swimtime->surfdex[i].snum)->tex_id;
				glBindTexture(GL_TEXTURE_2D, texy);				
				// TEXTURE-UNIT #0 (color/diffuse texture)
				glActiveTextureARB(GL_TEXTURE0_ARB);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texy);
				glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
				glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);

				// TEXTURE-UNIT #1 (lightmap) [*********TESTING*************************]
				/*
				if(ext_ARB_multitexture) {
					glActiveTextureARB(GL_TEXTURE1_ARB);				
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, textures[9]);
					glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);				
					glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
					glActiveTextureARB(GL_TEXTURE0_ARB);
				}
				*/
				texable = 1;
			} else {
				glDisable(GL_TEXTURE_2D);
				texable = 0;
			}
		} else {
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			texable = 0;
		}

		float last_x, last_y, last_z;

		for(z = 0; z < swimtime->surfdex[i].scount; z++) {

			glBegin(GL_TRIANGLES);

			for(x = 0; x < 3; x++) {	

				if(texable) {
					glTexCoord2f(-swimtime->vtexinfo[i][z].tex[x].u,-swimtime->vtexinfo[i][z].tex[x].v);
					if(ext_ARB_multitexture) glMultiTexCoord2fARB(GL_TEXTURE1_ARB,-(swimtime->vtexinfo[i][z].tex[x].u/2.0f)+0.5f,-(swimtime->vtexinfo[i][z].tex[x].v/2.0f)+0.5f);
				}

				glVertex3f(swimtime->vtexinfo[i][z].vertex[x].x,swimtime->vtexinfo[i][z].vertex[x].y,swimtime->vtexinfo[i][z].vertex[x].z);

				/*
				if(x == 2) {
					glVertex3f(swimtime->vtexinfo[i][z].vertex[0].x,swimtime->vtexinfo[i][z].vertex[0].y,swimtime->vtexinfo[i][z].vertex[0].z);
				} else {
					glVertex3f(swimtime->vtexinfo[i][z].vertex[x+1].x,swimtime->vtexinfo[i][z].vertex[x+1].y,swimtime->vtexinfo[i][z].vertex[x+1].z);
				}
				*/

				glNormal3f(swimtime->vtexinfo[i][z].normal.x,swimtime->vtexinfo[i][z].normal.y,swimtime->vtexinfo[i][z].normal.z);				
				
			}
			glEnd(); // TRIANGLES

			if(show_normals) {
				if(texable) glDisable(GL_TEXTURE_2D);
				glColor4f(1.0f,1.0f,0.0f,0.0f);
				mval = 1.0f;
				glBegin(GL_LINES);
				for(x = 0; x < 3; x++) {	
					glVertex3f(swimtime->vtexinfo[i][z].vertex[x].x,swimtime->vtexinfo[i][z].vertex[x].y,swimtime->vtexinfo[i][z].vertex[x].z);
					glVertex3f(swimtime->vtexinfo[i][z].vertex[x].x + (swimtime->vtexinfo[i][z].normal.x * mval),swimtime->vtexinfo[i][z].vertex[x].y + (swimtime->vtexinfo[i][z].normal.y * mval),swimtime->vtexinfo[i][z].vertex[x].z + (swimtime->vtexinfo[i][z].normal.z * mval));
				}
				glEnd();

				if(texable) glEnable(GL_TEXTURE_2D);
			}
		}

		if(ext_ARB_multitexture && texable) {
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE0_ARB);
		}

	}
	

	//glDisable(GL_LIGHTING);
	//glColor4f(1.0f,0.0f,0.0f,1.0f);
	//eprintf(0,0,0,"%s [%i]",swimtime->name,swimtime->surfaces);

	END_FUNC
	return 0;
}


// shows a display list

void show_ds (int listnum) {
	BEGIN_FUNC("show_ds")

	// NOT IMPLEMENTED!
	//glCallList(dlist[listnum]);

	END_FUNC
}

// deletes a d-list

void delete_ds (int listnum, int glyphs) {
	BEGIN_FUNC("delete_ds")

	// NOT IMPLEMENTED!
	//glDeleteLists(listnum, glyphs);

	END_FUNC
}

// sets the color & alpha

void bf_setcolor (float r, float g, float b, float a) {
	BEGIN_FUNC("bf_setcolor")

	glColor4f(r,g,b,a);

	END_FUNC
}

// texture shaz

void bf_enable_textures() {
	BEGIN_FUNC("bf_enable_textures")

	glEnable(GL_TEXTURE_2D);

	END_FUNC
}

void bf_disable_textures() {
	BEGIN_FUNC("bf_disable_textures")

	glDisable(GL_TEXTURE_2D);

	END_FUNC
}


// initializes the texture for use in radial blur
// (must be called BEFORE rad_blur_passX is called!)

void bf_init_radblur(int img_num) {
	BEGIN_FUNC("bf_init_radblur")

	BF_TEXNODE *radtex = bf_getimg(img_num);
	bf_create_texture(128, 128, img_num);
	strcpy(radtex->respkg,"core");
	strcpy(radtex->resid,"radtex");

	rtx_num = img_num;


	END_FUNC
	return;
}

// radial blur first pass
// (this must be done before any other object is drawn)

void bf_rad_blur_pass1(int obj_num, int tex_num) {
	BEGIN_FUNC("bf_rad_blur_pass1")

	glViewport(0,0,BF_RB_RES,BF_RB_RES); // change to smaller screen to match aspect ratio
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black Background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen

	draw_objx(obj_num);

	unsigned int texy = bf_getimg(tex_num)->tex_id;

	glBindTexture(GL_TEXTURE_2D,texy);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 0, 0, BF_RB_RES, BF_RB_RES, 0); // copy the screen to the texture
	glViewport(0,0,renderx->width,renderx->height); // revert to old screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black Background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen

	END_FUNC
	return;
}

// radial blur second pass
void bf_rad_blur_pass2(int tex_num) {
	BEGIN_FUNC("bf_rad_blur_pass2")

	glEnable(GL_TEXTURE_2D);

	// Ortho
	bf_init_ortho();

	// draw the blurring effect

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE); // change blending mode
	float zoomy = 0.0f;
	float fadey = 0.15f;

	// override tex_num value <----------------------------------<<<<
	tex_num = rtx_num;

	unsigned int texy = bf_getimg(tex_num)->tex_id;
	
	for(int i = 0; i < BF_RB_BLURSTEPS; i++) {
		glColor4f(1.0f,1.0f,1.0f,fadey); // fade out
		
		glBindTexture(GL_TEXTURE_2D, texy); // Select texture to use
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f+zoomy, 1.0f-zoomy);
		glVertex2d(0, 0);
		glTexCoord2f(1.0f-zoomy, 1.0f-zoomy);
		glVertex2d(renderx->width, 0);
		glTexCoord2f(1.0f-zoomy, 0.0f+zoomy);
		glVertex2d(renderx->width, renderx->height);
		glTexCoord2f(0.0f+zoomy, 0.0f+zoomy);
		glVertex2d(0, renderx->height);
		glEnd();

		zoomy += 0.017f;
		fadey /= 2.0f; //0.7f / 15.0f;			
	}

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // revert to old blend method
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);

	// Restore perspective view
	bf_exit_ortho();

	END_FUNC
	return;
}

void bf_apply_lights() {
	BEGIN_FUNC("bf_apply_lights")
	
	// render_ogl does not use bf_apply_lights
	// (this is used for render_d3d and possibly others)
	// in OpenGL, all lights are applied by default

	END_FUNC
	return;
}

void bf_update_light(BF_LIGHT *light) {

/*
typedef struct {	
	int lumens;			// luminous flux (lumens)
	float r, g, b, a;	// color/alpha
	float x, y, z, inf;	// position
	float nx,ny,nz;		// vector normal (for spot lights)
	int l_type;			// 0 = point
						// 1 = direction
						// 2 = spot
						// 3 = ambient
						// 4 = area
						// 5 = specular
	bool bEnabled;		// enabled?
	bool bDynamic;		// effect dynamic shadowing/lighting?
	bool bNegative;		// is it a subtractive/negative light?
	bool bNoLMaps;		// does not effect lightmaps?
	char resid[64];		// resource id/name
} BF_LIGHT;
*/

	glEnable(GL_LIGHTING);

	float zeroz[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float lcolor[4] = { 1.0f, 0.8f, 0.8f, 0.75f };
	float acolor[4] = { 0.6f, 0.6f, 0.8f, 0.2f };
	float pos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, (float*)&acolor);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, (float*)&lcolor);
	glLightfv(GL_LIGHT1, GL_POSITION, (float*)&pos);

	/*
	// zero-out stuff
	glLightfv(GL_LIGHT1, GL_DIFFUSE, (float*)&zeroz);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, (float*)&zeroz);
	glLightfv(GL_LIGHT1, GL_CONSTANT_ATTENUATION, (float*)&zeroz);
	glLightfv(GL_LIGHT1, GL_AMBIENT, (float*)&zeroz);

	// set the position (used with all light types)
	glLightfv(GL_LIGHT1, GL_POSITION, &light->x);

	switch(light->l_type) {
		case 0:	// point
		case 1: // directional
		case 4:	// area
			glLightfv(GL_LIGHT1, GL_DIFFUSE, (float*)(&light->r));
			break;
		case 3: // ambient
			glLightfv(GL_LIGHT1, GL_AMBIENT, (float*)&light->r);
			break;
		case 2: // spotlight			
			glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, (float*)&light->nx);
			glLightfv(GL_LIGHT1, GL_CONSTANT_ATTENUATION, (float*)&light->lumens);
			break;
		case 5: // specular highlighting
			glLightfv(GL_LIGHT1, GL_SPECULAR, (float*)&light->r);
			break;
	}
	*/
	glEnable(GL_LIGHT1);

	return;
}