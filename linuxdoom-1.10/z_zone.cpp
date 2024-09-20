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
//	Zone Memory Allocation. Neat.
//
//-----------------------------------------------------------------------------

#include "z_zone.h"
#include "doomdef.h"
#include "doomtype.h"

#include <spdlog/spdlog.h>

#include <stdlib.h>

import i_system;

//
// ZONE MEMORY ALLOCATION
//
// There is never any space between memblocks,
//  and there will never be two contiguous free memblocks.
// The rover can be left pointing at a non-empty block.
//
// It is of no value to free a cachable block,
//  because it will get overwritten automatically if needed.
//

#define ZONEID 0x1d4a11

typedef struct {
  // total bytes malloced, including header
  int size;

  // start / end cap for linked list
  memblock_t blocklist;

  memblock_t *rover;

} memzone_t;

memzone_t *mainzone;

//
// Z_ClearZone
//
void Z_ClearZone(memzone_t *zone) {
  memblock_t *block;

  // set the entire zone to one free block
  zone->blocklist.next = zone->blocklist.prev = block =
      (memblock_t *)((byte *)zone + sizeof(memzone_t));

  zone->blocklist.user = static_cast<void **>(static_cast<void *>(zone));
  zone->blocklist.tag = PU_STATIC;
  zone->rover = block;

  block->prev = block->next = &zone->blocklist;

  // NULL indicates a free block.
  block->user = NULL;

  block->size = zone->size - sizeof(memzone_t);
}

//
// Z_Init
//
void Z_Init(void) {
  memblock_t *block;
  int size;

  mainzone = (memzone_t *)I_ZoneBase(&size);
  mainzone->size = size;

  // set the entire zone to one free block
  mainzone->blocklist.next = mainzone->blocklist.prev = block =
      (memblock_t *)((byte *)mainzone + sizeof(memzone_t));

  mainzone->blocklist.user =
      static_cast<void **>(static_cast<void *>(mainzone));
  mainzone->blocklist.tag = PU_STATIC;
  mainzone->rover = block;

  block->prev = block->next = &mainzone->blocklist;

  // NULL indicates a free block.
  block->user = NULL;

  block->size = mainzone->size - sizeof(memzone_t);
}

//
// freeTags
//
void freeTags(int lowtag, int hightag) {
  memblock_t *block;
  memblock_t *next;

  for (block = mainzone->blocklist.next; block != &mainzone->blocklist;
       block = next) {
    // get link before freeing
    next = block->next;

    // free block?
    if (!block->user)
      continue;

    if (block->tag >= lowtag && block->tag <= hightag)
      free((byte *)block + sizeof(memblock_t));
  }
}

//
// Z_DumpHeap
// Note: TFileDumpHeap( stdout ) ?
//
void Z_DumpHeap(int lowtag, int hightag) {
  memblock_t *block;

  spdlog::info("zone size: {}  location: {}\n", mainzone->size, fmt::ptr(mainzone));

  spdlog::info("tag range: {} to {}\n", lowtag, hightag);

  for (block = mainzone->blocklist.next;; block = block->next) {
    if (block->tag >= lowtag && block->tag <= hightag)
      printf("block:%p    size:%7i    user:%p    tag:%3i\n", block, block->size,
             block->user, block->tag);

    if (block->next == &mainzone->blocklist) {
      // all blocks have been hit
      break;
    }

    if ((byte *)block + block->size != (byte *)block->next)
      printf("ERROR: block size does not touch the next block\n");

    if (block->next->prev != block)
      printf("ERROR: next block doesn't have proper back link\n");

    if (!block->user && !block->next->user)
      printf("ERROR: two consecutive free blocks\n");
  }
}

//
// Z_FileDumpHeap
//
void Z_FileDumpHeap(FILE *f) {
  memblock_t *block;

  fprintf(f, "zone size: %i  location: %p\n", mainzone->size, mainzone);

  for (block = mainzone->blocklist.next;; block = block->next) {
    fprintf(f, "block:%p    size:%7i    user:%p    tag:%3i\n", block,
            block->size, block->user, block->tag);

    if (block->next == &mainzone->blocklist) {
      // all blocks have been hit
      break;
    }

    if ((byte *)block + block->size != (byte *)block->next)
      fprintf(f, "ERROR: block size does not touch the next block\n");

    if (block->next->prev != block)
      fprintf(f, "ERROR: next block doesn't have proper back link\n");

    if (!block->user && !block->next->user)
      fprintf(f, "ERROR: two consecutive free blocks\n");
  }
}

//
// Z_CheckHeap
//
void Z_CheckHeap(void) {
  memblock_t *block;

  for (block = mainzone->blocklist.next;; block = block->next) {
    if (block->next == &mainzone->blocklist) {
      // all blocks have been hit
      break;
    }

    if ((byte *)block + block->size != (byte *)block->next)
      I_Error("Z_CheckHeap: block size does not touch the next block\n");

    if (block->next->prev != block)
      I_Error("Z_CheckHeap: next block doesn't have proper back link\n");

    if (!block->user && !block->next->user)
      I_Error("Z_CheckHeap: two consecutive free blocks\n");
  }
}

//
// freeMemory
//
int freeMemory(void) {
  memblock_t *block;
  int free;

  free = 0;

  for (block = mainzone->blocklist.next; block != &mainzone->blocklist;
       block = block->next) {
    if (!block->user || block->tag >= PU_PURGELEVEL)
      free += block->size;
  }
  return free;
}
