
static const char
rcsid[] = "$Id: hu_stuff.c,v 1.4 1997/02/03 16:47:52 b1 Exp $";

#include <ctype.h>

#include "doomdef.hpp"

#include "i_sound.hpp"

#include "hu_stuff.hpp"
#include "hu_lib.hpp"
#include "w_wad.hpp"

#include "i_sound.hpp"
#include "g_game.hpp"

#include "doomstat.hpp"

// Data.
#include "dstrings.hpp"
#include "sounds.hpp"

//
// Locally used constants, shortcuts.
//
#define HU_TITLE	(mapnames[(gameepisode-1)*9+gamemap-1])
#define HU_TITLE2	(mapnames2[gamemap-1])
#define HU_TITLEP	(mapnamesp[gamemap-1])
#define HU_TITLET	(mapnamest[gamemap-1])
#define HU_TITLEHEIGHT	1
#define HU_TITLEX	0
#define HU_TITLEY	(167 - hu_font[0]->height)

#define HU_INPUTTOGGLE	't'
#define HU_INPUTX	HU_MSGX
#define HU_INPUTY	(HU_MSGY + HU_MSGHEIGHT*(hu_font[0]->height +1))
#define HU_INPUTWIDTH	64
#define HU_INPUTHEIGHT	1



const char*	chat_macros[] =
{
    s_ChatMacro0.c_str(),
    s_ChatMacro1.c_str(),
    s_ChatMacro2.c_str(),
    s_ChatMacro3.c_str(),
    s_ChatMacro4.c_str(),
    s_ChatMacro5.c_str(),
    s_ChatMacro6.c_str(),
    s_ChatMacro7.c_str(),
    s_ChatMacro8.c_str(),
    s_ChatMacro9.c_str()
};

const char*	player_names[] =
{
    s_PlayerGreen.c_str(),
    s_PlayerIndigo.c_str(),
    s_PlayerBrown.c_str(),
    s_PlayerRed.c_str()
};


char			chat_char; // remove later.
static player_t*	plr;
patch_t*		hu_font[HU_FONTSIZE];
static hu_textline_t	w_title;
bool			chat_on;
static hu_itext_t	w_chat;
static bool		always_off = false;
static char		chat_dest[MAXPLAYERS];
static hu_itext_t w_inputbuffer[MAXPLAYERS];

static bool		message_on;
bool			message_dontfuckwithme;
static bool		message_nottobefuckedwith;

static hu_stext_t	w_message;
static int		message_counter;

extern int		showMessages;
extern bool		automapactive;

static bool		headsupactive = false;

//
// Builtin map names.
// The actual names can be found in DStrings.h.
//

const char*	mapnames[] =	// DOOM shareware/registered/retail (Ultimate) names.
{

    s_E1M1.c_str(),
    s_E1M2.c_str(),
    s_E1M3.c_str(),
    s_E1M4.c_str(),
    s_E1M5.c_str(),
    s_E1M6.c_str(),
    s_E1M7.c_str(),
    s_E1M8.c_str(),
    s_E1M9.c_str(),

    s_E2M1.c_str(),
    s_E2M2.c_str(),
    s_E2M3.c_str(),
    s_E2M4.c_str(),
    s_E2M5.c_str(),
    s_E2M6.c_str(),
    s_E2M7.c_str(),
    s_E2M8.c_str(),
    s_E2M9.c_str(),

    s_E3M1.c_str(),
    s_E3M2.c_str(),
    s_E3M3.c_str(),
    s_E3M4.c_str(),
    s_E3M5.c_str(),
    s_E3M6.c_str(),
    s_E3M7.c_str(),
    s_E3M8.c_str(),
    s_E3M9.c_str(),

    s_E4M1.c_str(),
    s_E4M2.c_str(),
    s_E4M3.c_str(),
    s_E4M4.c_str(),
    s_E4M5.c_str(),
    s_E4M6.c_str(),
    s_E4M7.c_str(),
    s_E4M8.c_str(),
    s_E4M9.c_str(),

    "NEWLEVEL",
    "NEWLEVEL",
    "NEWLEVEL",
    "NEWLEVEL",
    "NEWLEVEL",
    "NEWLEVEL",
    "NEWLEVEL",
    "NEWLEVEL",
    "NEWLEVEL"
};

const char*	mapnames2[] =	// DOOM 2 map names.
{
    s_Level1.c_str(),
    s_Level2.c_str(),
    s_Level3.c_str(),
    s_Level4.c_str(),
    s_Level5.c_str(),
    s_Level6.c_str(),
    s_Level7.c_str(),
    s_Level8.c_str(),
    s_Level9.c_str(),
    s_Level10.c_str(),
    s_Level11.c_str(),
	
    s_Level12.c_str(),
    s_Level13.c_str(),
    s_Level14.c_str(),
    s_Level15.c_str(),
    s_Level16.c_str(),
    s_Level17.c_str(),
    s_Level18.c_str(),
    s_Level19.c_str(),
    s_Level20.c_str(),
	
    s_Level21.c_str(),
    s_Level22.c_str(),
    s_Level23.c_str(),
    s_Level24.c_str(),
    s_Level25.c_str(),
    s_Level26.c_str(),
    s_Level27.c_str(),
    s_Level28.c_str(),
    s_Level29.c_str(),
    s_Level30.c_str(),
    s_Level31.c_str(),
    s_Level32.c_str()
};


const char*	mapnamesp[] =	// Plutonia WAD map names.
{
    s_PlutLevel1.c_str(),
    s_PlutLevel2.c_str(),
    s_PlutLevel3.c_str(),
    s_PlutLevel4.c_str(),
    s_PlutLevel5.c_str(),
    s_PlutLevel6.c_str(),
    s_PlutLevel7.c_str(),
    s_PlutLevel8.c_str(),
    s_PlutLevel9.c_str(),
    s_PlutLevel10.c_str(),
    s_PlutLevel11.c_str(),
	
    s_PlutLevel12.c_str(),
    s_PlutLevel13.c_str(),
    s_PlutLevel14.c_str(),
    s_PlutLevel15.c_str(),
    s_PlutLevel16.c_str(),
    s_PlutLevel17.c_str(),
    s_PlutLevel18.c_str(),
    s_PlutLevel19.c_str(),
    s_PlutLevel20.c_str(),
	
    s_PlutLevel21.c_str(),
    s_PlutLevel22.c_str(),
    s_PlutLevel23.c_str(),
    s_PlutLevel24.c_str(),
    s_PlutLevel25.c_str(),
    s_PlutLevel26.c_str(),
    s_PlutLevel27.c_str(),
    s_PlutLevel28.c_str(),
    s_PlutLevel29.c_str(),
    s_PlutLevel30.c_str(),
    s_PlutLevel31.c_str(),
    s_PlutLevel32.c_str()
};


const char *mapnamest[] =	// TNT WAD map names.
{
    s_TNTLevel1.c_str(),
    s_TNTLevel2.c_str(),
    s_TNTLevel3.c_str(),
    s_TNTLevel4.c_str(),
    s_TNTLevel5.c_str(),
    s_TNTLevel6.c_str(),
    s_TNTLevel7.c_str(),
    s_TNTLevel8.c_str(),
    s_TNTLevel9.c_str(),
    s_TNTLevel10.c_str(),
    s_TNTLevel11.c_str(),
	
    s_TNTLevel12.c_str(),
    s_TNTLevel13.c_str(),
    s_TNTLevel14.c_str(),
    s_TNTLevel15.c_str(),
    s_TNTLevel16.c_str(),
    s_TNTLevel17.c_str(),
    s_TNTLevel18.c_str(),
    s_TNTLevel19.c_str(),
    s_TNTLevel20.c_str(),
	
    s_TNTLevel21.c_str(),
    s_TNTLevel22.c_str(),
    s_TNTLevel23.c_str(),
    s_TNTLevel24.c_str(),
    s_TNTLevel25.c_str(),
    s_TNTLevel26.c_str(),
    s_TNTLevel27.c_str(),
    s_TNTLevel28.c_str(),
    s_TNTLevel29.c_str(),
    s_TNTLevel30.c_str(),
    s_TNTLevel31.c_str(),
    s_TNTLevel32.c_str()
};


const char*	shiftxform;

const char french_shiftxform[] =
{
    0,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31,
    ' ', '!', '"', '#', '$', '%', '&',
    '"', // shift-'
    '(', ')', '*', '+',
    '?', // shift-,
    '_', // shift--
    '>', // shift-.
    '?', // shift-/
    '0', // shift-0
    '1', // shift-1
    '2', // shift-2
    '3', // shift-3
    '4', // shift-4
    '5', // shift-5
    '6', // shift-6
    '7', // shift-7
    '8', // shift-8
    '9', // shift-9
    '/',
    '.', // shift-;
    '<',
    '+', // shift-=
    '>', '?', '@',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '[', // shift-[
    '!', // shift-backslash - OH MY GOD DOES WATCOM SUCK
    ']', // shift-]
    '"', '_',
    '\'', // shift-`
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '{', '|', '}', '~', 127

};

const char english_shiftxform[] =
{

    0,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31,
    ' ', '!', '"', '#', '$', '%', '&',
    '"', // shift-'
    '(', ')', '*', '+',
    '<', // shift-,
    '_', // shift--
    '>', // shift-.
    '?', // shift-/
    ')', // shift-0
    '!', // shift-1
    '@', // shift-2
    '#', // shift-3
    '$', // shift-4
    '%', // shift-5
    '^', // shift-6
    '&', // shift-7
    '*', // shift-8
    '(', // shift-9
    ':',
    ':', // shift-;
    '<',
    '+', // shift-=
    '>', '?', '@',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '[', // shift-[
    '!', // shift-backslash - OH MY GOD DOES WATCOM SUCK
    ']', // shift-]
    '"', '_',
    '\'', // shift-`
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '{', '|', '}', '~', 127
};

char frenchKeyMap[128]=
{
    0,
    1,2,3,4,5,6,7,8,9,10,
    11,12,13,14,15,16,17,18,19,20,
    21,22,23,24,25,26,27,28,29,30,
    31,
    ' ','!','"','#','$','%','&','%','(',')','*','+',';','-',':','!',
    '0','1','2','3','4','5','6','7','8','9',':','M','<','=','>','?',
    '@','Q','B','C','D','E','F','G','H','I','J','K','L',',','N','O',
    'P','A','R','S','T','U','V','Z','X','Y','W','^','\\','$','^','_',
    '@','Q','B','C','D','E','F','G','H','I','J','K','L',',','N','O',
    'P','A','R','S','T','U','V','Z','X','Y','W','^','\\','$','^',127
};

char ForeignTranslation(unsigned char ch)
{
    return ch < 128 ? frenchKeyMap[ch] : ch;
}
#include <iomanip>
void HU_Init(void)
{

    int			i;
    int			j;

	shiftxform = english_shiftxform;

    // load the heads-up font
    j = HU_FONTSTART;
    for (i=0;i<HU_FONTSIZE;i++)
    {
        std::stringstream fontStream;
        fontStream << "STCFN" << std::setfill('0') << std::setw(3) << j++;
        hu_font[i] = (patch_t *)WadManager::getLump(fontStream.str());
    }

}

void HU_Stop(void)
{
    headsupactive = false;
}

void HU_Start(void)
{

    int		i;
    const char*	s;

    if (headsupactive)
	HU_Stop();

    plr = &players[consoleplayer];
    message_on = false;
    message_dontfuckwithme = false;
    message_nottobefuckedwith = false;
    chat_on = false;

    // create the message widget
    HUlib_initSText(&w_message,
		    HU_MSGX, HU_MSGY, HU_MSGHEIGHT,
		    hu_font,
		    HU_FONTSTART, &message_on);

    // create the map title widget
    HUlib_initTextLine(&w_title,
		       HU_TITLEX, HU_TITLEY,
		       hu_font,
		       HU_FONTSTART);
    
    switch (Game::gamemode )
    {
    case GameMode_t::shareware:
      case GameMode_t::registered:
      case GameMode_t::retail:
	s = HU_TITLE;
	break;
	
      case GameMode_t::commercial:
      default:
	 s = HU_TITLE2;
	 break;
    }
    
    while (*s)
	HUlib_addCharToTextLine(&w_title, *(s++));

    // create the chat widget
    HUlib_initIText(&w_chat,
		    HU_INPUTX, HU_INPUTY,
		    hu_font,
		    HU_FONTSTART, &chat_on);

    // create the inputbuffer widgets
    for (i=0 ; i<MAXPLAYERS ; i++)
	HUlib_initIText(&w_inputbuffer[i], 0, 0, 0, 0, &always_off);

    headsupactive = true;

}

void HU_Drawer(void)
{

    HUlib_drawSText(&w_message);
    HUlib_drawIText(&w_chat);
    if (automapactive)
	HUlib_drawTextLine(&w_title, false);

}

void HU_Erase(void)
{

    HUlib_eraseSText(&w_message);
    HUlib_eraseIText(&w_chat);
    HUlib_eraseTextLine(&w_title);

}

void HU_Ticker(void)
{

    int i, rc;
    char c;

    // tick down message counter if message is up
    if (message_counter && !--message_counter)
    {
	message_on = false;
	message_nottobefuckedwith = false;
    }

    if (showMessages || message_dontfuckwithme)
    {

	// display message if necessary
	if ((plr->message && !message_nottobefuckedwith)
	    || (plr->message && message_dontfuckwithme))
	{
	    HUlib_addMessageToSText(&w_message, 0, plr->message);
	    plr->message = 0;
	    message_on = true;
	    message_counter = HU_MSGTIMEOUT;
	    message_nottobefuckedwith = message_dontfuckwithme;
	    message_dontfuckwithme = 0;
	}

    } // else message_on = false;

    // check for incoming chat characters
    if (netgame)
    {
	for (i=0 ; i<MAXPLAYERS; i++)
	{
	    if (!playeringame[i])
		continue;
	    if (i != consoleplayer
		&& (c = players[i].cmd.chatchar))
	    {
		if (c <= HU_BROADCAST)
		    chat_dest[i] = c;
		else
		{
		    if (c >= 'a' && c <= 'z')
			c = (char) shiftxform[(unsigned char) c];
		    rc = HUlib_keyInIText(&w_inputbuffer[i], c);
		    if (rc && c == sf::Keyboard::Return)
		    {
			if (w_inputbuffer[i].l.len
			    && (chat_dest[i] == consoleplayer+1
				|| chat_dest[i] == HU_BROADCAST))
			{
			    HUlib_addMessageToSText(&w_message,
						    player_names[i],
						    w_inputbuffer[i].l.l);
			    
			    message_nottobefuckedwith = true;
			    message_on = true;
			    message_counter = HU_MSGTIMEOUT;
			    if (Game::gamemode == GameMode_t::commercial )
                    I_Sound::startSound(0, sfx_radio);
			    else
                    I_Sound::startSound(0, sfx_tink);
			}
			HUlib_resetIText(&w_inputbuffer[i]);
		    }
		}
		players[i].cmd.chatchar = 0;
	    }
	}
    }

}

#define QUEUESIZE		128

static char	chatchars[QUEUESIZE];
static int	head = 0;
static int	tail = 0;


void HU_queueChatChar(char c)
{
    if (((head + 1) & (QUEUESIZE-1)) == tail)
    {
	plr->message = s_MessageUnsent.c_str();
    }
    else
    {
	chatchars[head] = c;
	head = (head + 1) & (QUEUESIZE-1);
    }
}

char HU_dequeueChatChar(void)
{
    char c;

    if (head != tail)
    {
	c = chatchars[tail];
	tail = (tail + 1) & (QUEUESIZE-1);
    }
    else
    {
	c = 0;
    }

    return c;
}

bool HU_Responder(sf::Event *ev)
{

    static std::string		lastmessage;
    const char*		macromessage;
    bool		eatkey = false;
    static bool	shiftdown = false;
    static bool	altdown = false;
    unsigned char 	c;
    int			i;
    int			numplayers;
    
    //pretty brutal hack here, todo: remove this entirely
    static const char		destination_keys[MAXPLAYERS] =
    {
	    s_KeyGreen[0],
	    s_KeyIndigo[0],
	    s_KeyBrown[0],
	    s_KeyRed[0]
    };
    
    static int		num_nobrainers = 0;

    numplayers = 0;
    for (i=0 ; i<MAXPLAYERS ; i++)
	numplayers += playeringame[i];

    if (ev->key.code == sf::Keyboard::RShift)
    {
	shiftdown = ev->type == sf::Event::KeyPressed;
	return false;
    }
    else if (ev->key.code == sf::Keyboard::RAlt || ev->key.code == sf::Keyboard::LAlt)
    {
	altdown = ev->type == sf::Event::KeyPressed;
	return false;
    }

    if (ev->type != sf::Event::KeyPressed)
	return false;

    if (!chat_on)
    {
	if (ev->key.code == sf::Keyboard::Return)
	{
	    message_on = true;
	    message_counter = HU_MSGTIMEOUT;
	    eatkey = true;
	}
	else if (netgame && ev->key.code == HU_INPUTTOGGLE)
	{
	    eatkey = chat_on = true;
	    HUlib_resetIText(&w_chat);
	    HU_queueChatChar(HU_BROADCAST);
	}
	else if (netgame && numplayers > 2)
	{
	    for (i=0; i<MAXPLAYERS ; i++)
	    {
		if (ev->key.code == destination_keys[i])
		{
		    if (playeringame[i] && i!=consoleplayer)
		    {
			eatkey = chat_on = true;
			HUlib_resetIText(&w_chat);
			HU_queueChatChar(i+1);
			break;
		    }
		    else if (i == consoleplayer)
		    {
			num_nobrainers++;
			if (num_nobrainers < 3)
			    plr->message = s_TalkToSelf1.c_str();
			else if (num_nobrainers < 6)
			    plr->message = s_TalkToSelf.c_str();
			else if (num_nobrainers < 9)
			    plr->message = s_TalkToSelf3.c_str();
			else if (num_nobrainers < 32)
			    plr->message = s_TalkToSelf4.c_str();
			else
			    plr->message = s_TalkToSelf5.c_str();
		    }
		}
	    }
	}
    }
    else
    {
	c = ev->key.code;
	// send a macro
	if (altdown)
	{
	    c = c - '0';
	    if (c > 9)
		return false;
	    // fprintf(stderr, "got here\n");
	    macromessage = chat_macros[c];
	    
	    // kill last message with a '\n'
	    HU_queueChatChar(sf::Keyboard::Return); // DEBUG!!!
	    
	    // send the macro message
	    while (*macromessage)
		HU_queueChatChar(*macromessage++);
	    HU_queueChatChar(sf::Keyboard::Return);
	    
	    // leave chat mode and notify that it was sent
	    chat_on = false;
	    lastmessage =  chat_macros[c];
	    plr->message = lastmessage.c_str();
	    eatkey = true;
	}
	else
	{
	    if (shiftdown || (c >= 'a' && c <= 'z'))
		c = shiftxform[c];
	    eatkey = HUlib_keyInIText(&w_chat, c);
	    if (eatkey)
	    {
		// static unsigned char buf[20]; // DEBUG
		HU_queueChatChar(c);
		
		// sprintf(buf, "KEY: %d => %d", ev->data1, c);
		//      plr->message = buf;
	    }
	    if (c == sf::Keyboard::Return)
	    {
		chat_on = false;
		if (w_chat.l.len)
		{
		    lastmessage = w_chat.l.l;
		    plr->message = lastmessage.c_str();
		}
	    }
	    else if (c == sf::Keyboard::Escape)
		chat_on = false;
	}
    }

    return eatkey;

}
