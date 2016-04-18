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
//	Refresh (R_*) module, global header.
//	All the rendering/drawing stuff is here.
//
//-----------------------------------------------------------------------------

#ifndef __R_LOCAL__
#define __R_LOCAL__

// Binary Angles, sine/cosine/atan lookups.
#include "tables.hpp"

// Screen size related parameters.
#include "doomdef.hpp"

// Include the refresh/render data structs.
#include "r_data.hpp"



//
// Separate header file for each module.
//
#include "r_main.hpp"
#include "r_bsp.hpp"
#include "r_segs.hpp"
#include "r_plane.hpp"
#include "r_data.hpp"
#include "r_things.hpp"
#include "r_draw.hpp"

#endif		// __R_LOCAL__
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
