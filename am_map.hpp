#pragma once

// Used by ST StatusBar stuff.
#define AM_MSGHEADER (('a'<<24)+('m'<<16))
#define AM_MSGENTERED (AM_MSGHEADER | ('e'<<8))
#define AM_MSGEXITED (AM_MSGHEADER | ('x'<<8))


// Called by main loop.
bool AM_Responder (sf::Event* ev);

// Called by main loop.
void AM_Ticker ();

// Called by main loop,
// called instead of view drawer if automap active.
void AM_Drawer ();

// Called to force the automap to quit
// if the level is completed while it is up.
void AM_Stop ();