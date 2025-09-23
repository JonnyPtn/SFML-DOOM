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
// DESCRIPTION:
//	System interface for sound.
//
//-----------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "i_sound.h"
#include "i_system.h"

#include <format>
#include <unordered_map>

#include "w_wad.h"

#include "doomdef.h"

#include <sfml/Audio.hpp>

constexpr auto SAMPLERATE = 11025; // Hz
constexpr auto SAMPLECOUNT = 512;
// Volume lookups.
int vol_lookup[128 * 256];

//
// This function loads the sound data from the WAD lump,
//  for single sound.
//
std::unordered_map<std::string, sf::SoundBuffer> buffers;

const sf::SoundBuffer &getsfx(const std::string &sfxname)
{
    const auto lumpName = std::format("ds{}", sfxname);

    if (!buffers.contains(lumpName))
    {

        // Now, there is a severe problem with the
        //  sound handling, in it is not (yet/anymore)
        //  gamemode aware. That means, sounds from
        //  DOOM II will be requested even with DOOM
        //  shareware.
        // The sound list is wired into sounds.c,
        //  which sets the external variable.
        // I do not do runtime patches to that
        //  variable. Instead, we will use a
        //  default sound for replacement.
        int sfxlump;
        if (W_CheckNumForName(lumpName) == -1)
            sfxlump = W_GetNumForName("dspistol");
        else
            sfxlump = W_GetNumForName(lumpName);

        const auto size = W_LumpLength(sfxlump);

        // Debug.
        // fprintf( stderr, "." );
        // fprintf( stderr, " -loading  %s (lump %d, %d bytes)\n",
        //	     sfxname, sfxlump, size );
        // fflush( stderr );

        const auto sfx = (unsigned char *)W_CacheLumpNum(sfxlump);

        // Pads the sound effect out to the mixing buffer size.
        // The original realloc would interfere with zone memory.
        const auto paddedsize = ((size - 8 + (SAMPLECOUNT - 1)) / SAMPLECOUNT) * SAMPLECOUNT;

        // Allocate from zone memory.
        std::vector<uint8_t> paddedsfx(paddedsize + 8);
        // ddt: (unsigned char *) realloc(sfx, paddedsize+8);
        // This should interfere with zone memory handling,
        //  which does not kick in in the soundserver.

        // Now copy and pad.
        memcpy(paddedsfx.data(), sfx, size);
        for (auto i = size; i < paddedsize + 8; i++)
            paddedsfx[i] = 128;

        // Convert 8 bit pcm to 16 bit for SFML
        std::vector<int16_t> converted(paddedsfx.size());
        for (auto i = 0; i < converted.size(); i++)
        {
            converted[i] = (paddedsfx[i] - 0x80) << 8;
        }

        buffers[sfxname] = {converted.data(), converted.size(), 1, SAMPLERATE, {sf::SoundChannel::Mono}};
    }
    return buffers[sfxname];
}

void I_SetSfxVolume(int volume)
{
    // Identical to DOS.
    // Basically, this should propagate
    //  the menu/config file setting
    //  to the state variable used in
    //  the mixing.
    snd_SfxVolume = volume;
}

// MUSIC API - dummy. Some code from DOS version.
void I_SetMusicVolume(int volume)
{
    // Internal state variable.
    snd_MusicVolume = volume;
    // Now set volume on output device.
    // Whatever( snd_MusciVolume );
}

//
// Retrieve the raw data lump index
//  for a given SFX name.
//
int I_GetSfxLumpNum(sfxinfo_t *sfx)
{
    char namebuf[9];
    sprintf(namebuf, "ds%s", sfx->name);
    return W_GetNumForName(namebuf);
}

//
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback)
//  is set, but currently not used by mixing.
//
std::vector<sf::Sound> activeSfx;

int I_StartSound(int id, int vol, int sep, int pitch, int priority)
{

    activeSfx.emplace_back(sf::Sound{buffers[S_sfx[id].name]});
    activeSfx.back().play();
    return id;
}

void I_StopSound(int handle)
{
    // You need the handle returned by StartSound.
    // Would be looping all channels,
    //  tracking down the handle,
    //  an setting the channel to zero.

    // UNUSED.
    handle = 0;
}

int I_SoundIsPlaying(int handle)
{
    // Ouch.
    return gametic < handle;
}

//
// This function loops all active (internal) sound
//  channels, retrieves a given number of samples
//  from the raw sound data, modifies it according
//  to the current (internal) channel parameters,
//  mixes the per channel samples into the global
//  mixbuffer, clamping it to the allowed range,
//  and sets up everything for transferring the
//  contents of the mixbuffer to the (two)
//  hardware channels (left and right, that is).
//
// This function currently supports only 16bit.
//
void I_UpdateSound(void)
{
    // JONNY TODO
}

//
// This would be used to write out the mixbuffer
//  during each game loop update.
// Updates sound buffer and audio device at runtime.
// It is called during Timer interrupt with SNDINTR.
// Mixing now done synchronous, and
//  only output be done asynchronous?
//
void I_SubmitSound(void)
{
    // Write it to DSP device.
    // JONNY TODO  write(audio_fd, mixbuffer, SAMPLECOUNT*BUFMUL);
}

void I_UpdateSoundParams(int handle, int vol, int sep, int pitch)
{
    // I fail too see that this is used.
    // Would be using the handle to identify
    //  on which channel the sound might be active,
    //  and resetting the channel parameters.

    // UNUSED.
    handle = vol = sep = pitch = 0;
}

void I_ShutdownSound(void)
{
    // Wait for all sounds to finish
    while (!activeSfx.empty() && activeSfx.front().getStatus() != sf::Sound::Status::Stopped)
    {
    };
}

void I_InitSound()
{

    for (auto i = 1; i < NUMSFX; i++)
    {
        // Alias? Example is the chaingun sound linked to pistol.
        if (!S_sfx[i].link)
        {
            // Load data from WAD file.
            getsfx(S_sfx[i].name);
        }
        else
        {
            // Previously loaded already?
            S_sfx[i].data = S_sfx[i].link->data;
        }
    }

    fprintf(stderr, " pre-cached all sound data\n");

    // Finished initialization.
    fprintf(stderr, "I_InitSound: sound module ready\n");
}

//
// MUSIC API.
// Still no music done.
// Remains. Dummies.
//
void I_InitMusic(void) {}

void I_ShutdownMusic(void) {}

static int looping = 0;
static int musicdies = -1;

void I_PlaySong(int handle, int looping)
{
    // UNUSED.
    handle = looping = 0;
    musicdies = gametic + TICRATE * 30;
}

void I_PauseSong(int handle)
{
    // UNUSED.
    handle = 0;
}

void I_ResumeSong(int handle)
{
    // UNUSED.
    handle = 0;
}

void I_StopSong(int handle)
{
    // UNUSED.
    handle = 0;

    looping = 0;
    musicdies = 0;
}

void I_UnRegisterSong(int handle)
{
    // UNUSED.
    handle = 0;
}

int I_RegisterSong(void *data)
{
    // UNUSED.
    data = NULL;

    return 1;
}

// Is the song playing?
int I_QrySongPlaying(int handle)
{
    // UNUSED.
    handle = 0;
    return looping || musicdies > gametic;
}