#pragma once


#ifdef LINUX
#include <math.h>
#else
#define PI				3.141592657
#endif


#include "m_fixed.hpp"
	
#define FINEANGLES		8192
#define FINEMASK		(FINEANGLES-1)


// 0x100000000 to 0x2000
#define ANGLETOFINESHIFT	19		

// Effective size is 10240.
extern  int		finesine[5*FINEANGLES/4];

// Re-use data, is just PI/2 pahse shift.
extern  int*	finecosine;


// Effective size is 4096.
extern int		finetangent[FINEANGLES/2];

// Binary Angle Measument, BAM.
#define ANG45			0x20000000
#define ANG90			0x40000000
#define ANG180		0x80000000
#define ANG270		0xc0000000


#define SLOPERANGE		2048
#define SLOPEBITS		11
#define DBITS			(FRACBITS-SLOPEBITS)

typedef unsigned angle_t;


// Effective size is 2049;
// The +1 size is to handle the case when x==y
//  without additional checking.
extern angle_t		tantoangle[SLOPERANGE+1];


// Utility function,
//  called by R_PointToAngle.
int
SlopeDiv
( unsigned	num,
  unsigned	den);

/*

TiMidity -- Experimental MIDI to WAVE converter
Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

tables.h
*/

#ifdef LOOKUP_SINE
extern float sine(int x);
#else
#include <math.h>
#define sine(x) (sin((2*PI/1024.0) * (x)))
#endif

#include <stdint.h>

#define SINE_CYCLE_LENGTH 1024
extern  int32_t  freq_table[];
extern double vol_table[];
extern double bend_fine[];
extern double bend_coarse[];
extern uint8_t *_l2u; /* 13-bit PCM to 8-bit u-law */
extern uint8_t _l2u_[]; /* used in LOOKUP_HACK */
#ifdef LOOKUP_HACK
extern int16_t _u2l[];
extern  int32_t  *mixup;
#ifdef LOOKUP_INTERPOLATION
extern int8_t *iplookup;
#endif
#endif

extern void init_tables(void);
