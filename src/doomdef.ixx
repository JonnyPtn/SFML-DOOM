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
//  Internally used data structures for virtually everything,
//   key definitions, lots of other stuff.
//
//-----------------------------------------------------------------------------
module;

#include <cstdint>
#include <stdio.h>
#include <string.h>

export module doomdef;

//
// Global parameters/defines.
//
// DOOM version
export constexpr int32_t VERSION = 110;

// Game mode handling - identify IWAD version
//  to handle IWAD dependend animations etc.
export enum GameMode_t {
  shareware,  // DOOM 1 shareware, E1, M9
  registered, // DOOM 1 registered, E3, M27
  commercial, // DOOM 2 retail, E1 M34
  // DOOM 2 german edition not handled
  retail,      // DOOM 1 retail, E4, M36
  indetermined // Well, no IWAD found.

};

// Mission packs - might be useful for TC stuff?
export enum GameMission_t {
  doom,      // DOOM 1
  doom2,     // DOOM 2
  pack_tnt,  // TNT mission pack
  pack_plut, // Plutonia pack
  none

};

// Identify language to use, software localization.
export enum Language_t {
  english,
  french,
  german,
  unknown

};

// If rangecheck is undefined,
// most parameter validation debugging code will not be compiled
#define RANGECHECK

// This one switches between MIT SHM (no proper mouse)
// and XFree86 DGA (mickey sampling). The original
// linuxdoom used SHM, which is default.
// #define X11_DGA		1

//
// For resize of screen, at start of game.
// It will not work dynamically, see visplanes.
//
#define BASE_WIDTH 320

// It is educational but futile to change this
//  scaling e.g. to 2. Drawing of status bar,
//  menues etc. is tied to the scale implied
//  by the graphics.
export constexpr auto SCREEN_MUL = 1;
#define INV_ASPECT_RATIO 0.625 // 0.75, ideally

// Defines suck. C sucks.
// C++ might sucks for OOP, but it sure is a better C.
// So there.
export constexpr auto SCREENWIDTH = 320;
// SCREEN_MUL*BASE_WIDTH //320
export constexpr auto SCREENHEIGHT = 200;
//(int)(SCREEN_MUL*BASE_WIDTH*INV_ASPECT_RATIO) //200

// The maximum number of players, multiplayer/networking.
export constexpr auto MAXPLAYERS = 4;

// State updates, number of tics / second.
export constexpr auto TICRATE = 35;

// The current state of the game: whether we are
// playing, gazing at the intermission screen,
// the game final animation, or a demo.
export enum gamestate_t { GS_LEVEL, GS_INTERMISSION, GS_FINALE, GS_DEMOSCREEN };

//
// Difficulty/skill settings/filters.
//

// Skill flags.
#define MTF_EASY 1
#define MTF_NORMAL 2
#define MTF_HARD 4

// Deaf monsters/do not react to sound.
export constexpr auto MTF_AMBUSH = 8;

export enum skill_t { sk_baby, sk_easy, sk_medium, sk_hard, sk_nightmare };

//
// Key cards.
//
export enum card_t {
  it_bluecard,
  it_yellowcard,
  it_redcard,
  it_blueskull,
  it_yellowskull,
  it_redskull,

  NUMCARDS

};

// The defined weapons,
//  including a marker indicating
//  user has not changed weapon.
export enum weapontype_t {
  wp_fist,
  wp_pistol,
  wp_shotgun,
  wp_chaingun,
  wp_missile,
  wp_plasma,
  wp_bfg,
  wp_chainsaw,
  wp_supershotgun,

  NUMWEAPONS,

  // No pending weapon change.
  wp_nochange

};

// Ammunition types defined.
export enum ammotype_t {
  am_clip,  // Pistol / chaingun ammo.
  am_shell, // Shotgun / double barreled shotgun.
  am_cell,  // Plasma rifle, BFG.
  am_misl,  // Missile launcher.
  NUMAMMO,
  am_noammo // Unlimited for chainsaw / fist.

};

// Power up artifacts.
export enum powertype_t {
  pw_invulnerability,
  pw_strength,
  pw_invisibility,
  pw_ironfeet,
  pw_allmap,
  pw_infrared,
  NUMPOWERS

};

//
// Power up durations,
//  how many seconds till expiration,
//  assuming TICRATE is 35 ticks/second.
//
export enum powerduration_t {
  INVULNTICS = (30 * TICRATE),
  INVISTICS = (60 * TICRATE),
  INFRATICS = (120 * TICRATE),
  IRONTICS = (60 * TICRATE)

};

// DOOM basic types (bool),
//  and max/min values.
// 

// Fixed point.
// #include "m_fixed.h"

// Endianess handling.
// #include "m_swap.h"

// Binary Angles, sine/cosine/atan lookups.
// 

// Event type.
// #include "d_event.h"

// Game function, skills.
// #include "g_game.h"

// All external data is defined here.
// #include "doomdata.h"

// All important printed strings.
// Language selection (message strings).
// #include "dstrings.h"

// Player is a special actor.
// struct player_s;

// #include "d_items.h"
// #include "d_player.h"
// #include "p_mobj.h"
// #include "d_net.h"

// PLAY
// #include "p_tick.h"

// Header, generated by sound utility.
// The utility was written by Dave Taylor.
// 

//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
