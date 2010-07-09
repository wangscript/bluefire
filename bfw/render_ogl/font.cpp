/*

  Bluefire: OpenGL Rendering Extention: Font Routines

  (c) Copyright 2004 Jacob Hipps

  Bluefire MDL Core: Winchester

*/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bluefire.h"
#include "ogl.h"

extern BF_FT_FONT *bleh_font;

int bf_ttf_font(char *fontname, int start_glyph, int num_glyphs, int size, BF_FT_FONT* font_ptr) {

    //zlogthis("load_ttfont: loading font \'%s\' as number %i (glyphs = %i, ASCII start = %i)...\n", fontname, num, num_glyphs, start_glyph);
    HFONT winfont, boufont;
    
    font_ptr->start = start_glyph;
    font_ptr->dlist = glGenLists(num_glyphs);
    font_ptr->tex_num = 0; // don't use a texture

    winfont = CreateFont(size, 0 /*width*/, 0, 0, FW_NORMAL /*font weight*/, FALSE /*italic, underline, strikeout*/, FALSE, FALSE, ANSI_CHARSET /*charset*/, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE /*family*/ | DEFAULT_PITCH /*pitch*/, fontname);
    boufont = (HFONT)SelectObject(renderx->hDC, winfont);
    wglUseFontBitmaps(renderx->hDC, start_glyph, num_glyphs, font_ptr->dlist);
    SelectObject(renderx->hDC, boufont);
    DeleteObject(winfont);

	zlogthis("load_ttfont: debug font set as current.\n");
	bleh_font = font_ptr;

    return 0;
}



void bf_ft_dlist(BF_FT_FONT *font_ptr, int start, int span, int dlist) {
	
	font_ptr->dlist = glGenLists(span);
	font_ptr->start = start - 16;	

	glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &font_ptr->tex_num);
	glBindTexture(GL_TEXTURE_2D, font_ptr->tex_num);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
	
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, font_ptr->sz * 16, font_ptr->sz * 16, 0, GL_ALPHA, GL_UNSIGNED_BYTE, font_ptr->facedex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, font_ptr->sz * 16, font_ptr->sz * 16, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, font_ptr->facedex);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DIFFUSE, font_ptr->sz * 16, font_ptr->sz * 16, 0, GL_DIFFUSE, GL_UNSIGNED_BYTE, font_ptr->facedex);

	int i;
	int j = start;

	float cx, cy;

    for (i = j; i < (span + start); i++) {
        cx = float(i%16)/float(16); // X Position Of Current Character
        cy = float(i/16)/float(16); // Y Position Of Current Character

		zlogthis("%i[%c] (%.4f,%.4f) ha(%i) vo(%i)\n",i,i,cx,cy,font_ptr->kern[i],font_ptr->ybear[i]);

        glNewList(font_ptr->dlist + i - start, GL_COMPILE);

		glTranslated(0,font_ptr->ybear[i],0); // y-bearing
        glBegin(GL_QUADS);

        glTexCoord2f(cx,cy); // bottom left
        glVertex2i(0,font_ptr->sz); // Vertex Coord (Top Left)

        glTexCoord2f(cx+0.0625f,cy); // Texture Coord (Bottom Right)
        glVertex2i(font_ptr->sz,font_ptr->sz); // Vertex Coord (Top Right)

        glTexCoord2f(cx+0.0625f,cy-0.0625f); // Texture Coord (Top Right)
        glVertex2i(font_ptr->sz,0); // Vertex Coord (Bottom Right)

        glTexCoord2f(cx,cy-0.0625f); // Texture Coord (Top Left)
        glVertex2i(0,0);

		glEnd();

		glTranslated(font_ptr->kern[i],-font_ptr->ybear[i],0); // kerning adj./neg. y-bearing
		glEndList();
	}

	// For vertically oriented type
	/*
	for(i = 0; i < span; i++) {
		glNewList(font_ptr->dlist + i, GL_COMPILE);
		glBegin(GL_QUADS);

		glTexCoord2f(0, float(j)/256.0f + (1.0f/256.0f));
		glVertex2i(0,font_ptr->sz);

		glTexCoord2f(float(font_ptr->kern[j]) / float(font_ptr->sz), float(j)/256.0f + (1.0f/256.0f));
		glVertex2i(font_ptr->kern[j],font_ptr->sz);

		glTexCoord2f(float(font_ptr->kern[j]) / float(font_ptr->sz), float(j)/256.0f);
		glVertex2i(font_ptr->kern[j],0);

		glTexCoord2f(0, float(j)/256.0f);
		glVertex2i(0,0);

		glEnd();
		glTranslated(font_ptr->kern[i],0,0); // kerning adj.
		glEndList();

		j++;
	}
	*/

	return;
}


int bf_tprintf(int x, int y, BF_FT_FONT *font_ptr, const char *fmt, ... ) {

    char text[512]; // maximum of 512 characters

	bf_disable_lighting();
	bf_enable_textures();

	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, font_ptr->tex_num);

	//glColor4f(1.0f,1.0f,1.0f,1.0f);

	bfr_set_blending(BF_AMODE_LUMA);

    va_list ap;
    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);
    glRasterPos2d(x, y); // set the position

    glPushAttrib(GL_LIST_BIT); // push attributes
    glListBase(font_ptr->dlist - font_ptr->start); // choose display lists to use
    glCallLists(strlen(text), GL_UNSIGNED_BYTE, text); // display the string using the display list

    glPopAttrib(); // pop the attributes
	
	bfr_set_blending(BF_AMODE_NORMAL);

    return 0;
}


int bf_tprintf3d(float x, float y, float z, BF_FT_FONT *font_ptr, const char *fmt, ... ) {

    char text[512]; // maximum of 512 characters

	int lite = glIsEnabled(GL_LIGHTING);

	if(lite) glDisable(GL_LIGHTING);

	//glColor4f(1.0f,1.0f,1.0f,1.0f);

    va_list ap;
    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);

	glLoadIdentity();
    glTranslatef(x, y, z); // set the position
    glPushAttrib(GL_LIST_BIT); // push attributes
    glListBase(font_ptr->dlist - font_ptr->start); // choose display lists to use
    glCallLists(strlen(text), GL_UNSIGNED_BYTE, text); // display the string using the display list	
    glPopAttrib(); // pop the attributes

	if(lite) glEnable(GL_LIGHTING);

    return 0;
}
