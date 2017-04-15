#pragma once

#include "doomdef.hpp"
#include "d_event.hpp"

//
// GAME
//
void G_DeathMatchSpawnPlayer (int playernum);

void G_InitNew (skill_t skill, int episode, int map);

// Can be called by the startup code or M_Responder.
// A normal game starts at map 1,
// but a warp test can start elsewhere
void G_DeferedInitNew (skill_t skill, int episode, int map);

void G_DeferedPlayDemo (char* demo);

// Can be called by the startup code or M_Responder,
// calls P_SetupLevel or W_EnterWorld.
void G_LoadGame (const std::string& name);

void G_DoLoadGame ();

// Called by M_Responder.
void G_SaveGame (int slot, char* description);

// Only called by startup code.
void G_RecordDemo (char* name);

void G_BeginRecording ();

void G_TimeDemo (char* name);
bool G_CheckDemoStatus ();

void G_ExitLevel ();
void G_SecretExitLevel ();

void G_WorldDone ();

void G_Ticker ();
bool G_Responder (sf::Event*	ev);

#include "doomdef.hpp"

class Game
{
public:
    static GameMode_t gamemode;
};