/*

  Bluefire: OpenGL Rendering Extention: FreeType2 Font Routines

  (c) Copyright 2006 Jacob Hipps

  Bluefire MDL Core: Winchester

*/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bluefire.h"
#include "ogl.h"

#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

struct {
	float h;											// Holds The Height Of The Font.
	GLuint ft_textures[128];									// Holds The Texture Id's 
	GLuint list_base;									// Holds The First Display List Id
} ft_font;


FT_Library library;

// exported
int  bf_ft_init();
void bf_ft_kill();
void bf_ft_addfont(const char * fname, unsigned int h);
void bf_ft_print(int font_id, float x, float y, const char *fmt, ...);
void bf_ft_font_remove();

extern int atr_bfstate;	// render state attribute index
extern short bfstate;	// render state bitmask
						// 1	= Lighting enabled
						// 2	= Texturing enabled
#define update_state glVertexAttrib1sARB(atr_bfstate,bfstate)

// local utility functions (private)
void ft_make_dlist ( FT_Face face, char ch, GLuint list_base, GLuint * tex_base );
inline void pushScreenCoordinateMatrix();
inline void pop_projection_matrix();
inline int next_p2 (int a);



int bf_ft_init() {
	BEGIN_FUNC("bf_ft_init")

	if(FT_Init_FreeType(&library)) {
		zlogthis("bf_ft_init: Error initializing FreeType. Type rasterization unavailable.\n");
		return -1;
	}

	int vmaj, vmin, vpat;

	FT_Library_Version(library, &vmaj, &vmin, &vpat);

	zlogthis("bf_ft_init: FreeType %i.%i.%i initialized succesfully.\n",vmaj,vmin,vpat);

	END_FUNC
	return 0;
}

/*
int bf_ft_open(char* filename, BF_FT_FONT* font_ptr) {
	BEGIN_FUNC("bf_ft_open")

	if(FT_New_Face(library, filename, 0, &font_ptr->face)) {
		zlogthis("bf_ttf_open: Error loading font from file \"%s\".\n",filename);
	}
	zlogthis("bf_ttf_open: Font \"%s\" opened.\n",filename);

	END_FUNC
	return 0;
}
*/

/*
int bf_ft_rasterize(BF_FT_FONT* font_ptr, int sz) {
	BEGIN_FUNC("bf_ft_rasterize")

	zlogthis("bf_ft_rasterize: Rasterizing font to %i pixels square.\n",sz);

	if(FT_Set_Pixel_Sizes(font_ptr->face, 0, sz)) {
		zlogthis("bf_ft_rasterize: Error setting font pixel parameters.\n");
	}

	int i, chdex;

	font_ptr->sz = sz;
	font_ptr->glyph_sz = font_ptr->sz * font_ptr->sz;


	if((font_ptr->facedex = (char*)malloc(font_ptr->glyph_sz * 256)) == NULL) {
		zlogthis("bf_ft_rasterize: Memory allocation error!\n");
		return -1;
	}

	memset(font_ptr->facedex, 0, font_ptr->glyph_sz * 256);

	int row, col;
	int xcz;
	int desty;

	FT_BBox bbox;

	for(i = 0; i < 256; i++) {
		chdex = FT_Get_Char_Index(font_ptr->face, i);
		FT_Load_Glyph(font_ptr->face, chdex, FT_LOAD_RENDER);

		font_ptr->kern[i] = font_ptr->face->glyph->metrics.width >> 6;
		//font_ptr->kern[i] = font_ptr->face->glyph->advance.x >> 6;
		//font_ptr->kern[i] = (font_ptr->face->glyph->metrics.horiAdvance >> 6) + (font_ptr->face->glyph->metrics.horiBearingX >> 6);
		//font_ptr->ybear[i] = font_ptr->face->glyph->metrics.horiBearingY >> 6;
		font_ptr->ybear[i] = font_ptr->face->glyph->bitmap_top - font_ptr->face->glyph->bitmap.rows;
		//font_ptr->face->glyph->bitmap.width + 3;


		// blit type two (16x16 glyph square block)
		row = (i / 16);
		col = (i % 16);

		zlogthis("%i '%c': (%i,%i) buffer = 0x%08x, size = %i x %i pixels, kern=%i, bmtop=%i\n",i,i,row,col,font_ptr->face->glyph->bitmap.buffer,font_ptr->face->glyph->bitmap.width,font_ptr->face->glyph->bitmap.rows,font_ptr->kern[i],font_ptr->ybear[i]);

		desty = font_ptr->sz - font_ptr->face->glyph->bitmap.rows;

		if(font_ptr->face->glyph->bitmap.buffer != 0) {
			for(xcz = 0; xcz < font_ptr->face->glyph->bitmap.rows; xcz++) {
				//memcpy(font_ptr->facedex + (row * 16 * font_ptr->glyph_sz) + (col * font_ptr->glyph_sz) + (xcz * 16 * font_ptr->sz), font_ptr->face->glyph->bitmap.buffer + (font_ptr->face->glyph->bitmap.width * xcz),font_ptr->face->glyph->bitmap.width);
				memcpy(font_ptr->facedex + (row * 16 * font_ptr->glyph_sz) + (font_ptr->sz * col) + (font_ptr->sz * 16 * desty), font_ptr->face->glyph->bitmap.buffer + (font_ptr->face->glyph->bitmap.width * xcz),font_ptr->face->glyph->bitmap.width);
				desty++;
			}
		}

	}


	// DUMPFILE for DEBUG <-----------------<---------<---------------------- DEBUG
	FILE *lizzy;

	if((lizzy = fopen("fdump.raw","wb")) == NULL) {
		zlogthis("could not open \"fdump.raw\" for writing!\n");
	} else {
		fwrite(font_ptr->facedex,font_ptr->glyph_sz,256,lizzy);
		fclose(lizzy);
	}
	// DUMPFILE for DEBUG <-----------------<---------<---------------------- DEBUG


	// release the face
	FT_Done_Face(font_ptr->face);

	END_FUNC
	return 0;
}
*/

/*
void bf_ft_kill_font(BF_FT_FONT *font_ptr) {
	BEGIN_FUNC("bf_ft_kill_font")

	free(font_ptr->facedex);
	font_ptr->sz = 0;

	END_FUNC
	return;
}

*/

void bf_ft_kill() {
	BEGIN_FUNC("bf_ft_kill")

	FT_Done_FreeType(library);

	END_FUNC
	return;
}

////// new vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv NEHE

void bf_ft_addfont(const char * fname, unsigned int h) {
	// Allocate Some Memory To Store The Texture Ids.
	//ft_font.ft_textures = new GLuint[128];

	ft_font.h = h;

	// The Object In Which FreeType Holds Information On A Given
	// Font Is Called A "face".
	FT_Face face;

	// This Is Where We Load In The Font Information From The File.
	// Of All The Places Where The Code Might Die, This Is The Most Likely,
	// As FT_New_Face Will Fail If The Font File Does Not Exist Or Is Somehow Broken.
	if (FT_New_Face( library, fname, 0, &face )) {
		zlogthis("FT_New_Face failed (there is probably a problem with your font file)\n");
	}

	// For Some Twisted Reason, FreeType Measures Font Size
	// In Terms Of 1/64ths Of Pixels.  Thus, To Make A Font
	// h Pixels High, We Need To Request A Size Of h*64.
	// (h << 6 Is Just A Prettier Way Of Writing h*64)
	FT_Set_Char_Size( face, ft_font.h * 64.0f, ft_font.h * 64.0f, 96, 96);

	// Here We Ask OpenGL To Allocate Resources For
	// All The Textures And Display Lists Which We
	// Are About To Create.  
	ft_font.list_base = glGenLists(128);
	glGenTextures( 128, ft_font.ft_textures );

	// This Is Where We Actually Create Each Of The Fonts Display Lists.
	bfr_set_blending(BF_AMODE_LUMA);
	for(unsigned char i=0;i<128;i++) {
		ft_make_dlist(face,i,ft_font.list_base,ft_font.ft_textures);
	}
	bfr_set_blending(BF_AMODE_NORMAL);

	// We Don't Need The Face Information Now That The Display
	// Lists Have Been Created, So We Free The Assosiated Resources.
	FT_Done_Face(face);

}

void bf_ft_font_remove() {
	glDeleteLists(ft_font.list_base,128);
	glDeleteTextures(128,ft_font.ft_textures);
	//delete [] textures;
}


// Create A Display List Corresponding To The Given Character.
void ft_make_dlist ( FT_Face face, char ch, GLuint list_base, GLuint * tex_base ) {

	// The First Thing We Do Is Get FreeType To Render Our Character
	// Into A Bitmap.  This Actually Requires A Couple Of FreeType Commands:

	// Load The Glyph For Our Character.
	if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT )) {
		zlogthis("FT_Load_Glyph failed\n");
	}

	// Move The Face's Glyph Into A Glyph Object.
	FT_Glyph glyph;
	if(FT_Get_Glyph( face->glyph, &glyph )) {
		zlogthis("FT_Get_Glyph failed\n");
	}

	// Convert The Glyph To A Bitmap.
	FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

	// This Reference Will Make Accessing The Bitmap Easier.
	FT_Bitmap& bitmap=bitmap_glyph->bitmap;


	// Use Our Helper Function To Get The Widths Of
	// The Bitmap Data That We Will Need In Order To Create
	// Our Texture.

	//zlogthis("PRE width = %i, height(rows) = %i\n",bitmap.width,bitmap.rows);
	int width = next_p2( bitmap.width );
	int height = next_p2( bitmap.rows );

	// Allocate Memory For The Texture Data.
	GLubyte* expanded_data;

	if((expanded_data = (GLubyte*)malloc(2*width*height)) == NULL) {
		zlogthis("Memory allocation error in ft_make_dlist!!\n");
		go_down();
	}

	// Here We Fill In The Data For The Expanded Bitmap.
	// Notice That We Are Using A Two Channel Bitmap (One For
	// Channel Luminosity And One For Alpha), But We Assign
	// Both Luminosity And Alpha To The Value That We
	// Find In The FreeType Bitmap. 
	// We Use The ?: Operator To Say That Value Which We Use
	// Will Be 0 If We Are In The Padding Zone, And Whatever
	// Is The FreeType Bitmap Otherwise.

	for(int j=0; j <height;j++) {
		for(int i=0; i < width; i++){
			expanded_data[2*(i+j*width)]= expanded_data[2*(i+j*width)+1] = 
				(i>=bitmap.width || j>=bitmap.rows) ?
				0 : bitmap.buffer[i + bitmap.width*j];
		}
	}

	//zlogthis("ch = %i, width = %i, height = %i\n",ch,width,height);

	// Now We Just Setup Some Texture Parameters.
	glBindTexture(GL_TEXTURE_2D, tex_base[ch]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	//zlogthis("tex_base[ch] = %i\n",tex_base[ch]);

	// Here We Actually Create The Texture Itself, Notice
	// That We Are Using GL_LUMINANCE_ALPHA To Indicate That
	// We Are Using 2 Channel Data.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
		GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );

	// With The Texture Created, We Don't Need The Expanded Data Anymore.
	//delete [] expanded_data;
	free(expanded_data);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	// Now We Create The Display List
	glNewList(ft_font.list_base+ch,GL_COMPILE);
	
	glBindTexture(GL_TEXTURE_2D,tex_base[ch]);
	//glBindTexture(GL_TEXTURE_2D,0);

	glPushMatrix();

	// First We Need To Move Over A Little So That
	// The Character Has The Right Amount Of Space
	// Between It And The One Before It.
	glTranslatef(bitmap_glyph->left,0,0);

	// Now We Move Down A Little In The Case That The
	// Bitmap Extends Past The Bottom Of The Line 
	// This Is Only True For Characters Like 'g' Or 'y'.
	glTranslatef(0,bitmap_glyph->top-bitmap.rows,0);

	// Now We Need To Account For The Fact That Many Of
	// Our Textures Are Filled With Empty Padding Space.
	// We Figure What Portion Of The Texture Is Used By 
	// The Actual Character And Store That Information In
	// The x And y Variables, Then When We Draw The
	// Quad, We Will Only Reference The Parts Of The Texture
	// That Contains The Character Itself.
	float x=(float)bitmap.width / (float)width,
	y = (float)bitmap.rows / (float)height;

	// Here We Draw The Texturemapped Quads.
	// The Bitmap That We Got From FreeType Was Not 
	// Oriented Quite Like We Would Like It To Be,
	// But We Link The Texture To The Quad
	// In Such A Way That The Result Will Be Properly Aligned.

	glBegin(GL_QUADS);
	update_state;
	glTexCoord2d(0,0); glVertex2f(0,bitmap.rows);
	glTexCoord2d(0,y); glVertex2f(0,0);
	glTexCoord2d(x,y); glVertex2f(bitmap.width,0);
	glTexCoord2d(x,0); glVertex2f(bitmap.width,bitmap.rows);
	glEnd();
	glPopMatrix();
	glTranslatef(face->glyph->advance.x >> 6 ,0,0);

	// Increment The Raster Position As If We Were A Bitmap Font.
	// (Only Needed If You Want To Calculate Text Length)
	// glBitmap(0,0,0,0,face->glyph->advance.x >> 6,0,NULL);

	// Finish The Display List
	glEndList();

}



// Much Like NeHe's glPrint Function, But Modified To Work
// With FreeType Fonts.
void bf_ft_print(int font_id, float x, float y, const char *fmt, ...)  {
        
	// We Want A Coordinate System Where Distance Is Measured In Window Pixels.
	pushScreenCoordinateMatrix();                                   
        
	GLuint font=ft_font.list_base;

	// We Make The Height A Little Bigger.  There Will Be Some Space Between Lines.
	float h=ft_font.h/.63f;                                                 
	char	text[512];									// Holds Our String
	va_list	ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		*text=0;									// Do Nothing
	else {
		va_start(ap, fmt);								// Parses The String For Variables
		vsprintf(text, fmt, ap);							// And Converts Symbols To Actual Numbers
		va_end(ap);									// Results Are Stored In Text
	}

	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT); 
	glMatrixMode(GL_MODELVIEW);
	bf_disable_lighting();
	bf_enable_textures();
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	bfr_set_blending(BF_AMODE_LUMA);

	glListBase(font);

	float modelview_matrix[16];     
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	// This Is Where The Text Display Actually Happens.
	// For Each Line Of Text We Reset The Modelview Matrix
	// So That The Line's Text Will Start In The Correct Position.
	// Notice That We Need To Reset The Matrix, Rather Than Just Translating
	// Down By h. This Is Because When Each Character Is
	// Drawn It Modifies The Current Matrix So That The Next Character
	// Will Be Drawn Immediately After It.  

	//for(int i=0;i<lines.size();i++) {
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(x,y,0);
		//glRasterPos2d(x, y); // set the position
		glMultMatrixf(modelview_matrix);

		// The Commented Out Raster Position Stuff Can Be Useful If You Need To
		// Know The Length Of The Text That You Are Creating.
		// If You Decide To Use It Make Sure To Also Uncomment The glBitmap Command
		// In make_dlist().
		// glRasterPos2f(0,0);
		//glCallLists(lines[i].length(), GL_UNSIGNED_BYTE, lines[i].c_str());
		glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);

		// float rpos[4];
		// glGetFloatv(GL_CURRENT_RASTER_POSITION ,rpos);
		// float len=x-rpos[0]; (Assuming No Rotations Have Happend)

		glPopMatrix();
	//}

	glPopAttrib();          

	pop_projection_matrix();
	bfr_set_blending(BF_AMODE_NORMAL);

}



// This Function Gets The First Power Of 2 >= The
// Int That We Pass It.
inline int next_p2 (int a )
{
	int rval=1;
	// rval<<=1 Is A Prettier Way Of Writing rval*=2; 
	while(rval<a) rval<<=1;
	return rval;
}


// A Fairly Straightforward Function That Pushes
// A Projection Matrix That Will Make Object World 
// Coordinates Identical To Window Coordinates.
inline void pushScreenCoordinateMatrix() {
	glPushAttrib(GL_TRANSFORM_BIT);
	GLint   viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(viewport[0],viewport[2],viewport[1],viewport[3]);
	glPopAttrib();
}

// Pops The Projection Matrix Without Changing The Current
// MatrixMode.
inline void pop_projection_matrix() {
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}

