/*

	Bluefire: Scenegraph Routines
	(c) Copyright 2009 Jacob Hipps/Neo-Retro Group
	http://bluefire.neoretro.net/spec/bfw/scene


	Implements:
		TODO - Scenegraph maintenence functions

*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bluefire.h"

extern void (__cdecl * go_down)();
extern BF_EXPORTZ z_config mainconfig;
extern BF_EXPORTZ ESM_RENDERZ mainscr;

// ---------- move to bluefire.h ------------------- vvv

// Scenegraph node type constants
#define BFN_ROOT		0	// top-most root node
#define BFN_OBJ			1	// objects
#define BFN_POLYS		2	// env polygon groups/brushes
#define BFN_AUDIO		3	// audio/sound objects
#define BFN_SCRIPT		4	// script
#define BFN_BBOARD		5	// billboards
#define BFN_COREFUNC	6	// Bluefire core/built-in functions
#define BFN_SHADEFRAG	7	// fragment shader program
#define BFN_SHADEVERT	8	// vertex shader program

// Scenegraph node structure
typedef struct {
	unsigned int guid;		// globally unique identifier
	char nodename[64];		// node name
	void *child;			// pointer to first child node, NULL if last
	unsigned int nodetype;	// type of node BFN_* (eg. root, object, etc.)
	float x, y, z, w;		// spatial position vector
	float Nx, Ny, Nz, Nw;	// normal/rotation vector
	float r, g, b, a;		// color/opacity vector
	bool bEnabled;			// toggle node culling
	bool bVisible;			// toggle visibility
	int dataSize;			// size of dataArea in bytes (0 if unused)
	void *dataArea;			// extra data (NULL pointer if unused)
} BF_SGNODE;

// Function declarations
BF_SGNODE* bf_scene_ptr_guid(unsigned int guid);
int bf_scene_init();


// ^^^ -------------- move above to global header -----------------------

// Scenegraph Core Nodes (built-ins)
/*
BF_SGNODE sgBuiltins[] = {	
	{ 0, "root",
};
*/

// Scenegraph Globals
int sgNodes = 0;
int sgMaxNodes = 0;
bool sgReady = false;
BF_SGNODE **sgMaster;

/***

bf_scene_init()

	Description:
		Initializes the scenegraph and prepares it for rendering by
		populating it with core nodes and structures.

	Arguments:
		(void)

	Return:
		0 on success, non-zero on error

	Status:
		?
	To-do:
		?

***/
int bf_scene_init() {
	BEGIN_FUNC("bf_scene_init")

	END_FUNC
	return 0;
}


/***

bf_scene_ptr_guid()

	Description:
		Returns the pointer to the scenegraph node with the specified
		GUID.

	Arguments:
		unsigned int guid		GUID of desired SG node

	Return:
		Pointer to BF_SGNODE structure on success, NULL on error

	Status:
		?
	To-do:
		?

***/
BF_SGNODE* bf_scene_ptr_guid(unsigned int guid) {

	return NULL;
}


