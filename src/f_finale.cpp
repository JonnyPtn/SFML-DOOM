#include <ctype.h>

// Functions.
#include "i_system.hpp"
#include "v_video.hpp"
#include "w_wad.hpp"
#include "i_sound.hpp"
#include "i_sound.hpp"

// Data.
#include "dstrings.hpp"
#include "sounds.hpp"

#include "doomstat.hpp"
#include "r_state.hpp"
#include "g_game.hpp"

// Stage of animation:
//  0 = text, 1 = art screen, 2 = character cast
int		finalestage;

int		finalecount;

#define	TEXTSPEED	3
#define	TEXTWAIT	250

const char*	finaletext;
const char*	finaleflat;

void	    F_StartCast (void);
void	    F_CastTicker (void);
bool        F_CastResponder (sf::Event *ev);
void	    F_CastDrawer (void);

//
// F_StartFinale
//
void F_StartFinale (void)
{
    gameaction = ga_nothing;
    gamestate = GS_FINALE;
    viewactive = false;
    automapactive = false;

    // Okay - IWAD dependend stuff.
    // This has been changed severly, and
    //  some stuff might have changed in the process.
    switch (Game::gamemode )
    {

      // DOOM 1 - E1, E3 or E4, but each nine missions
      case GameMode_t::shareware:
      case GameMode_t::registered:
      case GameMode_t::retail:
      {
          I_Sound::playMusic(mus_victor, true);
		
		switch (gameepisode)
		{
		  case 1:
		    finaleflat = "FLOOR4_8";
		    finaletext = s_E1M1.c_str();
		    break;
		  case 2:
		    finaleflat = "SFLR6_1";
		    finaletext = s_E1M2.c_str();
		    break;
		  case 3:
		    finaleflat = "MFLR8_4";
		    finaletext = s_E1M3.c_str();
		    break;
		  case 4:
		    finaleflat = "MFLR8_3";
		    finaletext = s_E1M4.c_str();
		    break;
		  default:
		    // Ouch.
		    break;
		}
		break;
      }
      
      // DOOM II and missions packs with E1, M34
      case GameMode_t::commercial:
      {
          I_Sound::playMusic(mus_read_m, true);

		switch (gamemap)
		{
		  case 6:
		    finaleflat = "SLIME16";
		    finaletext = s_C1Text.c_str();
		    break;
		  case 11:
		    finaleflat = "RROCK14";
		    finaletext = s_C2Text.c_str();
		    break;
		  case 20:
		    finaleflat = "RROCK07";
		    finaletext = s_C3Text.c_str();
		    break;
		  case 30:
		    finaleflat = "RROCK17";
		    finaletext = s_C4Text.c_str();
		    break;
		  case 15:
		    finaleflat = "RROCK13";
		    finaletext = s_C5Text.c_str();
		    break;
		  case 31:
		    finaleflat = "RROCK19";
		    finaletext = s_C6Text.c_str();
		    break;
		  default:
		    // Ouch.
		    break;
		}
		break;
      }	

      // Indeterminate.
      default:
          I_Sound::playMusic(mus_read_m, true);
		finaleflat = "F_SKY1"; // Not used anywhere else.
		finaletext = s_C1Text.c_str();  // FIXME - other text, music?
		break;
    }
    
    finalestage = 0;
    finalecount = 0;
}

bool F_Responder (sf::Event *event)
{
    if (finalestage == 2)
		return F_CastResponder (event);
	return false;
}

//
// F_Ticker
//
void F_Ticker (void)
{
    int		i;
    
    // check for skipping
    if ( (Game::gamemode == GameMode_t::commercial) && ( finalecount > 50) )
    {
      // go on to the next level
      for (i=0 ; i<MAXPLAYERS ; i++)
		if (players[i].cmd.buttons)
			break;
				
      if (i < MAXPLAYERS)
      {	
		if (gamemap == 30)
			F_StartCast ();
		else
			gameaction = ga_worlddone;
      }
    }
    
    // advance animation
    finalecount++;
	
    if (finalestage == 2)
    {
		F_CastTicker ();
		return;
    }
	
    if (Game::gamemode == GameMode_t::commercial)
		return;
		
    if (!finalestage && static_cast<unsigned int>(finalecount)>strlen (finaletext)*TEXTSPEED + TEXTWAIT)
    {
		finalecount = 0;
		finalestage = 1;
		wipegamestate = (gamestate_t)-1;		// force a wipe
		if (gameepisode == 3)
            I_Sound::playMusic(mus_bunny);
    }
}

//
// F_TextWrite
//

#include "hu_stuff.hpp"
extern	patch_t *hu_font[HU_FONTSIZE];

void F_TextWrite (void)
{
    unsigned char*	src;
    unsigned char*	dest;
    
    int		x,y,w;
    int		count;
    const char*	ch;
    int		c;
    int		cx;
    int		cy;
    
    // erase the entire screen to a tiled background
    src = (unsigned char*)WadManager::getLump ( finaleflat);
    dest = screens[0];
	
    for (y=0 ; y<SCREENHEIGHT ; y++)
    {
		for (x=0 ; x<SCREENWIDTH/64 ; x++)
		{
		    memcpy (dest, src+((y&63)<<6), 64);
		    dest += 64;
		}
		if (SCREENWIDTH&63)
		{
		    memcpy (dest, src+((y&63)<<6), SCREENWIDTH&63);
		    dest += (SCREENWIDTH&63);
		}
    }

    V_MarkRect (0, 0, SCREENWIDTH, SCREENHEIGHT);
    
    // draw some of the text onto the screen
    cx = 10;
    cy = 10;
    ch = finaletext;
	
    count = (finalecount - 10)/TEXTSPEED;
    if (count < 0)
		count = 0;
    for ( ; count ; count-- )
    {
		c = *ch++;
		if (!c)
		    break;
		if (c == '\n')
		{
		    cx = 10;
		    cy += 11;
		    continue;
		}
			
		c = toupper(c) - HU_FONTSTART;
		if (c < 0 || c> HU_FONTSIZE)
		{
		    cx += 4;
		    continue;
		}
			
		w = hu_font[c]->width;
		if (cx+w > SCREENWIDTH)
		    break;
		V_DrawPatch(cx, cy, 0, hu_font[c]);
		cx+=w;
    }
}

//
// Final DOOM 2 animation
// Casting by id Software.
//   in order of appearance
//
typedef struct
{
    const char		*name;
    mobjtype_t	type;
} castinfo_t;

castinfo_t	castorder[] = {
    {s_Zombie.c_str(), MT_POSSESSED},
    {s_ShotgunGuy.c_str(), MT_SHOTGUY},
    {s_HeavyDude.c_str(), MT_CHAINGUY},
    {s_Imp.c_str(), MT_TROOP},
    {s_Demon.c_str(), MT_SERGEANT},
    {s_LostSoul.c_str(), MT_SKULL},
    {s_CacoDemon.c_str(), MT_HEAD},
    {s_HellKnight.c_str(), MT_KNIGHT},
    {s_Baron.c_str(), MT_BRUISER},
    {s_Arach.c_str(), MT_BABY},
    {s_PainElemental.c_str(), MT_PAIN},
    {s_Revenant.c_str(), MT_UNDEAD},
    {s_Mancubus.c_str(), MT_FATSO},
    {s_ArchVile.c_str(), MT_VILE},
    {s_Spider.c_str(), MT_SPIDER},
    {s_CyberDemon.c_str(), MT_CYBORG},
    {s_Hero.c_str(), MT_PLAYER},

    {NULL,(mobjtype_t)0}
};

int		castnum;
int		casttics;
state_t*	caststate;
bool		castdeath;
int		castframes;
int		castonmelee;
bool		castattacking;


//
// F_StartCast
//
extern	gamestate_t     wipegamestate;


void F_StartCast (void)
{
    wipegamestate = (gamestate_t)-1;		// force a screen wipe
    castnum = 0;
    caststate = &states[mobjinfo[castorder[castnum].type].seestate];
    casttics = caststate->tics;
    castdeath = false;
    finalestage = 2;	
    castframes = 0;
    castonmelee = 0;
    castattacking = false;
    I_Sound::playMusic(mus_evil, true);
}

//
// F_CastTicker
//
void F_CastTicker (void)
{
    int		st;
    int		sfx;
	
    if (--casttics > 0)
		return;			// not time to change state yet
		
    if (caststate->tics == -1 || caststate->nextstate == S_NULL)
    {
		// switch from deathstate to next monster
		castnum++;
		castdeath = false;
		if (castorder[castnum].name == NULL)
		    castnum = 0;
		if (mobjinfo[castorder[castnum].type].seesound)
		    I_Sound::startSound (NULL, mobjinfo[castorder[castnum].type].seesound);
		caststate = &states[mobjinfo[castorder[castnum].type].seestate];
		castframes = 0;
    }
    else
    {
	// just advance to next state in animation
	if (caststate == &states[S_PLAY_ATK1])
	    goto stopattack;	// Oh, gross hack!
	st = caststate->nextstate;
	caststate = &states[st];
	castframes++;
	
	// sound hacks....
	switch (st)
	{
	  case S_PLAY_ATK1:	sfx = sfx_dshtgn; break;
	  case S_POSS_ATK2:	sfx = sfx_pistol; break;
	  case S_SPOS_ATK2:	sfx = sfx_shotgn; break;
	  case S_VILE_ATK2:	sfx = sfx_vilatk; break;
	  case S_SKEL_FIST2:	sfx = sfx_skeswg; break;
	  case S_SKEL_FIST4:	sfx = sfx_skepch; break;
	  case S_SKEL_MISS2:	sfx = sfx_skeatk; break;
	  case S_FATT_ATK8:
	  case S_FATT_ATK5:
	  case S_FATT_ATK2:	sfx = sfx_firsht; break;
	  case S_CPOS_ATK2:
	  case S_CPOS_ATK3:
	  case S_CPOS_ATK4:	sfx = sfx_shotgn; break;
	  case S_TROO_ATK3:	sfx = sfx_claw; break;
	  case S_SARG_ATK2:	sfx = sfx_sgtatk; break;
	  case S_BOSS_ATK2:
	  case S_BOS2_ATK2:
	  case S_HEAD_ATK2:	sfx = sfx_firsht; break;
	  case S_SKULL_ATK2:	sfx = sfx_sklatk; break;
	  case S_SPID_ATK2:
	  case S_SPID_ATK3:	sfx = sfx_shotgn; break;
	  case S_BSPI_ATK2:	sfx = sfx_plasma; break;
	  case S_CYBER_ATK2:
	  case S_CYBER_ATK4:
	  case S_CYBER_ATK6:	sfx = sfx_rlaunc; break;
	  case S_PAIN_ATK3:	sfx = sfx_sklatk; break;
	  default: sfx = 0; break;
	}
		
	if (sfx)
        I_Sound::startSound(NULL, sfx);
    }
	
    if (castframes == 12)
    {
		// go into attack frame
		castattacking = true;
		if (castonmelee)
		    caststate=&states[mobjinfo[castorder[castnum].type].meleestate];
		else
		    caststate=&states[mobjinfo[castorder[castnum].type].missilestate];
		castonmelee ^= 1;
		if (caststate == &states[S_NULL])
		{
		    if (castonmelee)
			caststate=
			    &states[mobjinfo[castorder[castnum].type].meleestate];
		    else
			caststate=
			    &states[mobjinfo[castorder[castnum].type].missilestate];
		}
    }
	
    if (castattacking)
    {
		if (castframes == 24 ||	caststate == &states[mobjinfo[castorder[castnum].type].seestate] )
		{
		  stopattack:
		    castattacking = false;
		    castframes = 0;
		    caststate = &states[mobjinfo[castorder[castnum].type].seestate];
		}
    }
	
    casttics = caststate->tics;
    if (casttics == -1)
		casttics = 15;
}

//
// F_CastResponder
//

bool F_CastResponder (sf::Event* ev)
{
    if (ev->type != sf::Event::KeyPressed)
		return false;
		
    if (castdeath)
		return true;			// already in dying frames
		
    // go into death frame
    castdeath = true;
    caststate = &states[mobjinfo[castorder[castnum].type].deathstate];
    casttics = caststate->tics;
    castframes = 0;
    castattacking = false;
    if (mobjinfo[castorder[castnum].type].deathsound)
        I_Sound::startSound(NULL, mobjinfo[castorder[castnum].type].deathsound);
	
    return true;
}


void F_CastPrint (const char* text)
{
    const char*	ch;
    int		c;
    int		cx;
    int		w;
    int		width;
    
    // find width
    ch = text;
    width = 0;
	
    while (ch)
    {
		c = *ch++;
		if (!c)
		    break;
		c = toupper(c) - HU_FONTSTART;
		if (c < 0 || c> HU_FONTSIZE)
		{
		    width += 4;
		    continue;
		}
			
		w = hu_font[c]->width;
		width += w;
    }
    
    // draw it
    cx = 160-width/2;
    ch = text;
    while (ch)
    {
		c = *ch++;
		if (!c)
		    break;
		c = toupper(c) - HU_FONTSTART;
		if (c < 0 || c> HU_FONTSIZE)
		{
		    cx += 4;
		    continue;
		}
			
		w = hu_font[c]->width;
		V_DrawPatch(cx, 180, 0, hu_font[c]);
		cx+=w;
    }
}

//
// F_CastDrawer
//
void V_DrawPatchFlipped (int x, int y, int scrn, patch_t *patch);

void F_CastDrawer (void)
{
    spritedef_t*	sprdef;
    spriteframe_t*	sprframe;
    int			lump;
    bool		flip;
    patch_t*		patch;
    
    // erase the entire screen to a background
    V_DrawPatch (0,0,0, (patch_t*)WadManager::getLump ("BOSSBACK"));

    F_CastPrint (castorder[castnum].name);
    
    // draw the current frame in the middle of the screen
    sprdef = &sprites[caststate->sprite];
    sprframe = &sprdef->spriteframes[ caststate->frame & FF_FRAMEMASK];
    lump = sprframe->lump[0];
    flip = (sprframe->flip[0])!=0;
			
    patch = (patch_t*)WadManager::WadManager::getLump (lump+firstspritelump);
    if (flip)
		V_DrawPatchFlipped (160,170,0,patch);
    else
		V_DrawPatch (160,170,0,patch);
}

//
// F_DrawPatchCol
//
void
F_DrawPatchCol( int x, patch_t*	patch, int col )
{
    column_t*	column;
    unsigned char*	source;
    unsigned char*	dest;
    unsigned char*	desttop;
    int		count;
	
    column = (column_t *)((unsigned char *)patch + patch->columnofs[col]);
    desttop = screens[0]+x;

    // step through the posts in a column
    while (column->topdelta != 0xff )
    {
		source = (unsigned char *)column + 3;
		dest = desttop + column->topdelta*SCREENWIDTH;
		count = column->length;
			
		while (count--)
		{
		    *dest = *source++;
		    dest += SCREENWIDTH;
		}
		column = (column_t *)(  (unsigned char *)column + column->length + 4 );
    }
}

//
// F_BunnyScroll
//
void F_BunnyScroll (void)
{
    int		scrolled;
    int		x;
    patch_t*	p1;
	patch_t*	p2;
    int		stage;
    static int	laststage;
		
    p1 = (patch_t*)WadManager::getLump ("PFUB2");
    p2 = (patch_t*)WadManager::getLump ("PFUB1");

    V_MarkRect (0, 0, SCREENWIDTH, SCREENHEIGHT);
	
    scrolled = SCREENWIDTH - (finalecount-230)/2;
    if (scrolled > SCREENWIDTH)
		scrolled = SCREENWIDTH;
    if (scrolled < 0)
		scrolled = 0;
		
    for ( x=0 ; x<SCREENWIDTH ; x++)
    {
		if (x+scrolled < SCREENWIDTH)
		    F_DrawPatchCol (x, p1, x+scrolled);
		else
		    F_DrawPatchCol (x, p2, x+scrolled - SCREENWIDTH);
    }
	
    if (finalecount < 1130)
		return;
    if (finalecount < 1180)
    {
		V_DrawPatch ((SCREENWIDTH-13*8)/2,
			     (SCREENHEIGHT-8*8)/2,0, (patch_t*)WadManager::getLump ("END0"));
		laststage = 0;
		return;
    }
	
    stage = (finalecount-1180) / 5;
    if (stage > 6)
		stage = 6;
    if (stage > laststage)
    {
        I_Sound::startSound(NULL, sfx_pistol);
		laststage = stage;
    }
	
    auto name = "END" + stage;
    V_DrawPatch ((SCREENWIDTH-13*8)/2, (SCREENHEIGHT-8*8)/2,0, (patch_t*)WadManager::getLump (name));
}


//
// F_Drawer
//
void F_Drawer (void)
{
    if (finalestage == 2)
    {
		F_CastDrawer ();
		return;
    }

    if (!finalestage)
		F_TextWrite ();
    else
    {
		switch (gameepisode)
		{
		  case 1:
		    if (Game::gamemode == GameMode_t::retail )
		      V_DrawPatch (0,0,0,
				  (patch_t*)WadManager::getLump("CREDIT"));
		    else
		      V_DrawPatch (0,0,0,
				  (patch_t*)(std::intptr_t(101)));
		    break;
		  case 2:
		    V_DrawPatch(0,0,0,
				(patch_t*)(std::intptr_t(101)));
		    break;
		  case 3:
		    F_BunnyScroll ();
		    break;
		  case 4:
		    V_DrawPatch (0,0,0,
				(patch_t*)WadManager::getLump("ENDPIC"));
		    break;
		}
    }	
}