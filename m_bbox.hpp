#pragma once

#include "m_fixed.hpp"


// Bounding box coordinate storage.
enum
{
    BOXTOP,
    BOXBOTTOM,
    BOXLEFT,
    BOXRIGHT
};	// bbox coordinates

// Bounding box functions.
void M_ClearBox (int*	box);

void
M_AddToBox
( int*	box,
  int	x,
  int	y );
