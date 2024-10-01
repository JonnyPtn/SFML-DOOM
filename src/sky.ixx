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
// $Log:$
//
// DESCRIPTION:
//  Sky rendering. The DOOM sky is a texture map like any
//  wall, wrapping around. A 1024 columns equal 360 degrees.
//  The default sky map is 256 columns and repeats 4 times
//  on a 320 screen?
//
//
//-----------------------------------------------------------------------------
module;
// Needed for FRACUNIT.
#include "m_fixed.h"

// Needed for Flat retrieval.
#include "r_data.h"

export module sky;

// SKY, store the number for name.
export constexpr auto  SKYFLATNAME = "F_SKY1";

// The sky map is 256*128*4 maps.
export constexpr auto ANGLETOSKYSHIFT = 22;

//
// sky mapping
//
export int skyflatnum;
export int skytexture;
export int skytexturemid;

//
// R_InitSkyMap
// Called whenever the view size changes.
//
export void R_InitSkyMap(void) {
  // skyflatnum = R_FlatNumForName ( SKYFLATNAME );
  skytexturemid = 100 * FRACUNIT;
}
