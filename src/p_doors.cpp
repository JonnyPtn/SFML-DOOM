#include "doomdef.hpp"
#include "p_local.hpp"

#include "i_sound.hpp"
#include "d_player.hpp"


// State.
#include "doomstat.hpp"
#include "r_state.hpp"

// Data.
#include "dstrings.hpp"
#include "sounds.hpp"

//
// VERTICAL DOORS
//

//
// T_VerticalDoor
//
void T_VerticalDoor (vldoor_t* door)
{
    result_e	res;
	
    switch(door->direction)
    {
	case static_cast<const int>(Direction::WAITING):
		// WAITING
		if (!--door->topcountdown)
		{
		    switch(door->type)
		    {
            case static_cast<const int>(vldoor_e::blazeRaise):
			door->direction = Direction::DOWN; // time to go back down
            I_Sound::startSound((mobj_t *)&door->sector->soundorg,
				     sfx_bdcls);
			break;
			
		      case static_cast<const int>(vldoor_e::normal):
			door->direction = Direction::DOWN; // time to go back down
            I_Sound::startSound((mobj_t *)&door->sector->soundorg,
				     sfx_dorcls);
			break;
			
		      case static_cast<const int>(vldoor_e::close30ThenOpen):
			door->direction = Direction::UP;
            I_Sound::startSound((mobj_t *)&door->sector->soundorg,
				     sfx_doropn);
			break;
			
		      default:
			break;
		    }
		}
		break;
	case static_cast<const int>(Direction::INITIAL_WAIT):
		//  INITIAL WAIT
		if (!--door->topcountdown)
		{
		    switch(door->type)
		    {
		      case static_cast<const int>(vldoor_e::raiseIn5Mins):
			door->direction = Direction::UP;
			door->type = vldoor_e::normal;
            I_Sound::startSound((mobj_t *)&door->sector->soundorg,
				     sfx_doropn);
			break;
			
		      default:
			break;
		    }
		}
		break;
	case static_cast<const int>(Direction::DOWN):
		// DOWN
		res = T_MovePlane(door->sector,
				  door->speed,
				  door->sector->floorheight,
				  false,1,door->direction);
		if (res == pastdest)
		{
		    switch(door->type)
		    {
		      case static_cast<const int>(vldoor_e::blazeRaise):
		      case static_cast<const int>(vldoor_e::blazeClose):
			door->sector->specialdata = NULL;
			P_RemoveThinker (&door->thinker);  // unlink and free
            I_Sound::startSound((mobj_t *)&door->sector->soundorg,
				     sfx_bdcls);
			break;
			
		      case static_cast<const int>(vldoor_e::normal):
		      case static_cast<const int>(vldoor_e::close):
			door->sector->specialdata = NULL;
			P_RemoveThinker (&door->thinker);  // unlink and free
			break;
			
		      case static_cast<const int>(vldoor_e::close30ThenOpen):
			door->direction = Direction::WAITING;
			door->topcountdown = 35*30;
			break;
			
		      default:
			break;
		    }
		}
		else if (res == crushed)
		{
		    switch(door->type)
		    {
		      case static_cast<const int>(vldoor_e::blazeClose):
		      case static_cast<const int>(vldoor_e::close):		// DO NOT GO BACK UP!
			break;
			
		      default:
			door->direction = Direction::UP;
            I_Sound::startSound((mobj_t *)&door->sector->soundorg,
				     sfx_doropn);
			break;
		    }
		}
		break;
	
	case static_cast<const int>(Direction::UP):
		// UP
		res = T_MovePlane(door->sector,
				  door->speed,
				  door->topheight,
				  false,1,door->direction);
		
		if (res == pastdest)
		{
		    switch(door->type)
		    {
		      case static_cast<const int>(vldoor_e::blazeRaise):
		      case static_cast<const int>(vldoor_e::normal):
			door->direction = Direction::WAITING; // wait at top
			door->topcountdown = door->topwait;
			break;
			
		      case static_cast<const int>(vldoor_e::close30ThenOpen):
		      case static_cast<const int>(vldoor_e::blazeOpen):
		      case static_cast<const int>(vldoor_e::open):
			door->sector->specialdata = NULL;
			P_RemoveThinker (&door->thinker);  // unlink and free
			break;
			
		      default:
			break;
		    }
		}
		break;
    }
}


//
// EV_DoLockedDoor
// Move a locked door up/down
//

int
EV_DoLockedDoor
( line_t*	line,
  vldoor_e	type,
  mobj_t*	thing )
{
    player_t*	p;
	
    p = thing->player;
	
    if (!p)
	return 0;
		
    switch(line->special)
    {
      case 99:	// Blue Lock
      case 133:
	if ( !p )
	    return 0;
	if (!p->cards[it_bluecard] && !p->cards[it_blueskull])
	{
	    p->message = PD_BLUEO;
        I_Sound::startSound(NULL,sfx_oof);
	    return 0;
	}
	break;
	
      case 134: // Red Lock
      case 135:
	if ( !p )
	    return 0;
	if (!p->cards[it_redcard] && !p->cards[it_redskull])
	{
	    p->message = PD_REDO;
        I_Sound::startSound(NULL,sfx_oof);
	    return 0;
	}
	break;
	
      case 136:	// Yellow Lock
      case 137:
	if ( !p )
	    return 0;
	if (!p->cards[it_yellowcard] &&
	    !p->cards[it_yellowskull])
	{
	    p->message = PD_YELLOWO;
        I_Sound::startSound(NULL,sfx_oof);
	    return 0;
	}
	break;	
    }

    return EV_DoDoor(line,type);
}


int
EV_DoDoor
( line_t*	line,
  vldoor_e	type )
{
    int		secnum,rtn;
    sector_t*	sec;
    vldoor_t*	door;
	
    secnum = -1;
    rtn = 0;
    
    while ((secnum = P_FindSectorFromLineTag(line,secnum)) >= 0)
    {
	sec = &sectors[secnum];
	if (sec->specialdata)
	    continue;
		
	
	// new door thinker
	rtn = 1;
	door = (vldoor_t*)malloc (sizeof(*door));
	P_AddThinker (&door->thinker);
	sec->specialdata = door;

	door->thinker.function.acp1 = (actionf_p1) T_VerticalDoor;
	door->sector = sec;
	door->type = type;
	door->topwait = VDOORWAIT;
	door->speed = VDOORSPEED;
		
	switch(type)
	{
    case static_cast<const int>(vldoor_e::blazeClose):
	    door->topheight = P_FindLowestCeilingSurrounding(sec);
	    door->topheight -= 4*FRACUNIT;
	    door->direction = Direction::DOWN;
	    door->speed = VDOORSPEED * 4;
        I_Sound::startSound((mobj_t *)&door->sector->soundorg,
			 sfx_bdcls);
	    break;
	    
	  case static_cast<const int>(vldoor_e::close):
	    door->topheight = P_FindLowestCeilingSurrounding(sec);
	    door->topheight -= 4*FRACUNIT;
	    door->direction = Direction::DOWN;
        I_Sound::startSound((mobj_t *)&door->sector->soundorg,
			 sfx_dorcls);
	    break;
	    
	  case static_cast<const int>(vldoor_e::close30ThenOpen):
	    door->topheight = sec->ceilingheight;
	    door->direction = Direction::DOWN;
        I_Sound::startSound((mobj_t *)&door->sector->soundorg,
			 sfx_dorcls);
	    break;
	    
	  case static_cast<const int>(vldoor_e::blazeRaise):
	  case static_cast<const int>(vldoor_e::blazeOpen):
	    door->direction = Direction::UP;
	    door->topheight = P_FindLowestCeilingSurrounding(sec);
	    door->topheight -= 4*FRACUNIT;
	    door->speed = VDOORSPEED * 4;
	    if (door->topheight != sec->ceilingheight)
            I_Sound::startSound((mobj_t *)&door->sector->soundorg,
			     sfx_bdopn);
	    break;
	    
	  case static_cast<const int>(vldoor_e::normal):
	  case static_cast<const int>(vldoor_e::open):
	    door->direction = Direction::UP;
	    door->topheight = P_FindLowestCeilingSurrounding(sec);
	    door->topheight -= 4*FRACUNIT;
	    if (door->topheight != sec->ceilingheight)
            I_Sound::startSound((mobj_t *)&door->sector->soundorg,
			     sfx_doropn);
	    break;
	    
	  default:
	    break;
	}
		
    }
    return rtn;
}


//
// EV_VerticalDoor : open a door manually, no tag value
//
void
EV_VerticalDoor
( line_t*	line,
  mobj_t*	thing )
{
    player_t*	player;
    int		secnum;
    sector_t*	sec;
    vldoor_t*	door;
    int		side;
	
    side = 0;	// only front sides can be used

    //	Check for locks
    player = thing->player;
		
    switch(line->special)
    {
      case 26: // Blue Lock
      case 32:
	if ( !player )
	    return;
	
	if (!player->cards[it_bluecard] && !player->cards[it_blueskull])
	{
	    player->message = PD_BLUEK;
        I_Sound::startSound(NULL,sfx_oof);
	    return;
	}
	break;
	
      case 27: // Yellow Lock
      case 34:
	if ( !player )
	    return;
	
	if (!player->cards[it_yellowcard] &&
	    !player->cards[it_yellowskull])
	{
	    player->message = PD_YELLOWK;
        I_Sound::startSound(NULL,sfx_oof);
	    return;
	}
	break;
	
      case 28: // Red Lock
      case 33:
	if ( !player )
	    return;
	
	if (!player->cards[it_redcard] && !player->cards[it_redskull])
	{
	    player->message = PD_REDK;
        I_Sound::startSound(NULL,sfx_oof);
	    return;
	}
	break;
    }
	
    // if the sector has an active thinker, use it
    sec = sides[ line->sidenum[side^1]] .sector;
    secnum = sec-sectors;

    if (sec->specialdata)
    {
	door = (vldoor_t*)sec->specialdata;
	switch(line->special)
	{
	  case	1: // ONLY FOR "RAISE" DOORS, NOT "OPEN"s
	  case	26:
	  case	27:
	  case	28:
	  case	117:
	    if (door->direction == Direction::DOWN)
		door->direction = Direction::UP;	// go back up
	    else
	    {
		if (!thing->player)
		    return;		// JDC: bad guys never close doors
		
		door->direction = Direction::DOWN;	// start going down immediately
	    }
	    return;
	}
    }
	
    // for proper sound
    switch(line->special)
    {
      case 117:	// BLAZING DOOR RAISE
      case 118:	// BLAZING DOOR OPEN
          I_Sound::startSound((mobj_t *)&sec->soundorg,sfx_bdopn);
	break;
	
      case 1:	// NORMAL DOOR SOUND
      case 31:
          I_Sound::startSound((mobj_t *)&sec->soundorg,sfx_doropn);
	break;
	
      default:	// LOCKED DOOR SOUND
          I_Sound::startSound((mobj_t *)&sec->soundorg,sfx_doropn);
	break;
    }
	
    
    // new door thinker
    door = (vldoor_t*)malloc (sizeof(*door));
    P_AddThinker (&door->thinker);
    sec->specialdata = door;
    door->thinker.function.acp1 = (actionf_p1) T_VerticalDoor;
    door->sector = sec;
    door->direction = Direction::UP;
    door->speed = VDOORSPEED;
    door->topwait = VDOORWAIT;

    switch(line->special)
    {
      case 1:
      case 26:
      case 27:
      case 28:
	door->type = vldoor_e::normal;
	break;
	
      case 31:
      case 32:
      case 33:
      case 34:
	door->type = vldoor_e::open;
	line->special = 0;
	break;
	
      case 117:	// blazing door raise
	door->type = vldoor_e::blazeRaise;
	door->speed = VDOORSPEED*4;
	break;
      case 118:	// blazing door open
	door->type = vldoor_e::blazeOpen;
	line->special = 0;
	door->speed = VDOORSPEED*4;
	break;
    }
    
    // find the top and bottom of the movement range
    door->topheight = P_FindLowestCeilingSurrounding(sec);
    door->topheight -= 4*FRACUNIT;
}


//
// Spawn a door that closes after 30 seconds
//
void P_SpawnDoorCloseIn30 (sector_t* sec)
{
    vldoor_t*	door;
	
    door = (vldoor_t*)malloc ( sizeof(*door));

    P_AddThinker (&door->thinker);

    sec->specialdata = door;
    sec->special = 0;

    door->thinker.function.acp1 = (actionf_p1)T_VerticalDoor;
    door->sector = sec;
    door->direction = Direction::WAITING;
    door->type = vldoor_e::normal;
    door->speed = VDOORSPEED;
    door->topcountdown = 30 * 35;
}

//
// Spawn a door that opens after 5 minutes
//
void
P_SpawnDoorRaiseIn5Mins
( sector_t*	sec,
  int		secnum )
{
    vldoor_t*	door;
	
    door = (vldoor_t*)malloc ( sizeof(*door));
    
    P_AddThinker (&door->thinker);

    sec->specialdata = door;
    sec->special = 0;

    door->thinker.function.acp1 = (actionf_p1)T_VerticalDoor;
    door->sector = sec;
    door->direction = Direction::INITIAL_WAIT;
    door->type = vldoor_e::raiseIn5Mins;
    door->speed = VDOORSPEED;
    door->topheight = P_FindLowestCeilingSurrounding(sec);
    door->topheight -= 4*FRACUNIT;
    door->topwait = VDOORWAIT;
    door->topcountdown = 5 * 60 * 35;
}



// UNUSED
// Separate into p_slidoor.c?

#if 0		// ABANDONED TO THE MISTS OF TIME!!!
//
// EV_SlidingDoor : slide a door horizontally
// (animate midtexture, then set noblocking line)
//


slideframe_t slideFrames[MAXSLIDEDOORS];

void P_InitSlidingDoorFrames(void)
{
    int		i;
    int		f1;
    int		f2;
    int		f3;
    int		f4;
	
    // DOOM II ONLY...
    if ( gamemode != commercial)
	return;
	
    for (i = 0;i < MAXSLIDEDOORS; i++)
    {
	if (!slideFrameNames[i].frontFrame1[0])
	    break;
			
	f1 = R_TextureNumForName(slideFrameNames[i].frontFrame1);
	f2 = R_TextureNumForName(slideFrameNames[i].frontFrame2);
	f3 = R_TextureNumForName(slideFrameNames[i].frontFrame3);
	f4 = R_TextureNumForName(slideFrameNames[i].frontFrame4);

	slideFrames[i].frontFrames[0] = f1;
	slideFrames[i].frontFrames[1] = f2;
	slideFrames[i].frontFrames[2] = f3;
	slideFrames[i].frontFrames[3] = f4;
		
	f1 = R_TextureNumForName(slideFrameNames[i].backFrame1);
	f2 = R_TextureNumForName(slideFrameNames[i].backFrame2);
	f3 = R_TextureNumForName(slideFrameNames[i].backFrame3);
	f4 = R_TextureNumForName(slideFrameNames[i].backFrame4);

	slideFrames[i].backFrames[0] = f1;
	slideFrames[i].backFrames[1] = f2;
	slideFrames[i].backFrames[2] = f3;
	slideFrames[i].backFrames[3] = f4;
    }
}


//
// Return index into "slideFrames" array
// for which door type to use
//
int P_FindSlidingDoorType(line_t*	line)
{
    int		i;
    int		val;
	
    for (i = 0;i < MAXSLIDEDOORS;i++)
    {
	val = sides[line->sidenum[0]].midtexture;
	if (val == slideFrames[i].frontFrames[0])
	    return i;
    }
	
    return -1;
}

void T_SlidingDoor (slidedoor_t*	door)
{
    switch(door->status)
    {
      case sd_opening:
	if (!door->timer--)
	{
	    if (++door->frame == SNUMFRAMES)
	    {
		// IF DOOR IS DONE OPENING...
		sides[door->line->sidenum[0]].midtexture = 0;
		sides[door->line->sidenum[1]].midtexture = 0;
		door->line->flags &= ML_BLOCKING^0xff;
					
		if (door->type == sdt_openOnly)
		{
		    door->frontsector->specialdata = NULL;
		    P_RemoveThinker (&door->thinker);
		    break;
		}
					
		door->timer = SDOORWAIT;
		door->status = sd_waiting;
	    }
	    else
	    {
		// IF DOOR NEEDS TO ANIMATE TO NEXT FRAME...
		door->timer = SWAITTICS;
					
		sides[door->line->sidenum[0]].midtexture =
		    slideFrames[door->whichDoorIndex].
		    frontFrames[door->frame];
		sides[door->line->sidenum[1]].midtexture =
		    slideFrames[door->whichDoorIndex].
		    backFrames[door->frame];
	    }
	}
	break;
			
      case sd_waiting:
	// IF DOOR IS DONE WAITING...
	if (!door->timer--)
	{
	    // CAN DOOR CLOSE?
	    if (door->frontsector->thinglist != NULL ||
		door->backsector->thinglist != NULL)
	    {
		door->timer = SDOORWAIT;
		break;
	    }

	    //door->frame = SNUMFRAMES-1;
	    door->status = sd_closing;
	    door->timer = SWAITTICS;
	}
	break;
			
      case sd_closing:
	if (!door->timer--)
	{
	    if (--door->frame < 0)
	    {
		// IF DOOR IS DONE CLOSING...
		door->line->flags |= ML_BLOCKING;
		door->frontsector->specialdata = NULL;
		P_RemoveThinker (&door->thinker);
		break;
	    }
	    else
	    {
		// IF DOOR NEEDS TO ANIMATE TO NEXT FRAME...
		door->timer = SWAITTICS;
					
		sides[door->line->sidenum[0]].midtexture =
		    slideFrames[door->whichDoorIndex].
		    frontFrames[door->frame];
		sides[door->line->sidenum[1]].midtexture =
		    slideFrames[door->whichDoorIndex].
		    backFrames[door->frame];
	    }
	}
	break;
    }
}



void
EV_SlidingDoor
( line_t*	line,
  mobj_t*	thing )
{
    sector_t*		sec;
    slidedoor_t*	door;
	
    // DOOM II ONLY...
    if (gamemode != commercial)
	return;
    
    // Make sure door isn't already being animated
    sec = line->frontsector;
    door = NULL;
    if (sec->specialdata)
    {
	if (!thing->player)
	    return;
			
	door = sec->specialdata;
	if (door->type == sdt_openAndClose)
	{
	    if (door->status == sd_waiting)
		door->status = sd_closing;
	}
	else
	    return;
    }
    
    // Init sliding door vars
    if (!door)
    {
	door = malloc (sizeof(*door));
	P_AddThinker (&door->thinker);
	sec->specialdata = door;
		
	door->type = sdt_openAndClose;
	door->status = sd_opening;
	door->whichDoorIndex = P_FindSlidingDoorType(line);

	if (door->whichDoorIndex < 0)
	    I_Error("EV_SlidingDoor: Can't use texture for sliding door!");
			
	door->frontsector = sec;
	door->backsector = line->backsector;
	door->thinker.function = T_SlidingDoor;
	door->timer = SWAITTICS;
	door->frame = 0;
	door->line = line;
    }
}
#endif
