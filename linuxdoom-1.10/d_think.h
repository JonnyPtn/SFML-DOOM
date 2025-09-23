//-----------------------------------------------------------------------------
//
//
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//  MapObj data. Map Objects or mobjs are actors, entities,
//  thinker, take-your-pick... anything that moves, acts, or
//  suffers state changes of more or less violent nature.
//
//-----------------------------------------------------------------------------

#ifndef __D_THINK__
#define __D_THINK__


//
// Experimental stuff.
// To compile this as "ANSI C with classes"
//  we will need to handle the various
//  action functions cleanly.
//
typedef void (*actionf_v)();
typedef void (*actionf_p1)(void *);
typedef void (*actionf_p2)(void *, void *);

struct mobj_s;
struct player_s;
struct pspdef_s;

union actionf_t
{
    actionf_t() {}

    actionf_t(int) {}

    actionf_t(void (*func)()) { acv = func; }

    actionf_t(void (*func)(mobj_s *)) { acp1 = (void (*)(void *))func; }

    actionf_t(void (*func)(player_s *, pspdef_s *)) { acp2 = (void (*)(void *, void *))func; }

    actionf_p1 acp1;
    actionf_v acv;
    actionf_p2 acp2;
};

// Historically, "think_t" is yet another
//  function pointer to a routine to handle
//  an actor.
typedef actionf_t think_t;

// Doubly linked list of actors.
typedef struct thinker_s
{
    struct thinker_s *prev;
    struct thinker_s *next;
    think_t function;

} thinker_t;

#endif
//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------