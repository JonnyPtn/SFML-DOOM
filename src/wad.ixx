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
module;

#include "m_swap.h"
#include "z_zone.h"

#include <array>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>

export module wad;

import system;

//
// TYPES
//
struct wadinfo_t {
  // Should be "IWAD" or "PWAD".
  char identification[4];
  int32_t numlumps;
  int32_t infotableofs;
};

typedef struct {
  int32_t filepos;
  int32_t size;
  std::array<char, 8> name;

} filelump_t;

//
// WADFILE I/O related stuff.
//
struct lumpinfo_t {
  char name[8];
  int32_t handle;
  int32_t position;
  int32_t size;
};

//
// GLOBALS
//

// Location of each lump on disk.
export std::vector<lumpinfo_t> lumpinfo;

std::vector<void *> lumpcache;

std::vector<std::ifstream> wadfiles;

#define strcmpi strcasecmp

void ExtractFileBase(const char *path, char *dest) {
  int length;

  const char *src = path + strlen(path) - 1;

  // back up until a \ or the start
  while (src != path && *(src - 1) != '\\' && *(src - 1) != '/') {
    src--;
  }

  // copy up to eight characters
  memset(dest, 0, 8);
  length = 0;

  while (*src && *src != '.') {
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
//
// If filename starts with a tilde, the file is handled
//  specially to allow map reloads.
// But: the reload feature is a fragile hack...

size_t reloadlump;
std::filesystem::path reloadpath;

void W_AddFile(const std::filesystem::path &filepath) {
  if (filepath.empty()) {
    printf("Cannot add empty file path\n");
    return;
  }

  if (!std::filesystem::exists(filepath)) {
    printf("File not found: %s\n", filepath.string().c_str());
    return;
  }

  auto filename = filepath.filename();

  wadinfo_t header;
  int length;
  std::vector<filelump_t> fileinfo;
  filelump_t singleinfo{};

  // open the file and add to directory

  // handle reload indicator.
  if (filename.string()[0] == '~') {
    filename = filename.string().substr(1);
    reloadpath = filepath;
    reloadlump = lumpinfo.size();
  }

  wadfiles.emplace_back(filepath.string(), std::ios::binary);
  if (!wadfiles.back().is_open()) {
    spdlog::error(" couldn't open {}", filepath.string());
    wadfiles.pop_back();
    return;
  }

  spdlog::info(" adding {}", filename.string());

  if (filename.extension() != ".wad") {
    // single lump file
    fileinfo = {singleinfo};
    singleinfo.filepos = 0;
    singleinfo.size = static_cast<int>(std::filesystem::file_size(filepath));
    std::copy(filename.string().begin(), filename.string().end(),
              singleinfo.name.begin());
    lumpinfo.emplace_back();
  } else {
    // WAD file
    wadfiles.back().read(reinterpret_cast<char *>(&header), sizeof(header));
    if (strncmp(header.identification, "IWAD", 4)) {
      // Homebrew levels?
      if (strncmp(header.identification, "PWAD", 4)) {
        I_Error("Wad file {} doesn't have or PWAD id\n", filename.string());
      }

      // ???modifiedgame = true;
    }
    header.numlumps = LONG(header.numlumps);
    header.infotableofs = LONG(header.infotableofs);
    length = header.numlumps * sizeof(filelump_t);
    fileinfo.resize(header.numlumps);
    wadfiles.back().seekg(header.infotableofs, std::ios::beg);
    wadfiles.back().read(reinterpret_cast<char *>(fileinfo.data()), length);
    lumpinfo.reserve(lumpinfo.size() + header.numlumps);
  }

  for (auto i = 0; i < header.numlumps; ++i) {
    const auto fileIndex = reloadpath.empty() ? wadfiles.size() - 1 : -1;
    lumpinfo.emplace_back();
    std::copy(fileinfo[i].name.begin(), fileinfo[i].name.end(),
              lumpinfo.back().name);
    lumpinfo.back().handle = static_cast<int>(fileIndex);
    lumpinfo.back().position = fileinfo[i].filepos;
    lumpinfo.back().size = fileinfo[i].size;
  }
}

//
// W_Reload
// Flushes any of the reloadable lumps in memory
//  and reloads the directory.
//
export void W_Reload(void) {
  int lumpcount{};
  lumpinfo_t *lump_p;
  filelump_t *fileinfo{};

  if (reloadpath.empty())
    return;

  // JONNY TODO
  // if ( (handle = open (reloadname,O_RDONLY | O_BINARY)) == -1)
  // I_Error ("W_Reload: couldn't open %s",reloadname);

  // JONNY TODO
  // read (handle, &header, sizeof(header));
  // lumpcount = LONG(header.numlumps);
  // header.infotableofs = LONG(header.infotableofs);
  // length = lumpcount*sizeof(filelump_t);
  // fileinfo = alloca (length);
  // lseek (handle, header.infotableofs, SEEK_SET);
  // read (handle, fileinfo, length);

  // Fill in lumpinfo
  lump_p = &lumpinfo[reloadlump];

  for (auto i = reloadlump; i < reloadlump + lumpcount;
       i++, lump_p++, fileinfo++) {
    if (lumpcache[i])
      free(lumpcache[i]);

    lump_p->position = LONG(fileinfo->filepos);
    lump_p->size = LONG(fileinfo->size);
  }

  // JONNY TODO
  // close (handle);
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
export void W_InitMultipleFiles(std::vector<std::string> &filenames) {
  // open all the files, load headers, and count lumps
  lumpinfo.clear();

  for (const auto &name : filenames) {
    W_AddFile(name);
  }

  if (lumpinfo.empty()) {
    I_Error("W_InitFiles: no files found");
  }

  // set up caching
  lumpcache.resize(lumpinfo.size());
}

//
// W_InitFile
// Just initialize from a single file.
//
void W_InitFile(std::string filename) {
  std::vector names{filename};
  W_InitMultipleFiles(names);
}

//
// W_CheckNumForName
// Returns -1 if name not found.
//

export int W_CheckNumForName(const std::string &name) {
  // scan backwards so patch lump files take precedence
  auto upper_name = name;
  std::transform(std::begin(upper_name), std::end(upper_name),
                 std::begin(upper_name), toupper);
  for (auto lump = lumpinfo.rbegin(); lump != lumpinfo.rend(); ++lump) {
    if (lump->name == upper_name) {
      return static_cast<int>(std::distance(lumpinfo.begin(), lump.base()) - 1);
    }
  }

  // TFB. Not found.
  return -1;
}

//
// W_GetNumForName
// Calls W_CheckNumForName, but bombs out if not found.
//
export int W_GetNumForName(const std::string &name) {
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
export int W_LumpLength(int lump) {
  if (lump >= lumpinfo.size()) {
    I_Error("W_LumpLength: {} out of bounds", lump);
  }

  return lumpinfo[lump].size;
}

//
// W_ReadLump
// Loads the lump into the given buffer,
//  which must be >= W_LumpLength().
//
export void W_ReadLump(int lump, void *dest) {

  if (lump >= lumpinfo.size()) {
    I_Error("W_ReadLump: {} out of bounds", lump);
  }

  const auto &l = lumpinfo[lump];

  // ??? I_BeginRead ();

  if (l.handle == -1) {
    // reloadable file, so use open / read / close
    if (std::ifstream file{reloadpath.c_str(), std::ios::binary};
        !file.is_open()) {
      I_Error("W_ReadLump: couldn't open {}", reloadpath.string());
    } else {
      file.seekg(l.position, std::ios::beg);
      file.read(reinterpret_cast<char *>(dest), l.size);

      if (!file) {
        I_Error("W_ReadLump: only read {} of {} on lump {}", file.gcount(),
                l.size, lump);
      }
    }
  } else {
    auto &file = wadfiles[l.handle];

    file.seekg(l.position, std::ios::beg);
    file.read(reinterpret_cast<char *>(dest), l.size);

    if (!file) {
      I_Error("W_ReadLump: only read {} of {} on lump {}", file.gcount(),
              l.size, lump);
    }
  }
}

//
// W_CacheLumpNum
//
export void *W_CacheLumpNum(uint32_t lump, int tag) {
  if (lump >= lumpinfo.size()) {
    I_Error("W_CacheLumpNum: {} out of bounds", lump);
  }

  if (!lumpcache[lump]) {
    // read the lump in

    // printf ("cache miss on lump {}\n",lump);
    const auto size = W_LumpLength(lump);
    lumpcache[lump] = static_cast<void *>(malloc(size));
    W_ReadLump(lump, lumpcache[lump]);
  } else {
    // printf ("cache hit on lump {}\n",lump);
  }

  return lumpcache[lump];
}

//
// W_CacheLumpName
//
export void *W_CacheLumpName(const std::string &name, int tag) {
  return W_CacheLumpNum(W_GetNumForName(name), tag);
}

//
// W_Profile
//
int info[2500][10];
int profilecount;

void W_Profile(void) {
  int i;
  memblock_t *block;
  void *ptr;
  char ch;
  FILE *f;
  int j;
  char name[9];

  for (i = 0; i < lumpinfo.size(); i++) {
    ptr = lumpcache[i];
    if (!ptr) {
      ch = ' ';
      continue;
    } else {
      block = (memblock_t *)((std::byte *)ptr - sizeof(memblock_t));
      if (block->tag < PU_PURGELEVEL)
        ch = 'S';
      else
        ch = 'P';
    }
    info[i][profilecount] = ch;
  }
  profilecount++;

  f = fopen("waddump.txt", "w");
  name[8] = 0;

  for (i = 0; i < lumpinfo.size(); i++) {
    memcpy(name, lumpinfo[i].name, 8);

    for (j = 0; j < 8; j++)
      if (!name[j])
        break;

    for (; j < 8; j++)
      name[j] = ' ';

    fprintf(f, "%s ", name);

    for (j = 0; j < profilecount; j++)
      fprintf(f, "    %c", info[i][j]);

    fprintf(f, "\n");
  }
  fclose(f);
}
