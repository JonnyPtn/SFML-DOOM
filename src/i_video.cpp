
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
bool	isFullscreen(false);

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

	static int		firsttime = 1;

	if (!firsttime)
		return;
	firsttime = 0;

	signal(SIGINT, (void(*)(int)) I_Quit);

	X_width = SCREENWIDTH * 2;
	X_height = SCREENHEIGHT * 2;

	window.reset(new sf::RenderWindow());
	window->create(sf::VideoMode(X_width, X_height), displayname); //quick double size for now
	window->setVerticalSyncEnabled(true);
	//window->setMouseCursorGrabbed(true);
	texture.reset(new sf::Texture);
	texture->create(SCREENWIDTH, SCREENHEIGHT);
	sprite.reset(new sf::Sprite());
	sprite->setTexture(*texture);
	sprite->setScale(sf::Vector2f(2.f,2.f));

	screens[0] = (unsigned char*)malloc(SCREENWIDTH*SCREENHEIGHT);

	//hide the mouse cursor
	window->setMouseCursorVisible(false);
}

bool pollEvent(sf::Event& ev)
{
    if (window && window->isOpen())
        return window->pollEvent(ev);
    else
        return false;
}

void toggleFullscreen()
{
	if (isFullscreen)
	{
		window->create(sf::VideoMode(X_width, X_height), "SFML-DOOM", sf::Style::Default);
		isFullscreen = false;
		sprite->setScale(2.f, 2.f);
	}
	else
	{
		window->create(sf::VideoMode::getDesktopMode(), "SFML-DOOM", sf::Style::Fullscreen);
		isFullscreen = true;

		//adjust sprite scale
		auto windowSize = static_cast<sf::Vector2f>(window->getSize());
		sprite->setScale(windowSize.x / SCREENWIDTH, windowSize.y / SCREENHEIGHT);
	}
	//window->setMouseCursorGrabbed(true);
	window->setMouseCursorVisible(false);
}