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
//	Handles WAD file header, directory, lump I/O.
//
//-----------------------------------------------------------------------------

#include <cassert>
#include <cstdlib>
#include <ctype.h>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <vector>
static const char rcsid[] = "$Id: w_wad.c,v 1.5 1997/02/03 16:47:57 b1 Exp $";

#ifdef NORMALUNIX
#include <alloca.h>
#include <ctype.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define O_BINARY 0
#endif

#include "doomtype.h"
#include "i_system.h"
#include "m_swap.h"

#ifdef __GNUG__
#pragma implementation "w_wad.h"
#endif
#include "w_wad.h"

//
// GLOBALS
//

std::vector<std::vector<unsigned char>> lumpcache;

#define strcmpi strcasecmp

void ExtractFileBase(const char *path, char *dest)
{
    const char *src;
    int length;

    src = path + strlen(path) - 1;

    // back up until a \ or the start
    while (src != path && *(src - 1) != '\\' && *(src - 1) != '/')
    {
        src--;
    }

    // copy up to eight characters
    memset(dest, 0, 8);
    length = 0;

    while (*src && *src != '.')
    {
        if (++length == 9)
            I_Error("Filename base of %s >8 chars", path);

        *dest++ = toupper((int)*src++);
    }
}

//
// LUMP BASED ROUTINES.
//

//
// W_AddFile
// All files are optional, but at least one file must be
//  found (PWAD, if all required lumps are present).
// Files with a .wad extension are wadlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.

void W_AddFile(std::filesystem::path filepath)
{
    wadinfo_t header;
    lumpinfo_t *lump_p;
    unsigned i;
    int length;
    int startlump;
    filelump_t *fileinfo;
    filelump_t singleinfo;
    int numlumps{};

    // open the file and add to directory
    auto file = std::make_shared<std::ifstream>(filepath, std::ios::binary);
    if (!file->good())
    {
        printf(" couldn't open %s\n", filepath.string().c_str());
        return;
    }

    printf(" adding %s\n", filepath.string().c_str());
    startlump = lumpinfo.size();

    if (filepath.extension() != ".wad")
    {
        // single lump file
        fileinfo = &singleinfo;
        singleinfo.filepos = 0;
        singleinfo.size = std::filesystem::file_size(filepath);
        ExtractFileBase(filepath.string().c_str(), singleinfo.name);
    }
    else
    {
        // WAD file
        file->read(reinterpret_cast<char *>(&header), sizeof(header));
        if (strncmp(header.identification, "IWAD", 4))
        {
            // Homebrew levels?
            if (strncmp(header.identification, "PWAD", 4))
            {
                I_Error("Wad file %s doesn't have IWAD "
                        "or PWAD id\n",
                        filepath.c_str());
            }

            // ???modifiedgame = true;
        }
        header.numlumps = LONG(header.numlumps);
        header.infotableofs = LONG(header.infotableofs);
        length = header.numlumps * sizeof(filelump_t);
        fileinfo = (filelump_t *)alloca(length);
        file->seekg(header.infotableofs);
        file->read(reinterpret_cast<char *>(fileinfo), length);
        numlumps += header.numlumps;
    }

    for (i = startlump; i < numlumps; i++, fileinfo++)
    {
        std::string name = fileinfo->name;
        if (name.length() > 8)
            name.resize(8);

        lumpinfo.emplace_back(name, file, fileinfo->filepos, fileinfo->size);
    }
}

//
// W_InitMultipleFiles
// Pass a null terminated list of files to use.
// All files are optional, but at least one file
//  must be found.
// Files with a .wad extension are idlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
// Lump names can appear multiple times.
// The name searcher looks backwards, so a later file
//  does override all earlier ones.
//
void W_InitMultipleFiles(const std::vector<std::string> &filenames)
{
    if (filenames.empty())
    {
        I_Error("No wads found or provided");
    }

    for (const auto &name : filenames)
        W_AddFile(name);

    if (lumpinfo.empty())
        I_Error("W_InitFiles: no files found");

    // set up caching
    lumpcache.resize(lumpinfo.size());
}

//
// W_InitFile
// Just initialize from a single file.
//
void W_InitFile(const std::string &filename) { W_InitMultipleFiles({filename}); }

//
// W_NumLumps
//
int W_NumLumps(void) { return lumpinfo.size(); }

//
// W_CheckNumForName
// Returns -1 if name not found.
//
int W_CheckNumForName(std::string name)
{
    if (name.length() > 8)
    {
        printf("Name too long: %s\n", name.c_str());
        name = name.substr(0, 8);
    }
    // Must be upper case
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    // scan backwards so patch lump files take precedence
    for (auto lump = lumpinfo.rbegin(); lump != lumpinfo.rend(); lump++)
    {
        if (lump->name == name)
        {
            return lumpinfo.size() - std::distance(lumpinfo.rbegin(), lump) - 1;
        }
    }

    // TFB. Not found.
    return -1;
}

//
// W_GetNumForName
// Calls W_CheckNumForName, but bombs out if not found.
//
int W_GetNumForName(const std::string &name)
{
    int i;

    i = W_CheckNumForName(name);

    if (i == -1)
        I_Error("W_GetNumForName: %s not found!", name.c_str());

    return i;
}

//
// W_LumpLength
// Returns the buffer size needed to load the given lump.
//
int W_LumpLength(int lump)
{
    if (lump >= lumpinfo.size())
        I_Error("W_LumpLength: %i >= numlumps", lump);

    return lumpinfo[lump].size;
}

//
// W_ReadLump
// Loads the lump into the given buffer,
//  which must be >= W_LumpLength().
//
void W_ReadLump(int lump, void *dest)
{
    int c;
    lumpinfo_t *l;
    int handle;

    if (lump >= lumpinfo.size())
        I_Error("W_ReadLump: %i >= numlumps", lump);

    l = &lumpinfo[lump];

    // ??? I_BeginRead ();

    l->handle->seekg(l->position);
    l->handle->read(reinterpret_cast<char *>(dest), l->size);

    if (!l->handle->good())
    {
        I_Error("Error reading from file");
    }

    // ??? I_EndRead ();
}

//
// W_CacheLumpNum
//
const void *W_CacheLumpNum(int lump)
{
    byte *ptr;

    if ((unsigned)lump >= lumpinfo.size())
        I_Error("W_CacheLumpNum: %i >= numlumps", lump);

    if (lumpcache[lump].empty())
    {
        // read the lump in
        auto size = W_LumpLength(lump);
        lumpcache[lump].resize(size);
        W_ReadLump(lump, lumpcache[lump].data());
    }
    else
    {
        // printf ("cache hit on lump %i\n",lump);
        //
    }

    return lumpcache[lump].data();
}

//
// W_CacheLumpName
//
const void *W_CacheLumpName(const char *name) { return W_CacheLumpNum(W_GetNumForName(name)); }

//
// W_Profile
//
int info[2500][10];
int profilecount;

void W_Profile(void)
{
    int i;
    void *ptr;
    char ch;
    FILE *f;
    int j;

    for (i = 0; i < lumpinfo.size(); i++)
    {
        ptr = lumpcache[i].data();
        if (!ptr)
        {
            ch = ' ';
            continue;
        }
        else
        {
        }
        info[i][profilecount] = ch;
    }
    profilecount++;

    f = fopen("waddump.txt", "w");

    for (i = 0; i < lumpinfo.size(); i++)
    {
        auto name = lumpinfo[i].name;

        for (j = 0; j < 8; j++)
            if (!name[j])
                break;

        for (; j < 8; j++)
            name[j] = ' ';

        fprintf(f, "%s ", name.c_str());

        for (j = 0; j < profilecount; j++)
            fprintf(f, "    %c", info[i][j]);

        fprintf(f, "\n");
    }
    fclose(f);
}