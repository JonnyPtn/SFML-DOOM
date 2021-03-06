#pragma once

extern seg_t*		curline;
extern side_t*		sidedef;
extern line_t*		linedef;
extern sector_t*	frontsector;
extern sector_t*	backsector;

extern int		rw_x;
extern int		rw_stopx;

extern bool		segtextured;

// false if the back side is the same plane
extern bool		markfloor;		
extern bool		markceiling;

extern bool		skymap;

extern drawseg_t	drawsegs[MAXDRAWSEGS];
extern drawseg_t*	ds_p;

extern unsigned char**	hscalelight;
extern unsigned char**	vscalelight;
extern unsigned char**	dscalelight;


typedef void (*drawfunc_t) (int start, int stop);


// BSP?
void R_ClearClipSegs (void);
void R_ClearDrawSegs (void);


void R_RenderBSPNode (int bspnum);