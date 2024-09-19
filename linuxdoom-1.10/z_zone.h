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
//      Zone Memory Allocation, perhaps NeXT ObjectiveC inspired.
//	Remark: this was the only stuff that, according
//	 to John Carmack, might have been useful for
//	 Quake.
//
//---------------------------------------------------------------------

#pragma once

#include <stdio.h>

//
// ZONE MEMORY
// PU - purge tags.
// Tags < 100 are not overwritten until freed.
#define PU_STATIC 1   // static entire execution time
#define PU_SOUND 2    // static while playing
#define PU_MUSIC 3    // static while playing
#define PU_DAVE 4     // anything else Dave wants static
#define PU_LEVEL 50   // static until level exited
#define PU_LEVSPEC 51 // a special thinker in a level
// Tags >= 100 are purgable whenever needed.
#define PU_PURGELEVEL 100
#define PU_CACHE 101

void Z_Init(void);
void freeTags(int lowtag, int hightag);
void Z_DumpHeap(int lowtag, int hightag);
void Z_FileDumpHeap(FILE *f);
void Z_CheckHeap(void);
int freeMemory(void);

typedef struct memblock_s {
  int size;    // including the header and possibly tiny fragments
  void **user; // NULL if a free block
  int tag;     // purgelevel
  int id;      // should be ZONEID
  struct memblock_s *next;
  struct memblock_s *prev;
} memblock_t;

//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
