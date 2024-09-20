//-----------------------------------------------------------------------------
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
//	System specific interface stuff.
//
//-----------------------------------------------------------------------------
module;

#include "d_event.h"
#include "d_ticcmd.h"

#include <string_view>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>

#include "doomdef.h"
#include "i_sound.h"
#include "i_video.h"
#include "m_misc.h"

#include "d_net.h"
#include "g_game.h"

#include <spdlog/spdlog.h>

#include <source_location>

// For debug break on error
#if WIN32
#include <windows.h>
#else
#include <signal.h>
#endif

export module i_system;

int mb_used = 6;

export void I_Tactile(int on, int off, int total) {
  // UNUSED.
  on = off = total = 0;
}

ticcmd_t emptycmd;

export ticcmd_t *I_BaseTiccmd(void) { return &emptycmd; }

int I_GetHeapSize(void) { return mb_used * 1024 * 1024; }

export byte *I_ZoneBase(int *size) {
  *size = mb_used * 1024 * 1024;
  return (byte *)malloc(*size);
}

//
// I_GetTime
// returns time in 1/70th second tics
//
export int I_GetTime(void) {
  using namespace std::chrono;
  using tic = duration<int, std::ratio<1, 70>>;
  const auto now = steady_clock::now();
  static const auto basetime = now;
  const auto tics = duration_cast<tic>(now - basetime).count();
  return tics;
}

//
// I_Init
//
export void I_Init(void) {
  I_InitSound();
  //  I_InitGraphics();
}

//
// I_Quit
//
export void I_Quit(void) {
  D_QuitNetGame();
  I_ShutdownSound();
  I_ShutdownMusic();
  M_SaveDefaults();
  exit(0);
}

byte *I_AllocLow(int length) {
  byte *mem;

  mem = (byte *)malloc(length);
  memset(mem, 0, length);
  return mem;
}

//
// I_Error
//
export template <typename... Args>
void I_Error(spdlog::format_string_t<Args...> fmt, Args &&...args) {
    spdlog::error(fmt, std::forward<Args>(args)...);
    // Shutdown. Here might be other errors.
    // @TODO Jonny - this is in the d_main module so would be circular
    // if (demorecording)
    // G_CheckDemoStatus();

    D_QuitNetGame();

#if WIN32
    DebugBreak();
#else
    raise(SIGTRAP);
#endif

    exit(-1);
  }