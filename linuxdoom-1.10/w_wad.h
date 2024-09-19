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
//	WAD I/O functions.
//
//-----------------------------------------------------------------------------

#pragma once

#include <array>
#include <string>
#include <vector>


//
// TYPES
//
struct wadinfo_t
{
    // Should be "IWAD" or "PWAD".
    char		identification[4];		
    int32_t			numlumps;
    int32_t			infotableofs;
    
};


typedef struct
{
    int32_t		filepos;
    int32_t		size;
    std::array<char,8>		name;
    
} filelump_t;

//
// WADFILE I/O related stuff.
//
struct lumpinfo_t
{
    char	    name[8];
    int32_t		handle;
    int32_t		position;
    int32_t		size;
};


extern	std::vector<void*>		lumpcache;
extern	std::vector<lumpinfo_t>	lumpinfo;

void    W_InitMultipleFiles (std::vector<std::string>& filenames);
void    W_Reload (void);

int	W_CheckNumForName (const std::string& name);
int	W_GetNumForName (const std::string& name);

int	W_LumpLength (int lump);
void    W_ReadLump (int lump, void *dest);

void*	W_CacheLumpNum (uint32_t lump, int tag);
void*	W_CacheLumpName (const std::string& name, int tag);
