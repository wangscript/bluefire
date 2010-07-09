/*

	Bluefire: Object Upkeep Routines
	(c) Copyright 2009 Jacob Hipps/Neo-Retro Group
	http://bluefire.neoretro.net/spec/bfw/obj


	Implements:
		bf_obj_count()	- Return object count
		bf_obj_getnum()	- Return index of named object
		bf_obj_getptr() - Return pointer to object structure
		bf_obj_free()	- Free object from object pool
		bf_obj_freeall()- Free all objects
		bf_obj_add()	- Returns the index of a newly allocated object
		bf_format_objx()- Translates a chunk of data from raw vertex data into an object structure

*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bluefire.h"

extern void (__cdecl * go_down)();

// Globals
BF_RNODE *obj_pool[MAX_NODES];	// Object pool
int objects_otl;				// object count

/***

bf_obj_count()

	Description:
		Returns the number of objects in the object pool

	Arguments:
		(none)

	Return:
		number of objects in the object pool (int)

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
int bf_obj_count() {
	BEGIN_FUNC("bf_obj_count")
	END_FUNC
	return objects_otl;
}

/***

bf_getobj_num()
bf_obj_getnum()

	Description:
		Returns the index number of the object with the specified name.
		This must be the full name of the object. For example,
		if we loaded the object "castle" from the package "coolstuff.bfz",
		the full name would be "coolstuff:castle".
		
	Arguments:
		char *obj_name		Full name of object

	Return:
		index of object, or -1 if not found (int)

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
int bf_getobj_num(char obj_name[]) {
	BEGIN_FUNC("bf_getobj_num")

	int nummy = -1;

	for(int i = 0; i < objects_otl; i++) {
		if(!strcmp(obj_name,obj_pool[i]->name)) {
			nummy = i;
			break;
		}
	}

	END_FUNC
	return nummy;
}

/***

bf_format_objx()

	Description:
		Loads raw object and surface data from a BFZ Object chunk
		and stores it in the object pool. If any textures are
		associated with the object, they are checked to see
		if they are already loaded; if not, then they are loaded
		and put into the image/texture pool.

	Arguments:
		char *delerium		Pointer to object data
		int obj_num			Index of destination object in the object pool
							(loads the data to the specified object index)

	Return:
		0 on success, non-zero on error

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
/*

  ObjX data organization:

  offset	description
  ----------------------------------------------------------
  0			Number of surfaces (unsigned short)
  1			SURFACEINFO
  ...		...
  x			BF_TRIANGLE
  ...		...

*/
int bf_format_objx(char *delerium, int obj_num) {
	BEGIN_FUNC("bf_format_objx")
	
	BF_RNODE *swimtime = obj_pool[obj_num];

	int i;

	// set default parameters
	swimtime->bEnabled = true; // enable the object 
	swimtime->bVisible = true; // it is visible
	swimtime->bObjX    = true; // obsolete!
	swimtime->bAllieCat= true; // enable debug stuff for this object
	swimtime->surfaces = (int)(*delerium);

	delerium += sizeof(int);   // increment the pointer (the integer at delerium's address contains
							   // the number of surfaces. After this follows the rest of the object data.)

	// allocate memory for surface info chunks
	if((swimtime->surfdex = (SURFACEINFO*)malloc(sizeof(SURFACEINFO) * swimtime->surfaces)) == NULL) {
		zlogthisx("Memory allocation error (surfdex)!");
		go_down();
	}

	// copy the surface information chunks
	memcpy(swimtime->surfdex,delerium,sizeof(SURFACEINFO) * swimtime->surfaces);


	// allocate memory for vertex info chunks
	for(i = 0; i < swimtime->surfaces; i++) {
		if((swimtime->vtexinfo[i] = (BF_TRIANGLE*)malloc(sizeof(BF_TRIANGLE) * swimtime->surfdex[i].scount)) == NULL) {
			zlogthisx("Memory allocation error (vtexinfo)!");
			go_down();
		} else {
			//zlogthisx("Memory allocated OK for vtexinfo[%i], scount=%i, vector=0x%08x, size=%i bytes",i,swimtime->surfdex[i].scount,swimtime->vtexinfo[i],sizeof(BF_TRIANGLE) * swimtime->surfdex[i].scount);
		}
	}

	// copy the vertex information chunks
	delerium += (sizeof(SURFACEINFO) * swimtime->surfaces);
	for(i = 0; i < swimtime->surfaces; i++) {
		memcpy(swimtime->vtexinfo[i], delerium, sizeof(BF_TRIANGLE) * swimtime->surfdex[i].scount);
		delerium += (sizeof(BF_TRIANGLE) * swimtime->surfdex[i].scount);
	}

	// ensure the textures are loaded correctly
	BFZ_FILE ifilex;
	ifilex.fptrx = NULL;

	char lastfile[128];
	char filz[128];
	char nrname[128];
	char newfilen[128];
	int xy;

	extern char *cur_file;

	lastfile[0] = 0;
	filz[0] = 0;

	for(i = 0; i < swimtime->surfaces; i++) {
		if(swimtime->surfdex[i].filename[0] != NULL) {

			if(bf_getimg_num(swimtime->surfdex[i].filename) == -1) {				
				
				for(xy = 0; xy < strlen(swimtime->surfdex[i].filename); xy++) {					
					if(swimtime->surfdex[i].filename[xy] == ':') {						
						break;
					}
				}				
				
				memcpy(filz,swimtime->surfdex[i].filename,xy);
				filz[xy] = 0;
				memcpy(nrname,swimtime->surfdex[i].filename + xy + 1, strlen(swimtime->surfdex[i].filename) - xy);

				if(filz[0] == '*') {
						strcpy(filz,cur_file);
				}

				if(strcmp(lastfile,filz)) {
					if(ifilex.fptrx) bfz_close(&ifilex);
					sprintf(newfilen,"%s.bfz",filz);
					bfz_open(newfilen,&ifilex);
					strcpy(lastfile,filz);
				}

				swimtime->surfdex[i].snum = bf_addimg();
				bfz_loadimg(&ifilex,nrname,swimtime->surfdex[i].snum);				
			} else {
				swimtime->surfdex[i].snum = bf_getimg_num(swimtime->surfdex[i].filename);				
			}
		}
	}
	if(ifilex.fptrx) bfz_close(&ifilex);

	END_FUNC
	return 0;
}

/***

bf_freeobject()
bf_obj_free()

	Description:
		Frees the specified object from the object pool. However, it does
		NOT free any textures that are associated with the object.

	Arguments:
		char *delerium		Pointer to object data
		int obj_num			Index of destination object in the object pool
							(loads the data to the specified object index)

	Return:
		nothing

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
void bf_freeobject(int nummy) {
	BEGIN_FUNC("bf_freeobject")

	BF_RNODE *swimtime = obj_pool[nummy];

	// free the object
	if(swimtime) {
		if(swimtime->surfdex) free(swimtime->surfdex);
		for(int i = 0; i < swimtime->surfaces; i++) {
			if(swimtime->vtexinfo[i]) {
				free(swimtime->vtexinfo[i]);
			}
		}

		free(swimtime);
	}

	// if the object we freed isn't the last one in queue, then
	// put the last object in queue in its place to prevent "holes" from forming
	if(nummy != (objects_otl - 1)) {
		swimtime = obj_pool[objects_otl];
	}

	objects_otl--;

	END_FUNC
	return;
}

/***

bf_freeobjpool()
bf_obj_freeall()

	Description:
		Frees all objects from the object pool.
		NOT free any textures that are associated with the objects.

	Arguments:
		(none)

	Return:
		nothing

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
void bf_freeobjpool() {
	BEGIN_FUNC("bf_freeobjpool")

	for(int i = 0; i < objects_otl; i++) {
		bf_freeobject(i);
	}

	zlogthis("info: bf_freeobjpool: All objects in object pool freed.\n");	

	END_FUNC
	return;
}


/***

bf_addobject()
bf_obj_add()

	Description:
		Adds an object to the object pool.
		This function only allocates the memory for the object
		and returns the object's index number. It does NOT load
		any data into the object.

	Arguments:
		(none)

	Return:
		index number of object (int)

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
int bf_addobject() {
	BEGIN_FUNC("bf_addobject")

	int nummy = objects_otl;
	objects_otl++;

	if((obj_pool[nummy] = (BF_RNODE*)malloc(sizeof(BF_RNODE))) == NULL) {
		zlogthis("Memory allocation error!");
		go_down();
	}

	memset(obj_pool[nummy],0,sizeof(BF_RNODE)); // zero-out the memory space
	
	END_FUNC
	return nummy;
}

/***

bf_getobject()
bf_obj_getptr()

	Description:
		Returns the pointer to the specified object.

	Arguments:
		int nummy		Index of object in the object pool

	Return:
		object pointer (BF_RNODE*)

	Status:
		Working, Level B
	To-do:
		Nothing.

***/
BF_RNODE* bf_getobject(int nummy) {	
	BEGIN_FUNC("bf_getobject")
	END_FUNC
	return obj_pool[nummy];
}
