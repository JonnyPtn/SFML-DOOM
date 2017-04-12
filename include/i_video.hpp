#pragma once


#include <SFML/Graphics.hpp>
#include <memory>

// Called by D_DoomMain,
// determines the hardware configuration
// and sets up the video mode
void I_InitGraphics (void);


void I_ShutdownGraphics(void);

// Takes full 8 bit values.
void I_SetPalette (unsigned char* palette);

void I_UpdateNoBlit (void);
void I_FinishUpdate (void);

// Wait for vertical retrace or pause a bit.
void I_WaitVBL(int count);

void I_ReadScreen (unsigned char* scr);

void I_BeginRead (void);
void I_EndRead (void);

bool pollEvent(sf::Event& ev);

void toggleFullscreen();

extern sf::Color	colors[256];

extern std::unique_ptr<sf::RenderWindow> window;
extern bool	isFullscreen;
extern std::unique_ptr<sf::Texture>	texture;
extern std::unique_ptr<sf::Sprite>		sprite;
extern sf::Color	colors[256];
extern int		X_screen;
extern int		X_width;
extern int		X_height;
