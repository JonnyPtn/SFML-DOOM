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
//	Put all global tate variables here.
//
//-----------------------------------------------------------------------------

export module doomstat;

import doomdef;

// Game Mode - identify IWAD as shareware, retail etc.
export GameMode_t gamemode = indetermined;
export GameMission_t gamemission = doom;

// Language.
export Language_t language = english;

// Set if homebrew PWAD stuff has been added.
export bool modifiedgame;
