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
// $Log:$
//
// DESCRIPTION:
//	Mission begin melt/wipe screen special effect.
//
//-----------------------------------------------------------------------------
module;
#include "i_video.h"
#include "m_random.h"
#include <memory>
#include <stdlib.h>
export module wipe;
import doomdef;
import video;

//
//                       SCREEN WIPE PACKAGE
//

export enum {
  // simple gradual pixel change for 8-bit only
  wipe_ColorXForm,

  // weird screen melt
  wipe_Melt,

  wipe_NUMWIPES
};

// when zero, stop the wipe
static bool go = 0;

static std::byte *wipe_scr_start;
static std::byte *wipe_scr_end;
static std::byte *wipe_scr;

void wipe_shittyColMajorXform(short *array, int width, int height) {
  int x;
  int y;
  short *dest;

  dest = (short *)malloc(width * height * 2);

  for (y = 0; y < height; y++)
    for (x = 0; x < width; x++)
      dest[x * height + y] = array[y * width + x];

  memcpy(array, dest, width * height * 2);

  free(dest);
}

int wipe_initColorXForm(int width, int height, int ticks) {
  memcpy(wipe_scr, wipe_scr_start, width * height);
  return 0;
}

int wipe_doColorXForm(int width, int height, int ticks) {
  bool changed;
  std::byte *w;
  std::byte *e;
  std::byte newval;

  changed = false;
  w = wipe_scr;
  e = wipe_scr_end;

  while (w != wipe_scr + width * height) {
    if (*w != *e) {
      if (*w > *e) {
        newval = static_cast<std::byte>(static_cast<int>(*w) - ticks);
        if (newval < *e)
          *w = *e;
        else
          *w = newval;
        changed = true;
      } else if (*w < *e) {
        newval = static_cast<std::byte>(static_cast<int>(*w) + ticks);
        if (newval > *e)
          *w = *e;
        else
          *w = newval;
        changed = true;
      }
    }
    w++;
    e++;
  }

  return !changed;
}

int wipe_exitColorXForm(int width, int height, int ticks) { return 0; }

static int *y;

int wipe_initMelt(int width, int height, int ticks) {
  int i, r;

  // copy start screen to main screen
  memcpy(wipe_scr, wipe_scr_start, width * height);

  // makes this wipe faster (in theory)
  // to have stuff in column-major format
  wipe_shittyColMajorXform((short *)wipe_scr_start, width / 2, height);
  wipe_shittyColMajorXform((short *)wipe_scr_end, width / 2, height);

  // setup initial column positions
  // (y<0 => not ready to scroll yet)
  y = (int *)malloc(width * sizeof(int));
  y[0] = -(M_Random() % 16);
  for (i = 1; i < width; i++) {
    r = (M_Random() % 3) - 1;
    y[i] = y[i - 1] + r;
    if (y[i] > 0)
      y[i] = 0;
    else if (y[i] == -16)
      y[i] = -15;
  }

  return 0;
}

int wipe_doMelt(int width, int height, int ticks) {
  int i;
  int j;
  int dy;
  int idx;

  short *s;
  short *d;
  bool done = true;

  width /= 2;

  while (ticks--) {
    for (i = 0; i < width; i++) {
      if (y[i] < 0) {
        y[i]++;
        done = false;
      } else if (y[i] < height) {
        dy = (y[i] < 16) ? y[i] + 1 : 8;
        if (y[i] + dy >= height)
          dy = height - y[i];
        s = &((short *)wipe_scr_end)[i * height + y[i]];
        d = &((short *)wipe_scr)[y[i] * width + i];
        idx = 0;
        for (j = dy; j; j--) {
          d[idx] = *(s++);
          idx += width;
        }
        y[i] += dy;
        s = &((short *)wipe_scr_start)[i * height];
        d = &((short *)wipe_scr)[y[i] * width + i];
        idx = 0;
        for (j = height - y[i]; j; j--) {
          d[idx] = *(s++);
          idx += width;
        }
        done = false;
      }
    }
  }

  return done;
}

int wipe_exitMelt(int width, int height, int ticks) {
  free(y);
  return 0;
}

export int wipe_StartScreen(int x, int y, int width, int height) {
  wipe_scr_start = screens[2].data();
  I_ReadScreen(wipe_scr_start);
  return 0;
}

export int wipe_EndScreen(int x, int y, int width, int height) {
  wipe_scr_end = screens[3].data();
  I_ReadScreen(wipe_scr_end);
  V_DrawBlock(x, y, 0, width, height, wipe_scr_start); // restore start scr.
  return 0;
}

export int wipe_ScreenWipe(int wipeno, int x, int y, int width, int height,
                    int ticks) {
  int rc;
  static int (*wipes[])(int, int, int) = {
      wipe_initColorXForm, wipe_doColorXForm, wipe_exitColorXForm,
      wipe_initMelt,       wipe_doMelt,       wipe_exitMelt};

  // initial stuff
  if (!go) {
    go = 1;
    // wipe_scr = (byte *) malloc(width*height, 0); // DEBUG
    wipe_scr = screens[0].data();
    (*wipes[wipeno * 3])(width, height, ticks);
  }

  // do a piece of wipe-in
  V_MarkRect(0, 0, width, height);
  rc = (*wipes[wipeno * 3 + 1])(width, height, ticks);
  //  V_DrawBlock(x, y, 0, width, height, wipe_scr); // DEBUG

  // final stuff
  if (rc) {
    go = 0;
    (*wipes[wipeno * 3 + 2])(width, height, ticks);
  }

  return !go;
}