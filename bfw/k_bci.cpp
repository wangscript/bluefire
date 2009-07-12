/*


  Bluefire: Bytecode Interpreter

  (c) Copyright 2005 Jacob Hipps

  Bluefire MDL Core: Winchester

*/

/*
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bluefire.h"

typedef unsigned short (__stdcall * BFBCI_VFUNC)(int offset);

typedef struct {
	char opcode;
	BFBCI_VFUNC xvector;
} BFBCI_XDEF;

/*

  Bytecode translation table

*/

/*
BFBFC_XDEF xlate[] =
{
	{ 0x01 , bci_mov   },	// assignment (dest: var)
	{ 0x02 , bci_movx  },   // assignment (dest: external mem)
	{ 0x03 , bci_add   },	// add
	{ 0x04 , bci_mult  },	// multiply
	{ 0x05 , bci_div   },   // divide
	{ 0x06 , bci_sub   },	// subtract
	{ 0x07 , bci_sqrt  },	// square root
	{ 0x08 , bci_exp   },	// exponent
	{ 0x09 , bci_shl   },	// bit shift left
	{ 0x0A , bci_shr   },	// bit shift right
	{ 0x0B , bci_inv   },	// bit inversion
	{ 0x0C , bci_ife   },	// logical if equal
	{ 0x0D , bci_ifne  },	// logical if not equal
	{ 0x0E , bci_ifg   },	// logical if greater
	{ 0x0F , bci_ifl   },	// logical if less than (ie. not greater)

	{ 0x10 , bci_sin   },	// sine
	{ 0x11 , bci_cos   },	// cosine
	{ 0x12 , bci_tan   },	// tangent
	{ 0x13 , bci_asin  },	// arcsine
	{ 0x14 , bci_acos  },	// arccosine
	{ 0x15 , bci_atan  },	// arctangent
	{ 0x16 , bci_log   },	// log base-10
	{ 0x17 , bci_loge  },	// natural log (log base-e)

	{ 0x20 , bci_for   },	// for control loop
	{ 0x21 , bci_do    },	// do control loop
	{ 0x22 , bci_whl   },	// while control loop
	{ 0x23 , bci_trig  },	// OnEvent trigger vector
	{ 0x24 , bci_frea  },	// foreach control loop
	{ 0x2F , bci_lret  },	// ctrl loop (do/while/OnEvent/foreach) return marker	

	{ 0x60 , bci_call  },	// call directive
	{ 0x61 , bci_ret   },	// return directive
	{ 0x62 , bci_callx },	// call external function	

	{ 0xA0 , bci_err   },	// exit on error

	{ 0xE0 , bci_fmrk  },	// function beginning marker
	{ 0xE1 , bci_fend  },	// function ending marker

	{ 0x00 , NULL }			// no operation (this must be the last in the list!)
};

*/
/*

  bci_load
	dseg	Pointer to data segment
	cseg	Pointer to code segment
	ds_size	Data segment size (bytes)
	cs_size	Code segment size (bytes)

*/

/*
int bci_load(char *dseg, char *cseg, int ds_size, int cs_size) {



	return 0;
}
*/