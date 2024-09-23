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
//	Gamma correction LUT.
//	Functions to draw patches (by post) directly to screen.
//	Functions to blit a block to the screen.
//
//-----------------------------------------------------------------------------

#pragma once



#include "doomdef.h"

// Needed because we are refering to patches.
#include "r_data.h"

//
// VIDEO
//

#define CENTERY (SCREENHEIGHT / 2)

// Screen 0 is the screen updated by I_Update screen.
// Screen 1 is an extra buffer.
using screen = std::array<std::byte, SCREENWIDTH * SCREENHEIGHT>;
extern std::array<screen, 5> screens;

extern int dirtybox[4];

extern unsigned char gammatable[5][256];
static int usegamma = 0;

void V_CopyRect(int srcx, int srcy, int srcscrn, int width, int height,
                int destx, int desty, int destscrn);

void V_DrawPatch(int x, int y, int scrn, patch_t *patch);

void V_DrawPatchDirect(int x, int y, int scrn, patch_t *patch);

// Draw a linear block of pixels into the view buffer.
void V_DrawBlock(int x, int y, int scrn, int width, int height, std::byte *src);

// Reads a linear block of pixels into the view buffer.
void V_GetBlock(int x, int y, int scrn, int width, int height, std::byte *dest);

void V_MarkRect(int x, int y, int width, int height);

//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
