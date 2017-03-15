#include <sys/stat.h>
#include <sys/types.h>
#include <chrono>
#include <fcntl.h>
#include <stdlib.h>

#include <ctype.h>


#include "doomdef.hpp"

#include "m_argv.hpp"

#include "w_wad.hpp"

#include "i_system.hpp"
#include "i_video.hpp"
#include "v_video.hpp"

#include "hu_stuff.hpp"

// State.
#include "doomstat.hpp"

// Data.
#include "dstrings.hpp"

#include "m_misc.hpp"

//
// M_DrawText
// Returns the final X coordinate
// HU_Init must have been called to init the font
//
extern patch_t*		hu_font[HU_FONTSIZE];

int
M_DrawText
( int		x,
  int		y,
  bool	direct,
  char*		string )
{
    int 	c;
    int		w;

    while (*string)
    {
	c = toupper(*string) - HU_FONTSTART;
	string++;
	if (c < 0 || c> HU_FONTSIZE)
	{
	    x += 4;
	    continue;
	}
		
	w = hu_font[c]->width;
	if (x+w > SCREENWIDTH)
	    break;
	if (direct)
	    V_DrawPatchDirect(x, y, 0, hu_font[c]);
	else
	    V_DrawPatch(x, y, 0, hu_font[c]);
	x+=w;
    }

    return x;
}

//
// DEFAULTS
//
int		usemouse;
int		usejoystick;

extern int	key_right;
extern int	key_left;
extern int	key_up;
extern int	key_down;

extern int	key_strafeleft;
extern int	key_straferight;

extern int	key_fire;
extern int	key_use;
extern int	key_strafe;
extern int	key_speed;

extern int	mousebfire;
extern int	mousebstrafe;
extern int	mousebforward;

extern int	joybuse;
extern int	joybspeed;

extern int	viewwidth;
extern int	viewheight;

extern int	mouseSensitivity;
extern int	showMessages;

extern int	detailLevel;

extern int	screenblocks;

extern int	showMessages;

extern const char*	chat_macros[];



typedef struct
{
    const char*	name;
    int*	location;
    std::intptr_t	defaultvalue;
    int		scantranslate;		// PC scan code hack
    int		untranslated;		// lousy hack
} default_t;

default_t	defaults[] =
{
    {"mouse_sensitivity",&mouseSensitivity, 5},
    {"show_messages",&showMessages, 1},

    {"use_mouse",&usemouse, 1},
    {"mouseb_fire",&mousebfire,0},
    {"mouseb_strafe",&mousebstrafe,1},
    {"mouseb_forward",&mousebforward,2},

    {"use_joystick",&usejoystick, 0},

    {"screenblocks",&screenblocks, 10},
    {"detaillevel",&detailLevel, 0},

    {"usegamma",&usegamma, 0},

    {"chatmacro0", (int *) &chat_macros[0], (std::intptr_t) s_ChatMacro0.c_str() },
    {"chatmacro1", (int *) &chat_macros[1], (std::intptr_t) s_ChatMacro1.c_str() },
    {"chatmacro2", (int *) &chat_macros[2], (std::intptr_t) s_ChatMacro2.c_str() },
    {"chatmacro3", (int *) &chat_macros[3], (std::intptr_t) s_ChatMacro3.c_str() },
    {"chatmacro4", (int *) &chat_macros[4], (std::intptr_t) s_ChatMacro4.c_str() },
    {"chatmacro5", (int *) &chat_macros[5], (std::intptr_t) s_ChatMacro5.c_str() },
    {"chatmacro6", (int *) &chat_macros[6], (std::intptr_t) s_ChatMacro6.c_str() },
    {"chatmacro7", (int *) &chat_macros[7], (std::intptr_t) s_ChatMacro7.c_str() },
    {"chatmacro8", (int *) &chat_macros[8], (std::intptr_t) s_ChatMacro8.c_str() },
    {"chatmacro9", (int *) &chat_macros[9], (std::intptr_t) s_ChatMacro9.c_str() }

};

int	numdefaults;
char*	defaultfile;


//
// M_SaveDefaults
//
void M_SaveDefaults (void)
{
    int		i;
    int		v;
    FILE*	f;
	
    f = fopen (defaultfile, "w");
    if (!f)
	return; // can't write the file, but don't complain
		
    for (i=0 ; i<numdefaults ; i++)
    {
	if (defaults[i].defaultvalue > -0xfff
	    && defaults[i].defaultvalue < 0xfff)
	{
	    v = *defaults[i].location;
	    fprintf (f,"%s\t\t%i\n",defaults[i].name,v);
	} else {
	    fprintf (f,"%s\t\t\"%s\"\n",defaults[i].name,
		     * (char **) (defaults[i].location));
	}
    }
	
    fclose (f);
}


//
// M_LoadDefaults
//
extern unsigned char	scantokey[128];

void M_LoadDefaults (void)
{
    int		i;
    int		len;
    FILE*	f;
    char	def[80];
    char	strparm[100];
    char*	newstring;
    int		parm;
    bool	isstring;
    
    // set everything to base values
    numdefaults = sizeof(defaults)/sizeof(defaults[0]);
    for (i=0 ; i<numdefaults ; i++)
	*defaults[i].location = defaults[i].defaultvalue;
    
    // check for a custom default file
    i = CmdParameters::M_CheckParm ("-config");
    if (i && i<CmdParameters::myargc-1)
    {
	defaultfile = const_cast<char*>(CmdParameters::myargv[i+1].c_str());
	printf ("	default file: %s\n",defaultfile);
    }
    else
	defaultfile = basedefault;
    
    // read the file in, overriding any set defaults
    f = fopen (defaultfile, "r");
    if (f)
    {
	while (!feof(f))
	{
	    isstring = false;
	    if (fscanf (f, "%79s %[^\n]\n", def, strparm) == 2)
	    {
		if (strparm[0] == '"')
		{
		    // get a string default
		    isstring = true;
		    len = strlen(strparm);
		    newstring = (char *) malloc(len);
		    strparm[len-1] = 0;
		    strcpy(newstring, strparm+1);
		}
		else if (strparm[0] == '0' && strparm[1] == 'x')
		    sscanf(strparm+2, "%x", &parm);
		else
		    sscanf(strparm, "%i", &parm);
		for (i=0 ; i<numdefaults ; i++)
		    if (!strcmp(def, defaults[i].name))
		    {
			if (!isstring)
			    *defaults[i].location = parm;
			else
			    *defaults[i].location =
				(std::intptr_t) newstring;
			break;
		    }
	    }
	}
		
	fclose (f);
    }
}


//
// SCREEN SHOTS
//


typedef struct
{
    char		manufacturer;
    char		version;
    char		encoding;
    char		bits_per_pixel;

    unsigned short	xmin;
    unsigned short	ymin;
    unsigned short	xmax;
    unsigned short	ymax;
    
    unsigned short	hres;
    unsigned short	vres;

    unsigned char	palette[48];
    
    char		reserved;
    char		color_planes;
    unsigned short	bytes_per_line;
    unsigned short	palette_type;
    
    char		filler[58];
    unsigned char	data;		// unbounded
} pcx_t;





//
// M_ScreenShot
//
void M_ScreenShot (void)
{
    auto ss = window->capture();
    
    // Use the current time for a unique file name
    std::string fileName = "DOOM-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".png";

    if (!ss.saveToFile(fileName))
    {
        I_Error(("M_ScreenShot: Couldn't save file " + fileName).c_str());
    }
	
    players[consoleplayer].message = "Screen shot";
}


