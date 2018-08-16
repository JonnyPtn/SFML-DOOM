#include <math.h>
#include "m_bbox.hpp"

#include "g_game.hpp"

#include "i_system.hpp"
#include "w_wad.hpp"

#include "doomdef.hpp"
#include "p_local.hpp"

#include "i_sound.hpp"
#include "sounds.hpp"

#include "doomstat.hpp"
#include "r_things.hpp"


void	P_SpawnMapThing (mapthing_t*	mthing);


//
// MAP related Lookup tables.
// Store VERTEXES, LINEDEFS, SIDEDEFS, etc.
//
int		numvertexes;
vertex_t*	vertexes;

int		numsegs;
seg_t*		segs;

int		numsectors;
sector_t*	sectors;

int		numsubsectors;
subsector_t*	subsectors;

int		numnodes;
node_t*		nodes;

int		numlines;
line_t*		lines;

int		numsides;
side_t*		sides;


// BLOCKMAP
// Created from axis aligned bounding box
// of the map, a rectangular array of
// blocks of size ...
// Used to speed up collision detection
// by spatial subdivision in 2D.
//
// Blockmap size.
int		bmapwidth;
int		bmapheight;	// size in mapblocks
short*		blockmap;	// int for larger maps
// offsets in blockmap are from here
short*		blockmaplump;		
// origin of block map
int		bmaporgx;
int		bmaporgy;
// for thing chains
mobj_t**	blocklinks;		


// REJECT
// For fast sight rejection.
// Speeds up enemy AI by skipping detailed
//  LineOf Sight calculation.
// Without special effect, this could be
//  used as a PVS lookup as well.
//
unsigned char*		rejectmatrix;


// Maintain single and multi player starting spots.
#define MAX_DEATHMATCH_STARTS	10

mapthing_t	deathmatchstarts[MAX_DEATHMATCH_STARTS];
mapthing_t*	deathmatch_p;
mapthing_t	playerstarts[MAXPLAYERS];





//
// P_LoadVertexes
//
void P_LoadVertexes (int lump)
{
    unsigned char*		data;
    int			i;
    mapvertex_t*	ml;
    vertex_t*		li;

    // Determine number of lumps:
    //  total lump length / vertex record length.
    numvertexes = WadManager::WadManager::getLumpLength (lump) / sizeof(mapvertex_t);

    // Allocate zone memory for buffer.
    vertexes = (vertex_t*)malloc (numvertexes*sizeof(vertex_t));	

    // Load data into cache.
    data = (unsigned char*)WadManager::getLump (lump);
	
    ml = (mapvertex_t *)data;
    li = vertexes;

    // Copy and convert vertex coordinates,
    // internal representation as fixed.
    for (i=0 ; i<numvertexes ; i++, li++, ml++)
    {
	li->x = ml->x<<FRACBITS;
	li->y = ml->y<<FRACBITS;
    }
}



//
// P_LoadSegs
//
void P_LoadSegs (int lump)
{
    unsigned char*		data;
    int			i;
    mapseg_t*		ml;
    seg_t*		li;
    line_t*		ldef;
    int			linedef;
    int			side;
	
    numsegs = WadManager::getLumpLength (lump) / sizeof(mapseg_t);
    segs = (seg_t*)malloc (numsegs*sizeof(seg_t));	
    memset (segs, 0, numsegs*sizeof(seg_t));
    data = (unsigned char*)WadManager::getLump (lump);
	
    ml = (mapseg_t *)data;
    li = segs;
    for (i=0 ; i<numsegs ; i++, li++, ml++)
    {
	li->v1 = &vertexes[ml->v1];
	li->v2 = &vertexes[ml->v2];
					
	li->angle = (ml->angle)<<16;
	li->offset = (ml->offset)<<16;
	linedef = ml->linedef;
	ldef = &lines[linedef];
	li->linedef = ldef;
	side = ml->side;
	li->sidedef = &sides[ldef->sidenum[side]];
	li->frontsector = sides[ldef->sidenum[side]].sector;
	if (ldef-> flags & ML_TWOSIDED)
	    li->backsector = sides[ldef->sidenum[side^1]].sector;
	else
	    li->backsector = 0;
    }
}


//
// P_LoadSubsectors
//
void P_LoadSubsectors (int lump)
{
    unsigned char*		data;
    int			i;
    mapsubsector_t*	ms;
    subsector_t*	ss;
	
    numsubsectors = WadManager::getLumpLength (lump) / sizeof(mapsubsector_t);
    subsectors = (subsector_t*)malloc (numsubsectors*sizeof(subsector_t));	
    data = (unsigned char*)WadManager::getLump (lump);
	
    ms = (mapsubsector_t *)data;
    memset (subsectors,0, numsubsectors*sizeof(subsector_t));
    ss = subsectors;
    
    for (i=0 ; i<numsubsectors ; i++, ss++, ms++)
    {
	ss->numlines = ms->numsegs;
	ss->firstline = ms->firstseg;
    }
}



//
// P_LoadSectors
//
void P_LoadSectors (int lump)
{
    unsigned char*		data;
    int			i;
    mapsector_t*	ms;
    sector_t*		ss;
	
    numsectors = WadManager::getLumpLength (lump) / sizeof(mapsector_t);
    sectors = (sector_t*)malloc (numsectors*sizeof(sector_t));	
    memset (sectors, 0, numsectors*sizeof(sector_t));
    data = (unsigned char*)WadManager::getLump (lump);
	
    ms = (mapsector_t *)data;
    ss = sectors;
    for (i=0 ; i<numsectors ; i++, ss++, ms++)
    {
	ss->floorheight = ms->floorheight<<FRACBITS;
	ss->ceilingheight = ms->ceilingheight<<FRACBITS;
	ss->floorpic = R_FlatNumForName(std::string(ms->floorpic,8));
	ss->ceilingpic = R_FlatNumForName(std::string(ms->ceilingpic,8));
	ss->lightlevel = ms->lightlevel;
	ss->special = ms->special;
	ss->tag = ms->tag;
	ss->thinglist = NULL;
    }
}


//
// P_LoadNodes
//
void P_LoadNodes (int lump)
{
    unsigned char*	data;
    int		i;
    int		j;
    int		k;
    mapnode_t*	mn;
    node_t*	no;
	
    numnodes = WadManager::getLumpLength (lump) / sizeof(mapnode_t);
    nodes = (node_t*)malloc (numnodes*sizeof(node_t));	
    data = (unsigned char*)WadManager::getLump (lump);
	
    mn = (mapnode_t *)data;
    no = nodes;
    
    for (i=0 ; i<numnodes ; i++, no++, mn++)
    {
	no->x = mn->x<<FRACBITS;
	no->y = mn->y<<FRACBITS;
	no->dx = mn->dx<<FRACBITS;
	no->dy = mn->dy<<FRACBITS;
	for (j=0 ; j<2 ; j++)
	{
	    no->children[j] = mn->children[j];
	    for (k=0 ; k<4 ; k++)
		no->bbox[j][k] = mn->bbox[j][k]<<FRACBITS;
	}
    }
}


//
// P_LoadThings
//
void P_LoadThings (int lump)
{
    unsigned char*		data;
    int			i;
    mapthing_t*		mt;
    int			numthings;
    bool		spawn;
	
    data =(unsigned char*) WadManager::getLump (lump);
    numthings = WadManager::getLumpLength (lump) / sizeof(mapthing_t);
	
    mt = (mapthing_t *)data;
    for (i=0 ; i<numthings ; i++, mt++)
    {
	spawn = true;

	// Do not spawn cool, new monsters if !commercial
	if (Game::gamemode != GameMode_t::commercial)
	{
	    switch(mt->type)
	    {
	      case 68:	// Arachnotron
	      case 64:	// Archvile
	      case 88:	// Boss Brain
	      case 89:	// Boss Shooter
	      case 69:	// Hell Knight
	      case 67:	// Mancubus
	      case 71:	// Pain Elemental
	      case 65:	// Former Human Commando
	      case 66:	// Revenant
	      case 84:	// Wolf SS
		spawn = false;
		break;
	    }
	}
	if (spawn == false)
	    break;

	// Do spawn all other stuff. 
	mt->x = mt->x;
	mt->y = mt->y;
	mt->angle = mt->angle;
	mt->type = mt->type;
	mt->options = mt->options;
	
	P_SpawnMapThing (mt);
    }
}


//
// P_LoadLineDefs
// Also counts secret lines for intermissions.
//
void P_LoadLineDefs (int lump)
{
    unsigned char*		data;
    int			i;
    maplinedef_t*	mld;
    line_t*		ld;
    vertex_t*		v1;
    vertex_t*		v2;
	
    numlines = WadManager::getLumpLength (lump) / sizeof(maplinedef_t);
    lines = (line_t*)malloc (numlines*sizeof(line_t));	
    memset (lines, 0, numlines*sizeof(line_t));
    data = (unsigned char*)WadManager::getLump (lump);
	
    mld = (maplinedef_t *)data;
    ld = lines;
    for (i=0 ; i<numlines ; i++, mld++, ld++)
    {
	ld->flags = mld->flags;
	ld->special = mld->special;
	ld->tag = mld->tag;
	v1 = ld->v1 = &vertexes[mld->v1];
	v2 = ld->v2 = &vertexes[mld->v2];
	ld->dx = v2->x - v1->x;
	ld->dy = v2->y - v1->y;
	
	if (!ld->dx)
	    ld->slopetype = ST_VERTICAL;
	else if (!ld->dy)
	    ld->slopetype = ST_HORIZONTAL;
	else
	{
	    if (FixedDiv (ld->dy , ld->dx) > 0)
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

	ld->sidenum[0] = mld->sidenum[0];
	ld->sidenum[1] = mld->sidenum[1];

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
void P_LoadSideDefs (int lump)
{
    unsigned char*		data;
    int			i;
    mapsidedef_t*	msd;
    side_t*		sd;
	
    numsides = WadManager::getLumpLength (lump) / sizeof(mapsidedef_t);
    sides = (side_t*)malloc (numsides*sizeof(side_t));	
    memset (sides, 0, numsides*sizeof(side_t));
    data = (unsigned char*)WadManager::getLump (lump);
	
    msd = (mapsidedef_t *)data;
    sd = sides;
    for (i=0 ; i<numsides ; i++, msd++, sd++)
    {
	sd->textureoffset = msd->textureoffset<<FRACBITS;
	sd->rowoffset = msd->rowoffset<<FRACBITS;
	sd->toptexture = R_TextureNumForName(msd->toptexture);
	sd->bottomtexture = R_TextureNumForName(msd->bottomtexture);
	sd->midtexture = R_TextureNumForName(msd->midtexture);
	sd->sector = &sectors[msd->sector];
    }
}


//
// P_LoadBlockMap
//
void P_LoadBlockMap (int lump)
{
    int		i;
    int		count;
	
    blockmaplump = (short*)WadManager::getLump (lump);
    blockmap = blockmaplump+4;
    count = WadManager::getLumpLength (lump)/2;

    for (i=0 ; i<count ; i++)
	blockmaplump[i] = blockmaplump[i];
		
    bmaporgx = blockmaplump[0]<<FRACBITS;
    bmaporgy = blockmaplump[1]<<FRACBITS;
    bmapwidth = blockmaplump[2];
    bmapheight = blockmaplump[3];
	
    // clear out mobj chains
    count = sizeof(*blocklinks)* bmapwidth*bmapheight;
    blocklinks = (mobj_t**)malloc (count);
    memset (blocklinks, 0, count);
}



//
// P_GroupLines
// Builds sector line lists and subsector sector numbers.
// Finds block bounding boxes for sectors.
//
void P_GroupLines (void)
{
    line_t**		linebuffer;
    int			i;
    int			j;
    int			total;
    line_t*		li;
    sector_t*		sector;
    subsector_t*	ss;
    seg_t*		seg;
    int		bbox[4];
    int			block;
	
    // look up sector number for each subsector
    ss = subsectors;
    for (i=0 ; i<numsubsectors ; i++, ss++)
    {
	seg = &segs[ss->firstline];
	ss->sector = seg->sidedef->sector;
    }

    // count number of lines in each sector
    li = lines;
    total = 0;
    for (i=0 ; i<numlines ; i++, li++)
    {
	total++;
	li->frontsector->linecount++;

	if (li->backsector && li->backsector != li->frontsector)
	{
	    li->backsector->linecount++;
	    total++;
	}
    }
	
    // build line tables for each sector	
    linebuffer = (line_t**)malloc (total*sizeof(void*));
    sector = sectors;
    for (i=0 ; i<numsectors ; i++, sector++)
    {
	M_ClearBox (bbox);
	sector->lines = linebuffer;
	li = lines;
	for (j=0 ; j<numlines ; j++, li++)
	{
	    if (li->frontsector == sector || li->backsector == sector)
	    {
		*linebuffer++ = li;
		M_AddToBox (bbox, li->v1->x, li->v1->y);
		M_AddToBox (bbox, li->v2->x, li->v2->y);
	    }
	}
	if (linebuffer - sector->lines != sector->linecount)
	    I_Error ("P_GroupLines: miscounted");
			
	// set the degenmobj_t to the middle of the bounding box
	sector->soundorg.x = (bbox[BOXRIGHT]+bbox[BOXLEFT])/2;
	sector->soundorg.y = (bbox[BOXTOP]+bbox[BOXBOTTOM])/2;
		
	// adjust bounding box to map blocks
	block = (bbox[BOXTOP]-bmaporgy+MAXRADIUS)>>MAPBLOCKSHIFT;
	block = block >= bmapheight ? bmapheight-1 : block;
	sector->blockbox[BOXTOP]=block;

	block = (bbox[BOXBOTTOM]-bmaporgy-MAXRADIUS)>>MAPBLOCKSHIFT;
	block = block < 0 ? 0 : block;
	sector->blockbox[BOXBOTTOM]=block;

	block = (bbox[BOXRIGHT]-bmaporgx+MAXRADIUS)>>MAPBLOCKSHIFT;
	block = block >= bmapwidth ? bmapwidth-1 : block;
	sector->blockbox[BOXRIGHT]=block;

	block = (bbox[BOXLEFT]-bmaporgx-MAXRADIUS)>>MAPBLOCKSHIFT;
	block = block < 0 ? 0 : block;
	sector->blockbox[BOXLEFT]=block;
    }
	
}


//
// P_SetupLevel
//
void
P_SetupLevel
( int		episode,
  int		map,
  int		playermask,
  skill_t	skill)
{
    int		i;
    char	lumpname[9];
    int		lumpnum;
	
    totalkills = totalitems = totalsecret = wminfo.maxfrags = 0;
    wminfo.partime = 180;
    for (i=0 ; i<MAXPLAYERS ; i++)
    {
	players[i].killcount = players[i].secretcount 
	    = players[i].itemcount = 0;
    }

    // Initial height of PointOfView
    // will be set by player think.
    players[consoleplayer].viewz = 1; 

    // Make sure all sounds are stopped
    I_Sound::getInstance()->stopAllSounds();

    //start the music for the level
    int mnum;

    // start new music for the level
    if (Game::gamemode == GameMode_t::commercial)
        mnum = mus_runnin + gamemap - 1;
    else
    {
        int spmus[] =
        {
            // Song - Who? - Where?

            mus_e3m4,	// American	e4m1
            mus_e3m2,	// Romero	e4m2
            mus_e3m3,	// Shawn	e4m3
            mus_e1m5,	// American	e4m4
            mus_e2m7,	// Tim 	e4m5
            mus_e2m4,	// Romero	e4m6
            mus_e2m6,	// J.Anderson	e4m7 CHIRON.WAD
            mus_e2m5,	// Shawn	e4m8
            mus_e1m9	// Tim		e4m9
        };

        if (gameepisode < 4)
            mnum = mus_e1m1 + (gameepisode - 1) * 9 + gamemap - 1;
        else
            mnum = spmus[gamemap - 1];
    }


    I_Sound::getInstance()->playMusic(mnum, true);

    P_InitThinkers ();	
	   
    // find map name
    if (Game::gamemode == GameMode_t::commercial)
    {
	if (map<10)
	    sprintf (lumpname,"map0%i", map);
	else
	    sprintf (lumpname,"map%i", map);
    }
    else
    {
	lumpname[0] = 'E';
	lumpname[1] = '0' + episode;
	lumpname[2] = 'M';
	lumpname[3] = '0' + map;
	lumpname[4] = 0;
    }

    lumpnum = WadManager::getNumForName (lumpname);
	
    leveltime = 0;
	
    // note: most of this ordering is important	
    P_LoadBlockMap (lumpnum+ML_BLOCKMAP);
    P_LoadVertexes (lumpnum+ML_VERTEXES);
    P_LoadSectors (lumpnum+ML_SECTORS);
    P_LoadSideDefs (lumpnum+ML_SIDEDEFS);

    P_LoadLineDefs (lumpnum+ML_LINEDEFS);
    P_LoadSubsectors (lumpnum+ML_SSECTORS);
    P_LoadNodes (lumpnum+ML_NODES);
    P_LoadSegs (lumpnum+ML_SEGS);
	
    rejectmatrix = (unsigned char*)WadManager::getLump (lumpnum+ML_REJECT);
    P_GroupLines ();

    bodyqueslot = 0;
    deathmatch_p = deathmatchstarts;
    P_LoadThings (lumpnum+ML_THINGS);
    
    // if deathmatch, randomly spawn the active players
    if (deathmatch)
    {
	for (i=0 ; i<MAXPLAYERS ; i++)
	    if (playeringame[i])
	    {
		players[i].mo = NULL;
		G_DeathMatchSpawnPlayer (i);
	    }
			
    }

    // clear special respawning que
    iquehead = iquetail = 0;		
	
    // set up world state
    P_SpawnSpecials ();
	
    // build subsector connect matrix
    //	UNUSED P_ConnectSubsectors ();

    // preload graphics
    if (precache)
	R_PrecacheLevel ();

    //printf ("free memory: 0x%x\n", freeMemory());

}



//
// P_Init
//
void P_Init (void)
{
    P_InitSwitchList ();
    P_InitPicAnims ();
    R_InitSprites (sprnames);
}



