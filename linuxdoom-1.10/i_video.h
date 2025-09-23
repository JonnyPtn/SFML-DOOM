//-----------------------------------------------------------------------------
//
//
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
//	System specific interface stuff.
//
//-----------------------------------------------------------------------------

#ifndef __I_VIDEO__
#define __I_VIDEO__

#include "doomtype.h"

#include <SFML/Graphics.hpp>

#ifdef __GNUG__
#pragma interface
#endif

inline sf::Color colors[256];

// Called by D_DoomMain,
// determines the hardware configuration
// and sets up the video mode
void I_InitGraphics(void);

// Takes full 8 bit values.
void I_SetPalette(const byte *palette);

void I_UpdateNoBlit(void);
void I_FinishUpdate(void);

void I_ReadScreen(byte *scr);

void I_BeginRead(void);
void I_EndRead(void);

#endif
//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------