#include <string.h>
#include <stdlib.h>

#include "doomdef.hpp" 
#include "doomstat.hpp"
#include "i_video.hpp"

#include "f_finale.hpp"
#include "m_argv.hpp"
#include "m_misc.hpp"
#include "m_menu.hpp"
#include "m_random.hpp"
#include "i_system.hpp"
#include "i_sound.hpp"

#include "p_setup.hpp"
#include "p_saveg.hpp"
#include "p_tick.hpp"

#include "d_main.hpp"

#include "wi_stuff.hpp"
#include "hu_stuff.hpp"
#include "st_stuff.hpp"
#include "am_map.hpp"

// Needs access to LFB.
#include "v_video.hpp"

#include "w_wad.hpp"

#include "p_local.hpp" 

#include "i_sound.hpp"

// Data.
#include "dstrings.hpp"
#include "sounds.hpp"

// SKY handling - still the wrong place.
#include "r_data.hpp"
#include "r_sky.hpp"
#include "g_game.hpp"

#define SAVEGAMESIZE	0x2c000
#define SAVESTRINGSIZE	24

GameMode_t Game::gamemode = GameMode_t::indetermined;

bool	G_CheckDemoStatus (void); 
void	G_ReadDemoTiccmd (ticcmd_t* cmd); 
void	G_WriteDemoTiccmd (ticcmd_t* cmd); 
void	G_PlayerReborn (int player); 
void	G_InitNew (skill_t skill, int episode, int map); 
 
void	G_DoReborn (int playernum); 
 
void	G_DoLoadLevel (void); 
void	G_DoNewGame (void); 
void	G_DoLoadGame (void); 
void	G_DoPlayDemo (void); 
void	G_DoCompleted (void);  
void	G_DoWorldDone (void); 
void	G_DoSaveGame (void); 
 
gameaction_t    gameaction; 
gamestate_t     gamestate; 
skill_t         gameskill; 
bool			respawnmonsters;
int             gameepisode; 
int             gamemap; 
 
bool			paused; 
bool			sendpause;             	// send a pause event next tic 
bool			sendsave;             		// send a save event next tic 
bool			usergame;					// ok to save / end game 
 
bool			timingdemo;				// if true, exit with report on completion 
bool			nodrawers;					// for comparative timing purposes 
bool			noblit;					// for comparative timing purposes 
int				starttime;          		// for comparative timing purposes  	 
 
bool			viewactive; 
 
bool			deathmatch;           		// only if started as net death 
bool			netgame;					// only true if packets are broadcast 
bool			playeringame[MAXPLAYERS]; 
player_t		players[MAXPLAYERS]; 
 
int				consoleplayer;          // player taking events and displaying 
int				displayplayer;          // view being displayed 
int				gametic; 
int				levelstarttic;          // gametic at level start 
int				totalkills, totalitems, totalsecret;    // for intermission 
 
char			demoname[32]; 
bool			demorecording; 
bool			demoplayback; 
bool			netdemo; 
unsigned char*	demobuffer;
unsigned char*	demo_p;
unsigned char*	demoend; 
bool			singledemo;            	// quit after playing a demo from cmdline 
 
bool			precache = true;        // if true, load all graphics at start 
 
wbstartstruct_t wminfo;               	// parms for world map / intermission 
 
short			consistancy[MAXPLAYERS][BACKUPTICS]; 
 
unsigned char*	savebuffer;
 
int     mousebfire; 
int     mousebstrafe; 
int     mousebforward; 
 
#define MAXPLMOVE		(forwardmove[1]) 
 
#define TURBOTHRESHOLD	0x32

int		forwardmove[2] = {0x19, 0x32}; 
int		sidemove[2] = {0x18, 0x28}; 
int		angleturn[3] = {640, 1280, 320};	// + slow turn 

#define SLOWTURNTICS	6 
 
#define NUMKEYS		1024

bool        gamekeydown[NUMKEYS]; 
int			turnheld;				// for accelerative turning 
 
bool		mousearray[4]; 
bool*		mousebuttons = &mousearray[1];		// allow [-1]

// mouse values are used once 
int             mousex;
int				mousey;   
sf::Vector2i lastMousePos = sf::Vector2i( 0,0 );

int     dclicktime;
int		dclickstate;
int		dclicks; 
int     dclicktime2;
int		dclickstate2;
int		dclicks2;

float   leftStickXMove;
float	leftStickYMove;
float   rightStickXMove;
float	rightStickYMove;

float	TriggerMove;

float	joyDeadZone(40.f);
std::vector<bool> joybuttons(sf::Joystick::ButtonCount,false);

int joybspeed(4);
int	joybuse(2);
 
int		savegameslot; 
char	savedescription[32]; 
 
#define	BODYQUESIZE	32

mobj_t*		bodyque[BODYQUESIZE]; 
int			bodyqueslot; 
 
void*		statcopy;				// for statistics driver
 
int G_CmdChecksum (ticcmd_t* cmd) 
{ 
    int		i;
    int		sum = 0; 
	 
    for (i=0 ; i< static_cast<int>(sizeof(*cmd))/4 - 1 ; i++)
		sum += ((int *)cmd)[i]; 
		 
    return sum; 
} 
 
//
// G_BuildTiccmd
// Builds a ticcmd from all of the available inputs
// or reads it from the demo buffer. 
// If recording a demo, write it out 
// 
void G_BuildTiccmd (ticcmd_t* cmd) 
{ 
    int		i; 
    int		speed;
    int		tspeed; 
    int		forward;
    int		side;
    
    ticcmd_t*	base;

    base = I_BaseTiccmd ();		// empty, or external driver
    memcpy (cmd,base,sizeof(*cmd)); 
	
    cmd->consistancy = consistancy[consoleplayer][maketic%BACKUPTICS]; 

    speed = gamekeydown[sf::Keyboard::LShift] || gamekeydown[sf::Keyboard::RShift] || joybuttons[joybspeed];
 
    forward = side = 0;
    
    // use two stage accelerative turning
    // on the keyboard and joystick
    if (rightStickXMove < -joyDeadZone || rightStickXMove > joyDeadZone  
	|| gamekeydown[sf::Keyboard::Right]
	|| gamekeydown[sf::Keyboard::Left]) 
		turnheld += ticdup; 
    else 
		turnheld = 0; 

    if (turnheld < SLOWTURNTICS) 
		tspeed = 2;             // slow turn 
    else 
		tspeed = speed;
     
	if (gamekeydown[sf::Keyboard::Right] || rightStickXMove > joyDeadZone)
	    cmd->angleturn -= angleturn[tspeed]; 
	if (gamekeydown[sf::Keyboard::Left] || rightStickXMove < -joyDeadZone)
	    cmd->angleturn += angleturn[tspeed]; 
 
    if (gamekeydown[sf::Keyboard::Up] || gamekeydown[sf::Keyboard::W] || leftStickYMove < -joyDeadZone)
		forward += forwardmove[speed]; 
    if (gamekeydown[sf::Keyboard::Down] || gamekeydown[sf::Keyboard::S] || leftStickYMove > joyDeadZone)
		forward -= forwardmove[speed]; 
	if (gamekeydown[sf::Keyboard::D] || leftStickXMove > joyDeadZone)
		side += sidemove[speed];
	if (gamekeydown[sf::Keyboard::A] || leftStickXMove < -joyDeadZone)
		side -= sidemove[speed];
    
    // buttons
    cmd->chatchar = HU_dequeueChatChar(); 
 
    if (gamekeydown[sf::Keyboard::LControl] || mousebuttons[mousebfire] 
	|| TriggerMove < -joyDeadZone) 
		cmd->buttons |= BT_ATTACK; 
 
    if (gamekeydown[sf::Keyboard::Space] || joybuttons[joybuse] ) 
    { 
		cmd->buttons |= BT_USE;
		// clear double clicks if hit use button 
		dclicks = 0;                   
    } 

    static std::array<sf::Keyboard::Key, NUMWEAPONS+1> weaponButtons = {
        sf::Keyboard::Num0,
        sf::Keyboard::Num1,
        sf::Keyboard::Num2,
        sf::Keyboard::Num3,
        sf::Keyboard::Num4,
        sf::Keyboard::Num5,
        sf::Keyboard::Num6,
        sf::Keyboard::Num7,
        sf::Keyboard::Num8,
        sf::Keyboard::Num9
    };
    // chainsaw overrides 
	for (i = 0; i < NUMWEAPONS - 1; i++)
	{
		if (gamekeydown[weaponButtons[i]])
		{
			cmd->buttons |= BT_CHANGE;
			cmd->buttons |= i << BT_WEAPONSHIFT;
			break;
		}
	}
    
    // mouse
    if (mousebuttons[mousebforward]) 
		forward += forwardmove[speed];
    
    // forward double click
    if (static_cast<int>(mousebuttons[mousebforward]) != dclickstate && dclicktime > 1 ) 
    { 
		dclickstate = mousebuttons[mousebforward]; 
		if (dclickstate) 
		    dclicks++; 
		if (dclicks == 2) 
		{ 
		    cmd->buttons |= BT_USE; 
		    dclicks = 0; 
		} 
		else 
		    dclicktime = 0; 
    } 
    else 
    { 
		dclicktime += ticdup; 
		if (dclicktime > 20) 
		{ 
		    dclicks = 0; 
		    dclickstate = 0; 
		} 
    }
    
	cmd->angleturn -= mousex*0x8; 

    mousex = mousey = 0; 
	 
    if (forward > MAXPLMOVE) 
		forward = MAXPLMOVE; 
    else if (forward < -MAXPLMOVE) 
		forward = -MAXPLMOVE; 
    if (side > MAXPLMOVE) 
		side = MAXPLMOVE; 
    else if (side < -MAXPLMOVE) 
		side = -MAXPLMOVE; 
 
    cmd->forwardmove += forward; 
    cmd->sidemove += side;
    
    // special buttons
    if (sendpause) 
    { 
		sendpause = false; 
		cmd->buttons = BT_SPECIAL | BTS_PAUSE; 
    } 
 
    if (sendsave) 
    { 
		sendsave = false; 
		cmd->buttons = BT_SPECIAL | BTS_SAVEGAME | (savegameslot<<BTS_SAVESHIFT); 
    } 
} 
 

//
// G_DoLoadLevel 
//
extern  gamestate_t     wipegamestate; 
 
void G_DoLoadLevel (void) 
{ 
    int             i; 

    // Set the sky map.
    // First thing, we have a dummy sky texture name,
    //  a flat. The data is in the WAD only because
    //  we look for an actual index, instead of simply
    //  setting one.
    skyflatnum = R_FlatNumForName ( SKYFLATNAME );

    // DOOM determines the sky texture to be used
    // depending on the current episode, and the game version.
    if ( (Game::gamemode == GameMode_t::commercial)
	 || (Game::gamemode == static_cast<GameMode_t>(GameMission_t::pack_tnt) )
	 || (Game::gamemode == static_cast<GameMode_t>(GameMission_t::pack_plut) ) )
    {
	skytexture = R_TextureNumForName ("SKY3");
	if (gamemap < 12)
	    skytexture = R_TextureNumForName ("SKY1");
	else
	    if (gamemap < 21)
		skytexture = R_TextureNumForName ("SKY2");
    }

    levelstarttic = gametic;        // for time calculation
    
    if (wipegamestate == GS_LEVEL) 
	wipegamestate = (gamestate_t)-1;             // force a wipe 

    gamestate = GS_LEVEL; 

    for (i=0 ; i<MAXPLAYERS ; i++) 
    { 
	if (playeringame[i] && players[i].playerstate == PST_DEAD) 
	    players[i].playerstate = PST_REBORN; 
	memset (players[i].frags,0,sizeof(players[i].frags)); 
    } 
		 
    P_SetupLevel (gameepisode, gamemap, 0, gameskill);    
    displayplayer = consoleplayer;		// view the guy you are playing    
    starttime = I_GetTime (); 
    gameaction = ga_nothing; 
    
    // clear cmd building stuff
    memset (gamekeydown, 0, sizeof(*gamekeydown)); 
    mousex = mousey = 0; 
    sendpause = sendsave = paused = false; 
    memset (mousebuttons, 0, sizeof(*mousebuttons)); 
	for (auto button : joybuttons)
		button = false;
} 
 
 
//
// G_Responder  
// Get info needed to make ticcmd_ts for the players.
// 
bool G_Responder (sf::Event* ev) 
{ 
    // allow spy mode changes even during the demo
    if (gamestate == GS_LEVEL && ev->type == sf::Event::KeyPressed
	&& ev->key.code == sf::Keyboard::F12 && (singledemo || !deathmatch) )
    {
		// spy mode 
		do 
		{ 
		    displayplayer++; 
		    if (displayplayer == MAXPLAYERS) 
				displayplayer = 0; 
		} while (!playeringame[displayplayer] && displayplayer != consoleplayer); 
		return true; 
    }
    
    // any other key pops up menu if in demos
    if (gameaction == ga_nothing && !singledemo && 
	(demoplayback || gamestate == GS_DEMOSCREEN) 
	) 
    { 
		if (ev->type == sf::Event::KeyPressed ||
		    (ev->type == sf::Event::MouseButtonPressed) ||
		    (ev->type == sf::Event::JoystickButtonPressed) )
		{ 
		    M_StartControlPanel (); 
		    return true; 
		} 
		return false; 
    } 
 
    if (gamestate == GS_LEVEL) 
    { 
		if (HU_Responder (ev)) 
		    return true;	// chat ate the event 
		if (ST_Responder (ev)) 
		    return true;	// status window ate it 
		if (AM_Responder (*ev)) 
		    return true;	// automap ate it 
    } 
	 
    if (gamestate == GS_FINALE) 
    { 
		if (F_Responder (ev)) 
		    return true;	// finale ate the event 
    } 
	 
    switch (ev->type) 
    { 
	case sf::Event::KeyPressed:
		//check for full 
		if (ev->key.code == sf::Keyboard::F &&
			ev->key.control)
		{
			toggleFullscreen();
		}
		else if (ev->key.code == sf::Keyboard::P)
		{ 
		    sendpause = true; 
		    return true; 
		} 
		if (ev->key.code <NUMKEYS && ev->key.code >= 0)
		    gamekeydown[ev->key.code] = true;
		return true;    // eat key down events 
 
      case sf::Event::KeyReleased:
		if (ev->key.code <NUMKEYS && ev->key.code>=0)
			gamekeydown[ev->key.code] = false;
		return false;   // always let key up events filter down 
		 
	  case sf::Event::MouseButtonPressed:
		  switch (ev->mouseButton.button)
		  {
		  case sf::Mouse::Left:
			  mousebuttons[0] = true;
			  break;
		  case sf::Mouse::Right:
			  mousebuttons[1] = true;
			  break;
		  case sf::Mouse::Middle:
			  mousebuttons[2] = true;
			  break;
		  }
		  break;
	  case sf::Event::MouseButtonReleased:
		  switch (ev->mouseButton.button)
		  {
		  case sf::Mouse::Left:
			  mousebuttons[0] = false;
			  break;
		  case sf::Mouse::Right:
			  mousebuttons[1] = false;
			  break;
		  case sf::Mouse::Middle:
			  mousebuttons[2] = false;
			  break;
		  }
		  break; 
	  case sf::Event::MouseMoved:
		  break;
	  case sf::Event::JoystickMoved:
		switch (ev->joystickMove.axis)
		{
		case sf::Joystick::Axis::X:
			leftStickXMove = ev->joystickMove.position;
			break;

		case sf::Joystick::Axis::Y:
			leftStickYMove = ev->joystickMove.position;
			break;
			
		case sf::Joystick::Axis::U:
			rightStickXMove = ev->joystickMove.position;
			break;

		case sf::Joystick::Axis::V:
			rightStickYMove = ev->joystickMove.position;

		case sf::Joystick::Axis::Z:
			TriggerMove = ev->joystickMove.position;
		}
		break;    // eat events 

	  case sf::Event::JoystickButtonPressed:
		  joybuttons[ev->joystickButton.button] = true;
		  break;

	  case sf::Event::JoystickButtonReleased:
		  joybuttons[ev->joystickButton.button] = false;
		  break;

      default: 
		break; 
    } 
    return false; 
} 
 
 
 
//
// G_Ticker
// Make ticcmd_ts for the players.
//
void G_Ticker (void) 
{ 
    int		i;
    int		buf; 
    ticcmd_t*	cmd;
    
	//first check for mouse movement and capture it
	sf::Vector2i windowSize;
	if (window->hasFocus())
	{
		sf::Vector2i windowSize(window->getSize());
		mousex = (sf::Mouse::getPosition(*window).x - windowSize.x / 2)*(mouseSensitivity + 5) / 10;
		mousey = (windowSize.y / 2 - sf::Mouse::getPosition(*window).y)*(mouseSensitivity + 5) / 10;
		sf::Mouse::setPosition(sf::Vector2i( windowSize.x / 2, windowSize.y / 2 ), *window);
	}

    // do player reborns if needed
    for (i=0 ; i<MAXPLAYERS ; i++) 
	if (playeringame[i] && players[i].playerstate == PST_REBORN) 
	    G_DoReborn (i);
    
    // do things to change the game state
    while (gameaction != ga_nothing) 
    { 
		switch (gameaction) 
		{ 
		  case ga_loadlevel: 
		    G_DoLoadLevel (); 
		    break; 
		  case ga_newgame: 
		    G_DoNewGame (); 
		    break; 
		  case ga_loadgame: 
		    G_DoLoadGame (); 
		    break; 
		  case ga_savegame: 
		    G_DoSaveGame (); 
		    break; 
		  case ga_playdemo: 
		    G_DoPlayDemo (); 
		    break; 
		  case ga_completed: 
		    G_DoCompleted (); 
		    break; 
		  case ga_victory: 
		    F_StartFinale (); 
		    break; 
		  case ga_worlddone: 
		    G_DoWorldDone (); 
		    break; 
		  case ga_screenshot: 
		    M_ScreenShot (); 
		    gameaction = ga_nothing; 
		    break; 
		  case ga_nothing: 
		    break; 
		} 
    }
    
    // get commands, check consistancy,
    // and build new consistancy check
    buf = (gametic/ticdup)%BACKUPTICS; 
 
    for (i=0 ; i<MAXPLAYERS ; i++)
    {
		if (playeringame[i]) 
		{ 
		    cmd = &players[i].cmd; 
 
		    memcpy (cmd, &netcmds[i][buf], sizeof(ticcmd_t)); 
 
		    if (demoplayback) 
			G_ReadDemoTiccmd (cmd); 
		    if (demorecording) 
			G_WriteDemoTiccmd (cmd);
		    
		    // check for turbo cheats
		    if (cmd->forwardmove > TURBOTHRESHOLD 
			&& !(gametic&31) && ((gametic>>5)&3) == i )
		    {
			static char turbomessage[80];
			extern char *player_names[4];
			sprintf (turbomessage, "%s is turbo!",player_names[i]);
			players[consoleplayer].message = turbomessage;
		    }
				
		    if (netgame && !netdemo && !(gametic%ticdup) ) 
		    { 
			if (gametic > BACKUPTICS 
			    && consistancy[i][buf] != cmd->consistancy) 
			{ 
			    I_Error ("consistency failure (%i should be %i)",
				     cmd->consistancy, consistancy[i][buf]); 
			} 
			if (players[i].mo) 
			    consistancy[i][buf] = players[i].mo->x; 
			else 
			    consistancy[i][buf] = rndindex; 
		    } 
		}
    }
    
    // check for special buttons
    for (i=0 ; i<MAXPLAYERS ; i++)
    {
	if (playeringame[i]) 
	{ 
	    if (players[i].cmd.buttons & BT_SPECIAL) 
	    { 
		switch (players[i].cmd.buttons & BT_SPECIALMASK) 
		{ 
		  case BTS_PAUSE: 
		    paused ^= 1; 
		    if (paused) 
			    I_Sound::pauseSound (); 
		    else 
			    I_Sound::resumeSound (); 
		    break; 
					 
		  case BTS_SAVEGAME: 
		    if (!savedescription[0]) 
			strcpy (savedescription, "NET GAME"); 
		    savegameslot =  
			(players[i].cmd.buttons & BTS_SAVEMASK)>>BTS_SAVESHIFT; 
		    gameaction = ga_savegame; 
		    break; 
		} 
	    } 
	}
    }
    
    // do main actions
    switch (gamestate) 
    { 
      case GS_LEVEL: 
		P_Ticker (); 
		ST_Ticker (); 
		AM_Ticker (); 
		HU_Ticker ();            
		break; 
	 
      case GS_INTERMISSION: 
		WI_Ticker (); 
		break; 
			 
      case GS_FINALE: 
		F_Ticker (); 
		break; 
 
      case GS_DEMOSCREEN: 
		D_PageTicker (); 
		break; 
    }        
} 
 
 
//
// PLAYER STRUCTURE FUNCTIONS
// also see P_SpawnPlayer in P_Things
//

//
// G_InitPlayer 
// Called at the start.
// Called by the game initialization functions.
//
void G_InitPlayer (int player) 
{ 
    player_t*	p; 
 
    // set up the saved info         
    p = &players[player]; 
	 
    // clear everything else to defaults 
    G_PlayerReborn (player); 
	 
} 
 
 

//
// G_PlayerFinishLevel
// Can when a player completes a level.
//
void G_PlayerFinishLevel (int player) 
{ 
    player_t*	p; 
	 
    p = &players[player]; 
	 
    memset (p->powers, 0, sizeof (p->powers)); 
    memset (p->cards, 0, sizeof (p->cards)); 
    p->mo->flags &= ~MF_SHADOW;		// cancel invisibility 
    p->extralight = 0;			// cancel gun flashes 
    p->fixedcolormap = 0;		// cancel ir gogles 
    p->damagecount = 0;			// no palette changes 
    p->bonuscount = 0; 
} 
 

//
// G_PlayerReborn
// Called after a player dies 
// almost everything is cleared and initialized 
//
void G_PlayerReborn (int player) 
{ 
    player_t*	p; 
    int		i; 
    int		frags[MAXPLAYERS]; 
    int		killcount;
    int		itemcount;
    int		secretcount; 
	 
    memcpy (frags,players[player].frags,sizeof(frags)); 
    killcount = players[player].killcount; 
    itemcount = players[player].itemcount; 
    secretcount = players[player].secretcount; 
	 
    p = &players[player]; 
    memset (p, 0, sizeof(*p)); 
 
    memcpy (players[player].frags, frags, sizeof(players[player].frags)); 
    players[player].killcount = killcount; 
    players[player].itemcount = itemcount; 
    players[player].secretcount = secretcount; 
 
    p->usedown = p->attackdown = true;	// don't do anything immediately 
    p->playerstate = PST_LIVE;       
    p->health = MAXHEALTH; 
    p->readyweapon = p->pendingweapon = wp_pistol; 
    p->weaponowned[wp_fist] = true; 
    p->weaponowned[wp_pistol] = true; 
    p->ammo[am_clip] = 50; 
	 
    for (i=0 ; i<NUMAMMO ; i++) 
	p->maxammo[i] = maxammo[i]; 
		 
}

//
// G_CheckSpot  
// Returns false if the player cannot be respawned
// at the given mapthing_t spot  
// because something is occupying it 
//
void P_SpawnPlayer (mapthing_t* mthing); 
 
bool
G_CheckSpot
( int		playernum,
  mapthing_t*	mthing ) 
{ 
    int		x;
    int		y; 
    subsector_t*	ss; 
    unsigned		an; 
    mobj_t*		mo; 
    int			i;
	
    if (!players[playernum].mo)
    {
	// first spawn of level, before corpses
	for (i=0 ; i<playernum ; i++)
	    if (players[i].mo->x == mthing->x << FRACBITS
		&& players[i].mo->y == mthing->y << FRACBITS)
		return false;	
	return true;
    }
		
    x = mthing->x << FRACBITS; 
    y = mthing->y << FRACBITS; 
	 
    if (!P_CheckPosition (players[playernum].mo, x, y) ) 
	return false; 
 
    // flush an old corpse if needed 
    if (bodyqueslot >= BODYQUESIZE) 
	P_RemoveMobj (bodyque[bodyqueslot%BODYQUESIZE]); 
    bodyque[bodyqueslot%BODYQUESIZE] = players[playernum].mo; 
    bodyqueslot++; 
	
    // spawn a teleport fog 
    ss = R_PointInSubsector (x,y); 
    an = ( ANG45 * (mthing->angle/45) ) >> ANGLETOFINESHIFT; 
 
    mo = P_SpawnMobj (x+20*finecosine[an], y+20*finesine[an] 
		      , ss->sector->floorheight 
		      , MT_TFOG); 
	 
    if (players[consoleplayer].viewz != 1) 
        I_Sound::startSound(mo, sfx_telept);	// don't start sound on first frame 
 
    return true; 
} 


//
// G_DeathMatchSpawnPlayer 
// Spawns a player at one of the random death match spots 
// called at level load and each death 
//
void G_DeathMatchSpawnPlayer (int playernum) 
{ 
    int             i,j; 
    int				selections; 
	 
    selections = deathmatch_p - deathmatchstarts; 
    if (selections < 4) 
	I_Error ("Only %i deathmatch spots, 4 required", selections); 
 
    for (j=0 ; j<20 ; j++) 
    { 
		i = P_Random() % selections; 
		if (G_CheckSpot (playernum, &deathmatchstarts[i]) ) 
		{ 
		    deathmatchstarts[i].type = playernum+1; 
		    P_SpawnPlayer (&deathmatchstarts[i]); 
		    return; 
		} 
    } 
 
    // no good spot, so the player will probably get stuck 
    P_SpawnPlayer (&playerstarts[playernum]); 
} 

//
// G_DoReborn 
// 
void G_DoReborn (int playernum) 
{ 
    int                             i; 
	 
    if (!netgame)
    {
		// reload the level from scratch
		gameaction = ga_loadlevel;  
    }
    else 
    {
		// respawn at the start

		// first dissasociate the corpse 
		players[playernum].mo->player = NULL;   
			 
		// spawn at random spot if in death match 
		if (deathmatch) 
		{ 
		    G_DeathMatchSpawnPlayer (playernum); 
		    return; 
		} 
			 
		if (G_CheckSpot (playernum, &playerstarts[playernum]) ) 
		{ 
		    P_SpawnPlayer (&playerstarts[playernum]); 
		    return; 
		}
		
		// try to spawn at one of the other players spots 
		for (i=0 ; i<MAXPLAYERS ; i++)
		{
		    if (G_CheckSpot (playernum, &playerstarts[i]) ) 
		    { 
			playerstarts[i].type = playernum+1;	// fake as other player 
			P_SpawnPlayer (&playerstarts[i]); 
			playerstarts[i].type = i+1;		// restore 
			return; 
		    }	    
		    // he's going to be inside something.  Too bad.
		}
		P_SpawnPlayer (&playerstarts[playernum]); 
    } 
} 

// DOOM Par Times
int pars[4][10] = 
{ 
    {0}, 
    {0,30,75,120,90,165,180,180,30,165}, 
    {0,90,90,90,120,90,360,240,30,170}, 
    {0,90,45,90,150,90,90,165,30,135} 
}; 

// DOOM II Par Times
int cpars[32] =
{
    30,90,120,120,90,150,120,120,270,90,	//  1-10
    210,150,150,150,210,150,420,150,210,150,	// 11-20
    240,150,180,150,150,300,330,420,300,180,	// 21-30
    120,30					// 31-32
};
 

//
// G_DoCompleted 
//
bool		secretexit; 
extern char*	pagename; 
 
void G_ExitLevel (void) 
{ 
    secretexit = false; 
    gameaction = ga_completed; 
} 

// Here's for the german edition.
void G_SecretExitLevel (void) 
{ 
    // IF NO WOLF3D LEVELS, NO SECRET EXIT!
    if ( (Game::gamemode == GameMode_t::commercial)
      && (WadManager::WadManager::checkNumForName("map31")<0))
		secretexit = false;
    else
		secretexit = true; 
    gameaction = ga_completed; 
} 
 
void G_DoCompleted (void) 
{ 
    int             i; 
	 
    gameaction = ga_nothing; 
 
    for (i=0 ; i<MAXPLAYERS ; i++) 
	if (playeringame[i]) 
	    G_PlayerFinishLevel (i);        // take away cards and stuff 
	 
    if (automapactive) 
		AM_Stop (); 
	
    if (Game::gamemode != GameMode_t::commercial)
	switch(gamemap)
	{
	  case 8:
	    gameaction = ga_victory;
	    return;
	  case 9: 
	    for (i=0 ; i<MAXPLAYERS ; i++) 
		players[i].didsecret = true; 
	    break;
	}
		
    if ( (gamemap == 8)
	 && (Game::gamemode != GameMode_t::commercial) )
    {
		// victory 
		gameaction = ga_victory; 
		return; 
    } 
	 
    if ( (gamemap == 9)
	 && (Game::gamemode != GameMode_t::commercial) )
    {
		// exit secret level 
		for (i=0 ; i<MAXPLAYERS ; i++) 
		    players[i].didsecret = true; 
    }     
	 
    wminfo.didsecret = players[consoleplayer].didsecret; 
    wminfo.epsd = gameepisode -1; 
    wminfo.last = gamemap -1;
    
    // wminfo.next is 0 biased, unlike gamemap
    if (Game::gamemode == GameMode_t::commercial)
    {
	if (secretexit)
	    switch(gamemap)
	    {
	      case 15: wminfo.next = 30; break;
	      case 31: wminfo.next = 31; break;
	    }
	else
	    switch(gamemap)
	    {
	      case 31:
	      case 32: wminfo.next = 15; break;
	      default: wminfo.next = gamemap;
	    }
    }
    else
    {
	if (secretexit) 
	    wminfo.next = 8; 	// go to secret level 
	else if (gamemap == 9) 
	{
	    // returning from secret level 
	    switch (gameepisode) 
	    { 
	      case 1: 
			wminfo.next = 3; 
			break; 
	      case 2: 
			wminfo.next = 5; 
			break; 
	      case 3: 
			wminfo.next = 6; 
			break; 
	      case 4:
			wminfo.next = 2;
			break;
	    }                
	} 
	else 
	    wminfo.next = gamemap;          // go to next level 
    }
		 
    wminfo.maxkills = totalkills; 
    wminfo.maxitems = totalitems; 
    wminfo.maxsecret = totalsecret; 
    wminfo.maxfrags = 0; 
    if (Game::gamemode == GameMode_t::commercial )
		wminfo.partime = 35*cpars[gamemap-1]; 
    else
		wminfo.partime = 35*pars[gameepisode][gamemap]; 
    wminfo.pnum = consoleplayer; 
 
    for (i=0 ; i<MAXPLAYERS ; i++) 
    { 
		wminfo.plyr[i].in = playeringame[i]; 
		wminfo.plyr[i].skills = players[i].killcount; 
		wminfo.plyr[i].sitems = players[i].itemcount; 
		wminfo.plyr[i].ssecret = players[i].secretcount; 
		wminfo.plyr[i].stime = leveltime; 
		memcpy (wminfo.plyr[i].frags, players[i].frags 
			, sizeof(wminfo.plyr[i].frags)); 
    } 
 
    gamestate = GS_INTERMISSION; 
    viewactive = false; 
    automapactive = false; 
 
    if (statcopy)
		memcpy (statcopy, &wminfo, sizeof(wminfo));
	
    WI_Start (&wminfo); 
} 


//
// G_WorldDone 
//
void G_WorldDone (void) 
{ 
    gameaction = ga_worlddone; 

    if (secretexit) 
		players[consoleplayer].didsecret = true; 

    if (Game::gamemode == GameMode_t::commercial )
    {
		switch (gamemap)
		{
		  case 15:
		  case 31:
		    if (!secretexit)
			break;
		  case 6:
		  case 11:
		  case 20:
		  case 30:
		    F_StartFinale ();
		    break;
		}
    }
} 
 
void G_DoWorldDone (void) 
{        
    gamestate = GS_LEVEL; 
    gamemap = wminfo.next+1; 
    G_DoLoadLevel (); 
    gameaction = ga_nothing; 
    viewactive = true; 
} 
 


//
// G_InitFromSavegame
// Can be called by the startup code or the menu task. 
//
extern bool setsizeneeded;
void R_ExecuteSetViewSize (void);

char	savename[256];

void G_LoadGame (char* name) 
{ 
    strcpy (savename, name); 
    gameaction = ga_loadgame; 
} 
 
#define VERSIONSIZE		16 


void G_DoLoadGame (void) 
{ 
    int		length; 
    int		i; 
    int		a,b,c;
    std::vector<char> saveFileData;
	 
    gameaction = ga_nothing; 
	 
    std::ifstream file;
    file.open(savename, std::ios::binary | std::ios::ate);
    auto size = file.tellg();
    saveFileData.resize(size);
    file.seekg(std::ios::beg);
    file.read(saveFileData.data(), size);
    save_p = reinterpret_cast<unsigned char*>(saveFileData.data())+ SAVESTRINGSIZE;
    
    // skip the description field
    save_p += VERSIONSIZE; 
			 
    gameskill = (skill_t)*save_p++; 
    gameepisode = *save_p++; 
    gamemap = *save_p++; 
    for (i=0 ; i<MAXPLAYERS ; i++) 
	playeringame[i] = (*save_p++)!=0; 

    // load a base level 
    G_InitNew (gameskill, gameepisode, gamemap); 
 
    // get the times 
    a = *save_p++; 
    b = *save_p++; 
    c = *save_p++; 
    leveltime = (a<<16) + (b<<8) + c; 
	 
    // dearchive all the modifications
    P_UnArchivePlayers (); 
    P_UnArchiveWorld (); 
    P_UnArchiveThinkers (); 
    P_UnArchiveSpecials (); 
 
    if (*save_p != 0x1d) 
	I_Error ("Bad savegame");
 
    if (setsizeneeded)
	R_ExecuteSetViewSize ();
    
    // draw the pattern into the back screen
    R_FillBackScreen ();   
} 
 

//
// G_SaveGame
// Called by the menu task.
// Description is a 24 unsigned char text string 
//
void
G_SaveGame
( int	slot,
  char*	description ) 
{ 
    savegameslot = slot; 
    strcpy (savedescription, description); 
    sendsave = true; 
} 
 
void G_DoSaveGame (void) 
{ 
    char	name[100]; 
    char	name2[VERSIONSIZE]; 
    char*	description; 
    int		length; 
    int		i; 
	
    if (CmdParameters::M_CheckParm("-cdrom"))
	sprintf(name,"c:\\doomdata\\\"SAVEGAMENAME\"%d.dsg",savegameslot);
    else
	sprintf (name,SAVEGAMENAME"%d.dsg",savegameslot); 
    description = savedescription; 
	 
    save_p = savebuffer = screens[1]+0x4000; 
	 
    memcpy (save_p, description, SAVESTRINGSIZE); 
    save_p += SAVESTRINGSIZE; 
    memset (name2,0,sizeof(name2)); 
    memcpy (save_p, name2, VERSIONSIZE); 
    save_p += VERSIONSIZE; 
	 
    *save_p++ = gameskill; 
    *save_p++ = gameepisode; 
    *save_p++ = gamemap; 
    for (i=0 ; i<MAXPLAYERS ; i++) 
	*save_p++ = playeringame[i]; 
    *save_p++ = leveltime>>16; 
    *save_p++ = leveltime>>8; 
    *save_p++ = leveltime; 
 
    P_ArchivePlayers (); 
    P_ArchiveWorld (); 
    P_ArchiveThinkers (); 
    P_ArchiveSpecials (); 
	 
    *save_p++ = 0x1d;		// consistancy marker 
	 
    length = save_p - savebuffer; 
    if (length > SAVEGAMESIZE) 
	I_Error ("Savegame buffer overrun"); 
    std::ofstream file;
    file.open(name, std::ios::binary | std::ios::trunc);
    file.write(reinterpret_cast<char*>(savebuffer), length);
    gameaction = ga_nothing; 
    savedescription[0] = 0;		 
	 
    players[consoleplayer].message = GGSAVED; 

    // draw the pattern into the back screen
    R_FillBackScreen ();	
} 
 

//
// G_InitNew
// Can be called by the startup code or the menu task,
// consoleplayer, displayplayer, playeringame[] should be set. 
//
skill_t	d_skill; 
int     d_episode; 
int     d_map; 
 
void
G_DeferedInitNew
( skill_t	skill,
  int		episode,
  int		map) 
{ 
    d_skill = skill; 
    d_episode = episode; 
    d_map = map; 
    gameaction = ga_newgame; 
} 


void G_DoNewGame (void) 
{
    demoplayback = false; 
    netdemo = false;
    netgame = false;
    deathmatch = false;
    playeringame[1] = playeringame[2] = playeringame[3] = 0;
    respawnparm = false;
    fastparm = false;
    nomonsters = false;
    consoleplayer = 0;
    G_InitNew (d_skill, d_episode, d_map); 
    gameaction = ga_nothing; 
} 

// The sky texture to be used instead of the F_SKY1 dummy.
extern  int	skytexture; 


void
G_InitNew
(skill_t	skill,
	int		episode,
	int		map)
{
	int             i;

	if (paused)
	{
		paused = false;
		I_Sound::resumeSound();
	}


	if (skill > sk_nightmare)
		skill = sk_nightmare;


	// This was quite messy with SPECIAL and commented parts.
	// Supposedly hacks to make the latest edition work.
	// It might not work properly.
	if (episode < 1)
		episode = 1;

	if (Game::gamemode == GameMode_t::retail)
	{
		if (episode > 4)
			episode = 4;
	}
	else if (Game::gamemode == GameMode_t::shareware)
	{
		if (episode > 1)
			episode = 1;	// only start episode 1 on shareware
	}
	else
	{
		if (episode > 3)
			episode = 3;
	}



	if (map < 1)
		map = 1;

	if ((map > 9)
		&& (Game::gamemode != GameMode_t::commercial))
		map = 9;

	M_ClearRandom();

	if (skill == sk_nightmare || respawnparm)
		respawnmonsters = true;
	else
		respawnmonsters = false;

	if (fastparm || (skill == sk_nightmare && gameskill != sk_nightmare))
	{
		for (i = S_SARG_RUN1; i <= S_SARG_PAIN2; i++)
			states[i].tics >>= 1;
		mobjinfo[MT_BRUISERSHOT].speed = 20 * FRACUNIT;
		mobjinfo[MT_HEADSHOT].speed = 20 * FRACUNIT;
		mobjinfo[MT_TROOPSHOT].speed = 20 * FRACUNIT;
	}
	else if (skill != sk_nightmare && gameskill == sk_nightmare)
	{
		for (i = S_SARG_RUN1; i <= S_SARG_PAIN2; i++)
			states[i].tics <<= 1;
		mobjinfo[MT_BRUISERSHOT].speed = 15 * FRACUNIT;
		mobjinfo[MT_HEADSHOT].speed = 10 * FRACUNIT;
		mobjinfo[MT_TROOPSHOT].speed = 10 * FRACUNIT;
	}


	// force players to be initialized upon first level load         
	for (i = 0; i < MAXPLAYERS; i++)
		players[i].playerstate = PST_REBORN;

	usergame = true;                // will be set false if a demo 
	paused = false;
	demoplayback = false;
	automapactive = false;
	viewactive = true;
	gameepisode = episode;
	gamemap = map;
	gameskill = skill;

	viewactive = true;

	// set the sky map for the episode
	if (Game::gamemode == GameMode_t::commercial)
	{
		skytexture = R_TextureNumForName("SKY3");
		if (gamemap < 12)
			skytexture = R_TextureNumForName("SKY1");
		else
			if (gamemap < 21)
				skytexture = R_TextureNumForName("SKY2");
	}
	else
	{
		switch (episode)
		{
		case 1:
			skytexture = R_TextureNumForName("SKY1");
			break;
		case 2:
			skytexture = R_TextureNumForName("SKY2");
			break;
		case 3:
			skytexture = R_TextureNumForName("SKY3");
			break;
		case 4:	// Special Edition sky
			skytexture = R_TextureNumForName("SKY4");
			break;
		}
	}
    G_DoLoadLevel (); 
} 
 

//
// DEMO RECORDING 
// 
#define DEMOMARKER		0x80


void G_ReadDemoTiccmd (ticcmd_t* cmd) 
{ 
    if (*demo_p == DEMOMARKER) 
    {
	// end of demo data stream 
	G_CheckDemoStatus (); 
	return; 
    } 
    cmd->forwardmove = ((signed char)*demo_p++); 
    cmd->sidemove = ((signed char)*demo_p++); 
    cmd->angleturn = ((unsigned char)*demo_p++)<<8; 
    cmd->buttons = (unsigned char)*demo_p++; 
} 


void G_WriteDemoTiccmd (ticcmd_t* cmd) 
{ 
    if (gamekeydown['q'])           // press q to end demo recording 
		G_CheckDemoStatus (); 
    *demo_p++ = cmd->forwardmove; 
    *demo_p++ = cmd->sidemove; 
    *demo_p++ = (cmd->angleturn+128)>>8; 
    *demo_p++ = cmd->buttons; 
    demo_p -= 4; 
    if (demo_p > demoend - 16)
    {
		// no more space 
		G_CheckDemoStatus (); 
		return; 
    } 
	
    G_ReadDemoTiccmd (cmd);         // make SURE it is exactly the same 
} 
 
 
 
//
// G_RecordDemo 
// 
void G_RecordDemo (char* name) 
{ 
    int             i; 
    int				maxsize;
	
    usergame = false; 
    strcpy (demoname, name); 
    strcat (demoname, ".lmp"); 
    maxsize = 0x20000;
    i = CmdParameters::M_CheckParm ("-maxdemo");
    if (i && i<CmdParameters::myargc-1)
		maxsize = atoi(CmdParameters::myargv[i+1].c_str())*1024;
    demobuffer = (unsigned char*)malloc (maxsize); 
    demoend = demobuffer + maxsize;
	
    demorecording = true; 
} 
 
 
void G_BeginRecording (void) 
{ 
    int             i; 
		
    demo_p = demobuffer;
	
    *demo_p++ = gameskill; 
    *demo_p++ = gameepisode; 
    *demo_p++ = gamemap; 
    *demo_p++ = deathmatch; 
    *demo_p++ = respawnparm;
    *demo_p++ = fastparm;
    *demo_p++ = nomonsters;
    *demo_p++ = consoleplayer;
	 
    for (i=0 ; i<MAXPLAYERS ; i++) 
		*demo_p++ = playeringame[i]; 		 
} 

char*	defdemoname; 
 
void G_DeferedPlayDemo (char* name) 
{ 
    defdemoname = name; 
    gameaction = ga_playdemo; 
} 
 
void G_DoPlayDemo (void) 
{ 
    skill_t skill; 
    int             i, episode, map; 


    gameaction = ga_nothing; 
    demobuffer = demo_p = (unsigned char*)WadManager::getLump (defdemoname); 
	if (*demo_p++ != 109)	//magic number, I know, but was a define
	{
		fprintf(stderr, "Demo is from a different game version!\n");
		gameaction = ga_nothing;
		return;
	}
    
    skill = (skill_t)*demo_p++; 
    episode = *demo_p++; 
    map = *demo_p++; 
    deathmatch = (*demo_p++)!=0;
    respawnparm = (*demo_p++)!=0;
    fastparm = (*demo_p++)!=0;
    nomonsters = (*demo_p++)!=0;
    consoleplayer = *demo_p++;
	
    for (i=0 ; i<MAXPLAYERS ; i++) 
		playeringame[i] = (*demo_p++)!=0; 
    if (playeringame[1]) 
    { 
		netgame = true; 
		netdemo = true; 
    }

    // don't spend a lot of time in loadlevel 
    precache = false;
    G_InitNew (skill, episode, map); 
    precache = true; 

    usergame = false; 
    demoplayback = true; 
} 

//
// G_TimeDemo 
//
void G_TimeDemo (char* name) 
{ 	 
    nodrawers = CmdParameters::M_CheckParm ("-nodraw")!=0;
    noblit = CmdParameters::M_CheckParm ("-noblit")!=0;
    timingdemo = true; 
    singletics = true; 

    defdemoname = name; 
    gameaction = ga_playdemo; 
} 
 
 
/* 
=================== 
= 
= G_CheckDemoStatus 
= 
= Called after a death or level completion to allow demos to be cleaned up 
= Returns true if a new demo loop action will take place 
=================== 
*/ 
 
bool G_CheckDemoStatus (void) 
{ 
    int             endtime; 
	 
    if (timingdemo) 
    { 
		endtime = I_GetTime (); 
		I_Error ("timed %i gametics in %i realtics",gametic 
			 , endtime-starttime); 
    } 
	 
    if (demoplayback) 
    { 
		if (singledemo) 
		    I_Quit (); 
			 
		demoplayback = false; 
		netdemo = false;
		netgame = false;
		deathmatch = false;
		playeringame[1] = playeringame[2] = playeringame[3] = 0;
		respawnparm = false;
		fastparm = false;
		nomonsters = false;
		consoleplayer = 0;
		D_AdvanceDemo (); 
		return true; 
    } 
 
    if (demorecording) 
    { 
		*demo_p++ = DEMOMARKER; 
        std::ofstream file;
        file.open(demoname);
        file.write(reinterpret_cast<char*>(demobuffer), demo_p - demobuffer);
		free (demobuffer); 
		demorecording = false; 
		I_Error ("Demo %s recorded",demoname); 
    } 
    return false; 
} 
