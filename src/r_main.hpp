#pragma once

#include "d_player.hpp"
#include "r_data.hpp"

//
// POV related.
//
extern int		viewcos;
extern int		viewsin;

extern int		viewwidth;
extern int		viewheight;
extern int		viewwindowx;
extern int		viewwindowy;



extern int		centerx;
extern int		centery;

extern int		centerxfrac;
extern int		centeryfrac;
extern int		projection;

extern int		validcount;

extern int		linecount;
extern int		loopcount;


//
// Lighting LUT.
// Used for z-depth cuing per column/row,
//  and other lighting effects (sector ambient, flash).
//

// Lighting constants.
// Now why not 32 levels here?
#define LIGHTLEVELS	        16
#define LIGHTSEGSHIFT	         4

#define MAXLIGHTSCALE		48
#define LIGHTSCALESHIFT		12
#define MAXLIGHTZ	       128
#define LIGHTZSHIFT		20

extern unsigned char*	scalelight[LIGHTLEVELS][MAXLIGHTSCALE];
extern unsigned char*	scalelightfixed[MAXLIGHTSCALE];
extern unsigned char*	zlight[LIGHTLEVELS][MAXLIGHTZ];

extern int		extralight;
extern unsigned char*	fixedcolormap;


// Number of diminishing brightness levels.
// There a 0-31, i.e. 32 LUT in the COLORMAP lump.
#define NUMCOLORMAPS		32


// Blocky/low detail mode.
//B remove this?
//  0 = high, 1 = low
extern	int		detailshift;	


//
// Function pointers to switch refresh/drawing functions.
// Used to select shadow mode etc.
//
extern void		(*colfunc) (void);
extern void		(*basecolfunc) (void);
extern void		(*fuzzcolfunc) (void);
// No shadow effects on floors.
extern void		(*spanfunc) (void);


//
// Utility functions.
int
R_PointOnSide
( int	x,
  int	y,
  node_t*	node );

int
R_PointOnSegSide
( int	x,
  int	y,
  seg_t*	line );

angle_t
R_PointToAngle
( int	x,
  int	y );

angle_t
R_PointToAngle2
( int	x1,
  int	y1,
  int	x2,
  int	y2 );

int
R_PointToDist
( int	x,
  int	y );


int R_ScaleFromGlobalAngle (angle_t visangle);

subsector_t*
R_PointInSubsector
( int	x,
  int	y );

void
R_AddPointToBox
( int		x,
  int		y,
  int*	box );



//
// REFRESH - the actual rendering functions.
//

// Called by G_Drawer.
void R_RenderPlayerView (player_t *player);

// Called by startup code.
void R_Init (void);

// Called by M_Responder.
void R_SetViewSize (int blocks, int detail);