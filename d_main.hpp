#pragma once

#ifndef __D_MAIN__
#define __D_MAIN__

#include "d_event.hpp"

#ifdef __GNUG__
#pragma interface
#endif


#define MAXWADFILES             20
extern char*		wadfiles[MAXWADFILES];

void D_AddFile(char *file);



//
// D_DoomMain()
// Not a globally visible function, just included for source reference,
// calls all startup code, parses command line options.
// If not overrided by user input, calls N_AdvanceDemo.
//
void D_DoomMain(void);

// Called by IO functions when input is detected.
void D_PostEvent(sf::Event ev);



//
// BASE LEVEL
//
void D_PageTicker(void);
void D_PageDrawer(void);
void D_AdvanceDemo(void);
void D_StartTitle(void);

#endif
