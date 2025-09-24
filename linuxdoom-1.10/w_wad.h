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
//	WAD I/O functions.
//
//-----------------------------------------------------------------------------

#pragma once

#include <fstream>
#include <string>
#include <vector>

#ifdef __GNUG__
#pragma interface
#endif

//
// TYPES
//
typedef struct
{
    // Should be "IWAD" or "PWAD".
    char identification[4];
    int numlumps;
    int infotableofs;

} wadinfo_t;

struct filelump_t
{
    int filepos;
    int size;
    char name[8]{};
};

//
// WADFILE I/O related stuff.
//
struct lumpinfo_t
{
    std::string name{};
    std::shared_ptr<std::ifstream> handle;
    int position;
    int size;
};

inline std::vector<lumpinfo_t> lumpinfo;

void W_InitMultipleFiles(const std::vector<std::string> &filenames);

int W_CheckNumForName(std::string name);
int W_GetNumForName(const std::string &name);

int W_LumpLength(int lump);
void W_ReadLump(int lump, void *dest);

const void *W_CacheLumpNum(int lump);
const void *W_CacheLumpName(const char *name);


//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------