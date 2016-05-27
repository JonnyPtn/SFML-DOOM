
#include <stdlib.h>

#include <stdarg.h>
#include <sys/types.h>
#include <signal.h>
#include <array>

#include "doomstat.hpp"
#include "i_system.hpp"
#include "v_video.hpp"
#include "m_argv.hpp"
#include "d_main.hpp"

#include "doomdef.hpp"

#define POINTER_WARP_COUNTDOWN	1

std::unique_ptr<sf::RenderWindow> window;

std::unique_ptr<sf::Texture>	texture;
std::unique_ptr<sf::Sprite>		sprite;
sf::Color	colors[256];
int		X_screen;
int		X_width;
int		X_height;

void I_ShutdownGraphics(void)
{
	sprite.release();
	texture.release();
	window.release();
}

static int	lastmousex = 0;
static int	lastmousey = 0;
bool		mousemoved = false;
bool		shmFinished;

//
// I_FinishUpdate
//
void I_FinishUpdate(void)
{
	sf::Uint8 colouredPixels[SCREENHEIGHT*SCREENWIDTH * 4] = { 0 };
	for (int i = 0; i < SCREENHEIGHT*SCREENWIDTH; i++)
	{
		colouredPixels[i * 4] = colors[screens[0][i]].r;
		colouredPixels[i * 4 + 1] = colors[screens[0][i]].g;
		colouredPixels[i * 4 + 2] = colors[screens[0][i]].b;
		colouredPixels[i * 4 + 3] = 255;
	}
	texture->update(colouredPixels);
	window->clear();
	window->draw(*sprite);
	window->display();
}


//
// I_ReadScreen
//
void I_ReadScreen (unsigned char* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}


//
// Palette stuff.
//

void UploadNewPalette( unsigned char *palette)
{
	static bool	firstcall = true;
	
	// initialize the colormap
	if (firstcall)
	{
		firstcall = false;
		for (int i=0 ; i<256 ; i++)
		{
		    colors[i] = sf::Color::White;
		}
	}

	// set the X colormap entries
	int c;
	for (int i=0 ; i<256 ; i++)
	{
		c = gammatable[usegamma][*palette++];
		colors[i].r = (c<<8) + c;
		c = gammatable[usegamma][*palette++];
		colors[i].g = (c<<8) + c;
		c = gammatable[usegamma][*palette++];
		colors[i].b = (c<<8) + c;
	}
}

//
// I_SetPalette
//
void I_SetPalette (unsigned char* palette)
{
    UploadNewPalette(palette);
}

void I_InitGraphics(void)
{

	std::string	displayname;
	int			n;
	int			pnum;
	int			x = 0;
	int			y = 0;

	// warning: char format, different type arg
	char		xsign = ' ';
	char		ysign = ' ';
	static int		firsttime = 1;

	if (!firsttime)
		return;
	firsttime = 0;

	signal(SIGINT, (void(*)(int)) I_Quit);

	X_width = SCREENWIDTH;
	X_height = SCREENHEIGHT;

	// check for command-line display name
	if ((pnum = CmdParameters::M_CheckParm("-disp"))) // suggest parentheses around assignment
		displayname = CmdParameters::myargv[pnum + 1];
	else
		displayname = "";

	// check for command-line geometry
	if ((pnum = CmdParameters::M_CheckParm("-geom"))) // suggest parentheses around assignment
	{
		// warning: char format, different type arg 3,5
		n = sscanf(CmdParameters::myargv[pnum + 1].c_str(), "%c%d%c%d", &xsign, &x, &ysign, &y);

		if (n == 2)
			x = y = 0;
		else if (n == 6)
		{
			if (xsign == '-')
				x = -x;
			if (ysign == '-')
				y = -y;
		}
		else
			I_Error("bad -geom parameter");
	}

	window.reset(new sf::RenderWindow());
	window->create(sf::VideoMode(X_width * 2, X_height * 2), displayname); //quick double size for now
	window->setVerticalSyncEnabled(true);
	texture.reset(new sf::Texture);
	texture->create(X_width, X_height);
	sprite.reset(new sf::Sprite());
	sprite->setTexture(*texture);
	sprite->setScale({2.f,2.f});

	screens[0] = (unsigned char*)malloc(SCREENWIDTH*SCREENHEIGHT);

	//hide the mouse cursor
	window->setMouseCursorVisible(false);
}

bool pollEvent(sf::Event& ev)
{
	return window->pollEvent(ev);
}
