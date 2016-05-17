#pragma once

#ifndef __R_LOCAL__
#include "r_local.hpp"
#endif

#define FLOATSPEED		(FRACUNIT*4)


#define MAXHEALTH		100
#define VIEWHEIGHT		(41*FRACUNIT)

// mapblocks are used to check movement
// against lines and things
#define MAPBLOCKUNITS	128
#define MAPBLOCKSIZE	(MAPBLOCKUNITS*FRACUNIT)
#define MAPBLOCKSHIFT	(FRACBITS+7)
#define MAPBMASK		(MAPBLOCKSIZE-1)
#define MAPBTOFRAC		(MAPBLOCKSHIFT-FRACBITS)


// player radius for movement checking
#define PLAYERRADIUS	16*FRACUNIT

// MAXRADIUS is for precalculated sector block boxes
// the spider demon is larger,
// but we do not have any moving sectors nearby
#define MAXRADIUS		32*FRACUNIT

#define GRAVITY		FRACUNIT
#define MAXMOVE		(30*FRACUNIT)

#define USERANGE		(64*FRACUNIT)
#define MELEERANGE		(64*FRACUNIT)
#define MISSILERANGE	(32*64*FRACUNIT)

// follow a player exlusively for 3 seconds
#define	BASETHRESHOLD	 	100



//
// P_TICK
//

// both the head and tail of the thinker list
extern	thinker_t	thinkercap;	


void P_InitThinkers (void);
void P_AddThinker (thinker_t* thinker);
void P_RemoveThinker (thinker_t* thinker);


//
// P_PSPR
//
void P_SetupPsprites (player_t* curplayer);
void P_MovePsprites (player_t* curplayer);
void P_DropWeapon (player_t* player);


//
// P_USER
//
void	P_PlayerThink (player_t* player);


//
// P_MOBJ
//
#define ONFLOORZ		MININT
#define ONCEILINGZ		MAXINT

// Time interval for item respawning.
#define ITEMQUESIZE		128

extern mapthing_t	itemrespawnque[ITEMQUESIZE];
extern int		itemrespawntime[ITEMQUESIZE];
extern int		iquehead;
extern int		iquetail;


void P_RespawnSpecials (void);

mobj_t*
P_SpawnMobj
( int	x,
  int	y,
  int	z,
  mobjtype_t	type );

void 	P_RemoveMobj (mobj_t* th);
bool	P_SetMobjState (mobj_t* mobj, statenum_t state);
void 	P_MobjThinker (mobj_t* mobj);

void	P_SpawnPuff (int x, int y, int z);
void 	P_SpawnBlood (int x, int y, int z, int damage);
mobj_t* P_SpawnMissile (mobj_t* source, mobj_t* dest, mobjtype_t type);
void	P_SpawnPlayerMissile (mobj_t* source, mobjtype_t type);


//
// P_ENEMY
//
void P_NoiseAlert (mobj_t* target, mobj_t* emmiter);


//
// P_MAPUTL
//
typedef struct
{
    int	x;
    int	y;
    int	dx;
    int	dy;
    
} divline_t;

typedef struct
{
    int	frac;		// along trace line
    bool	isaline;
    union {
	mobj_t*	thing;
	line_t*	line;
    }			d;
} intercept_t;

#define MAXINTERCEPTS	128

extern intercept_t	intercepts[MAXINTERCEPTS];
extern intercept_t*	intercept_p;

typedef bool (*traverser_t) (intercept_t *in);

int P_AproxDistance (int dx, int dy);
int 	P_PointOnLineSide (int x, int y, line_t* line);
int 	P_PointOnDivlineSide (int x, int y, divline_t* line);
void 	P_MakeDivline (line_t* li, divline_t* dl);
int P_InterceptVector (divline_t* v2, divline_t* v1);
int 	P_BoxOnLineSide (int* tmbox, line_t* ld);

extern int		opentop;
extern int 		openbottom;
extern int		openrange;
extern int		lowfloor;

void 	P_LineOpening (line_t* linedef);

bool P_BlockLinesIterator (int x, int y, bool(*func)(line_t*) );
bool P_BlockThingsIterator (int x, int y, bool(*func)(mobj_t*) );

#define PT_ADDLINES		1
#define PT_ADDTHINGS	2
#define PT_EARLYOUT		4

extern divline_t	trace;

bool
P_PathTraverse
( int	x1,
  int	y1,
  int	x2,
  int	y2,
  int		flags,
  bool	(*trav) (intercept_t *));

void P_UnsetThingPosition (mobj_t* thing);
void P_SetThingPosition (mobj_t* thing);


//
// P_MAP
//

// If "floatok" true, move would be ok
// if within "tmfloorz - tmceilingz".
extern bool		floatok;
extern int		tmfloorz;
extern int		tmceilingz;


extern	line_t*		ceilingline;

bool P_CheckPosition (mobj_t *thing, int x, int y);
bool P_TryMove (mobj_t* thing, int x, int y);
bool P_TeleportMove (mobj_t* thing, int x, int y);
void	P_SlideMove (mobj_t* mo);
bool P_CheckSight (mobj_t* t1, mobj_t* t2);
void 	P_UseLines (player_t* player);

bool P_ChangeSector (sector_t* sector, bool crunch);

extern mobj_t*	linetarget;	// who got hit (or NULL)

int
P_AimLineAttack
( mobj_t*	t1,
  angle_t	angle,
  int	distance );

void
P_LineAttack
( mobj_t*	t1,
  angle_t	angle,
  int	distance,
  int	slope,
  int		damage );

void
P_RadiusAttack
( mobj_t*	spot,
  mobj_t*	source,
  int		damage );



//
// P_SETUP
//
extern unsigned char*		rejectmatrix;	// for fast sight rejection
extern short*		blockmaplump;	// offsets in blockmap are from here
extern short*		blockmap;
extern int		bmapwidth;
extern int		bmapheight;	// in mapblocks
extern int		bmaporgx;
extern int		bmaporgy;	// origin of block map
extern mobj_t**		blocklinks;	// for thing chains



//
// P_INTER
//
extern int		maxammo[NUMAMMO];
extern int		clipammo[NUMAMMO];

void
P_TouchSpecialThing
( mobj_t*	special,
  mobj_t*	toucher );

void
P_DamageMobj
( mobj_t*	target,
  mobj_t*	inflictor,
  mobj_t*	source,
  int		damage );


//
// P_SPEC
//
#include "p_spec.hpp"