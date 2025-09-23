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

static const char rcsid[] = "$Id: p_setup.c,v 1.5 1997/02/03 22:45:12 b1 Exp $";

#include <math.h>

#include "m_bbox.h"
#include "m_swap.h"

#include "g_game.h"

#include "i_system.h"
#include "w_wad.h"

#include "doomdef.h"
#include "p_local.h"

#include "s_sound.h"

#include "doomstat.h"

void P_SpawnMapThing(mapthing_t *mthing);

//
// MAP related Lookup tables.
// Store VERTEXES, LINEDEFS, SIDEDEFS, etc.
//
int numvertexes;
std::vector<vertex_t> vertexes;

int numsegs;
std::vector<seg_t> segs;

int numsectors;
std::vector<sector_t> sectors;

int numsubsectors;
std::vector<subsector_t> subsectors;

std::vector<node_t> nodes;

int numlines;
std::vector<line_t> lines;

int numsides;
std::vector<side_t> sides;

// BLOCKMAP
// Created from axis aligned bounding box
// of the map, a rectangular array of
// blocks of size ...
// Used to speed up collision detection
// by spatial subdivision in 2D.
//
// Blockmap size.
int bmapwidth;
int bmapheight;        // size in mapblocks
const short *blockmap; // int for larger maps
// offsets in blockmap are from here
const short *blockmaplump;
// origin of block map
fixed_t bmaporgx;
fixed_t bmaporgy;
// for thing chains
mobj_t **blocklinks;

// REJECT
// For fast sight rejection.
// Speeds up enemy AI by skipping detailed
//  LineOf Sight calculation.
// Without special effect, this could be
//  used as a PVS lookup as well.
//
const byte *rejectmatrix;

// Maintain single and multi player starting spots.
#define MAX_DEATHMATCH_STARTS 10

mapthing_t deathmatchstarts[MAX_DEATHMATCH_STARTS];
mapthing_t *deathmatch_p;
mapthing_t playerstarts[MAXPLAYERS];

//
// P_LoadVertexes
//
void P_LoadVertexes(int lump)
{
    int i;
    vertex_t *li;

    // Determine number of lumps:
    //  total lump length / vertex record length.
    numvertexes = W_LumpLength(lump) / sizeof(mapvertex_t);

    // Allocate zone memory for buffer.
    vertexes.resize(numvertexes);

    // Load data into cache.
    const auto *data = (const byte *)W_CacheLumpNum(lump);

    const auto *ml = (const mapvertex_t *)data;
    li = vertexes.data();

    // Copy and convert vertex coordinates,
    // internal representation as fixed.
    for (i = 0; i < numvertexes; i++, li++, ml++)
    {
        li->x = SHORT(ml->x) << FRACBITS;
        li->y = SHORT(ml->y) << FRACBITS;
    }
}

//
// P_LoadSegs
//
void P_LoadSegs(int lump)
{
    int i;
    mapseg_t *ml;
    seg_t *li;
    line_t *ldef;
    int linedef;
    int side;

    numsegs = W_LumpLength(lump) / sizeof(mapseg_t);
    segs.resize(numsegs);
    const auto *data = (const byte *)W_CacheLumpNum(lump);

    ml = (mapseg_t *)data;
    li = segs.data();
    for (i = 0; i < numsegs; i++, li++, ml++)
    {
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
}

//
// P_LoadSubsectors
//
void P_LoadSubsectors(int lump)
{
    int i;
    mapsubsector_t *ms;
    subsector_t *ss;

    numsubsectors = W_LumpLength(lump) / sizeof(mapsubsector_t);
    subsectors.resize(numsubsectors);
    const auto *data = (const byte *)W_CacheLumpNum(lump);

    ms = (mapsubsector_t *)data;
    ss = subsectors.data();

    for (i = 0; i < numsubsectors; i++, ss++, ms++)
    {
        ss->numlines = SHORT(ms->numsegs);
        ss->firstline = SHORT(ms->firstseg);
    }
}

//
// P_LoadSectors
//
void P_LoadSectors(int lump)
{
    int i;
    mapsector_t *ms;
    sector_t *ss;

    numsectors = W_LumpLength(lump) / sizeof(mapsector_t);
    sectors.resize(numsectors);
    const auto *data = (const byte *)W_CacheLumpNum(lump);

    ms = (mapsector_t *)data;
    ss = sectors.data();
    for (i = 0; i < numsectors; i++, ss++, ms++)
    {
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
void P_LoadNodes(int lump)
{
    int j;
    int k;
    mapnode_t *mn;

    auto numnodes = W_LumpLength(lump) / sizeof(mapnode_t);
    nodes.resize(numnodes);
    const auto *data = (const byte *)W_CacheLumpNum(lump);

    mn = (mapnode_t *)data;

    for (auto &node : nodes)
    {
        node.x = SHORT(mn->x) << FRACBITS;
        node.y = SHORT(mn->y) << FRACBITS;
        node.dx = SHORT(mn->dx) << FRACBITS;
        node.dy = SHORT(mn->dy) << FRACBITS;
        for (j = 0; j < 2; j++)
        {
            node.children[j] = SHORT(mn->children[j]);
            for (k = 0; k < 4; k++)
                node.bbox[j][k] = SHORT(mn->bbox[j][k]) << FRACBITS;
        }
        mn++;
    }
}

//
// P_LoadThings
//
void P_LoadThings(int lump)
{
    int i;
    mapthing_t *mt;
    int numthings;
    boolean spawn;

    const auto *data = (const byte *)W_CacheLumpNum(lump);
    numthings = W_LumpLength(lump) / sizeof(mapthing_t);

    mt = (mapthing_t *)data;
    for (i = 0; i < numthings; i++, mt++)
    {
        spawn = true;

        // Do not spawn cool, new monsters if !commercial
        if (gamemode != commercial)
        {
            switch (mt->type)
            {
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
void P_LoadLineDefs(int lump)
{
    int i;
    maplinedef_t *mld;
    line_t *ld;
    vertex_t *v1;
    vertex_t *v2;

    numlines = W_LumpLength(lump) / sizeof(maplinedef_t);
    lines.resize(numlines);
    const auto *data = (const byte *)W_CacheLumpNum(lump);

    mld = (maplinedef_t *)data;
    ld = lines.data();
    for (i = 0; i < numlines; i++, mld++, ld++)
    {
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
        else
        {
            if (FixedDiv(ld->dy, ld->dx) > 0)
                ld->slopetype = ST_POSITIVE;
            else
                ld->slopetype = ST_NEGATIVE;
        }

        if (v1->x < v2->x)
        {
            ld->bbox[BOXLEFT] = v1->x;
            ld->bbox[BOXRIGHT] = v2->x;
        }
        else
        {
            ld->bbox[BOXLEFT] = v2->x;
            ld->bbox[BOXRIGHT] = v1->x;
        }

        if (v1->y < v2->y)
        {
            ld->bbox[BOXBOTTOM] = v1->y;
            ld->bbox[BOXTOP] = v2->y;
        }
        else
        {
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
}

//
// P_LoadSideDefs
//
void P_LoadSideDefs(int lump)
{
    int i;
    mapsidedef_t *msd;
    side_t *sd;

    numsides = W_LumpLength(lump) / sizeof(mapsidedef_t);
    sides.resize(numsides);

    const auto *data = (const byte *)W_CacheLumpNum(lump);

    msd = (mapsidedef_t *)data;
    sd = sides.data();
    for (i = 0; i < numsides; i++, msd++, sd++)
    {
        sd->textureoffset = SHORT(msd->textureoffset) << FRACBITS;
        sd->rowoffset = SHORT(msd->rowoffset) << FRACBITS;
        sd->toptexture = R_TextureNumForName(msd->toptexture);
        sd->bottomtexture = R_TextureNumForName(msd->bottomtexture);
        sd->midtexture = R_TextureNumForName(msd->midtexture);
        sd->sector = &sectors[SHORT(msd->sector)];
    }
}

//
// P_LoadBlockMap
//
void P_LoadBlockMap(int lump)
{
    int i;
    int count;

    blockmaplump = (const short *)W_CacheLumpNum(lump);
    blockmap = blockmaplump + 4;

    bmaporgx = blockmaplump[0] << FRACBITS;
    bmaporgy = blockmaplump[1] << FRACBITS;
    bmapwidth = blockmaplump[2];
    bmapheight = blockmaplump[3];

    // clear out mobj chains
    count = sizeof(*blocklinks) * bmapwidth * bmapheight;
    blocklinks = (mobj_t **)malloc(count);
    memset(blocklinks, 0, count);
}

//
// P_GroupLines
// Builds sector line lists and subsector sector numbers.
// Finds block bounding boxes for sectors.
//
void P_GroupLines(void)
{
    int i;
    int j;
    fixed_t bbox[4];
    int block;

    // look up sector number for each subsector
    for (auto &subsector : subsectors)
    {
        const auto &seg = segs[subsector.firstline];
        subsector.sector = seg.sidedef->sector;
    }

    // count number of lines in each sector
    auto total = 0;
    for (const auto &line : lines)
    {
        total++;
        line.frontsector->linecount++;

        if (line.backsector && line.backsector != line.frontsector)
        {
            line.backsector->linecount++;
            total++;
        }
    }

    // build line tables for each sector
    for (auto &sector : sectors)
    {
        M_ClearBox(bbox);
        auto li = lines.data();
        for (j = 0; j < numlines; j++, li++)
        {
            if (li->frontsector == &sector || li->backsector == &sector)
            {
                sector.lines.emplace_back(li);
                M_AddToBox(bbox, li->v1->x, li->v1->y);
                M_AddToBox(bbox, li->v2->x, li->v2->y);
            }
        }
        if (sector.lines.size() != sector.linecount)
            I_Error("P_GroupLines: miscounted");

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
void P_SetupLevel(int episode, int map, int playermask, skill_t skill)
{
    int i;
    char lumpname[9];
    int lumpnum;

    totalkills = totalitems = totalsecret = wminfo.maxfrags = 0;
    wminfo.partime = 180;
    for (i = 0; i < MAXPLAYERS; i++)
    {
        players[i].killcount = players[i].secretcount = players[i].itemcount = 0;
    }

    // Initial height of PointOfView
    // will be set by player think.
    players[consoleplayer].viewz = 1;

    // Make sure all sounds are stopped before Z_FreeTags.
    S_Start();

#if 0 // UNUSED
    if (debugfile)
    {

        Z_FileDumpHeap(debugfile);
    }
    else
#endif

    // UNUSED W_Profile ();
    P_InitThinkers();

    // find map name
    if (gamemode == commercial)
    {
        if (map < 10)
            sprintf(lumpname, "map0%i", map);
        else
            sprintf(lumpname, "map%i", map);
    }
    else
    {
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

    rejectmatrix = (const byte *)W_CacheLumpNum(lumpnum + ML_REJECT);
    P_GroupLines();

    bodyqueslot = 0;
    deathmatch_p = deathmatchstarts;
    P_LoadThings(lumpnum + ML_THINGS);

    // if deathmatch, randomly spawn the active players
    if (deathmatch)
    {
        for (i = 0; i < MAXPLAYERS; i++)
            if (playeringame[i])
            {
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

    // printf ("free memory: 0x%x\n", Z_FreeMemory());
}

//
// P_Init
//
void P_Init(void)
{
    P_InitSwitchList();
    P_InitPicAnims();
    R_InitSprites((char **)sprnames);
}