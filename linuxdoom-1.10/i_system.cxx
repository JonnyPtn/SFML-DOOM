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
//
//
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

#include <cassert>
#include <chrono>

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

#include "i_system.h"

int mb_used = 6;

void I_Tactile(int on, int off, int total)
{
    // UNUSED.
    on = off = total = 0;
}

ticcmd_t emptycmd;

ticcmd_t *I_BaseTiccmd(void) { return &emptycmd; }

int I_GetHeapSize(void) { return mb_used * 1024 * 1024; }

//
// I_GetTime
// returns time in 1/TICRATE second tics
//
int I_GetTime(void)
{
    using namespace std::chrono;
    using tic = duration<int, std::ratio<1, TICRATE>>;
    const auto now = steady_clock::now();
    static const auto basetime = now;
    const auto tics = duration_cast<tic>(now - basetime).count();
    return tics;
}

//
// I_Init
//
void I_Init(void)
{
    I_InitSound();
    //  I_InitGraphics();
}

//
// I_Quit
//
void I_Quit(void)
{
    D_QuitNetGame();
    I_ShutdownSound();
    I_ShutdownMusic();
    M_SaveDefaults();
    exit(0);
}

void I_BeginRead(void) {}

void I_EndRead(void) {}

//
// I_Error
//
extern boolean demorecording;

void I_Error(const char *error, ...)
{
    va_list argptr;

    // Message first.
    va_start(argptr, error);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, error, argptr);
    fprintf(stderr, "\n");
    va_end(argptr);

    fflush(stderr);

    // Shutdown. Here might be other errors.
    if (demorecording)
        G_CheckDemoStatus();

    D_QuitNetGame();

    assert(false);

    exit(-1);
}