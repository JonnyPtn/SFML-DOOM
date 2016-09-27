#define	BGCOLOR		7
#define	FGCOLOR		8

#include "doomdef.hpp"
#include "doomstat.hpp"

#include "dstrings.hpp"
#include "sounds.hpp"

#include "z_zone.hpp"
#include "w_wad.hpp"
#include "s_sound.hpp"
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

#include <malloc.h>
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

std::ofstream		 debugfile;

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
		I_SetPalette ((unsigned char*)W_CacheLumpName ("PLAYPAL",PU_CACHE));

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
				  y,0, (patch_t*)W_CacheLumpName ("M_PAUSE", PU_CACHE));
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
		char    filename[20];
		sprintf (filename,"debug%i.txt",consoleplayer);
		printf ("debug output to: %s\n",filename);
		debugfile.open (filename);
    }
	
    I_InitGraphics ();

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
		
		S_UpdateSounds (players[consoleplayer].mo);// move positional sounds

		// Update display, next frame, with current state.
		D_Display ();

		I_UpdateSound();
    }
}

//
//  DEMO LOOP
//
int             demosequence;
int             pagetic;
char                    *pagename;

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
    V_DrawPatch (0,0, 0, (patch_t*)W_CacheLumpName(pagename, PU_CACHE));
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

    if ( gamemode == retail )
		demosequence = (demosequence+1)%7;
    else
		demosequence = (demosequence+1)%6;
    
    switch (demosequence)
    {
    case 0:
		if ( gamemode == commercial )
		    pagetic = 35 * 11;
		else
		    pagetic = 170;
		gamestate = GS_DEMOSCREEN;
		pagename = "TITLEPIC";
		if ( gamemode == commercial )
		  S_StartMusic(mus_dm2ttl);
		else
		  S_StartMusic (mus_intro);
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
		if ( gamemode == commercial)
		{
		    pagetic = 35 * 11;
		    pagename = "TITLEPIC";
		    S_StartMusic(mus_dm2ttl);
		}
		else
		{
		    pagetic = 200;

		    if ( gamemode == retail )
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
char            title[128];

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
    char*	doom1wad;
    char*	doomwad;
    char*	doomuwad;
    char*	doom2wad;

    char*	doom2fwad;
    char*	plutoniawad;
    char*	tntwad;

    char *doomwaddir;
    doomwaddir = getenv("DOOMWADDIR");
    if (!doomwaddir)
	doomwaddir = ".";

    // Commercial.
    doom2wad = (char*)malloc(strlen(doomwaddir)+1+9+1);
    sprintf(doom2wad, "%s/doom2.wad", doomwaddir);

    // Retail.
    doomuwad = (char*)malloc(strlen(doomwaddir)+1+8+1);
    sprintf(doomuwad, "%s/doomu.wad", doomwaddir);
    
    // Registered.
    doomwad = (char*)malloc(strlen(doomwaddir)+1+8+1);
    sprintf(doomwad, "%s/doom.wad", doomwaddir);
    
    // Shareware.
    doom1wad = (char*)malloc(strlen(doomwaddir)+1+9+1);
    sprintf(doom1wad, "%s/doom1.wad", doomwaddir);

     // Bug, dear Shawn.
    // Insufficient malloc, caused spurious realloc errors.
    plutoniawad = (char*)malloc(strlen(doomwaddir)+1+/*9*/12+1);
    sprintf(plutoniawad, "%s/plutonia.wad", doomwaddir);

    tntwad = (char*)malloc(strlen(doomwaddir)+1+9+1);
    sprintf(tntwad, "%s/tnt.wad", doomwaddir);

	// French stuff.
    doom2fwad = (char*)malloc(strlen(doomwaddir)+1+10+1);
    sprintf(doom2fwad, "%s/doom2f.wad", doomwaddir);

    if ( !access (doom2fwad,R_OK) )
    {
		gamemode = commercial;
		// C'est ridicule!
		// Let's handle languages in config files, okay?
		language = french;
		printf("French version\n");
		D_AddFile (doom2fwad);
		return;
    }

    if ( !access (doom2wad,R_OK) )
    {
		gamemode = commercial;
		D_AddFile (doom2wad);
		return;
    }

    if ( !access (plutoniawad, R_OK ) )
    {
		gamemode = commercial;
		D_AddFile (plutoniawad);
		return;
    }

    if ( !access ( tntwad, R_OK ) )
    {
		gamemode = commercial;
		D_AddFile (tntwad);
		return;
    }

    if ( !access (doomuwad,R_OK) )
    {
		gamemode = retail;
		D_AddFile (doomuwad);
		return;
    }

    if ( !access (doomwad,R_OK) )
    {
		gamemode = registered;
		D_AddFile (doomwad);
		return;
    }

    if ( !access (doom1wad,R_OK) )
    {
		gamemode = shareware;
		D_AddFile (doom1wad);
		return;
    }

    printf("Game mode indeterminate.\n");
    gamemode = indetermined;

    // We don't abort. Let's see what the PWAD contains.
    //exit(1);
    //I_Error ("Game mode indeterminate\n");
}

//
// D_DoomMain
//
void D_DoomMain (void)
{
    int             p;
    char            file[256];

	
    IdentifyVersion ();
	
    setbuf (stdout, NULL);
    modifiedgame = false;
	
    nomonsters = CmdParameters::M_CheckParm ("-nomonsters")!=0;
    respawnparm = CmdParameters::M_CheckParm ("-respawn")!=0;
    fastparm = CmdParameters::M_CheckParm ("-fast")!=0;
    if (CmdParameters::M_CheckParm ("-altdeath") || CmdParameters::M_CheckParm("-deathmatch")	)
		deathmatch = true;

    switch ( gamemode )
    {
    case retail:
		sprintf (title,
			 "                         "
			 "The Ultimate DOOM Startup v%i.%i"
			 "                           ",
			 0,1);
		break;
    case shareware:
		sprintf (title,
			 "                            "
			 "DOOM Shareware Startup v%i.%i"
			 "                           ",
			 0,1);
		break;
    case registered:
		sprintf (title,
			 "                            "
			 "DOOM Registered Startup v%i.%i"
			 "                           ",
			0, 1);
		break;
    case commercial:
		sprintf (title,
			 "                         "
			 "DOOM 2: Hell on Earth v%i.%i"
			 "                           ",
			0, 1);
		break;
     default:
		sprintf (title,
			 "                     "
			 "Public DOOM - v%i.%i"
			 "                           ",
			0, 1);
		break;
    }
    
    printf ("%s\n",title);
    
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
    
    // add any files specified on the command line with -file wadfile
    // to the wad list
    //
    // convenience hack to allow -wart e m to add a wad file
    // prepend a tilde to the filename so wadfile will be reloadable
    p = CmdParameters::M_CheckParm ("-wart");
    if (p)
    {
		CmdParameters::myargv[p][4] = 'p';     // big hack, change to -warp

		// Map name handling.
		switch (gamemode )
		{
		case shareware:
		case retail:
		case registered:
		    sprintf (file,"~\"DEVMAPS\"E%cM%c.wad",
				CmdParameters::myargv[p+1][0], CmdParameters::myargv[p+2][0]);
		    printf("Warping to Episode %s, Map %s.\n",
				CmdParameters::myargv[p+1].c_str(), CmdParameters::myargv[p+2].c_str());
		    break;
		    
		case commercial:
		default:
		    p = atoi (CmdParameters::myargv[p+1].c_str());
		    if (p<10)
				sprintf (file,"~\"DEVMAPS\"cdata/map0%i.wad", p);
		    else
				sprintf (file,"~\"DEVMAPS\"cdata/map%i.wad", p);
		    break;
		}
		D_AddFile (file);
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
		sprintf (file,"%s.lmp", CmdParameters::myargv[p+1].c_str());
		D_AddFile (file);
		printf("Playing demo %s.lmp.\n", CmdParameters::myargv[p+1].c_str());
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

    printf ("Z_Init: Init zone memory allocation daemon. \n");
    Z_Init ();

    printf ("W_Init: Init WADfiles.\n");
    W_InitMultipleFiles (wadfiles);
    

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
		if (gamemode == registered)
		    for (i = 0;i < 23; i++)
			if (W_CheckNumForName(name[i])<0)
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
    switch ( gamemode )
    {
      case shareware:
      case indetermined:
		printf (
		    "===========================================================================\n"
		    "                                Shareware!\n"
		    "===========================================================================\n"
		);
		break;
      case registered:
      case retail:
      case commercial:
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

    printf ("I_Init: Setting up machine state.\n");
    I_Init ();

    printf ("D_CheckNetGame: Checking network game status.\n");
    D_CheckNetGame ();

    printf ("S_Init: Setting up sound.\n");
    S_Init (snd_SfxVolume /* *8 */, snd_MusicVolume /* *8*/ );

    printf ("HU_Init: Setting up heads up display.\n");
    HU_Init ();

    printf ("ST_Init: Init status bar.\n");
    ST_Init ();

    // check for a driver that wants intermission stats
    p = CmdParameters::M_CheckParm ("-statcopy");
    if (p && p<CmdParameters::myargc-1)
    {
		// for statistics driver
		extern  void*	statcopy;
		statcopy = (void*)atol(CmdParameters::myargv[p+1].c_str());
		printf ("External statistics registered.\n");
    }
    
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
		if (CmdParameters::M_CheckParm("-cdrom"))
		    sprintf(file, "c:\\doomdata\\\"SAVEGAMENAME\"%c.dsg", CmdParameters::myargv[p+1][0]);
		else
		    sprintf(file, SAVEGAMENAME"%c.dsg", CmdParameters::myargv[p+1][0]);
		G_LoadGame (file);
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