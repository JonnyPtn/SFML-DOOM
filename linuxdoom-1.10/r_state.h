// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//	Refresh/render internal state variables (global).
//
//-----------------------------------------------------------------------------

#pragma once

// Need data structure definitions.
#include "d_player.h"
#include "r_data.h"

#include <vector>

//
// Refresh internal data structures,
//  for rendering.
//

// needed for texture pegging
extern std::vector<fixed_t> textureheight;

// needed for pre rendering (fracs)
extern fixed_t *spritewidth;

extern fixed_t *spriteoffset;
extern fixed_t *spritetopoffset;

extern lighttable_t *colormaps;

extern int scaledviewwidth;

extern int firstflat;

// for global animation
extern int *flattranslation;

// Sprite....
extern int firstspritelump;
extern int lastspritelump;
extern int numspritelumps;

//
// Lookup tables for map data.
//
extern std::vector<spritedef_t> sprites;

//
// POV data.
//
extern fixed_t viewx;
extern fixed_t viewy;
extern fixed_t viewz;

extern angle_t viewangle;
extern player_t *viewplayer;

// ?
extern angle_t clipangle;

extern int viewangletox[FINEANGLES / 2];
extern angle_t xtoviewangle[SCREENWIDTH + 1];
// extern fixed_t		finetangent[FINEANGLES/2];

extern fixed_t rw_distance;
extern angle_t rw_normalangle;

// angle to line origin
extern int rw_angle1;

// Segs count?
extern int sscount;

extern visplane_t *floorplane;
extern visplane_t *ceilingplane;

//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
