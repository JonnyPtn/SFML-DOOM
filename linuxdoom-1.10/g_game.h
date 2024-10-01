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
static int gamemap;
static int gametic;
static int gameepisode;
static bool playeringame[MAXPLAYERS];
static player_t players[MAXPLAYERS];
static uint8_t deathmatch; // only if started as net death
static bool netgame;    // only true if packets are broadcast
static int consoleplayer; // player taking events and displaying
static bool viewactive;
static bool singledemo; // quit after playing a demo from cmdline
static skill_t gameskill;
static int totalkills, totalitems, totalsecret; // for intermission
static wbstartstruct_t wminfo; // parms for world map / intermission
static bool usergame;  // ok to save / end game
static bool demoplayback;
static gamestate_t gamestate;
static bool paused;
static int displayplayer; // view being displayed
static int bodyqueslot;
static bool precache = true; // if true, load all graphics at start
static bool nodrawers;  // for comparative timing purposes
static bool respawnmonsters;

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
