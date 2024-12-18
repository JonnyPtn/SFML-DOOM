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
// Revision 1.3  1997/01/29 20:10
// DESCRIPTION:
//	Preparation of data for rendering,
//	generation of lookups, caching, retrieval by name.
//
//-----------------------------------------------------------------------------

#include "p_local.h"

#ifdef LINUX
#include <alloca.h>
#endif

#include "r_data.h"

#include <stdlib.h>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>

#include "g_game.h"

import engine;
import wad;
import sky;
import tick;
import setup;

//
// Graphics.
// DOOM graphics for walls and sprites
// is stored in vertical runs of opaque pixels (posts).
// A column is composed of zero or more posts,
// a patch or sprite is composed of zero or more columns.
//

//
// Texture definition.
// Each texture is composed of one or more patches,
// with patches being lumps stored in the WAD.
// The lumps are referenced by number, and patched
// into the rectangular texture space using origin
// and possibly other attributes.
//
typedef struct {
  short originx;
  short originy;
  short patch;
  short stepdir;
  short colormap;
} mappatch_t;

//
// Texture definition.
// A DOOM wall texture is a list of patches
// which are to be combined in a predefined order.
//
typedef struct {
  char name[8];
  bool masked;
  char padding[3];
  short width;
  short height;
  char columndirectory[4]; // OBSOLETE
  short patchcount;
  mappatch_t patches[1];
} maptexture_t;

// A single patch from a texture definition,
//  basically a rectangular area within
//  the texture rectangle.
typedef struct {
  // Block origin (allways UL),
  // which has allready accounted
  // for the internal origin of the patch.
  int32_t originx;
  int32_t originy;
  int32_t patch;
} texpatch_t;

// A maptexturedef_t describes a rectangular texture,
//  which is composed of one or more mappatch_t structures
//  that arrange graphic patches.
struct texture_t {
  // Keep name for switch changing, etc.
  std::string name;
  short width;
  short height;
  std::vector<texpatch_t> patches;
};

int firstflat;
int lastflat;
int numflats;

int firstpatch;
int lastpatch;
int numpatches;

int firstspritelump;
int lastspritelump;
int numspritelumps;

int numtextures;
std::vector<texture_t> textures;

std::vector<int> texturewidthmask;
std::vector<fixed_t> textureheight; // needed for texture pegging
std::vector<int> texturecompositesize;
std::vector<std::vector<short>> texturecolumnlump;
std::vector<std::vector<unsigned short>> texturecolumnofs;
std::vector<std::vector<std::byte>> texturecomposite;

// for global animation
int *flattranslation;

// needed for pre rendering
fixed_t *spritewidth;
fixed_t *spriteoffset;
fixed_t *spritetopoffset;

//
// MAPTEXTURE_T CACHING
// When a texture is first needed,
//  it counts the number of composite columns
//  required in the texture and allocates space
//  for a column directory and any new columns.
// The directory will simply point inside other patches
//  if there is only one patch in a given column,
//  but any columns with multiple patches
//  will have new column_ts generated.
//

//
// R_DrawColumnInCache
// Clip and draw a column
//  from a patch into a cached post.
//
void R_DrawColumnInCache(column_t *patch, std::byte *cache, int originy,
                         int cacheheight) {
  int count;
  int position;
  std::byte *source;

  while (patch->topdelta != std::byte{0xff}) {
    source = (std::byte *)patch + 3;
    count = static_cast<int>(patch->length);
    position = originy + static_cast<int>(patch->topdelta);

    if (position < 0) {
      count += position;
      position = 0;
    }

    if (position + count > cacheheight)
      count = cacheheight - position;

    if (count > 0)
      memcpy(cache + position, source, count);

    patch = (column_t *)((std::byte *)patch + static_cast<int>(patch->length) + 4);
  }
}

//
// R_GenerateComposite
// Using the texture definition,
//  the composite texture is created from the patches,
//  and each column is cached.
//
void  R_GenerateComposite(int texnum) {
  patch_t *realpatch;
  int x;
  int x1;
  int x2;
  column_t *patchcol;

  const auto &texture = textures[texnum];

  texturecomposite[texnum].resize(texturecompositesize[texnum]);

  const auto &collump = texturecolumnlump[texnum];
  const auto &colofs = texturecolumnofs[texnum];

  // Composite the columns together.
  for (const auto &patch : texture.patches) {
    realpatch = static_cast<patch_t *>(wad::get(patch.patch));
    x1 = patch.originx;
    x2 = x1 + realpatch->width;

    if (x1 < 0)
      x = 0;
    else
      x = x1;

    if (x2 > texture.width)
      x2 = texture.width;

    for (; x < x2; x++) {
      // Column does not have multiple patches?
      if (collump[x] >= 0)
        continue;

      patchcol =
          (column_t *)((std::byte *)realpatch + realpatch->columnofs[x - x1]);
      R_DrawColumnInCache(patchcol, texturecomposite[texnum].data() + colofs[x], patch.originy, texture.height);
    }
  }
}

//
// R_GenerateLookup
//
void R_GenerateLookup(int texnum) {
  std::byte *patchcount; // patchcount[texture->width]
  patch_t *realpatch;
  int x;
  int x1;
  int x2;

  auto &texture = textures[texnum];

  // Composited texture not created yet.
  texturecomposite[texnum] = {};

  texturecompositesize[texnum] = 0;
  auto &collump = texturecolumnlump[texnum];
  auto &colofs = texturecolumnofs[texnum];

  // Now count the number of columns
  //  that are covered by more than one patch.
  // Fill in the lump / offset, so columns
  //  with only a single patch are all done.
  patchcount = (std::byte *)malloc(texture.width);
  memset(patchcount, 0, texture.width);

  for (const auto &patch : texture.patches) {
    realpatch = static_cast<patch_t *>(wad::get(patch.patch));
    x1 = patch.originx;
    x2 = x1 + realpatch->width;

    if (x1 < 0)
      x = 0;
    else
      x = x1;

    if (x2 > texture.width)
      x2 = texture.width;
    for (; x < x2; x++) {
      patchcount[x] = static_cast<std::byte>(static_cast<int>(patchcount[x]) + 1);
      collump[x] = patch.patch;
      colofs[x] = realpatch->columnofs[x - x1] + 3;
    }
  }

  for (x = 0; x < texture.width; x++) {
    if (patchcount[x] == std::byte{0}) {
      printf("R_GenerateLookup: column without a patch (%s)\n",
             texture.name.c_str());
      return;
    }
    // logger::error ("R_GenerateLookup: column without a patch");

    if (static_cast<int>(patchcount[x]) > 1) {
      // Use the cached .
      collump[x] = -1;
      colofs[x] = texturecompositesize[texnum];

      if (texturecompositesize[texnum] > 0x10000 - texture.height) {
        logger::error("R_GenerateLookup: texture {} is >64k", texnum);
      }

      texturecompositesize[texnum] += texture.height;
    }
  }
}

//
// R_GetColumn
//
std::byte *R_GetColumn(int tex, int col) {
  int lump;
  int ofs;

  col &= texturewidthmask[tex];
  lump = texturecolumnlump[tex][col];
  ofs = texturecolumnofs[tex][col];

  if (lump > 0)
    return (std::byte *)wad::get(lump) + ofs;

  if (texturecomposite[tex].empty())
    R_GenerateComposite(tex);

  return texturecomposite[tex].data() + ofs;
}

//
// R_InitTextures
// Initializes the texture list
//  with the textures from the world map.
//
void R_InitTextures(void) {
  maptexture_t *mtexture;
  mappatch_t *mpatch;
  texpatch_t *patch;

  int i;
  int j;

  int *maptex;
  int *maptex2;
  int *maptex1;

  char *names;
  char *name_p;

  int nummappatches;
  int offset;
  int maxoff;
  int maxoff2;
  int numtextures1;
  int numtextures2;

  int *directory;

  int temp1;
  int temp2;
  int temp3;

  // Load the patch names from pnames.lmp.
  names = static_cast<char *>(wad::get("PNAMES"));
  nummappatches = *((int *)names);
  name_p = names + 4;
  std::vector<int> patchlookup(nummappatches);

  for (i = 0; i < nummappatches; i++) {
    std::string name(name_p + i * 8);
    if (name.length() > 8) {
      name = name.substr(0, 8);
    }
    patchlookup[i] = wad::index_of(name);
    patchlookup[i] = patchlookup[i];
  }

  // Load the map texture definitions from textures.lmp.
  // The data is contained in one or two lumps,
  //  TEXTURE1 for shareware, plus TEXTURE2 for commercial.
  maptex = maptex1 = static_cast<int *>(wad::get("TEXTURE1"));
  numtextures1 = *maptex;
  maxoff = wad::lump_length(wad::index_of({"TEXTURE1"}));
  directory = maptex + 1;

  if ( wad::index_of( "TEXTURE2" ) != -1 )
  {
    maptex2 = static_cast<int *>(wad::get("TEXTURE2"));
    numtextures2 = *maptex2;
    maxoff2 = wad::lump_length(wad::index_of("TEXTURE2"));
  } else {
    maptex2 = NULL;
    numtextures2 = 0;
    maxoff2 = 0;
  }
  numtextures = numtextures1 + numtextures2;

  textures.resize(numtextures);
  texturecolumnlump.resize(numtextures);
  texturecolumnofs.resize(numtextures);
  texturecomposite.resize(numtextures);
  texturecompositesize.resize(numtextures);
  texturewidthmask.resize(numtextures);
  textureheight.resize(numtextures);

  //	Really complex printing shit...
  temp1 = wad::index_of("S_START"); // P_???????
  temp2 = wad::index_of("S_END") - 1;
  temp3 = ((temp2 - temp1 + 63) / 64) + ((numtextures + 63) / 64);
  printf("[");
  for (i = 0; i < temp3; i++)
    printf(" ");
  printf("         ]");
  for (i = 0; i < temp3; i++)
    printf("\x8");
  printf("\x8\x8\x8\x8\x8\x8\x8\x8\x8\x8");

  for (i = 0; i < numtextures; i++, directory++) {
    if (!(i & 63))
      printf(".");

    if (i == numtextures1) {
      // Start looking in second texture file.
      maptex = maptex2;
      maxoff = maxoff2;
      directory = maptex + 1;
    }

    offset = *directory;

    if (offset > maxoff)
      logger::error("R_InitTextures: bad texture directory");

    mtexture = (maptexture_t *)((std::byte *)maptex + offset);

    auto &texture = textures[i];

    texture.width = mtexture->width;
    texture.height = mtexture->height;
    texture.patches.resize(mtexture->patchcount);

    texture.name = mtexture->name;
    mpatch = &mtexture->patches[0];
    patch = &texture.patches[0];

    for (j = 0; j < texture.patches.size(); j++, mpatch++, patch++) {
      patch->originx = mpatch->originx;
      patch->originy = mpatch->originy;
      patch->patch = patchlookup[mpatch->patch];
      if (patch->patch == -1) {
        logger::error("R_InitTextures: Missing patch in texture %s",
                texture.name.c_str());
      }
    }
    texturecolumnlump[i].resize(texture.width * 2);
    texturecolumnofs[i].resize(texture.width * 2);

    j = 1;
    while (j * 2 <= texture.width)
      j <<= 1;

    texturewidthmask[i] = j - 1;
    textureheight[i] = texture.height << FRACBITS;
  }

  // Precalculate whatever possible.
  for (i = 0; i < numtextures; i++) {
    R_GenerateLookup(i);
  }

  // Create translation table for global animation.
  texturetranslation.resize(numtextures);

  for (i = 0; i < numtextures; i++)
    texturetranslation[i] = i;
}

//
// R_InitFlats
//
void R_InitFlats(void) {
  int i;

  firstflat = wad::index_of("F_START") + 1;
  lastflat = wad::index_of("F_END") - 1;
  numflats = lastflat - firstflat + 1;

  // Create translation table for global animation.
  flattranslation = static_cast<int *>(malloc((numflats + 1) * 4));

  for (i = 0; i < numflats; i++)
    flattranslation[i] = i;
}

//
// R_InitSpriteLumps
// Finds the width and hoffset of all sprites in the wad,
//  so the sprite does not need to be cached completely
//  just for having the header info ready during rendering.
//
void R_InitSpriteLumps(void) {
  int i;
  patch_t *patch;

  firstspritelump = wad::index_of("S_START") + 1;
  lastspritelump = wad::index_of("S_END") - 1;

  numspritelumps = lastspritelump - firstspritelump + 1;
  spritewidth = static_cast<fixed_t *>(malloc(numspritelumps * 4));
  spriteoffset = static_cast<fixed_t *>(malloc(numspritelumps * 4));
  spritetopoffset = static_cast<fixed_t *>(malloc(numspritelumps * 4));

  for (i = 0; i < numspritelumps; i++) {
    if (!(i & 63))
      printf(".");

    patch =
        static_cast<patch_t *>(wad::get(firstspritelump + i));
    spritewidth[i] = patch->width << FRACBITS;
    spriteoffset[i] = patch->leftoffset << FRACBITS;
    spritetopoffset[i] = patch->topoffset << FRACBITS;
  }
}

//
// R_InitColormaps
//
void R_InitColormaps(void) {
  // Load in the light tables,
  //  256 byte align tables.
  auto lump = wad::index_of("COLORMAP");
  auto length = wad::lump_length(lump) + 255;
  colormaps.resize(length);
  // TODO JONNY wtf
  //colormaps = (std::byte *)((reinterpret_cast<intptr_t>(colormaps) + 255) & ~0xff);
  //W_ReadLump(lump, colormaps.data());
}

//
// R_InitData
// Locates all the lumps
//  that will be used by all views
// Must be called after W_Init.
//
void R_InitData(void) {
  R_InitTextures();
  printf("\nInitTextures");
  R_InitFlats();
  printf("\nInitFlats");
  R_InitSpriteLumps();
  printf("\nInitSprites");
  R_InitColormaps();
  printf("\nInitColormaps");
}

//
// R_FlatNumForName
// Retrieval, get a flat number for a flat name.
//
int R_FlatNumForName(const std::string &name) {
    const auto i = wad::index_of( name.substr( 0, 8 ) );

  if (i == -1) {
    logger::error("R_FlatNumForName: %s not found", name.c_str());
  }
  return i - firstflat;
}

//
// R_CheckTextureNumForName
// Check whether texture is available.
// Filter out NoTexture indicator.
//
int R_CheckTextureNumForName(const std::string &name) {
  int i;

  // "NoTexture" marker.
  if (name[0] == '-')
    return 0;

  for (i = 0; i < numtextures; i++)
    if (textures[i].name == name)
      return i;

  return -1;
}

//
// R_TextureNumForName
// Calls R_CheckTextureNumForName,
//  aborts with error message.
//
int R_TextureNumForName(const std::string &name) {
  auto i = R_CheckTextureNumForName(name);

  if (i == -1) {
    constexpr auto max_name_length = 8;
    if(name.length() > max_name_length)
    {
      const auto truncated_name = name.substr(0,8);
      spdlog::info("{} longer than {} characters truncated to {}", name, max_name_length, truncated_name);
      i = R_CheckTextureNumForName(truncated_name);
    }
    if (i == -1) {
      logger::error("{} texture not found", name);
    }
  }
  return i;
}

//
// R_PrecacheLevel
// Preloads all relevant graphics for the level.
//
int flatmemory;
int texturememory;
int spritememory;

void R_PrecacheLevel(void) {
  char *flatpresent;
  char *texturepresent;
  char *spritepresent;

  int i;
  int j;
  int k;
  int lump;

  thinker_t *th;
  spriteframe_t *sf;

  if (demoplayback)
    return;

  // Precache flats.
  flatpresent = static_cast<char *>(alloca(numflats));
  memset(flatpresent, 0, numflats);

  for (i = 0; i < sectors.size(); i++) {
    flatpresent[sectors[i].floorpic] = 1;
    flatpresent[sectors[i].ceilingpic] = 1;
  }

  flatmemory = 0;

  for (i = 0; i < numflats; i++) {
    if (flatpresent[i]) {
      lump = firstflat + i;
      flatmemory += wad::lumpinfo[lump].size;
      wad::get(lump);
    }
  }

  // Precache textures.
  texturepresent = static_cast<char *>(alloca(numtextures));
  memset(texturepresent, 0, numtextures);

  for (i = 0; i < sides.size(); i++) {
    texturepresent[sides[i].toptexture] = 1;
    texturepresent[sides[i].midtexture] = 1;
    texturepresent[sides[i].bottomtexture] = 1;
  }

  // Sky texture is always present.
  // Note that F_SKY1 is the name used to
  //  indicate a sky floor/ceiling as a flat,
  //  while the sky texture is stored like
  //  a wall texture, with an episode dependend
  //  name.
  texturepresent[skytexture] = 1;

  texturememory = 0;
  for (i = 0; i < numtextures; i++) {
    if (!texturepresent[i])
      continue;

    const auto &texture = textures[i];

    for (const auto &patch : texture.patches) {
      lump = patch.patch;
        texturememory += wad::lumpinfo[lump].size;
      wad::get(lump);
    }
  }

  // Precache sprites.
  spritepresent = static_cast<char *>(alloca(sprites.size()));
  memset(spritepresent, 0, sprites.size());

  for (th = thinkercap.next; th != &thinkercap; th = th->next) {
    if (th->function.acp1 == (actionf_p1)P_MobjThinker)
      spritepresent[((mobj_t *)th)->sprite] = 1;
  }

  spritememory = 0;
  for (i = 0; i < sprites.size(); i++) {
    if (!spritepresent[i])
      continue;

    for (j = 0; j < sprites[i].size(); j++) {
      sf = &sprites[i][j];
      for (k = 0; k < 8; k++) {
        lump = firstspritelump + sf->lump[k];
          spritememory += wad::lumpinfo[lump].size;
        wad::get(lump);
      }
    }
  }
}
