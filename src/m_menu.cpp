#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>

#include "doomdef.hpp"
#include "dstrings.hpp"

#include "d_main.hpp"

#include "i_system.hpp"
#include "i_video.hpp"
#include "v_video.hpp"
#include "w_wad.hpp"

#include "r_local.hpp"


#include "hu_stuff.hpp"

#include "g_game.hpp"

#include "m_argv.hpp"

#include "i_sound.hpp"

#include "doomstat.hpp"

// Data.
#include "sounds.hpp"

#include "m_menu.hpp"

extern patch_t*	hu_font[HU_FONTSIZE];
extern bool		message_dontfuckwithme;

extern bool		chat_on;		// in heads-up code

//
// defaulted values
//
int			mouseSensitivity;       // has default

// Show messages has default, 0 = off, 1 = on
int			showMessages;
	

// Blocky mode, has default, 0 = high, 1 = normal
int			detailLevel;		
int			screenblocks;		// has default

// temp for screenblocks (0-9)
int			screenSize;		

// -1 = no quicksave slot picked!
int			quickSaveSlot;          

 // 1 = message to be printed
int			messageToPrint;
// ...and here is the message string!
std::string	messageString;		

// message x & y
int			messx;			
int			messy;
int			messageLastMenuActive;

// timed message = no input from user
bool			messageNeedsInput;     

void    (*messageRoutine)(int response);

#define SAVESTRINGSIZE 	24

const char* gammamsg[5] =
{
    s_GammaLevelZero.c_str(),
    s_GammaLevelOne.c_str(),
    s_GammaLevelTwo.c_str(),
    s_GammaLevelThree.c_str(),
    s_GammaLevelFour.c_str()
};

// we are going to be entering a savegame string
int			saveStringEnter;              
int             	saveSlot;	// which slot to save in
int			saveCharIndex;	// which char we're editing
// old save description before edit
std::string			saveOldString;  

bool			inhelpscreens;
bool			menuactive;

#define SKULLXOFF		-32
#define LINEHEIGHT		16

extern bool		sendpause;
std::string		savegamestrings[10];

std::string	endstring;


//
// MENU TYPEDEFS
//
typedef struct
{
    // 0 = no cursor here, 1 = ok, 2 = arrows ok
    short	status;
    
    char	name[10];
    
    // choice = menu item #.
    // if status = 2,
    //   choice=0:leftarrow,1:rightarrow
    void	(*routine)(int choice);
    
    // hotkey in menu
    sf::Keyboard::Key	alphaKey;			
} menuitem_t;



typedef struct menu_s
{
    short		numitems;	// # of menu items
    struct menu_s*	prevMenu;	// previous menu
    menuitem_t*		menuitems;	// menu items
    void		(*routine)();	// draw routine
    short		x;
    short		y;		// x,y of menu
    short		lastOn;		// last item user was on in menu
} menu_t;

short		itemOn;			// menu item skull is on
short		skullAnimCounter;	// skull animation counter
short		whichSkull;		// which skull to draw

// graphic name of skulls
// warning: initializer-string for array of chars is too long
char    skullName[2][/*8*/9] = {"M_SKULL1","M_SKULL2"};

// current menudef
menu_t*	currentMenu;                          

//
// PROTOTYPES
//
void M_NewGame(int choice);
void M_Episode(int choice);
void M_ChooseSkill(int choice);
void M_LoadGame(int choice);
void M_SaveGame(int choice);
void M_Options(int choice);
void M_EndGame(int choice);
void M_ReadThis(int choice);
void M_ReadThis2(int choice);
void M_QuitDOOM(int choice);

void M_ChangeMessages(int choice);
void M_ChangeSensitivity(int choice);
void M_SfxVol(int choice);
void M_MusicVol(int choice);
void M_ChangeDetail(int choice);
void M_SizeDisplay(int choice);
void M_Sound(int choice);

void M_FinishReadThis(int choice);
void M_LoadSelect(int choice);
void M_SaveSelect(int choice);
void M_ReadSaveStrings(void);
void M_QuickSave(void);
void M_QuickLoad(void);

void M_DrawMainMenu(void);
void M_DrawReadThis1(void);
void M_DrawReadThis2(void);
void M_DrawNewGame(void);
void M_DrawEpisode(void);
void M_DrawOptions(void);
void M_DrawSound(void);
void M_DrawLoad(void);
void M_DrawSave(void);

void M_DrawSaveLoadBorder(int x,int y);
void M_SetupNextMenu(menu_t *menudef);
void M_DrawThermo(int x,int y,int thermWidth,int thermDot);
void M_DrawEmptyCell(menu_t *menu,int item);
void M_DrawSelCell(menu_t *menu,int item);
void M_WriteText(int x, int y, const std::string& string);
int  M_StringWidth(const std::string& string);
int  M_StringHeight(const std::string& string);
void M_StartControlPanel(void);
void M_StartMessage(const std::string& string, void(*routine)(int),bool input);
void M_StopMessage(void);
void M_ClearMenus (void);




//
// DOOM MENU
//
enum
{
    newgame = 0,
    options,
    loadgame,
    savegame,
    readthis,
    quitdoom,
    main_end
} main_e;

menuitem_t MainMenu[]=
{
    {1,"M_NGAME",M_NewGame,sf::Keyboard::N},
    {1,"M_OPTION",M_Options,sf::Keyboard::O},
    {1,"M_LOADG",M_LoadGame,sf::Keyboard::L},
    {1,"M_SAVEG",M_SaveGame,sf::Keyboard::S},
    // Another hickup with Special edition.
    {1,"M_RDTHIS",M_ReadThis,sf::Keyboard::R},
    {1,"M_QUITG",M_QuitDOOM,sf::Keyboard::Q}
};

menu_t  MainDef =
{
    main_end,
    NULL,
    MainMenu,
    M_DrawMainMenu,
    97,64,
    0
};


//
// EPISODE SELECT
//
enum
{
    ep1,
    ep2,
    ep3,
    ep4,
    ep_end
} episodes_e;

menuitem_t EpisodeMenu[]=
{
    {1,"M_EPI1", M_Episode,sf::Keyboard::K},
    {1,"M_EPI2", M_Episode,sf::Keyboard::T},
    {1,"M_EPI3", M_Episode,sf::Keyboard::I},
    {1,"M_EPI4", M_Episode,sf::Keyboard::T}
};

menu_t  EpiDef =
{
    ep_end,		// # of menu items
    &MainDef,		// previous menu
    EpisodeMenu,	// menuitem_t ->
    M_DrawEpisode,	// drawing routine ->
    48,63,              // x,y
    ep1			// lastOn
};

//
// NEW GAME
//
enum
{
    killthings,
    toorough,
    hurtme,
    violence,
    nightmare,
    newg_end
} newgame_e;

menuitem_t NewGameMenu[]=
{
    {1,"M_JKILL",	M_ChooseSkill, sf::Keyboard::I},
    {1,"M_ROUGH",	M_ChooseSkill, sf::Keyboard::H},
    {1,"M_HURT",	M_ChooseSkill, sf::Keyboard::H},
    {1,"M_ULTRA",	M_ChooseSkill, sf::Keyboard::U},
    {1,"M_NMARE",	M_ChooseSkill, sf::Keyboard::N}
};

menu_t  NewDef =
{
    newg_end,		// # of menu items
    &EpiDef,		// previous menu
    NewGameMenu,	// menuitem_t ->
    M_DrawNewGame,	// drawing routine ->
    48,63,              // x,y
    hurtme		// lastOn
};



//
// OPTIONS MENU
//
enum
{
    endgame,
    messages,
    detail,
    scrnsize,
    option_empty1,
    mousesens,
    option_empty2,
    soundvol,
    opt_end
} options_e;

menuitem_t OptionsMenu[]=
{
    {1,"M_ENDGAM",	M_EndGame,sf::Keyboard::E},
    {1,"M_MESSG",	M_ChangeMessages,sf::Keyboard::M},
    {1,"M_DETAIL",	M_ChangeDetail,sf::Keyboard::G},
    {2,"M_SCRNSZ",	M_SizeDisplay,sf::Keyboard::S},
    {-1,"",0},
    {2,"M_MSENS",	M_ChangeSensitivity,sf::Keyboard::M},
    {-1,"",0},
    {1,"M_SVOL",	M_Sound,sf::Keyboard::S}
};

menu_t  OptionsDef =
{
    opt_end,
    &MainDef,
    OptionsMenu,
    M_DrawOptions,
    60,37,
    0
};

//
// Read This! MENU 1 & 2
//
enum
{
    rdthsempty1,
    read1_end
} read_e;

menuitem_t ReadMenu1[] =
{
    {1,"",M_ReadThis2,sf::Keyboard::Unknown}
};

menu_t  ReadDef1 =
{
    read1_end,
    &MainDef,
    ReadMenu1,
    M_DrawReadThis1,
    280,185,
    0
};

enum
{
    rdthsempty2,
    read2_end
} read_e2;

menuitem_t ReadMenu2[]=
{
    {1,"",M_FinishReadThis,sf::Keyboard::Unknown}
};

menu_t  ReadDef2 =
{
    read2_end,
    &ReadDef1,
    ReadMenu2,
    M_DrawReadThis2,
    330,175,
    0
};

//
// SOUND VOLUME MENU
//
enum
{
    sfx_vol,
    sfx_empty1,
    music_vol,
    sfx_empty2,
    sound_end
} sound_e;

menuitem_t SoundMenu[]=
{
    {2,"M_SFXVOL",M_SfxVol,sf::Keyboard::S},
    {-1,"",0},
    {2,"M_MUSVOL",M_MusicVol,sf::Keyboard::M},
    {-1,"",0}
};

menu_t  SoundDef =
{
    sound_end,
    &OptionsDef,
    SoundMenu,
    M_DrawSound,
    80,64,
    0
};

//
// LOAD GAME MENU
//
enum
{
    load1,
    load2,
    load3,
    load4,
    load5,
    load6,
    load_end
} load_e;

menuitem_t LoadMenu[]=
{
    {1,"", M_LoadSelect,sf::Keyboard::Num1},
    {1,"", M_LoadSelect,sf::Keyboard::Num2},
    {1,"", M_LoadSelect,sf::Keyboard::Num3},
    {1,"", M_LoadSelect,sf::Keyboard::Num4},
    {1,"", M_LoadSelect,sf::Keyboard::Num5},
    {1,"", M_LoadSelect,sf::Keyboard::Num6}
};

menu_t  LoadDef =
{
    load_end,
    &MainDef,
    LoadMenu,
    M_DrawLoad,
    80,54,
    0
};

//
// SAVE GAME MENU
//
menuitem_t SaveMenu[]=
{
    {1,"", M_SaveSelect,sf::Keyboard::Num1},
    {1,"", M_SaveSelect,sf::Keyboard::Num2},
    {1,"", M_SaveSelect,sf::Keyboard::Num3},
    {1,"", M_SaveSelect,sf::Keyboard::Num4},
    {1,"", M_SaveSelect,sf::Keyboard::Num5},
    {1,"", M_SaveSelect,sf::Keyboard::Num6}
};

menu_t  SaveDef =
{
    load_end,
    &MainDef,
    SaveMenu,
    M_DrawSave,
    80,54,
    0
};


//
// M_ReadSaveStrings
//  read the strings from the savegame files
//
void M_ReadSaveStrings(void)
{
    int             i;
	
    for (i = 0;i < load_end;i++)
    {
		auto name = SAVEGAMENAME + std::to_string(i) + ".dsg";

        std::ifstream handle;
        handle.open(name, std::ios::binary);
		if (!handle.good())
		{
		    savegamestrings[i] = s_EmptySlot;
		    LoadMenu[i].status = 0;
		    continue;
		}
		std::array<char, SAVESTRINGSIZE> buffer = { {0} };
		handle.read (buffer.data(), SAVESTRINGSIZE);
		savegamestrings[i] = buffer.data();
		LoadMenu[i].status = 1;
    }
}


//
// M_LoadGame & Cie.
//
void M_DrawLoad(void)
{
    int             i;
	
    V_DrawPatchDirect (72,28,0,(patch_t*)WadManager::getLump("M_LOADG"));
    for (i = 0;i < load_end; i++)
    {
		M_DrawSaveLoadBorder(LoadDef.x,LoadDef.y+LINEHEIGHT*i);
		M_WriteText(LoadDef.x,LoadDef.y+LINEHEIGHT*i,const_cast<char*>(savegamestrings[i].c_str()));
    }
}



//
// Draw border for the savegame description
//
void M_DrawSaveLoadBorder(int x,int y)
{
    int             i;
	
    V_DrawPatchDirect (x-8,y+7,0, (patch_t*)WadManager::getLump("M_LSLEFT"));
	
    for (i = 0;i < 24;i++)
    {
		V_DrawPatchDirect (x,y+7,0, (patch_t*)WadManager::getLump("M_LSCNTR"));
		x += 8;
    }

    V_DrawPatchDirect (x,y+7,0, (patch_t*)WadManager::getLump("M_LSRGHT"));
}



//
// User wants to load this game
//
void M_LoadSelect(int choice)
{	
	auto name = SAVEGAMENAME + std::to_string(choice) + ".dsg";
    G_LoadGame (name);
    M_ClearMenus ();
}

//
// Selected from DOOM menu
//
void M_LoadGame (int choice)
{
    if (netgame)
    {
		M_StartMessage(s_CantLoadNet,NULL,false);
		return;
    }
	
    M_SetupNextMenu(&LoadDef);
    M_ReadSaveStrings();
}


//
//  M_SaveGame & Cie.
//
void M_DrawSave(void)
{
    int             i;
	
    V_DrawPatchDirect (72,28,0, (patch_t*)WadManager::getLump("M_SAVEG"));
    for (i = 0;i < load_end; i++)
    {
		M_DrawSaveLoadBorder(LoadDef.x,LoadDef.y+LINEHEIGHT*i);
		M_WriteText(LoadDef.x,LoadDef.y+LINEHEIGHT*i,const_cast<char*>(savegamestrings[i].c_str()));
    }
	
    if (saveStringEnter)
    {
		i = M_StringWidth(const_cast<char*>(savegamestrings[saveSlot].c_str()));
		M_WriteText(LoadDef.x + i,LoadDef.y+LINEHEIGHT*saveSlot,"_");
    }
}

//
// M_Responder calls this when user is finished
//
void M_DoSave(int slot)
{
    G_SaveGame (slot,savegamestrings[slot]);
    M_ClearMenus ();

    // PICK QUICKSAVE SLOT YET?
    if (quickSaveSlot == -2)
		quickSaveSlot = slot;
}

//
// User wants to save. Start string input for M_Responder
//
void M_SaveSelect(int choice)
{
    // we are going to be intercepting all chars
    saveStringEnter = 1;
    
    saveSlot = choice;
    saveOldString = savegamestrings[choice];
    if (savegamestrings[choice] == s_EmptySlot)
		savegamestrings[choice] = "";
	saveCharIndex = static_cast<int>(savegamestrings[choice].length());
}

//
// Selected from DOOM menu
//
void M_SaveGame (int choice)
{
    if (!usergame)
    {
	M_StartMessage(s_CantSaveDead,NULL,false);
	return;
    }
	
    if (gamestate != GS_LEVEL)
	return;
	
    M_SetupNextMenu(&SaveDef);
    M_ReadSaveStrings();
}



//
//      M_QuickSave
//
char    tempstring[80];

void M_QuickSaveResponse(int ch)
{
    if (ch == 'y')
    {
	M_DoSave(quickSaveSlot);
	I_Sound::startSound(NULL,sfx_swtchx);
    }
}

void M_QuickSave(void)
{
    if (!usergame)
    {
		I_Sound::startSound(NULL,sfx_oof);
		return;
    }

    if (gamestate != GS_LEVEL)
		return;
	
    if (quickSaveSlot < 0)
    {
		M_StartControlPanel();
		M_ReadSaveStrings();
		M_SetupNextMenu(&SaveDef);
		quickSaveSlot = -2;	// means to pick a slot now
		return;
    }
    sprintf(tempstring,s_QsaveConfirm.c_str(),savegamestrings[quickSaveSlot].c_str());
    M_StartMessage(tempstring,M_QuickSaveResponse,true);
}



//
// M_QuickLoad
//
void M_QuickLoadResponse(int ch)
{
    if (ch == 'y')
    {
		M_LoadSelect(quickSaveSlot);
        I_Sound::startSound(NULL,sfx_swtchx);
    }
}


void M_QuickLoad(void)
{
    if (netgame)
    {
		M_StartMessage(s_CantQloadNet,NULL,false);
		return;
    }
	
    if (quickSaveSlot < 0)
    {
		M_StartMessage(s_HaventPickedQsaveSlot,NULL,false);
		return;
    }
    sprintf(tempstring,s_QloadConfirm.c_str(),savegamestrings[quickSaveSlot].c_str());
    M_StartMessage(tempstring,M_QuickLoadResponse,true);
}




//
// Read This Menus
// Had a "quick hack to fix romero bug"
//
void M_DrawReadThis1(void)
{
    inhelpscreens = true;
    switch (Game::gamemode )
    {
      case GameMode_t::commercial:
		V_DrawPatchDirect (0,0,0, (patch_t*)WadManager::getLump("HELP"));
		break;
      case GameMode_t::shareware:
      case GameMode_t::registered:
      case GameMode_t::retail:
		V_DrawPatchDirect (0,0,0, (patch_t*)WadManager::getLump("HELP1"));
		break;
      default:
	break;
    }
    return;
}



//
// Read This Menus - optional second page.
//
void M_DrawReadThis2(void)
{
    inhelpscreens = true;
    switch (Game::gamemode )
    {
      case GameMode_t::retail:
      case GameMode_t::commercial:
		// This hack keeps us from having to change menus.
		V_DrawPatchDirect (0,0,0, (patch_t*)WadManager::getLump("CREDIT"));
		break;
      case GameMode_t::shareware:
      case GameMode_t::registered:
		V_DrawPatchDirect (0,0,0, (patch_t*)WadManager::getLump("HELP2"));
		break;
      default:
	break;
    }
    return;
}


//
// Change Sfx & Music volumes
//
void M_DrawSound(void)
{
    V_DrawPatchDirect (60,38,0, (patch_t*)WadManager::getLump("M_SVOL"));

    M_DrawThermo(SoundDef.x, SoundDef.y + LINEHEIGHT*(sfx_vol + 1),
        16, I_Sound::getSfxVolume());

    M_DrawThermo(SoundDef.x,SoundDef.y+LINEHEIGHT*(music_vol+1),
		 16,I_Sound::getMusicVolume());
}

void M_Sound(int choice)
{
    M_SetupNextMenu(&SoundDef);
}

void M_SfxVol(int choice)
{
    auto currentSfxVol = I_Sound::getSfxVolume();
    switch(choice)
    {
      case 0:
          if (currentSfxVol)
              --currentSfxVol;
	    break;

      case 1:
          if (currentSfxVol < 15)
              ++currentSfxVol;
	    break;
    }
	
    I_Sound::setSfxVolume(currentSfxVol);
}

void M_MusicVol(int choice)
{
    auto currentMusVol = I_Sound::getMusicVolume();
    switch(choice)
    {
      case 0:
	if (currentMusVol)
        currentMusVol--;
	break;
      case 1:
	if (currentMusVol < 15)
        currentMusVol++;
	break;
    }
	
    I_Sound::setMusicVolume(currentMusVol);
}




//
// M_DrawMainMenu
//
void M_DrawMainMenu(void)
{
    V_DrawPatchDirect (94,2,0, (patch_t*)WadManager::getLump("M_DOOM"));
}




//
// M_NewGame
//
void M_DrawNewGame(void)
{
    V_DrawPatchDirect (96,14,0, (patch_t*)WadManager::getLump("M_NEWG"));
    V_DrawPatchDirect (54,38,0, (patch_t*)WadManager::getLump("M_SKILL"));
}

void M_NewGame(int choice)
{
    if (netgame && !demoplayback)
    {
	M_StartMessage(s_CantNewNet,NULL,false);
	return;
    }

	M_SetupNextMenu(&EpiDef);
}


//
//      M_Episode
//
int     epi;

void M_DrawEpisode(void)
{
    V_DrawPatchDirect (54,38,0, (patch_t*)WadManager::getLump("M_EPISOD"));
}

void M_VerifyNightmare(int ch)
{
    if (ch != 'y')
	return;
		
    G_DeferredInitNew((skill_t)nightmare,epi+1,1);
    M_ClearMenus ();
}

void M_ChooseSkill(int choice)
{
    if (choice == nightmare)
    {
	M_StartMessage( s_NightmareConfirm,M_VerifyNightmare,true);
	return;
    }
	
    G_DeferredInitNew((skill_t)choice,epi+1,1);
    M_ClearMenus ();
}

void M_Episode(int choice)
{	 
	if ((Game::gamemode == GameMode_t::shareware)
		&& choice)
	{
		M_StartMessage(s_Shareware, NULL, false);
		M_SetupNextMenu(&ReadDef1);
		return;
	}

	// Yet another hack...
	if ((Game::gamemode == GameMode_t::registered)
		&& (choice > 2))
	{
		fprintf(stderr,
			"M_Episode: 4th episode requires UltimateDOOM\n");
		choice = 0;
	}
    epi = choice;
    M_SetupNextMenu(&NewDef);
}



//
// M_Options
//
char    detailNames[2][9]	= {"M_GDHIGH","M_GDLOW"};
char	msgNames[2][9]		= {"M_MSGOFF","M_MSGON"};


void M_DrawOptions(void)
{
    V_DrawPatchDirect (108,15,0, (patch_t*)WadManager::getLump("M_OPTTTL"));
	
    V_DrawPatchDirect (OptionsDef.x + 175,OptionsDef.y+LINEHEIGHT*detail,0,
		(patch_t*)WadManager::getLump(detailNames[detailLevel]));

    V_DrawPatchDirect (OptionsDef.x + 120,OptionsDef.y+LINEHEIGHT*messages,0,
		(patch_t*)WadManager::getLump(msgNames[showMessages]));

    M_DrawThermo(OptionsDef.x,OptionsDef.y+LINEHEIGHT*(mousesens+1),
		 10,mouseSensitivity);
	
    M_DrawThermo(OptionsDef.x,OptionsDef.y+LINEHEIGHT*(scrnsize+1),
		 9,screenSize);
}

void M_Options(int choice)
{
    M_SetupNextMenu(&OptionsDef);
}



//
//      Toggle messages on/off
//
void M_ChangeMessages(int choice)
{
    // warning: unused parameter `int choice'
    choice = 0;
    showMessages = 1 - showMessages;
	
    if (!showMessages)
	players[consoleplayer].message = s_MessagesOff.c_str();
    else
	players[consoleplayer].message = s_MessagesOn.c_str();

    message_dontfuckwithme = true;
}


//
// M_EndGame
//
void M_EndGameResponse(int ch)
{
    if (ch != 'y')
	return;
		
    currentMenu->lastOn = itemOn;
    M_ClearMenus ();
    D_StartTitle ();
}

void M_EndGame(int choice)
{
    choice = 0;
    if (!usergame)
    {
        I_Sound::startSound(NULL,sfx_oof);
	    return;
    }
	
    if (netgame)
    {
	M_StartMessage(s_CantEndNet,NULL,false);
	return;
    }
	
    M_StartMessage(s_EndConfirm,M_EndGameResponse,true);
}




//
// M_ReadThis
//
void M_ReadThis(int choice)
{
    choice = 0;
    M_SetupNextMenu(&ReadDef1);
}

void M_ReadThis2(int choice)
{
    choice = 0;
    M_SetupNextMenu(&ReadDef2);
}

void M_FinishReadThis(int choice)
{
    choice = 0;
    M_SetupNextMenu(&MainDef);
}




//
// M_QuitDOOM
//
int     quitsounds[8] =
{
    sfx_pldeth,
    sfx_dmpain,
    sfx_popain,
    sfx_slop,
    sfx_telept,
    sfx_posit1,
    sfx_posit3,
    sfx_sgtatk
};

int     quitsounds2[8] =
{
    sfx_vilact,
    sfx_getpow,
    sfx_boscub,
    sfx_slop,
    sfx_skeswg,
    sfx_kntdth,
    sfx_bspact,
    sfx_sgtatk
};



void M_QuitResponse(int ch)
{
    if (ch != 'y')
	return;
    if (!netgame)
    {
	if (Game::gamemode == GameMode_t::commercial)
        I_Sound::startSound(NULL,quitsounds2[(gametic>>2)&7]);
	else
        I_Sound::startSound(NULL,quitsounds[(gametic>>2)&7]);
	I_WaitVBL(105);
    }
    I_Quit ();
}




void M_QuitDOOM(int choice)
{
    endstring = endmsg[ (gametic%(NUM_QUITMESSAGES-2))+1 ] + "\n\n" + std::string(s_YtoQuit);
  
  M_StartMessage(endstring,M_QuitResponse,true);
}




void M_ChangeSensitivity(int choice)
{
    switch(choice)
    {
      case 0:
	if (mouseSensitivity)
	    mouseSensitivity--;
	break;
      case 1:
	if (mouseSensitivity < 9)
	    mouseSensitivity++;
	break;
    }
}




void M_ChangeDetail(int choice)
{
    choice = 0;
    detailLevel = 1 - detailLevel;

    // FIXME - does not work. Remove anyway?
    fprintf( stderr, "M_ChangeDetail: low detail mode n.a.\n");

    return;
    
    /*R_SetViewSize (screenblocks, detailLevel);

    if (!detailLevel)
	players[consoleplayer].message = DETAILHI;
    else
	players[consoleplayer].message = DETAILLO;*/
}




void M_SizeDisplay(int choice)
{
    switch(choice)
    {
      case 0:
	if (screenSize > 0)
	{
	    screenblocks--;
	    screenSize--;
	}
	break;
      case 1:
	if (screenSize < 8)
	{
	    screenblocks++;
	    screenSize++;
	}
	break;
    }
	

    R_SetViewSize (screenblocks, detailLevel);
}




//
//      Menu Functions
//
void
M_DrawThermo
( int	x,
  int	y,
  int	thermWidth,
  int	thermDot )
{
    int		xx;
    int		i;

    xx = x;
    V_DrawPatchDirect (xx,y,0,(patch_t*)WadManager::getLump("M_THERML"));
    xx += 8;
    for (i=0;i<thermWidth;i++)
    {
	V_DrawPatchDirect (xx,y,0, (patch_t*)WadManager::getLump("M_THERMM"));
	xx += 8;
    }
    V_DrawPatchDirect (xx,y,0, (patch_t*)WadManager::getLump("M_THERMR"));

    V_DrawPatchDirect ((x+8) + thermDot*8,y,
		       0, (patch_t*)WadManager::getLump("M_THERMO"));
}



void
M_DrawEmptyCell
( menu_t*	menu,
  int		item )
{
    V_DrawPatchDirect (menu->x - 10,        menu->y+item*LINEHEIGHT - 1, 0,
		(patch_t*)WadManager::getLump("M_CELL1"));
}

void
M_DrawSelCell
( menu_t*	menu,
  int		item )
{
    V_DrawPatchDirect (menu->x - 10,        menu->y+item*LINEHEIGHT - 1, 0,
		(patch_t*)WadManager::getLump("M_CELL2"));
}


void M_StartMessage( const std::string& string, void(*routine)(int), bool input )
{
    messageLastMenuActive = menuactive;
    messageToPrint = 1;
    messageString = string;
	messageRoutine = routine;
    messageNeedsInput = input;
    menuactive = true;
    return;
}



void M_StopMessage(void)
{
    menuactive = messageLastMenuActive!=0;
    messageToPrint = 0;
}



//
// Find string width from hu_font chars
//
int M_StringWidth(const std::string& string)
{
    unsigned int             i;
    int             w = 0;
    int             c;
	
    for (i = 0;i < string.length();i++)
    {
	c = toupper(string[i]) - HU_FONTSTART;
	if (c < 0 || c >= HU_FONTSIZE)
	    w += 4;
	else
	    w += hu_font[c]->width;
    }
		
    return w;
}



//
//      Find string height from hu_font chars
//
int M_StringHeight(const std::string& string)
{
    unsigned int    i;
    int             h;
    int             height = hu_font[0]->height;
	
    h = height;
    for (auto& character : string)
		if (character == '\n')
			h += height;
		
    return h;
}


//
//      Write a string using the hu_font
//
void M_WriteText(int x, int y, const std::string& string)
{
    int cx(x), cy(y);

    for (auto c : string)
    {
        if(c == '\n')
        {
            cx = x;
            cy += 12; // Abracadabra
            continue;
        }

        c = toupper(c) - HU_FONTSTART;
        if(c < 0 || c >= HU_FONTSIZE)
        {
            cx += 4;
            continue;
        }

        auto w = hu_font[c]->width;
        if(cx + w > SCREENWIDTH)
            break;
        
        V_DrawPatchDirect(cx, cy, 0, hu_font[c]);
	    cx += w;
    }
}



//
// CONTROL PANEL
//

//
// M_Responder
//
bool M_Responder (sf::Event* ev)
{
    int             ch;
    int             i;
    static  int     joywait = 0;
    static  int     mousewait = 0;
    static  int     mousey = 0;
    static  int     lasty = 0;
    static  int     mousex = 0;
    static  int     lastx = 0;
	
    ch = -1;
	
	//JONNY//
	if (ev->type != sf::Event::KeyPressed 
		&& ev->type != sf::Event::TextEntered 
		&& ev->type != sf::Event::JoystickMoved
		&& ev->type != sf::Event::JoystickButtonPressed)
		return false;
	else if (ev->type == sf::Event::TextEntered)
		ch = ev->text.unicode;

	if (ev->type == sf::Event::KeyPressed)
	{
		ch = ev->key.code;
		//always check for fullscreen toggle first
		if (ev->key.code == sf::Keyboard::F &&
			ev->key.control)
		{
			toggleFullscreen();
			return true;
		}
	}
	else if (ev->type == sf::Event::JoystickMoved)
	{
		static bool joystickHeld(false);
		static const float joyDeadZone(40.f);
		switch (ev->joystickMove.axis)
		{
		case sf::Joystick::Axis::Y:
			if (ev->joystickMove.position < -joyDeadZone)
			{
				if (!joystickHeld)
				{
					ch = sf::Keyboard::Up;
					joystickHeld = true;
				}
			}
			else if (ev->joystickMove.position > joyDeadZone)
			{
				if (!joystickHeld)
				{
					ch = sf::Keyboard::Down;
					joystickHeld = true;
				}
			}
			else if (joystickHeld)
			{
				joystickHeld = false;
			}
			break;

		default:
			break;
		}
	}
	else if (ev->type == sf::Event::JoystickButtonPressed)
	{
		if (ev->joystickButton.button == 0) //a
			ch = sf::Keyboard::Return;
		else if (ev->joystickButton.button == 1) //b
			ch = sf::Keyboard::BackSpace;

	}
    
    if (ch == -1)
		return false;

    
    // Save Game string input
    if (saveStringEnter)
    {
	switch(ev->key.code)
	{
	  case sf::Keyboard::BackSpace:
	    if (saveCharIndex > 0)
	    {
			saveCharIndex--;
			savegamestrings[saveSlot][saveCharIndex] = 0;
	    }
	    break;
				
	  case sf::Keyboard::Escape:
	    saveStringEnter = 0;
	    savegamestrings[saveSlot] = saveOldString;
	    break;
				
	  case sf::Keyboard::Return:
	    saveStringEnter = 0;
	    if (savegamestrings[saveSlot][0])
		M_DoSave(saveSlot);
	    break;
				
	  default:
	    ch = toupper(ch);
	    if (ch != 32)
		if (ch-HU_FONTSTART < 0 || ch-HU_FONTSTART >= HU_FONTSIZE)
		    break;
	    if (ch >= 32 && ch <= 127 &&
		saveCharIndex < SAVESTRINGSIZE-1 &&
		M_StringWidth(const_cast<char*>(savegamestrings[saveSlot].c_str())) <
		(SAVESTRINGSIZE-2)*8)
	    {
		savegamestrings[saveSlot][saveCharIndex++] = ch;
		savegamestrings[saveSlot][saveCharIndex] = 0;
	    }
	    break;
	}
	return true;
    }
    
    // Take care of any messages that need input
    if (messageToPrint)
    {
	if (messageNeedsInput == true &&
	    !(ch == ' ' || ch == 'n' || ch == 'y' || ch == sf::Keyboard::Escape))
	    return false;
		
	menuactive = messageLastMenuActive!=0;
	messageToPrint = 0;
	if (messageRoutine)
	    messageRoutine(ch);
			
	menuactive = false;
    I_Sound::startSound(NULL,sfx_swtchx);
	return true;
    }

    // F-Keys
    if (!menuactive && ev->type == sf::Event::KeyPressed)
	switch(ev->key.code)
	{
	case sf::Keyboard::Dash:         // Screen size down
	    if (automapactive || chat_on)
		return false;
	    M_SizeDisplay(0);
        I_Sound::startSound(NULL,sfx_stnmov);
	    return true;
				
	case sf::Keyboard::Equal:        // Screen size up
	    if (automapactive || chat_on)
		return false;
	    M_SizeDisplay(1);
        I_Sound::startSound(NULL,sfx_stnmov);
	    return true;
				
	case sf::Keyboard::F1:            // Help key
	    M_StartControlPanel ();

	    if (Game::gamemode == GameMode_t::retail )
	      currentMenu = &ReadDef2;
	    else
	      currentMenu = &ReadDef1;
	    
	    itemOn = 0;
        I_Sound::startSound(NULL,sfx_swtchn);
	    return true;
				
	case sf::Keyboard::F2:            // Save
	    M_StartControlPanel();
        I_Sound::startSound(NULL,sfx_swtchn);
	    M_SaveGame(0);
	    return true;
				
	case sf::Keyboard::F3:            // Load
	    M_StartControlPanel();
        I_Sound::startSound(NULL,sfx_swtchn);
	    M_LoadGame(0);
	    return true;
				
	case sf::Keyboard::F4:            // Sound Volume
	    M_StartControlPanel ();
	    currentMenu = &SoundDef;
	    itemOn = sfx_vol;
        I_Sound::startSound(NULL,sfx_swtchn);
	    return true;
				
	  case sf::Keyboard::F5:            // Detail toggle
	    M_ChangeDetail(0);
        I_Sound::startSound(NULL,sfx_swtchn);
	    return true;
				
	  case sf::Keyboard::F6:            // Quicksave
          I_Sound::startSound(NULL,sfx_swtchn);
	    M_QuickSave();
	    return true;
				
	  case sf::Keyboard::F7:            // End game
          I_Sound::startSound(NULL,sfx_swtchn);
	    M_EndGame(0);
	    return true;
				
	  case sf::Keyboard::F8:            // Toggle messages
	    M_ChangeMessages(0);
        I_Sound::startSound(NULL,sfx_swtchn);
	    return true;
				
	  case sf::Keyboard::F9:            // Quickload
          I_Sound::startSound(NULL,sfx_swtchn);
	    M_QuickLoad();
	    return true;
				
	  case sf::Keyboard::F10:           // Quit DOOM
          I_Sound::startSound(NULL,sfx_swtchn);
	    M_QuitDOOM(0);
	    return true;
				
	  case sf::Keyboard::F11:           // gamma toggle
	    usegamma++;
	    if (usegamma > 4)
		usegamma = 0;
	    players[consoleplayer].message = gammamsg[usegamma];
	    I_SetPalette ((unsigned char*)WadManager::getLump ("PLAYPAL"));
	    return true;
				
	}

    
    // Pop-up menu?
    if (!menuactive)
    {
	if (ch == sf::Keyboard::Escape)
	{
	    M_StartControlPanel ();
        I_Sound::startSound(NULL,sfx_swtchn);
	    return true;
	}
	return false;
    }

    
    // Keys usable within menu
	if (menuactive && ev->type == sf::Event::KeyPressed)
	{
		switch (ev->key.code)
		{
		case sf::Keyboard::Down:
			do
			{
				if (itemOn + 1 > currentMenu->numitems - 1)
					itemOn = 0;
				else itemOn++;
                I_Sound::startSound(NULL, sfx_pstop);
			} while (currentMenu->menuitems[itemOn].status == -1);
			return true;

		case sf::Keyboard::Up:
			do
			{
				if (!itemOn)
					itemOn = currentMenu->numitems - 1;
				else itemOn--;
                I_Sound::startSound(NULL, sfx_pstop);
			} while (currentMenu->menuitems[itemOn].status == -1);
			return true;

		case sf::Keyboard::Left:
			if (currentMenu->menuitems[itemOn].routine &&
				currentMenu->menuitems[itemOn].status == 2)
			{
                I_Sound::startSound(NULL, sfx_stnmov);
				currentMenu->menuitems[itemOn].routine(0);
			}
			return true;

		case sf::Keyboard::Right:
			if (currentMenu->menuitems[itemOn].routine &&
				currentMenu->menuitems[itemOn].status == 2)
			{
                I_Sound::startSound(NULL, sfx_stnmov);
				currentMenu->menuitems[itemOn].routine(1);
			}
			return true;

		case sf::Keyboard::Return:
			if (currentMenu->menuitems[itemOn].routine &&
				currentMenu->menuitems[itemOn].status)
			{
				currentMenu->lastOn = itemOn;
				if (currentMenu->menuitems[itemOn].status == 2)
				{
					currentMenu->menuitems[itemOn].routine(1);      // right arrow
                    I_Sound::startSound(NULL, sfx_stnmov);
				}
				else
				{
					currentMenu->menuitems[itemOn].routine(itemOn);
                    I_Sound::startSound(NULL, sfx_pistol);
				}
			}
			return true;

		case sf::Keyboard::Escape:
			currentMenu->lastOn = itemOn;
			M_ClearMenus();
            I_Sound::startSound(NULL, sfx_swtchx);
			return true;

		case sf::Keyboard::BackSpace:
			currentMenu->lastOn = itemOn;
			if (currentMenu->prevMenu)
			{
				currentMenu = currentMenu->prevMenu;
				itemOn = currentMenu->lastOn;
                I_Sound::startSound(NULL, sfx_swtchn);
			}
			return true;

		default:
			for (i = itemOn + 1; i < currentMenu->numitems; i++)
				if (currentMenu->menuitems[i].alphaKey == ev->key.code)
				{
					itemOn = i;
                    I_Sound::startSound(NULL, sfx_pstop);
					return true;
				}
			for (i = 0; i <= itemOn; i++)
				if (currentMenu->menuitems[i].alphaKey == ev->key.code)
				{
					itemOn = i;
                    I_Sound::startSound(NULL, sfx_pstop);
					return true;
				}
			break;
		}
	}

    return false;
}



//
// M_StartControlPanel
//
void M_StartControlPanel (void)
{
    // intro might call this repeatedly
    if (menuactive)
	return;
    
    menuactive = 1;
    currentMenu = &MainDef;         // JDC
    itemOn = currentMenu->lastOn;   // JDC
}


//
// M_Drawer
// Called after the view has been rendered,
// but before it has been blitted.
//
void M_Drawer (void)
{
    static short	x;
    static short	y;
    unsigned short		i;
    short		max;
    std::string		string;
    int			start;

    inhelpscreens = false;

    
    // Horiz. & Vertically center string and print it.
    if (messageToPrint)
    {
		start = 0;
		y = 100 - M_StringHeight(messageString)/2;
		for (auto character : messageString)
		{
			if (character == '\n')
			{
				//new line, write the line then clear it
				x = 160 - M_StringWidth(const_cast<char*>(string.c_str())) / 2;
				M_WriteText(x, y, const_cast<char*>(string.c_str()));
				string.clear();
				y += hu_font[0]->height;
			}
			else
				string.push_back(character);
		}

		//in case there's no new line at the end of the string
		//show the final line
		if (!string.empty())
		{
			x = 160 - M_StringWidth(const_cast<char*>(string.c_str())) / 2;
			M_WriteText(x, y, const_cast<char*>(string.c_str()));
		}
		return;
    }

    if (!menuactive)
	return;

    if (currentMenu->routine)
	currentMenu->routine();         // call Draw routine
    
    // DRAW MENU
    x = currentMenu->x;
    y = currentMenu->y;
    max = currentMenu->numitems;

    for (i=0;i<max;i++)
    {
	if (currentMenu->menuitems[i].name[0])
	    V_DrawPatchDirect (x,y,0,
			(patch_t*)WadManager::getLump(currentMenu->menuitems[i].name ));
	y += LINEHEIGHT;
    }

    
    // DRAW SKULL
    V_DrawPatchDirect(x + SKULLXOFF,currentMenu->y - 5 + itemOn*LINEHEIGHT, 0,
		(patch_t*)WadManager::getLump(skullName[whichSkull]));

}


//
// M_ClearMenus
//
void M_ClearMenus (void)
{
    menuactive = 0;
    // if (!netgame && usergame && paused)
    //       sendpause = true;
}




//
// M_SetupNextMenu
//
void M_SetupNextMenu(menu_t *menudef)
{
    currentMenu = menudef;
    itemOn = currentMenu->lastOn;
}


//
// M_Ticker
//
void M_Ticker (void)
{
    if (--skullAnimCounter <= 0)
    {
	whichSkull ^= 1;
	skullAnimCounter = 8;
    }
}


//
// M_Init
//
void M_Init (void)
{
    currentMenu = &MainDef;
    menuactive = 0;
    itemOn = currentMenu->lastOn;
    whichSkull = 0;
    skullAnimCounter = 10;
    screenSize = screenblocks - 3;
    messageToPrint = 0;
    messageString = "";
    messageLastMenuActive = menuactive;
    quickSaveSlot = -1;

    // Here we could catch other version dependencies,
    //  like HELP1/2, and four episodes.

  
    switch (Game::gamemode )
    {
      case GameMode_t::commercial:
	// This is used because DOOM 2 had only one HELP
        //  page. I use CREDIT as second page now, but
	//  kept this hack for educational purposes.
	MainMenu[readthis] = MainMenu[quitdoom];
	MainDef.numitems--;
	MainDef.y += 8;
	NewDef.prevMenu = &MainDef;
	ReadDef1.routine = M_DrawReadThis1;
	ReadDef1.x = 330;
	ReadDef1.y = 165;
	ReadMenu1[0].routine = M_FinishReadThis;
	break;
      case GameMode_t::shareware:
	// Episode 2 and 3 are handled,
	//  branching to an ad screen.
      case GameMode_t::registered:
	// We need to remove the fourth episode.
	EpiDef.numitems--;
	break;
      case GameMode_t::retail:
	// We are fine.
      default:
	break;
    }
    
}

