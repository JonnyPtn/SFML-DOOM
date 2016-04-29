#pragma once

#include "doomtype.hpp"
#include <SFML/Graphics.hpp>

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

extern sf::Color	colors[256];
