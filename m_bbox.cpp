
#include "m_bbox.hpp"
#include <limits>
void M_ClearBox (fixed_t *box)
{
    box[BOXTOP] = box[BOXRIGHT] = std::numeric_limits<int>::min();
	box[BOXBOTTOM] = box[BOXLEFT] = std::numeric_limits<int>::max();
}

void
M_AddToBox
( fixed_t*	box,
  fixed_t	x,
  fixed_t	y )
{
    if (x<box[BOXLEFT])
	box[BOXLEFT] = x;
    else if (x>box[BOXRIGHT])
	box[BOXRIGHT] = x;
    if (y<box[BOXBOTTOM])
	box[BOXBOTTOM] = y;
    else if (y>box[BOXTOP])
	box[BOXTOP] = y;
}





