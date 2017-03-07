#pragma once

extern unsigned char*	dc_colormap;
extern int		dc_x;
extern int		dc_yl;
extern int		dc_yh;
extern int		dc_iscale;
extern int		dc_texturemid;

// first pixel in a column
extern unsigned char*		dc_source;		


// The span blitting interface.
// Hook in assembler or system specific BLT
//  here.
void 	R_DrawColumn (void);
void 	R_DrawColumnLow (void);

// The Spectre/Invisibility effect.
void 	R_DrawFuzzColumn (void);
void 	R_DrawFuzzColumnLow (void);

// Draw with color translation tables,
//  for player sprite rendering,
//  Green/Red/Blue/Indigo shirts.
void	R_DrawTranslatedColumn (void);
void	R_DrawTranslatedColumnLow (void);

void
R_VideoErase
( unsigned	ofs,
  int		count );

extern int		ds_y;
extern int		ds_x1;
extern int		ds_x2;

extern unsigned char*	ds_colormap;

extern int		ds_xfrac;
extern int		ds_yfrac;
extern int		ds_xstep;
extern int		ds_ystep;

// start of a 64*64 tile image
extern unsigned char*		ds_source;		

extern unsigned char*		translationtables;
extern unsigned char*		dc_translation;


// Span blitting for rows, floor/ceiling.
// No Sepctre effect needed.
void 	R_DrawSpan (void);

// Low resolution mode, 160x200?
void 	R_DrawSpanLow (void);


void
R_InitBuffer
( int		width,
  int		height );


// Initialize color translation tables,
//  for player rendering etc.
void	R_InitTranslationTables (void);



// Rendering function.
void R_FillBackScreen (void);

// If the view size is not full screen, draws a border around it.
void R_DrawViewBorder (void);