/*

TiMidity -- Experimental MIDI to WAVE converter
Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

common.c

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <timidity/config.h>
#include <timidity/common.h>
#include <timidity/output.h>

/* I guess "rb" should be right for any libc */
#define OPEN_MODE "rb"

std::string current_filename;

#ifdef DEFAULT_PATH
/* The paths in this list will be tried whenever we're reading a file */
static PathList defaultpathlist = { DEFAULT_PATH,0 };
static PathList *pathlist = &defaultpathlist; /* This is a linked list */
#else
static PathList *pathlist = 0;
#endif

/* Try to open a file for reading. If the filename ends in one of the
defined compressor extensions, pipe the file through the decompressor */
static std::ifstream * try_to_open(char *name, int decompress, int noise_mode)
{
    std::ifstream * fp;

    fp = new std::ifstream(name,std::ios::binary);

    if (!fp)
        return 0;

    return fp;
}

/* This is meant to find and open files for reading, possibly piping
them through a decompressor. */
std::ifstream * open_file(const char *name, int decompress, int noise_mode)
{
    std::ifstream * fp;
    PathList *plp = pathlist;
    int l;

    if (!name || !(*name))
    {
        printf( "Attempted to open nameless file.\n");
        return 0;
    }

    /* First try the given name */

    current_filename = name;

    printf( "Trying to open %s\n", current_filename.c_str());
    if ((fp = try_to_open(const_cast<char*>(current_filename.c_str()), decompress, noise_mode)))
        return fp;

    if (name[0] != PATH_SEP)
        while (plp)  /* Try along the path then */
        {
            current_filename.clear();
            l = strlen(plp->path);
            if (l)
            {
                current_filename = plp->path;
                if (current_filename[l - 1] != PATH_SEP)
                    current_filename += PATH_STRING;
            }
            current_filename += name;
            printf( "Trying to open %s\n", current_filename.c_str());
            if ((fp = try_to_open(const_cast<char*>(current_filename.c_str()), decompress, noise_mode)))
                return fp;

            plp = (PathList*)plp->next;
        }

    /* Nothing could be opened. */

    current_filename.clear();

    if (noise_mode >= 2)
        printf( "%s: %s\n", name, strerror(errno));

    return 0;
}

/* This closes files opened with open_file */
void close_file(std::ifstream * fp)
{
    delete fp;
}

/* This is meant for skipping a few bytes in a file or fifo. */
void skip(std::ifstream * fp, size_t len)
{
    long int c;
    char tmp[1024];
    while (len>0)
    {
        c = len;
        if (c>1024) c = 1024;
        len -= c;
        long before = fp->tellg();
        fp->read(tmp, c);
        if ((before + c) != fp->tellg())
            printf( "%s: skip: %s\n",
                current_filename.c_str(), strerror(errno));
    }
}

//extern void *Real_Tim_Malloc( size_t );
/* This'll allocate memory or die. */
void *safe_malloc(size_t count)
{
    void *p;
    if (count > (1 << 21))
    {
        printf(
            "Strange, I feel like allocating %d bytes. This must be a bug.\n",
            static_cast<int>(count));
    }
    else if ((p = Real_Tim_Malloc(count)))
        return p;
    else
        printf( "Sorry. Couldn't malloc %d bytes.", static_cast<int>(count));

    //exit(10);
    return(NULL);
}

/* This adds a directory to the path list */
void add_to_pathlist(char *s)
{
    PathList *plp = (PathList*)safe_malloc(sizeof(PathList));
    strcpy((plp->path = (char *)safe_malloc(strlen(s) + 1)), s);
    plp->next = pathlist;
    pathlist = plp;
}

/* Required memory management functions */
void *Real_Tim_Malloc(int sz) {
    return malloc(sz);
}

void Real_Tim_Free(void *pt) {
    free(pt);
}

void* Real_Malloc(unsigned int sz) {
    return malloc(sz);
}
