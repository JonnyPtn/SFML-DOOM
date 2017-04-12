#pragma once


//
// MISC
//


bool
M_WriteFile
( char const*	name,
  void*		source,
  int		length );

void M_ScreenShot (void);

void M_LoadDefaults (void);

void M_SaveDefaults (void);


int
M_DrawText
( int		x,
  int		y,
  bool	direct,
  char*		string );