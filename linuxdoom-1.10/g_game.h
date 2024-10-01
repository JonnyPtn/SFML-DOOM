// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
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
//   Duh.
//
//-----------------------------------------------------------------------------

#pragma once

#include "d_event.h"
#include "d_ticcmd.h"
#include "d_player.h"

#include <SFML/Window.hpp>

import doomdef;

// previously externs in doomstat.h
inline int gamemap;
inline int gametic;
inline int gameepisode;
inline bool playeringame[MAXPLAYERS];
inline player_t players[MAXPLAYERS];
inline uint8_t deathmatch; // only if started as net death
inline bool netgame;    // only true if packets are broadcast
inline int consoleplayer; // player taking events and displaying
inline bool viewactive;
inline bool singledemo; // quit after playing a demo from cmdline
inline skill_t gameskill;
inline int totalkills, totalitems, totalsecret; // for intermission
inline wbstartstruct_t wminfo; // parms for world map / intermission
inline bool usergame;  // ok to save / end game
inline bool demoplayback;
inline gamestate_t gamestate;
inline bool paused;
inline int displayplayer; // view being displayed
inline int bodyqueslot;
inline bool precache = true; // if true, load all graphics at start
inline bool nodrawers;  // for comparative timing purposes
inline bool respawnmonsters;

//
// GAME
//
void G_DeathMatchSpawnPlayer(int playernum);

void G_InitNew(skill_t skill, int episode, int map);

// Can be called by the startup code or M_Responder.
// A normal game starts at map 1,
// but a warp test can start elsewhere
void G_DeferedInitNew(skill_t skill, int episode, int map);

void G_DeferedPlayDemo(const std::string &demo);

// Can be called by the startup code or M_Responder,
// calls P_SetupLevel or W_EnterWorld.
void G_LoadGame(const std::string& name);

void G_DoLoadGame(void);

// Called by M_Responder.
void G_SaveGame(int slot, char *description);

// Only called by startup code.
void G_RecordDemo(const std::string &name);

void G_BeginRecording(void);

void G_PlayDemo(char *name);

void G_TimeDemo(const std::string &name);

bool G_CheckDemoStatus(void);

void G_ExitLevel(void);

void G_SecretExitLevel(void);

void G_WorldDone(void);

void G_Ticker(void);

bool G_Responder(const sf::Event &ev);

void G_ScreenShot(void);

void G_BuildTiccmd(ticcmd_t *cmd);

//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
