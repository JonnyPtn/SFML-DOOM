#pragma once

// All important printed strings.
// Language selection (message strings).
// Use -DFRENCH etc.

#ifdef FRENCH
#include "d_french.hpp"
#else
#include "d_englsh.hpp"
#endif

#include <string>
#include <array>

// Misc. other strings.
#define SAVEGAMENAME	"doomsav"


//
// File locations,
//  relative to current position.
// Path names are OS-sensitive.
//
#define DEVMAPS "devmaps"
#define DEVDATA "devdata"


// Not done in french?

// QuitDOOM messages
#define NUM_QUITMESSAGES   22

extern std::array<std::string,NUM_QUITMESSAGES+1> endmsg;