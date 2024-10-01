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
//	Do all the WAD I/O, get map description,
//	set up initial state and misc. LUTs.
//
//-----------------------------------------------------------------------------
module;
#include <math.h>



#include "m_bbox.h"
#include "m_swap.h"

#include "g_game.h"
#include "p_local.h"


#include "r_data.h"
#include "r_things.h"

export module setup;

import system;
import wad;
import doomstat;
import tick;
import sound;

//
// MAP related Lookup tables.
// Store VERTEXES, LINEDEFS, SIDEDEFS, etc.
//
export int numvertexes;
export vertex_t *vertexes;

int numsegs;
export seg_t *segs;

int numsectors;
export std::vector<sector_t> sectors;

int numsubsectors;
export subsector_t *subsectors;

export int numnodes;
export node_t *nodes;

export int numlines;
export line_t *lines;

export int numsides;
export side_t *sides;

// BLOCKMAP
// Created from axis aligned bounding box
// of the map, a rectangular array of
// blocks of size ...
// Used to speed up collision detection
// by spatial subdivision in 2D.
//
// Blockmap size.
export int bmapwidth;
export int bmapheight;  // size in mapblocks
export short *blockmap; // int for larger maps
// offsets in blockmap are from here
export short *blockmaplump;
// origin of block map
export fixed_t bmaporgx;
export fixed_t bmaporgy;
// for thing chains
export mobj_t **blocklinks;

// REJECT
// For fast sight rejection.
// Speeds up enemy AI by skipping detailed
//  LineOf Sight calculation.
// Without special effect, this could be
//  used as a PVS lookup as well.
//
export std::byte *rejectmatrix;

// Maintain single and multi player starting spots.
#define MAX_DEATHMATCH_STARTS 10

export mapthing_t deathmatchstarts[MAX_DEATHMATCH_STARTS];
export mapthing_t *deathmatch_p;
export mapthing_t playerstarts[MAXPLAYERS];

//
// P_LoadVertexes
//
void P_LoadVertexes(int lump) {
  std::byte *data;
  int i;
  mapvertex_t *ml;
  vertex_t *li;

  // Determine number of lumps:
  //  total lump length / vertex record length.
  numvertexes = W_LumpLength(lump) / sizeof(mapvertex_t);

  // Allocate zone memory for buffer.
  vertexes = static_cast<vertex_t *>(malloc(numvertexes * sizeof(vertex_t)));

  // Load data into cache.
  data = static_cast<std::byte *>(W_CacheLumpNum(lump));

  ml = (mapvertex_t *)data;
  li = vertexes;

  // Copy and convert vertex coordinates,
  // internal representation as fixed.
  for (i = 0; i < numvertexes; i++, li++, ml++) {
    li->x = SHORT(ml->x) << FRACBITS;
    li->y = SHORT(ml->y) << FRACBITS;
  }

  // Free buffer memory.
  free(data);
}

//
// P_LoadSegs
//
void P_LoadSegs(int lump) {
  std::byte *data;
  int i;
  mapseg_t *ml;
  seg_t *li;
  line_t *ldef;
  int linedef;
  int side;

  numsegs = W_LumpLength(lump) / sizeof(mapseg_t);
  segs = static_cast<seg_t *>(malloc(numsegs * sizeof(seg_t)));
  memset(segs, 0, numsegs * sizeof(seg_t));
  data = static_cast<std::byte *>(W_CacheLumpNum(lump));

  ml = (mapseg_t *)data;
  li = segs;
  for (i = 0; i < numsegs; i++, li++, ml++) {
    li->v1 = &vertexes[SHORT(ml->v1)];
    li->v2 = &vertexes[SHORT(ml->v2)];

    li->angle = (SHORT(ml->angle)) << 16;
    li->offset = (SHORT(ml->offset)) << 16;
    linedef = SHORT(ml->linedef);
    ldef = &lines[linedef];
    li->linedef = ldef;
    side = SHORT(ml->side);
    li->sidedef = &sides[ldef->sidenum[side]];
    li->frontsector = sides[ldef->sidenum[side]].sector;
    if (ldef->flags & ML_TWOSIDED)
      li->backsector = sides[ldef->sidenum[side ^ 1]].sector;
    else
      li->backsector = 0;
  }

  free(data);
}

//
// P_LoadSubsectors
//
void P_LoadSubsectors(int lump) {
  std::byte *data;
  int i;
  mapsubsector_t *ms;
  subsector_t *ss;

  numsubsectors = W_LumpLength(lump) / sizeof(mapsubsector_t);
  subsectors =
      static_cast<subsector_t *>(malloc(numsubsectors * sizeof(subsector_t)));
  data = static_cast<std::byte *>(W_CacheLumpNum(lump));

  ms = (mapsubsector_t *)data;
  memset(subsectors, 0, numsubsectors * sizeof(subsector_t));
  ss = subsectors;

  for (i = 0; i < numsubsectors; i++, ss++, ms++) {
    ss->numlines = SHORT(ms->numsegs);
    ss->firstline = SHORT(ms->firstseg);
  }

  free(data);
}

//
// P_LoadSectors
//
void P_LoadSectors(int lump) {
  std::byte *data;
  int i;
  mapsector_t *ms;
  sector_t *ss;

  numsectors = W_LumpLength(lump) / sizeof(mapsector_t);
  sectors.resize(numsectors);
  data = static_cast<std::byte *>(W_CacheLumpNum(lump));

  ms = (mapsector_t *)data;
  ss = sectors.data();
  for (i = 0; i < numsectors; i++, ss++, ms++) {
    ss->floorheight = SHORT(ms->floorheight) << FRACBITS;
    ss->ceilingheight = SHORT(ms->ceilingheight) << FRACBITS;
    ss->floorpic = R_FlatNumForName(ms->floorpic);
    ss->ceilingpic = R_FlatNumForName(ms->ceilingpic);
    ss->lightlevel = SHORT(ms->lightlevel);
    ss->special = SHORT(ms->special);
    ss->tag = SHORT(ms->tag);
    ss->thinglist = NULL;
  }
}

//
// P_LoadNodes
//
void P_LoadNodes(int lump) {
  std::byte *data;
  int i;
  int j;
  int k;
  mapnode_t *mn;
  node_t *no;

  numnodes = W_LumpLength(lump) / sizeof(mapnode_t);
  nodes = static_cast<node_t *>(malloc(numnodes * sizeof(node_t)));
  data = static_cast<std::byte *>(W_CacheLumpNum(lump));

  mn = (mapnode_t *)data;
  no = nodes;

  for (i = 0; i < numnodes; i++, no++, mn++) {
    no->x = SHORT(mn->x) << FRACBITS;
    no->y = SHORT(mn->y) << FRACBITS;
    no->dx = SHORT(mn->dx) << FRACBITS;
    no->dy = SHORT(mn->dy) << FRACBITS;
    for (j = 0; j < 2; j++) {
      no->children[j] = SHORT(mn->children[j]);
      for (k = 0; k < 4; k++)
        no->bbox[j][k] = SHORT(mn->bbox[j][k]) << FRACBITS;
    }
  }

  free(data);
}

//
// P_LoadThings
//
void P_LoadThings(int lump) {
  std::byte *data;
  int i;
  mapthing_t *mt;
  int numthings;
  bool spawn;

  data = static_cast<std::byte *>(W_CacheLumpNum(lump));
  numthings = W_LumpLength(lump) / sizeof(mapthing_t);

  mt = (mapthing_t *)data;
  for (i = 0; i < numthings; i++, mt++) {
    spawn = true;

    // Do not spawn cool, new monsters if !commercial
    if (gamemode != commercial) {
      switch (mt->type) {
      case 68: // Arachnotron
      case 64: // Archvile
      case 88: // Boss Brain
      case 89: // Boss Shooter
      case 69: // Hell Knight
      case 67: // Mancubus
      case 71: // Pain Elemental
      case 65: // Former Human Commando
      case 66: // Revenant
      case 84: // Wolf SS
        spawn = false;
        break;
      }
    }
    if (spawn == false)
      break;

    // Do spawn all other stuff.
    mt->x = SHORT(mt->x);
    mt->y = SHORT(mt->y);
    mt->angle = SHORT(mt->angle);
    mt->type = SHORT(mt->type);
    mt->options = SHORT(mt->options);

    P_SpawnMapThing(mt);
  }
}

//
// P_LoadLineDefs
// Also counts secret lines for intermissions.
//
void P_LoadLineDefs(int lump) {
  std::byte *data;
  int i;
  maplinedef_t *mld;
  line_t *ld;
  vertex_t *v1;
  vertex_t *v2;

  numlines = W_LumpLength(lump) / sizeof(maplinedef_t);
  lines = static_cast<line_t *>(malloc(numlines * sizeof(line_t)));
  memset(lines, 0, numlines * sizeof(line_t));
  data = static_cast<std::byte *>(W_CacheLumpNum(lump));

  mld = (maplinedef_t *)data;
  ld = lines;
  for (i = 0; i < numlines; i++, mld++, ld++) {
    ld->flags = SHORT(mld->flags);
    ld->special = SHORT(mld->special);
    ld->tag = SHORT(mld->tag);
    v1 = ld->v1 = &vertexes[SHORT(mld->v1)];
    v2 = ld->v2 = &vertexes[SHORT(mld->v2)];
    ld->dx = v2->x - v1->x;
    ld->dy = v2->y - v1->y;

    if (!ld->dx)
      ld->slopetype = ST_VERTICAL;
    else if (!ld->dy)
      ld->slopetype = ST_HORIZONTAL;
    else {
      if (FixedDiv(ld->dy, ld->dx) > 0)
        ld->slopetype = ST_POSITIVE;
      else
        ld->slopetype = ST_NEGATIVE;
    }

    if (v1->x < v2->x) {
      ld->bbox[BOXLEFT] = v1->x;
      ld->bbox[BOXRIGHT] = v2->x;
    } else {
      ld->bbox[BOXLEFT] = v2->x;
      ld->bbox[BOXRIGHT] = v1->x;
    }

    if (v1->y < v2->y) {
      ld->bbox[BOXBOTTOM] = v1->y;
      ld->bbox[BOXTOP] = v2->y;
    } else {
      ld->bbox[BOXBOTTOM] = v2->y;
      ld->bbox[BOXTOP] = v1->y;
    }

    ld->sidenum[0] = SHORT(mld->sidenum[0]);
    ld->sidenum[1] = SHORT(mld->sidenum[1]);

    if (ld->sidenum[0] != -1)
      ld->frontsector = sides[ld->sidenum[0]].sector;
    else
      ld->frontsector = 0;

    if (ld->sidenum[1] != -1)
      ld->backsector = sides[ld->sidenum[1]].sector;
    else
      ld->backsector = 0;
  }

  free(data);
}

//
// P_LoadSideDefs
//
void P_LoadSideDefs(int lump) {
  std::byte *data;
  int i;
  mapsidedef_t *msd;
  side_t *sd;

  numsides = W_LumpLength(lump) / sizeof(mapsidedef_t);
  sides = static_cast<side_t *>(malloc(numsides * sizeof(side_t)));
  memset(sides, 0, numsides * sizeof(side_t));
  data = static_cast<std::byte *>(W_CacheLumpNum(lump));

  msd = (mapsidedef_t *)data;
  sd = sides;
  for (i = 0; i < numsides; i++, msd++, sd++) {
    sd->textureoffset = SHORT(msd->textureoffset) << FRACBITS;
    sd->rowoffset = SHORT(msd->rowoffset) << FRACBITS;
    sd->toptexture = R_TextureNumForName(msd->toptexture);
    sd->bottomtexture = R_TextureNumForName(msd->bottomtexture);
    sd->midtexture = R_TextureNumForName(msd->midtexture);
    sd->sector = &sectors[SHORT(msd->sector)];
  }

  free(data);
}

//
// P_LoadBlockMap
//
void P_LoadBlockMap(int lump) {
  int i;
  int count;

  blockmaplump = static_cast<short *>(W_CacheLumpNum(lump));
  blockmap = blockmaplump + 4;
  count = W_LumpLength(lump) / 2;

  for (i = 0; i < count; i++)
    blockmaplump[i] = SHORT(blockmaplump[i]);

  bmaporgx = blockmaplump[0] << FRACBITS;
  bmaporgy = blockmaplump[1] << FRACBITS;
  bmapwidth = blockmaplump[2];
  bmapheight = blockmaplump[3];

  // clear out mobj chains
  count = sizeof(*blocklinks) * bmapwidth * bmapheight;
  blocklinks = static_cast<mobj_t **>(malloc(count));
  memset(blocklinks, 0, count);
}

//
// P_GroupLines
// Builds sector line lists and subsector sector numbers.
// Finds block bounding boxes for sectors.
//
void P_GroupLines(void) {
  int j;
  line_t *li;
  subsector_t *ss;
  seg_t *seg;
  fixed_t bbox[4];
  int block;

  // look up sector number for each subsector
  ss = subsectors;
  for (auto i = 0; i < numsubsectors; i++, ss++) {
    seg = &segs[ss->firstline];
    ss->sector = seg->sidedef->sector;
  }

  // build line tables for each sector
  for (auto &sector : sectors) {
    M_ClearBox(bbox);
    li = lines;
    for (j = 0; j < numlines; j++, li++) {
      if (li->frontsector == &sector || li->backsector == &sector) {
        sector.lines.push_back(li);
        M_AddToBox(bbox, li->v1->x, li->v1->y);
        M_AddToBox(bbox, li->v2->x, li->v2->y);
      }
    }

    // set the degenmobj_t to the middle of the bounding box
    sector.soundorg.x = (bbox[BOXRIGHT] + bbox[BOXLEFT]) / 2;
    sector.soundorg.y = (bbox[BOXTOP] + bbox[BOXBOTTOM]) / 2;

    // adjust bounding box to map blocks
    block = (bbox[BOXTOP] - bmaporgy + MAXRADIUS) >> MAPBLOCKSHIFT;
    block = block >= bmapheight ? bmapheight - 1 : block;
    sector.blockbox[BOXTOP] = block;

    block = (bbox[BOXBOTTOM] - bmaporgy - MAXRADIUS) >> MAPBLOCKSHIFT;
    block = block < 0 ? 0 : block;
    sector.blockbox[BOXBOTTOM] = block;

    block = (bbox[BOXRIGHT] - bmaporgx + MAXRADIUS) >> MAPBLOCKSHIFT;
    block = block >= bmapwidth ? bmapwidth - 1 : block;
    sector.blockbox[BOXRIGHT] = block;

    block = (bbox[BOXLEFT] - bmaporgx - MAXRADIUS) >> MAPBLOCKSHIFT;
    block = block < 0 ? 0 : block;
    sector.blockbox[BOXLEFT] = block;
  }
}

//
// P_SetupLevel
//
export void P_SetupLevel(int episode, int map, int playermask, skill_t skill) {
  int i;
  char lumpname[9];
  int lumpnum;

  totalkills = totalitems = totalsecret = wminfo.maxfrags = 0;
  wminfo.partime = 180;
  for (i = 0; i < MAXPLAYERS; i++) {
    players[i].killcount = players[i].secretcount = players[i].itemcount = 0;
  }

  // Initial height of PointOfView
  // will be set by player think.
  players[consoleplayer].viewz = 1;

  // Make sure all sounds are stopped before freeTags.
  S_Start();

  P_InitThinkers();

  // if working with a devlopment map, reload it
  W_Reload();

  // find map name
  if (gamemode == commercial) {
    if (map < 10)
      snprintf(lumpname, 9, "map0%i", map);
    else
      snprintf(lumpname, 9, "map%i", map);
  } else {
    lumpname[0] = 'E';
    lumpname[1] = '0' + episode;
    lumpname[2] = 'M';
    lumpname[3] = '0' + map;
    lumpname[4] = 0;
  }

  lumpnum = W_GetNumForName(lumpname);

  leveltime = 0;

  // note: most of this ordering is important
  P_LoadBlockMap(lumpnum + ML_BLOCKMAP);
  P_LoadVertexes(lumpnum + ML_VERTEXES);
  P_LoadSectors(lumpnum + ML_SECTORS);
  P_LoadSideDefs(lumpnum + ML_SIDEDEFS);

  P_LoadLineDefs(lumpnum + ML_LINEDEFS);
  P_LoadSubsectors(lumpnum + ML_SSECTORS);
  P_LoadNodes(lumpnum + ML_NODES);
  P_LoadSegs(lumpnum + ML_SEGS);

  rejectmatrix =
      static_cast<std::byte *>(W_CacheLumpNum(lumpnum + ML_REJECT));
  P_GroupLines();

  bodyqueslot = 0;
  deathmatch_p = deathmatchstarts;
  P_LoadThings(lumpnum + ML_THINGS);

  // if deathmatch, randomly spawn the active players
  if (deathmatch) {
    for (i = 0; i < MAXPLAYERS; i++)
      if (playeringame[i]) {
        players[i].mo = NULL;
        G_DeathMatchSpawnPlayer(i);
      }
  }

  // clear special respawning que
  iquehead = iquetail = 0;

  // set up world state
  P_SpawnSpecials();

  // build subsector connect matrix
  //	UNUSED P_ConnectSubsectors ();

  // preload graphics
  if (precache)
    R_PrecacheLevel();

  // printf ("free memory: 0x%x\n", freeMemory());
}

//
// P_Init
//
export void P_Init(void) {
  P_InitSwitchList();
  P_InitPicAnims();
  R_InitSprites(sprnames);
}
