#define	BGCOLOR		7
#define	FGCOLOR		8

#include <iostream>

#include "doomdef.hpp"
#include "doomstat.hpp"

#include "dstrings.hpp"
#include "sounds.hpp"

#include "w_wad.hpp"
#include "i_sound.hpp"
#include "v_video.hpp"

#include "f_finale.hpp"
#include "f_wipe.hpp"

#include "m_argv.hpp"
#include "m_misc.hpp"
#include "m_menu.hpp"

#include "i_system.hpp"
#include "i_sound.hpp"
#include "i_video.hpp"

#include "g_game.hpp"

#include "hu_stuff.hpp"
#include "wi_stuff.hpp"
#include "st_stuff.hpp"
#include "am_map.hpp"

#include "p_setup.hpp"
#include "r_local.hpp"


#include "d_main.hpp"

#include <stdlib.h>
#include <SFML/Graphics.hpp>
#define R_OK 4

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef __APPLE__
#include "ResourcePath.hpp"
#endif

//
// D-DoomLoop()
// Not a globally visible function,
//  just included for source reference,
//  called by D_DoomMain, never exits.
// Manages timing and IO,
//  calls all ?_Responder, ?_Ticker, and ?_Drawer,
//  calls I_GetTime, I_StartFrame, and I_StartTic
//
void D_DoomLoop (void);

std::vector<std::string> wadfiles;

bool         nomonsters;	// checkparm of -nomonsters
bool         respawnparm;	// checkparm of -respawn
bool         fastparm;		// checkparm of -fast

bool         drone;

bool		 singletics = false; // debug flag to cancel adaptiveness

extern  bool inhelpscreens;

skill_t		 startskill;
int          startepisode;
int			 startmap;
bool		 autostart;

bool		 verboseOutput = false;

bool		 advancedemo;

char		wadfile[1024];		// primary wad file
char		basedefault[1024];      // default file

void D_CheckNetGame (void);
void D_ProcessEvents (void);
void G_BuildTiccmd (ticcmd_t* cmd);
void D_DoAdvanceDemo (void);

//
// D_ProcessEvents
// Send all the events of the given timestamp down the responder chain
//
void D_ProcessEvents (void)
{
    sf::Event	ev;
	
    while(pollEvent(ev))
    {
		if (ev.type == sf::Event::Closed)
			I_Error("Window Closed");

		if (M_Responder (&ev))
		    continue;               // menu ate the event
		
		G_Responder (&ev);
    }
}

//
// D_Display
//  draw current display, possibly wiping it from the previous
//

// wipegamestate can be set to -1 to force a wipe on the next draw
gamestate_t     wipegamestate = GS_DEMOSCREEN;
extern  bool	setsizeneeded;
extern  int     showMessages;
void R_ExecuteSetViewSize (void);

void D_Display (void)
{
    static  bool		viewactivestate = false;
    static  bool		menuactivestate = false;
    static  bool		inhelpscreensstate = false;
    static  bool		fullscreen = false;
    static  gamestate_t	oldgamestate = (gamestate_t)-1;
    static  int			borderdrawcount;
    int					nowtime;
    int					tics;
    int					wipestart;
    int					y;
    bool				done;
    bool				wipe;
    bool				redrawsbar;
		
    redrawsbar = false;
    
    // change the view size if needed
    if (setsizeneeded)
    {
		R_ExecuteSetViewSize ();
		oldgamestate = (gamestate_t)-1;                      // force background redraw
		borderdrawcount = 3;
    }

    // save the current screen if about to wipe
    if (gamestate != wipegamestate)
    {
		wipe = true;
		wipe_StartScreen(0, 0, SCREENWIDTH, SCREENHEIGHT);
    }
    else
		wipe = false;

    if (gamestate == GS_LEVEL && gametic)
		HU_Erase();
    
    // do buffered drawing
    switch (gamestate)
    {
	case GS_LEVEL:
		if (!gametic)
			break;
		if (automapactive)
		    AM_Drawer ();
		if (wipe || (viewheight != 200 && fullscreen) )
		    redrawsbar = true;
		if (inhelpscreensstate && !inhelpscreens)
		    redrawsbar = true;              // just put away the help screen
		ST_Drawer (viewheight == 200, redrawsbar );
		fullscreen = viewheight == 200;
		break;

    case GS_INTERMISSION:
		WI_Drawer ();
			break;

    case GS_FINALE:
		F_Drawer ();
			break;

    case GS_DEMOSCREEN:
		D_PageDrawer ();
			break;
    }
    
    // draw the view directly
    if (gamestate == GS_LEVEL && !automapactive && gametic)
		R_RenderPlayerView (&players[displayplayer]);

    if (gamestate == GS_LEVEL && gametic)
		HU_Drawer ();
    
    // clean up border stuff
    if (gamestate != oldgamestate && gamestate != GS_LEVEL)
		I_SetPalette ((unsigned char*)WadManager::WadManager::getLump ("PLAYPAL"));

    // see if the border needs to be initially drawn
    if (gamestate == GS_LEVEL && oldgamestate != GS_LEVEL)
    {
		viewactivestate = false;        // view was not active
		R_FillBackScreen ();    // draw the pattern into the back screen
    }

    // see if the border needs to be updated to the screen
    if (gamestate == GS_LEVEL && !automapactive && scaledviewwidth != 320)
    {
		if (menuactive || menuactivestate || !viewactivestate)
		    borderdrawcount = 3;
		if (borderdrawcount)
		{
		    R_DrawViewBorder ();    // erase old menu stuff
		    borderdrawcount--;
		}
    }

    menuactivestate = menuactive;
    viewactivestate = viewactive;
    inhelpscreensstate = inhelpscreens;
    oldgamestate = wipegamestate = gamestate;
    
    // draw pause pic
    if (paused)
    {
		if (automapactive)
		    y = 4;
		else
		    y = viewwindowy+4;
		V_DrawPatchDirect(viewwindowx+(scaledviewwidth-68)/2,
				  y,0, (patch_t*)WadManager::getLump ("M_PAUSE"));
    }

    // menus go directly to the screen
    M_Drawer ();          // menu is drawn even on top of everything
    NetUpdate ();         // send out any new accumulation

    // normal update
    if (!wipe)
    {
		I_FinishUpdate ();              // page flip or blit buffer
		return;
    }
    
    // wipe update
    wipe_EndScreen(0, 0, SCREENWIDTH, SCREENHEIGHT);

    wipestart = I_GetTime () - 1;

    do
    {
		do
		{
		    nowtime = I_GetTime ();
		    tics = nowtime - wipestart;
		} while (!tics);
		wipestart = nowtime;
		done = (wipe_ScreenWipe(wipe_Melt
				       , 0, 0, SCREENWIDTH, SCREENHEIGHT, tics))!=0;
		M_Drawer ();                            // menu is drawn even on top of wipes
		I_FinishUpdate ();                      // page flip or blit buffer
    } while (!done);
}

//
//  D_DoomLoop
//
extern  bool         demorecording;

void D_DoomLoop (void)
{

    if (demorecording)
		G_BeginRecording ();
		
    if (CmdParameters::M_CheckParm ("-debugfile"))
    {
        verboseOutput = true;
    }

    while (1)
    {          
	
		// process one or more tics
		if (singletics)
		{
		    D_ProcessEvents ();
		    G_BuildTiccmd (&netcmds[consoleplayer][maketic%BACKUPTICS]);
		    
			if (advancedemo)
				D_DoAdvanceDemo ();
		    
			M_Ticker ();
		    G_Ticker ();
		    gametic++;
		    maketic++;
		}
		else
		{
		    TryRunTics (); // will run at least one tic
		}
		
		I_Sound::S_UpdateSounds (players[consoleplayer].mo);// move positional sounds

		// Update display, next frame, with current state.
		D_Display ();
    }
}

//
//  DEMO LOOP
//
int             demosequence;
int             pagetic;
std::string     pagename;

//
// D_PageTicker
// Handles timing for warped projection
//
void D_PageTicker (void)
{
    if (--pagetic < 0)
		D_AdvanceDemo ();
}



//
// D_PageDrawer
//
void D_PageDrawer (void)
{
    V_DrawPatch (0,0, 0, (patch_t*)WadManager::getLump(pagename));
}

//
// D_AdvanceDemo
// Called after each demo or intro demosequence finishes
//
void D_AdvanceDemo (void)
{
    advancedemo = true;
}

//
// This cycles through the demo sequences.
// FIXME - version dependend demo numbers?
//
 void D_DoAdvanceDemo (void)
{
    players[consoleplayer].playerstate = PST_LIVE;  // not reborn
    advancedemo = false;
    usergame = false;               // no save / end game here
    paused = false;
    gameaction = ga_nothing;

    if (Game::gamemode == GameMode_t::retail )
		demosequence = (demosequence+1)%7;
    else
		demosequence = (demosequence+1)%6;
    
    switch (demosequence)
    {
    case 0:
		if (Game::gamemode == GameMode_t::commercial )
		    pagetic = 35 * 11;
		else
		    pagetic = 170;
		gamestate = GS_DEMOSCREEN;
		pagename = "TITLEPIC";
		if (Game::gamemode == GameMode_t::commercial )
		    I_Sound::playMusic(mus_dm2ttl);
		else
            I_Sound::playMusic(mus_intro);
		break;
    case 1:
		G_DeferedPlayDemo ("demo1");
		break;
    case 2:
		pagetic = 200;
		gamestate = GS_DEMOSCREEN;
		pagename = "CREDIT";
		break;
    case 3:
		G_DeferedPlayDemo ("demo2");
		break;
    case 4:
		gamestate = GS_DEMOSCREEN;
		if (Game::gamemode == GameMode_t::commercial)
		{
		    pagetic = 35 * 11;
		    pagename = "TITLEPIC";
            I_Sound::playMusic(mus_dm2ttl);
		}
		else
		{
		    pagetic = 200;

		    if (Game::gamemode == GameMode_t::retail )
		      pagename = "CREDIT";
		    else
		      pagename = "HELP2";
		}
		break;
    case 5:
		G_DeferedPlayDemo ("demo3");
		break;
    // THE DEFINITIVE DOOM Special Edition demo
    case 6:
		G_DeferedPlayDemo ("demo4");
		break;
    }
}

//
// D_StartTitle
//
void D_StartTitle (void)
{
    gameaction = ga_nothing;
    demosequence = -1;
    D_AdvanceDemo ();
}

//      print title for every printed line
std::string title;

//
// D_AddFile
//
void D_AddFile (const std::string& file)
{	
    wadfiles.push_back(file);
}

//
// IdentifyVersion
// Checks availability of IWAD files by name,
// to determine whether registered/commercial features
// should be executed (notably loading PWAD's).
//
void IdentifyVersion (void)
{
    std::string	doom1wad;
    std::string	doomwad;
    std::string	doomuwad;
    std::string doom2wad;

    std::string plutoniawad;
    std::string tntwad;
    
    std::string rPath = "./";
    
#ifdef __APPLE__
    rPath = resourcePath();
#endif

    // Commercial.
    doom2wad = rPath + "doom2.wad";

    // Retail.
    doomuwad = rPath + "doomu.wad";
    
    // Registered.
    doomwad = rPath + "doom.wad";
    
    // Shareware.
    doom1wad = rPath + "doom1.wad";

    // Plutonia
    plutoniawad = rPath + "plutonia.wad";

    // TNT
    tntwad = rPath + "tnt.wad";

    // See which file we can open
    std::ifstream file;

    std::cout << "Looking for " << doom2wad << std::endl;
    file.open(doom2wad);
    if (file.good())
    {
        Game::gamemode = GameMode_t::commercial;
		D_AddFile (doom2wad);
		return;
    }

    std::cout << "Looking for " << plutoniawad << std::endl;
    file.open(plutoniawad);
    if (file.good())
    {
        Game::gamemode = GameMode_t::commercial;
		D_AddFile (plutoniawad);
		return;
    }

    std::cout << "Looking for " << tntwad << std::endl;
    file.open(tntwad);
    if (file.good())
    {
        Game::gamemode = GameMode_t::commercial;
		D_AddFile (tntwad);
		return;
    }

    std::cout << "Looking for " << doomuwad << std::endl;
    file.open(doomuwad);
    if (file.good())
    {
        Game::gamemode = GameMode_t::retail;
		D_AddFile (doomuwad);
		return;
    }

    std::cout << "Looking for " << doomwad << std::endl;
    file.open(doomwad);
    if (file.good())
    {
        Game::gamemode = GameMode_t::registered;
		D_AddFile (doomwad);
		return;
    }

    std::cout << "Looking for " << doom1wad << std::endl;
    file.open(doom1wad);
    if (file.good())
    {
        Game::gamemode = GameMode_t::shareware;
		D_AddFile (doom1wad);
		return;
    }

    printf("Game mode indeterminate.\n");
    Game::gamemode = GameMode_t::indetermined;
}

//
// D_DoomMain
//
void D_DoomMain (void)
{
    int             p;
    std::string            file[256];

	
    IdentifyVersion ();
	
    modifiedgame = false;
	
    nomonsters = CmdParameters::M_CheckParm ("-nomonsters")!=0;
    respawnparm = CmdParameters::M_CheckParm ("-respawn")!=0;
    fastparm = CmdParameters::M_CheckParm ("-fast")!=0;
    if (CmdParameters::M_CheckParm ("-altdeath") || CmdParameters::M_CheckParm("-deathmatch")	)
		deathmatch = true;

    switch (Game::gamemode )
    {

    case GameMode_t::retail:
        title = "\n The Ultimate SFML-DOOM Startup \n\n";
		break;

    case GameMode_t::shareware:
        title = "\n SFML-DOOM Shareware Startup \n\n";
		break;

    case GameMode_t::registered:
        title = "\n SFML-DOOM Registered Startup \n\n";
		break;
    case GameMode_t::commercial:
        title = "\n SFML-DOOM 2: Hell on Earth \n\n";
		break;

     default:
		title = "\n Public DOOM \n\n";
		break;
    }
    
    std::cout << title;
    
    // turbo option
    if ( (p= CmdParameters::M_CheckParm ("-turbo")) )
    {
		int     scale = 200;
		extern int forwardmove[2];
		extern int sidemove[2];
		
		if (p<CmdParameters::myargc-1)
		    scale = atoi (CmdParameters::myargv[p+1].c_str());
		if (scale < 10)
		    scale = 10;
		if (scale > 400)
		    scale = 400;
		printf ("turbo scale: %i%%\n",scale);
		forwardmove[0] = forwardmove[0]*scale/100;
		forwardmove[1] = forwardmove[1]*scale/100;
		sidemove[0] = sidemove[0]*scale/100;
		sidemove[1] = sidemove[1]*scale/100;
    }
  
    p = CmdParameters::M_CheckParm ("-file");
    if (p)
    {
		// the parms after p are wadfile/lump names,
		// until end of parms or another - preceded parm
		modifiedgame = true;            // homebrew levels
		while (++p != CmdParameters::myargc && CmdParameters::myargv[p][0] != '-')
		    D_AddFile (const_cast<char*>(CmdParameters::myargv[p].c_str()));
    }

    p = CmdParameters::M_CheckParm ("-playdemo");

    if (!p)
		p = CmdParameters::M_CheckParm ("-timedemo");

    if (p && p < CmdParameters::myargc-1)
    {
        // Load the demo file
        auto demo = CmdParameters::myargv[p + 1] + ".lmp";
		D_AddFile (demo);
		std::cout << "Playing demo " << demo << std::endl;
    }
    
    // get skill / episode / map from parms
    startskill = sk_medium;
    startepisode = 1;
    startmap = 1;
    autostart = false;

    p = CmdParameters::M_CheckParm ("-skill");
    if (p && p < CmdParameters::myargc-1)
    {
		startskill = (skill_t)(CmdParameters::myargv[p+1][0]-'1');
		autostart = true;
    }

    p = CmdParameters::M_CheckParm ("-episode");
    if (p && p < CmdParameters::myargc-1)
    {
		startepisode = CmdParameters::myargv[p+1][0]-'0';
		startmap = 1;
		autostart = true;
    }
	
    p = CmdParameters::M_CheckParm ("-timer");
    if (p && p < CmdParameters::myargc-1 && deathmatch)
    {
		int     time;
		time = atoi(CmdParameters::myargv[p+1].c_str());
		printf("Levels will end after %d minute",time);
		if (time>1)
		    printf("s");
		printf(".\n");
    }

    p = CmdParameters::M_CheckParm ("-warp");
    if (p && p < CmdParameters::myargc-1)
    {
		startepisode = CmdParameters::myargv[p+1][0]-'0';
		startmap = CmdParameters::myargv[p+2][0]-'0';
		autostart = true;
    }
    
    // init subsystems
    printf ("V_Init: allocate screens.\n");
    V_Init ();

    printf ("M_LoadDefaults: Load system defaults.\n");
    M_LoadDefaults ();              // load before initing other systems

    printf ("W_Init: Init WADfiles.\n");
    WadManager::initMultipleFiles (wadfiles);
    

    // Check for -file in shareware
    if (modifiedgame)
    {
		// These are the lumps that will be checked in IWAD,
		// if any one is not present, execution will be aborted.
		char name[23][9]=
		{
		    "e2m1","e2m2","e2m3","e2m4","e2m5","e2m6","e2m7","e2m8","e2m9",
		    "e3m1","e3m3","e3m3","e3m4","e3m5","e3m6","e3m7","e3m8","e3m9",
		    "dphoof","bfgga0","heada1","cybra1","spida1d1"
		};
		int i;
		
		// Check for fake IWAD with right name,
		// but w/o all the lumps of the registered version. 
		if (Game::gamemode == GameMode_t::registered)
		    for (i = 0;i < 23; i++)
			if (WadManager::WadManager::checkNumForName(name[i])<0)
			    I_Error("\nThis is not the registered version.");
    }
    
    // Iff additonal PWAD files are used, print modified banner
    if (modifiedgame)
    {
		/*m*/printf (
		    "===========================================================================\n"
		    "ATTENTION:  This version of DOOM has been modified.  If you would like to\n"
		    "get a copy of the original game, call 1-800-IDGAMES or see the readme file.\n"
		    "        You will not receive technical support for modified games.\n"
		    "                      press enter to continue\n"
		    "===========================================================================\n"
		    );
		getchar ();
    }
	

    // Check and print which version is executed.
    switch (Game::gamemode )
    {
      case GameMode_t::shareware:
      case GameMode_t::indetermined:
		printf (
		    "===========================================================================\n"
		    "                                Shareware!\n"
		    "===========================================================================\n"
		);
		break;
      case GameMode_t::registered:
      case GameMode_t::retail:
      case GameMode_t::commercial:
		printf (
		    "===========================================================================\n"
		    "                 Commercial product - do not distribute!\n"
		    "         Please report software piracy to the SPA: 1-800-388-PIR8\n"
		    "===========================================================================\n"
		);
		break;
	
      default:
		break;
    }

    printf ("M_Init: Init miscellaneous info.\n");
    M_Init ();

    printf ("R_Init: Init DOOM refresh daemon - ");
    R_Init ();

    printf ("\nP_Init: Init Playloop state.\n");
    P_Init ();

    printf("\nI_InitGraphics: Set up window for rendering\n");
    I_InitGraphics();

    printf ("D_CheckNetGame: Checking network game status.\n");
    D_CheckNetGame ();

    printf ("Setting up sound.\n");
    I_Sound::initialise ();

    printf ("HU_Init: Setting up heads up display.\n");
    HU_Init ();

    printf ("ST_Init: Init status bar.\n");
    ST_Init ();
    
    // start the apropriate game based on parms
    p = CmdParameters::M_CheckParm ("-record");

    if (p && p < CmdParameters::myargc-1)
    {
		G_RecordDemo (const_cast<char*>(CmdParameters::myargv[p+1].c_str()));
		autostart = true;
    }
	
    p = CmdParameters::M_CheckParm ("-playdemo");
    if (p && p < CmdParameters::myargc-1)
    {
		singledemo = true;              // quit after one demo
		G_DeferedPlayDemo (const_cast<char*>(CmdParameters::myargv[p+1].c_str()));
		D_DoomLoop ();  // never returns
    }
	
    p = CmdParameters::M_CheckParm ("-timedemo");
    if (p && p < CmdParameters::myargc-1)
    {
		G_TimeDemo (const_cast<char*>(CmdParameters::myargv[p+1].c_str()));
		D_DoomLoop ();  // never returns
    }
	
    p = CmdParameters::M_CheckParm ("-loadgame");
    if (p && p < CmdParameters::myargc-1)
    {
		auto save = SAVEGAMENAME + CmdParameters::myargv[p+1] + ".dsg";
		G_LoadGame (save);
    }
	

    if ( gameaction != ga_loadgame )
    {
		if (autostart || netgame)
		    G_InitNew (startskill, startepisode, startmap);
		else
		    D_StartTitle ();                // start up intro loop
    }

    D_DoomLoop ();  // never returns
}
