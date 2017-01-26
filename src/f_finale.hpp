#pragma once

#include "doomtype.hpp"
#include "d_event.hpp"
//
// FINALE
//

// Called by main loop.
bool F_Responder (sf::Event* ev);

// Called by main loop.
void F_Ticker (void);

// Called by main loop.
void F_Drawer (void);


void F_StartFinale (void);