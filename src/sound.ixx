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
//	System interface for sound.
//
//-----------------------------------------------------------------------------
module;


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <sys/types.h>

#include <fcntl.h>

// Timer stuff. Experimental.
#include <signal.h>
#include <time.h>


#include "g_game.h"
#include "m_random.h"
#include "r_main.h"

#include <string>
export module sound;
import system;
import wad;
import argv;
import doomdef;
import doomstat;
import tables;

//
// SoundFX struct.
//
typedef struct sfxinfo_struct sfxinfo_t;

struct sfxinfo_struct {
  // up to 6-character name
  std::string name;

  // Sfx singularity (only one at a time)
  int singularity;

  // Sfx priority
  int priority;

  // referenced sound if a link
  sfxinfo_t *link;

  // pitch if a link
  int pitch;

  // volume if a link
  int volume;

  // sound data
  void *data;

  // this is checked every second to see if sound
  // can be thrown out (if 0, then decrement, if -1,
  // then throw out, if > 0, then it is in use)
  int usefulness;

  // lump number of sfx
  int lumpnum;
};

//
// MusicInfo struct.
//
typedef struct {
  // up to 6-character name
  std::string name;

  // lump number of music
  int lumpnum;

  // music data
  void *data;

  // music handle once registered
  int handle;

} musicinfo_t;

// the complete set of sound effects
extern sfxinfo_t S_sfx[];

// the complete set of music
extern musicinfo_t S_music[];

//
// Identifiers for all music in game.
//

typedef enum {
  mus_None,
  mus_e1m1,
  mus_e1m2,
  mus_e1m3,
  mus_e1m4,
  mus_e1m5,
  mus_e1m6,
  mus_e1m7,
  mus_e1m8,
  mus_e1m9,
  mus_e2m1,
  mus_e2m2,
  mus_e2m3,
  mus_e2m4,
  mus_e2m5,
  mus_e2m6,
  mus_e2m7,
  mus_e2m8,
  mus_e2m9,
  mus_e3m1,
  mus_e3m2,
  mus_e3m3,
  mus_e3m4,
  mus_e3m5,
  mus_e3m6,
  mus_e3m7,
  mus_e3m8,
  mus_e3m9,
  mus_inter,
  mus_intro,
  mus_bunny,
  mus_victor,
  mus_introa,
  mus_runnin,
  mus_stalks,
  mus_countd,
  mus_betwee,
  mus_doom,
  mus_the_da,
  mus_shawn,
  mus_ddtblu,
  mus_in_cit,
  mus_dead,
  mus_stlks2,
  mus_theda2,
  mus_doom2,
  mus_ddtbl2,
  mus_runni2,
  mus_dead2,
  mus_stlks3,
  mus_romero,
  mus_shawn2,
  mus_messag,
  mus_count2,
  mus_ddtbl3,
  mus_ampie,
  mus_theda3,
  mus_adrian,
  mus_messg2,
  mus_romer2,
  mus_tense,
  mus_shawn3,
  mus_openin,
  mus_evil,
  mus_ultima,
  mus_read_m,
  mus_dm2ttl,
  mus_dm2int,
  NUMMUSIC
} musicenum_t;

//
// Identifiers for all sfx in game.
//

typedef enum {
  sfx_None,
  sfx_pistol,
  sfx_shotgn,
  sfx_sgcock,
  sfx_dshtgn,
  sfx_dbopn,
  sfx_dbcls,
  sfx_dbload,
  sfx_plasma,
  sfx_bfg,
  sfx_sawup,
  sfx_sawidl,
  sfx_sawful,
  sfx_sawhit,
  sfx_rlaunc,
  sfx_rxplod,
  sfx_firsht,
  sfx_firxpl,
  sfx_pstart,
  sfx_pstop,
  sfx_doropn,
  sfx_dorcls,
  sfx_stnmov,
  sfx_swtchn,
  sfx_swtchx,
  sfx_plpain,
  sfx_dmpain,
  sfx_popain,
  sfx_vipain,
  sfx_mnpain,
  sfx_pepain,
  sfx_slop,
  sfx_itemup,
  sfx_wpnup,
  sfx_oof,
  sfx_telept,
  sfx_posit1,
  sfx_posit2,
  sfx_posit3,
  sfx_bgsit1,
  sfx_bgsit2,
  sfx_sgtsit,
  sfx_cacsit,
  sfx_brssit,
  sfx_cybsit,
  sfx_spisit,
  sfx_bspsit,
  sfx_kntsit,
  sfx_vilsit,
  sfx_mansit,
  sfx_pesit,
  sfx_sklatk,
  sfx_sgtatk,
  sfx_skepch,
  sfx_vilatk,
  sfx_claw,
  sfx_skeswg,
  sfx_pldeth,
  sfx_pdiehi,
  sfx_podth1,
  sfx_podth2,
  sfx_podth3,
  sfx_bgdth1,
  sfx_bgdth2,
  sfx_sgtdth,
  sfx_cacdth,
  sfx_skldth,
  sfx_brsdth,
  sfx_cybdth,
  sfx_spidth,
  sfx_bspdth,
  sfx_vildth,
  sfx_kntdth,
  sfx_pedth,
  sfx_skedth,
  sfx_posact,
  sfx_bgact,
  sfx_dmact,
  sfx_bspact,
  sfx_bspwlk,
  sfx_vilact,
  sfx_noway,
  sfx_barexp,
  sfx_punch,
  sfx_hoof,
  sfx_metal,
  sfx_chgun,
  sfx_tink,
  sfx_bdopn,
  sfx_bdcls,
  sfx_itmbk,
  sfx_flame,
  sfx_flamst,
  sfx_getpow,
  sfx_bospit,
  sfx_boscub,
  sfx_bossit,
  sfx_bospn,
  sfx_bosdth,
  sfx_manatk,
  sfx_mandth,
  sfx_sssit,
  sfx_ssdth,
  sfx_keenpn,
  sfx_keendt,
  sfx_skeact,
  sfx_skesit,
  sfx_skeatk,
  sfx_radio,
  NUMSFX
} sfxenum_t;

//
// Information about all the music
//

musicinfo_t S_music[] = {
    {"e1m1", 0},   {"e1m2", 0},   {"e1m3", 0},   {"e1m4", 0},   {"e1m5", 0},
    {"e1m6", 0},   {"e1m7", 0},   {"e1m8", 0},   {"e1m9", 0},   {"e2m1", 0},
    {"e2m2", 0},   {"e2m3", 0},   {"e2m4", 0},   {"e2m5", 0},   {"e2m6", 0},
    {"e2m7", 0},   {"e2m8", 0},   {"e2m9", 0},   {"e3m1", 0},   {"e3m2", 0},
    {"e3m3", 0},   {"e3m4", 0},   {"e3m5", 0},   {"e3m6", 0},   {"e3m7", 0},
    {"e3m8", 0},   {"e3m9", 0},   {"inter", 0},  {"intro", 0},  {"bunny", 0},
    {"victor", 0}, {"introa", 0}, {"runnin", 0}, {"stalks", 0}, {"countd", 0},
    {"betwee", 0}, {"doom", 0},   {"the_da", 0}, {"shawn", 0},  {"ddtblu", 0},
    {"in_cit", 0}, {"dead", 0},   {"stlks2", 0}, {"theda2", 0}, {"doom2", 0},
    {"ddtbl2", 0}, {"runni2", 0}, {"dead2", 0},  {"stlks3", 0}, {"romero", 0},
    {"shawn2", 0}, {"messag", 0}, {"count2", 0}, {"ddtbl3", 0}, {"ampie", 0},
    {"theda3", 0}, {"adrian", 0}, {"messg2", 0}, {"romer2", 0}, {"tense", 0},
    {"shawn3", 0}, {"openin", 0}, {"evil", 0},   {"ultima", 0}, {"read_m", 0},
    {"dm2ttl", 0}, {"dm2int", 0}};

//
// Information about all the sfx
//

sfxinfo_t S_sfx[] = {
    // S_sfx[0] needs to be a dummy for odd reasons.
    {"none", false, 0, 0, -1, -1, 0},

    {"pistol", false, 64, 0, -1, -1, 0},
    {"shotgn", false, 64, 0, -1, -1, 0},
    {"sgcock", false, 64, 0, -1, -1, 0},
    {"dshtgn", false, 64, 0, -1, -1, 0},
    {"dbopn", false, 64, 0, -1, -1, 0},
    {"dbcls", false, 64, 0, -1, -1, 0},
    {"dbload", false, 64, 0, -1, -1, 0},
    {"plasma", false, 64, 0, -1, -1, 0},
    {"bfg", false, 64, 0, -1, -1, 0},
    {"sawup", false, 64, 0, -1, -1, 0},
    {"sawidl", false, 118, 0, -1, -1, 0},
    {"sawful", false, 64, 0, -1, -1, 0},
    {"sawhit", false, 64, 0, -1, -1, 0},
    {"rlaunc", false, 64, 0, -1, -1, 0},
    {"rxplod", false, 70, 0, -1, -1, 0},
    {"firsht", false, 70, 0, -1, -1, 0},
    {"firxpl", false, 70, 0, -1, -1, 0},
    {"pstart", false, 100, 0, -1, -1, 0},
    {"pstop", false, 100, 0, -1, -1, 0},
    {"doropn", false, 100, 0, -1, -1, 0},
    {"dorcls", false, 100, 0, -1, -1, 0},
    {"stnmov", false, 119, 0, -1, -1, 0},
    {"swtchn", false, 78, 0, -1, -1, 0},
    {"swtchx", false, 78, 0, -1, -1, 0},
    {"plpain", false, 96, 0, -1, -1, 0},
    {"dmpain", false, 96, 0, -1, -1, 0},
    {"popain", false, 96, 0, -1, -1, 0},
    {"vipain", false, 96, 0, -1, -1, 0},
    {"mnpain", false, 96, 0, -1, -1, 0},
    {"pepain", false, 96, 0, -1, -1, 0},
    {"slop", false, 78, 0, -1, -1, 0},
    {"itemup", true, 78, 0, -1, -1, 0},
    {"wpnup", true, 78, 0, -1, -1, 0},
    {"oof", false, 96, 0, -1, -1, 0},
    {"telept", false, 32, 0, -1, -1, 0},
    {"posit1", true, 98, 0, -1, -1, 0},
    {"posit2", true, 98, 0, -1, -1, 0},
    {"posit3", true, 98, 0, -1, -1, 0},
    {"bgsit1", true, 98, 0, -1, -1, 0},
    {"bgsit2", true, 98, 0, -1, -1, 0},
    {"sgtsit", true, 98, 0, -1, -1, 0},
    {"cacsit", true, 98, 0, -1, -1, 0},
    {"brssit", true, 94, 0, -1, -1, 0},
    {"cybsit", true, 92, 0, -1, -1, 0},
    {"spisit", true, 90, 0, -1, -1, 0},
    {"bspsit", true, 90, 0, -1, -1, 0},
    {"kntsit", true, 90, 0, -1, -1, 0},
    {"vilsit", true, 90, 0, -1, -1, 0},
    {"mansit", true, 90, 0, -1, -1, 0},
    {"pesit", true, 90, 0, -1, -1, 0},
    {"sklatk", false, 70, 0, -1, -1, 0},
    {"sgtatk", false, 70, 0, -1, -1, 0},
    {"skepch", false, 70, 0, -1, -1, 0},
    {"vilatk", false, 70, 0, -1, -1, 0},
    {"claw", false, 70, 0, -1, -1, 0},
    {"skeswg", false, 70, 0, -1, -1, 0},
    {"pldeth", false, 32, 0, -1, -1, 0},
    {"pdiehi", false, 32, 0, -1, -1, 0},
    {"podth1", false, 70, 0, -1, -1, 0},
    {"podth2", false, 70, 0, -1, -1, 0},
    {"podth3", false, 70, 0, -1, -1, 0},
    {"bgdth1", false, 70, 0, -1, -1, 0},
    {"bgdth2", false, 70, 0, -1, -1, 0},
    {"sgtdth", false, 70, 0, -1, -1, 0},
    {"cacdth", false, 70, 0, -1, -1, 0},
    {"skldth", false, 70, 0, -1, -1, 0},
    {"brsdth", false, 32, 0, -1, -1, 0},
    {"cybdth", false, 32, 0, -1, -1, 0},
    {"spidth", false, 32, 0, -1, -1, 0},
    {"bspdth", false, 32, 0, -1, -1, 0},
    {"vildth", false, 32, 0, -1, -1, 0},
    {"kntdth", false, 32, 0, -1, -1, 0},
    {"pedth", false, 32, 0, -1, -1, 0},
    {"skedth", false, 32, 0, -1, -1, 0},
    {"posact", true, 120, 0, -1, -1, 0},
    {"bgact", true, 120, 0, -1, -1, 0},
    {"dmact", true, 120, 0, -1, -1, 0},
    {"bspact", true, 100, 0, -1, -1, 0},
    {"bspwlk", true, 100, 0, -1, -1, 0},
    {"vilact", true, 100, 0, -1, -1, 0},
    {"noway", false, 78, 0, -1, -1, 0},
    {"barexp", false, 60, 0, -1, -1, 0},
    {"punch", false, 64, 0, -1, -1, 0},
    {"hoof", false, 70, 0, -1, -1, 0},
    {"metal", false, 70, 0, -1, -1, 0},
    {"chgun", false, 64, &S_sfx[sfx_pistol], 150, 0, 0},
    {"tink", false, 60, 0, -1, -1, 0},
    {"bdopn", false, 100, 0, -1, -1, 0},
    {"bdcls", false, 100, 0, -1, -1, 0},
    {"itmbk", false, 100, 0, -1, -1, 0},
    {"flame", false, 32, 0, -1, -1, 0},
    {"flamst", false, 32, 0, -1, -1, 0},
    {"getpow", false, 60, 0, -1, -1, 0},
    {"bospit", false, 70, 0, -1, -1, 0},
    {"boscub", false, 70, 0, -1, -1, 0},
    {"bossit", false, 70, 0, -1, -1, 0},
    {"bospn", false, 70, 0, -1, -1, 0},
    {"bosdth", false, 70, 0, -1, -1, 0},
    {"manatk", false, 70, 0, -1, -1, 0},
    {"mandth", false, 70, 0, -1, -1, 0},
    {"sssit", false, 70, 0, -1, -1, 0},
    {"ssdth", false, 70, 0, -1, -1, 0},
    {"keenpn", false, 70, 0, -1, -1, 0},
    {"keendt", false, 70, 0, -1, -1, 0},
    {"skeact", false, 70, 0, -1, -1, 0},
    {"skesit", false, 70, 0, -1, -1, 0},
    {"skeatk", false, 70, 0, -1, -1, 0},
    {"radio", false, 60, 0, -1, -1, 0}};


// These are not used, but should be (menu).
// Maximum volume of a sound effect.
// Internal default is max out of 0-15.
export int snd_SfxVolume = 8;

// Maximum volume of music. Useless so far.
export int snd_MusicVolume = 15;

// Update all 30 millisecs, approx. 30fps synchronized.
// Linux resolution is allegedly 10 millisecs,
//  scale is microseconds.
#define SOUND_INTERVAL 500

// Get the interrupt. Set duration in millisecs.
int I_SoundSetTimer(int duration_of_tick);
void I_SoundDelTimer(void);

// A quick hack to establish a protocol between
// synchronous mix buffer updates and asynchronous
// audio writes. Probably redundant with gametic.
static int flag = 0;

// The number of internal mixing channels,
//  the samples calculated for each mixing step,
//  the size of the 16bit, 2 hardware channel (stereo)
//  mixing buffer, and the samplerate of the raw data.

// Needed for calling the actual sound output.
#define SAMPLECOUNT 512
#define NUM_CHANNELS 8
// It is 2 for 16bit, and 2 for two channels.
#define BUFMUL 4
#define MIXBUFFERSIZE (SAMPLECOUNT * BUFMUL)

#define SAMPLERATE 11025 // Hz
#define SAMPLESIZE 2     // 16bit

// The actual lengths of all sound effects.
int lengths[NUMSFX];

// The actual output device.
int audio_fd;

// The global mixing buffer.
// Basically, samples from all active internal channels
//  are modifed and added, and stored in the buffer
//  that is submitted to the audio device.
signed short mixbuffer[MIXBUFFERSIZE];

// The channel step amount...
unsigned int channelstep[NUM_CHANNELS];
// ... and a 0.16 bit remainder of last step.
unsigned int channelstepremainder[NUM_CHANNELS];

typedef struct {
  // sound information (if null, channel avail.)
  sfxinfo_t *sfxinfo;

  // origin of sound
  void *origin;

  // handle of the sound being played
  int handle;

} channel_t;

// The channel data pointers, start and end.
std::array<channel_t,NUM_CHANNELS> channels;
unsigned char *channelsend[NUM_CHANNELS];

// Time/gametic that the channel started playing,
//  used to determine oldest, which automatically
//  has lowest priority.
// In case number of active sounds exceeds
//  available channels.
int channelstart[NUM_CHANNELS];

// The sound in channel handles,
//  determined on registration,
//  might be used to unregister/stop/modify,
//  currently unused.
int channelhandles[NUM_CHANNELS];

// SFX id of the playing sound effect.
// Used to catch duplicates (like chainsaw).
int channelids[NUM_CHANNELS];

// Pitch to stepping lookup, unused.
int steptable[256];

// Volume lookups.
int vol_lookup[128 * 256];

// Hardware left and right channel volume lookup.
int *channelleftvol_lookup[NUM_CHANNELS];
int *channelrightvol_lookup[NUM_CHANNELS];

//
// This function loads the sound data from the WAD lump,
//  for single sound.
//
void *getsfx(char *sfxname, int *len) {
  unsigned char *sfx;
  unsigned char *paddedsfx;
  int i;
  int size;
  int paddedsize;
  char name[20];
  int sfxlump;

  // Get the sound data from the WAD, allocate lump
  //  in zone memory.
  snprintf(name, 20, "ds%s", sfxname);

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
  if (W_CheckNumForName({name}) == -1)
    sfxlump = W_GetNumForName("dspistol");
  else
    sfxlump = W_GetNumForName(name);

  size = W_LumpLength(sfxlump);

  // Debug.
  // fprintf( stderr, "." );
  // fprintf( stderr, " -loading  %s (lump %d, %d bytes)\n",
  //	     sfxname, sfxlump, size );
  // fflush( stderr );

  sfx = (unsigned char *)W_CacheLumpNum(sfxlump);

  // Pads the sound effect out to the mixing buffer size.
  // The original realloc would interfere with zone memory.
  paddedsize = ((size - 8 + (SAMPLECOUNT - 1)) / SAMPLECOUNT) * SAMPLECOUNT;

  // Allocate from zone memory.
  paddedsfx = (unsigned char *)malloc(paddedsize + 8);
  // ddt: (unsigned char *) realloc(sfx, paddedsize+8);
  // This should interfere with zone memory handling,
  //  which does not kick in in the soundserver.

  // Now copy and pad.
  memcpy(paddedsfx, sfx, size);
  for (i = size; i < paddedsize + 8; i++)
    paddedsfx[i] = 128;

  // Remove the cached lump.
  free(sfx);

  // Preserve padded length.
  *len = paddedsize;

  // Return allocated padded data.
  return (void *)(paddedsfx + 8);
}

//
// This function adds a sound to the
//  list of currently active sounds,
//  which is maintained as a given number
//  (eight, usually) of internal channels.
// Returns a handle.
//
int addsfx(int sfxid, int volume, int step, int seperation) {
  static unsigned short handlenums = 0;

  int i;
  int rc = -1;

  int oldest = gametic;
  int oldestnum = 0;
  int slot;

  int rightvol;
  int leftvol;

  // Chainsaw troubles.
  // Play these sound effects only one at a time.
  if (sfxid == sfx_sawup || sfxid == sfx_sawidl || sfxid == sfx_sawful ||
      sfxid == sfx_sawhit || sfxid == sfx_stnmov || sfxid == sfx_pistol) {
    // Loop all channels, check.
    for (i = 0; i < NUM_CHANNELS; i++) {
      // Active, and using the same SFX?
      if ((true /*channels[i]*/) && (channelids[i] == sfxid)) {
        // Reset.
        //channels[i] = 0;
        // We are sure that iff,
        //  there will only be one.
        break;
      }
    }
  }

  // Loop all channels to find oldest SFX.
  /*for (i = 0; (i < NUM_CHANNELS) && (channels[i]); i++) {
    if (channelstart[i] < oldest) {
      oldestnum = i;
      oldest = channelstart[i];
    }
  }*/

  // Tales from the cryptic.
  // If we found a channel, fine.
  // If not, we simply overwrite the first one, 0.
  // Probably only happens at startup.
  if (i == NUM_CHANNELS)
    slot = oldestnum;
  else
    slot = i;

  // Okay, in the less recent channel,
  //  we will handle the new SFX.
  // Set pointer to raw data.
  //channels[slot] = (unsigned char *)S_sfx[sfxid].data;
  // Set pointer to end of raw data.
  //channelsend[slot] = channels[slot] + lengths[sfxid];

  // Reset current handle number, limited to 0..100.
  if (!handlenums)
    handlenums = 100;

  // Assign current handle number.
  // Preserved so sounds could be stopped (unused).
  channelhandles[slot] = rc = handlenums++;

  // Set stepping???
  // Kinda getting the impression this is never used.
  channelstep[slot] = step;
  // ???
  channelstepremainder[slot] = 0;
  // Should be gametic, I presume.
  channelstart[slot] = gametic;

  // Separation, that is, orientation/stereo.
  //  range is: 1 - 256
  seperation += 1;

  // Per left/right channel.
  //  x^2 seperation,
  //  adjust volume properly.
  leftvol = volume - ((volume * seperation * seperation) >> 16); ///(256*256);
  seperation = seperation - 257;
  rightvol = volume - ((volume * seperation * seperation) >> 16);

  // Sanity check, clamp volume.
  if (rightvol < 0 || rightvol > 127)
    I_Error("rightvol out of bounds");

  if (leftvol < 0 || leftvol > 127)
    I_Error("leftvol out of bounds");

  // Get the proper lookup table piece
  //  for this volume level???
  channelleftvol_lookup[slot] = &vol_lookup[leftvol * 256];
  channelrightvol_lookup[slot] = &vol_lookup[rightvol * 256];

  // Preserve sound SFX id,
  //  e.g. for avoiding duplicates of chainsaw.
  channelids[slot] = sfxid;

  // You tell me.
  return rc;
}

//
// SFX API
// Note: this was called by S_Init.
// However, whatever they did in the
// old DPMS based DOS version, this
// were simply dummies in the Linux
// version.
// See soundserver initdata().
//
export void I_SetChannels() {
  // Init internal lookups (raw data, mixing buffer, channels).
  // This function sets up internal lookups used during
  //  the mixing process.
  int i;
  int j;

  int *steptablemid = steptable + 128;

  // Okay, reset internal mixing channels to zero.
  channels = {};

  // This table provides step widths for pitch parameters.
  // I fail to see that this is currently used.
  for (i = -128; i < 128; i++)
    steptablemid[i] = (int)(pow(2.0, (i / 64.0)) * 65536.0);

  // Generates volume lookup tables
  //  which also turn the unsigned samples
  //  into signed samples.
  for (i = 0; i < 128; i++)
    for (j = 0; j < 256; j++)
      vol_lookup[i * 256 + j] = (i * (j - 128) * 256) / 127;
}

void I_SetSfxVolume(int volume) {
  // Identical to DOS.
  // Basically, this should propagate
  //  the menu/config file setting
  //  to the state variable used in
  //  the mixing.
  snd_SfxVolume = volume;
}

// MUSIC API - dummy. Some code from DOS version.
export void I_SetMusicVolume(int volume) {
  // Internal state variable.
  snd_MusicVolume = volume;
  // Now set volume on output device.
  // Whatever( snd_MusciVolume );
}

//
// Retrieve the raw data lump index
//  for a given SFX name.
//
export int I_GetSfxLumpNum(sfxinfo_t *sfx) {
  if (W_CheckNumForName(sfx->name) != -1) {
    return W_GetNumForName(sfx->name);
  } else {
    I_Debug("Failed getting sfx lump number for: {}", sfx->name);
    return -1;
  }
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
export int I_StartSound(int id, int vol, int sep, int pitch, int priority) {

  // UNUSED
  priority = 0;
  // Debug.
  // fprintf( stderr, "starting sound %d", id );

  // Returns a handle (not used).
  id = addsfx(id, vol, steptable[pitch], sep);

  // fprintf( stderr, "/handle is %d\n", id );

  return id;
}

export void I_StopSound(int handle) {
  // You need the handle returned by StartSound.
  // Would be looping all channels,
  //  tracking down the handle,
  //  an setting the channel to zero.

  // UNUSED.
  handle = 0;
}

export int I_SoundIsPlaying(int handle) {
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
export void I_UpdateSound(void) {
  // Debug. Count buffer misses with interrupt.
  static int misses = 0;

  // Mix current sound data.
  // Data, from raw sound, for right and left.
  unsigned int sample;
  int dl;
  int dr;

  // Pointers in global mixbuffer, left, right, end.
  signed short *leftout;
  signed short *rightout;
  signed short *leftend;
  // Step in mixbuffer, left and right, thus two.
  int step;

  // Mixing channel index.
  int chan;

  // Left and right channel
  //  are in global mixbuffer, alternating.
  leftout = mixbuffer;
  rightout = mixbuffer + 1;
  step = 2;

  // Determine end, for left channel only
  //  (right channel is implicit).
  leftend = mixbuffer + SAMPLECOUNT * step;

  // Mix sounds into the mixing buffer.
  // Loop over step*SAMPLECOUNT,
  //  that is 512 values for two channels.
  while (leftout != leftend) {
    // Reset left/right value.
    dl = 0;
    dr = 0;

    // Love thy L2 chache - made this a loop.
    // Now more channels could be set at compile time
    //  as well. Thus loop those  channels.
    for (chan = 0; chan < NUM_CHANNELS; chan++) {
      // Check channel, if active.
      /*if (channels[chan]) {
        // Get the raw data from the channel.
        sample = *channels[chan];
        // Add left and right part
        //  for this channel (sound)
        //  to the current data.
        // Adjust volume accordingly.
        dl += channelleftvol_lookup[chan][sample];
        dr += channelrightvol_lookup[chan][sample];
        // Increment index ???
        channelstepremainder[chan] += channelstep[chan];
        // MSB is next sample???
        channels[chan] += channelstepremainder[chan] >> 16;
        // Limit to LSB???
        channelstepremainder[chan] &= 65536 - 1;

        // Check whether we are done.
        if (channels[chan] >= channelsend[chan])
          channels[chan] = 0;
      }*/
    }

    // Clamp to range. Left hardware channel.
    // Has been char instead of short.
    // if (dl > 127) *leftout = 127;
    // else if (dl < -128) *leftout = -128;
    // else *leftout = dl;

    if (dl > 0x7fff)
      *leftout = 0x7fff;
    else if (dl < -0x8000)
      *leftout = -0x8000;
    else
      *leftout = dl;

    // Same for right hardware channel.
    if (dr > 0x7fff)
      *rightout = 0x7fff;
    else if (dr < -0x8000)
      *rightout = -0x8000;
    else
      *rightout = dr;

    // Increment current pointers in mixbuffer.
    leftout += step;
    rightout += step;
  }

  // Debug check.
  if (flag) {
    misses += flag;
    flag = 0;
  }

  if (misses > 10) {
    fprintf(stderr, "I_SoundUpdate: missed 10 buffer writes\n");
    misses = 0;
  }

  // Increment flag for update.
  flag++;
}

//
// This would be used to write out the mixbuffer
//  during each game loop update.
// Updates sound buffer and audio device at runtime.
// It is called during Timer interrupt with SNDINTR.
// Mixing now done synchronous, and
//  only output be done asynchronous?
//
void I_SubmitSound(void) {
  // Write it to DSP device.
  //  write(audio_fd, mixbuffer, SAMPLECOUNT*BUFMUL);
}

export void I_UpdateSoundParams(int handle, int vol, int sep, int pitch) {
  // I fail too see that this is used.
  // Would be using the handle to identify
  //  on which channel the sound might be active,
  //  and resetting the channel parameters.

  // UNUSED.
  handle = vol = sep = pitch = 0;
}

void I_ShutdownSound(void) {
  // Wait till all pending sounds are finished.
  int done = 0;
  int i;

  // FIXME (below).
  fprintf(stderr, "I_ShutdownSound: NOT finishing pending sounds\n");
  fflush(stderr);

  while (!done) {
    //for (i = 0; i < 8 && !channels[i]; i++)
      ;

    // FIXME. No proper channel output.
    // if (i==8)
    done = 1;
  }
  I_SoundDelTimer();

  // Cleaning up -releasing the DSP device.
  // close(audio_fd);

  // Done.
  return;
}

void I_InitSound() {

  int i;

  fprintf(stderr, "I_SoundSetTimer: %d microsecs\n", SOUND_INTERVAL);
  I_SoundSetTimer(SOUND_INTERVAL);

  // Secure and configure sound device first.
  fprintf(stderr, "I_InitSound: ");

  // audio_fd = open("/dev/dsp", O_WRONLY);
  // if (audio_fd < 0)
  //   fprintf(stderr, "Could not open /dev/dsp\n");

  i = 11 | (2 << 16);
  // myioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &i);
  // myioctl(audio_fd, SNDCTL_DSP_RESET, 0);

  i = SAMPLERATE;

  // myioctl(audio_fd, SNDCTL_DSP_SPEED, &i);

  i = 1;
  // myioctl(audio_fd, SNDCTL_DSP_STEREO, &i);

  // myioctl(audio_fd, SNDCTL_DSP_GETFMTS, &i);

  // if (i &= AFMT_S16_LE)
  // myioctl(audio_fd, SNDCTL_DSP_SETFMT, &i);
  // else
  // fprintf(stderr, "Could not play signed 16 data\n");

  fprintf(stderr, " configured audio device\n");

  // Initialize external data (all sounds) at start, keep static.
  fprintf(stderr, "I_InitSound: ");

  for (i = 1; i < NUMSFX; i++) {
    // Alias? Example is the chaingun sound linked to pistol.
    if (!S_sfx[i].link) {
      // Load data from WAD file.
      // S_sfx[i].data = getsfx(S_sfx[i].name, &lengths[i]);
    } else {
      // Previously loaded already?
      S_sfx[i].data = S_sfx[i].link->data;
      lengths[i] = lengths[(S_sfx[i].link - S_sfx) / sizeof(sfxinfo_t)];
    }
  }

  fprintf(stderr, " pre-cached all sound data\n");

  // Now initialize mixbuffer with zero.
  for (i = 0; i < MIXBUFFERSIZE; i++)
    mixbuffer[i] = 0;

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

export void I_PlaySong(int handle, int looping) {
  // UNUSED.
  handle = looping = 0;
  musicdies = gametic + TICRATE * 30;
}

export void I_PauseSong(int handle) {
  // UNUSED.
  handle = 0;
}

export void I_ResumeSong(int handle) {
  // UNUSED.
  handle = 0;
}

export void I_StopSong(int handle) {
  // UNUSED.
  handle = 0;

  looping = 0;
  musicdies = 0;
}

export void I_UnRegisterSong(int handle) {
  // UNUSED.
  handle = 0;
}

export int I_RegisterSong(void *data) {
  // UNUSED.
  data = NULL;

  return 1;
}

// Is the song playing?
int I_QrySongPlaying(int handle) {
  // UNUSED.
  handle = 0;
  return looping || musicdies > gametic;
}

//
// Experimental stuff.
// A Linux timer interrupt, for asynchronous
//  sound output.
// I ripped this out of the Timer class in
//  our Difference Engine, including a few
//  SUN remains...
//
#ifdef sun
typedef sigset_t tSigSet;
#else
typedef int tSigSet;
#endif

// We might use SIGVTALRM and ITIMER_VIRTUAL, if the process
//  time independend timer happens to get lost due to heavy load.
// SIGALRM and ITIMER_REAL doesn't really work well.
// There are issues with profiling as well.
// static int /*__itimer_which*/  itimer = ITIMER_REAL;

// static int sig = SIGALRM;

// Interrupt handler.
void I_HandleSoundTimer(int ignore) {
  // Debug.
  // fprintf( stderr, "%c", '+' ); fflush( stderr );

  // Feed sound device if necesary.
  if (flag) {
    // See I_SubmitSound().
    // Write it to DSP device.
    //    write(audio_fd, mixbuffer, SAMPLECOUNT*BUFMUL);

    // Reset flag counter.
    flag = 0;
  } else
    return;

  // UNUSED, but required.
  ignore = 0;
  return;
}

// Get the interrupt. Set duration in millisecs.
int I_SoundSetTimer(int duration_of_tick) {
  // Needed for gametick clockwork.
  //  struct itimerval    value;
  //  struct itimerval    ovalue;
  //  struct sigaction    act;
  //  struct sigaction    oact;

  int res;

  // This sets to SA_ONESHOT and SA_NOMASK, thus we can not use it.
  //     signal( _sig, handle_SIG_TICK );

  // Now we have to change this attribute for repeated calls.
//  act.sa_handler = I_HandleSoundTimer;
#ifndef sun
  // ac	t.sa_mask = _sig;
#endif
  //  act.sa_flags = SA_RESTART;

  //  sigaction( sig, &act, &oact );

  //  value.it_interval.tv_sec    = 0;
  //  value.it_interval.tv_usec   = duration_of_tick;
  //  value.it_value.tv_sec       = 0;
  //  value.it_value.tv_usec      = duration_of_tick;

  // Error is -1.
  //  res = setitimer( itimer, &value, &ovalue );

  // Debug.
  //  if ( res == -1 )
  fprintf(stderr, "I_SoundSetTimer: interrupt n.a.\n");

  //  return res;
  return {};
}

// Remove the interrupt. Set duration to zero.
void I_SoundDelTimer() {
  // Debug.
  if (I_SoundSetTimer(0) == -1)
    fprintf(stderr, "I_SoundDelTimer: failed to remove interrupt. Doh!\n");
}

#define S_MAX_VOLUME 127

// when to clip out sounds
// Does not fit the large outdoor areas.
#define S_CLIPPING_DIST (1200 * 0x10000)

// Distance tp origin when sounds should be maxed out.
// This should relate to movement clipping resolution
// (see BLOCKMAP handling).
// Originally: (200*0x10000).
#define S_CLOSE_DIST (160 * 0x10000)

#define S_ATTENUATOR ((S_CLIPPING_DIST - S_CLOSE_DIST) >> FRACBITS)

// Adjustable by menu.
#define NORM_VOLUME snd_MaxVolume

#define NORM_PITCH 128
#define NORM_PRIORITY 64
#define NORM_SEP 128

#define S_PITCH_PERTURB 1
#define S_STEREO_SWING (96 * 0x10000)

// percent attenuation from front to back
#define S_IFRACVOL 30

#define NA 0
#define S_NUMCHANNELS 2

// Current music/sfx card - index useless
//  w/o a reference LUT in a sound module.
extern int snd_MusicDevice;
extern int snd_SfxDevice;
// Config file? Same disclaimer as above.
extern int snd_DesiredMusicDevice;
extern int snd_DesiredSfxDevice;

// whether songs are mus_paused
static bool mus_paused;

// music currently being played
static musicinfo_t *mus_playing = 0;

static int nextcleanup;

//
// Internals.
//
int S_getChannel(void *origin, sfxinfo_t *sfxinfo);

int S_AdjustSoundParams(mobj_t *listener, mobj_t *source, int *vol, int *sep,
                        int *pitch);

void S_StopChannel(int cnum);

export void S_SetSfxVolume(int volume) {

  if (volume < 0 || volume > 127)
    I_Error("Attempt to set sfx volume at %d", volume);

  snd_SfxVolume = volume;
}


export void S_SetMusicVolume(int volume) {
  if (volume < 0 || volume > 127) {
    I_Error("Attempt to set music volume at %d", volume);
  }

  I_SetMusicVolume(127);
  I_SetMusicVolume(volume);
  snd_MusicVolume = volume;
}

//
// Initializes sound stuff, including volume
// Sets channels, SFX and music volume,
//  allocates channel buffer, sets S_sfx lookup.
//
export void S_Init(int sfxVolume, int musicVolume) {
  int i;

  fprintf(stderr, "S_Init: default sfx volume %d\n", sfxVolume);

  // Whatever these did with DMX, these are rather dummies now.
  I_SetChannels();

  S_SetSfxVolume(sfxVolume);
  // No music with Linux - another dummy.
  S_SetMusicVolume(musicVolume);

  // Allocating the internal channels for mixing
  // (the maximum numer of sounds rendered
  // simultaneously) within zone memory.
  //channels = (channel_t *)malloc(numChannels * sizeof(channel_t));

  // Free all channels for use
  for (i = 0; i < numChannels; i++)
    channels[i].sfxinfo = 0;

  // no sounds are playing, and they are not mus_paused
  mus_paused = 0;

  // Note that sounds have not been cached (yet).
  for (i = 1; i < NUMSFX; i++)
    S_sfx[i].lumpnum = S_sfx[i].usefulness = -1;
}
void S_StopMusic(void) {
  if (mus_playing) {
    if (mus_paused)
      I_ResumeSong(mus_playing->handle);

    I_StopSong(mus_playing->handle);
    I_UnRegisterSong(mus_playing->handle);

    mus_playing->data = 0;
    mus_playing = 0;
  }
}

export void S_ChangeMusic(int musicnum, int looping) {
  musicinfo_t *music{};
  char namebuf[9];

  if ((musicnum <= mus_None) || (musicnum >= NUMMUSIC)) {
    I_Error("Bad music number %d", musicnum);
  } else
    music = &S_music[musicnum];

  if (mus_playing == music)
    return;

  // shutdown old music
  S_StopMusic();

  // get lumpnum if neccessary
  if (!music->lumpnum) {
    music->lumpnum = W_GetNumForName(music->name);
  }

  // load & register it
  music->data = (void *)W_CacheLumpNum(music->lumpnum);
  music->handle = I_RegisterSong(music->data);

  // play it
  I_PlaySong(music->handle, looping);

  mus_playing = music;
}

//
// Per level startup code.
// Kills playing sounds at start of level,
//  determines music if any, changes music.
//
export void S_Start(void) {
  int cnum;
  int mnum;

  // kill all playing sounds at start of level
  //  (trust me - a good idea)
  for (cnum = 0; cnum < numChannels; cnum++)
    if (channels[cnum].sfxinfo)
      S_StopChannel(cnum);

  // start new music for the level
  mus_paused = 0;

  if (gamemode == commercial)
    mnum = mus_runnin + gamemap - 1;
  else {
    int spmus[] = {
        // Song - Who? - Where?

        mus_e3m4, // American	e4m1
        mus_e3m2, // Romero	e4m2
        mus_e3m3, // Shawn	e4m3
        mus_e1m5, // American	e4m4
        mus_e2m7, // Tim 	e4m5
        mus_e2m4, // Romero	e4m6
        mus_e2m6, // J.Anderson	e4m7 CHIRON.WAD
        mus_e2m5, // Shawn	e4m8
        mus_e1m9  // Tim		e4m9
    };

    if (gameepisode < 4)
      mnum = mus_e1m1 + (gameepisode - 1) * 9 + gamemap - 1;
    else
      mnum = spmus[gamemap - 1];
  }

  // HACK FOR COMMERCIAL
  //  if (commercial && mnum > mus_e3m9)
  //      mnum -= mus_e3m9;

  S_ChangeMusic(mnum, true);

  nextcleanup = 15;
}

export void S_StopSound(void *origin) {

  int cnum;

  for (cnum = 0; cnum < numChannels; cnum++) {
    if (channels[cnum].sfxinfo && channels[cnum].origin == origin) {
      S_StopChannel(cnum);
      break;
    }
  }
}

void S_StartSoundAtVolume(void *origin_p, int sfx_id, int volume) {

  int rc;
  int sep;
  int pitch;
  int priority;
  sfxinfo_t *sfx;
  int cnum;

  mobj_t *origin = (mobj_t *)origin_p;

  I_Debug("S_StartSoundAtVolume: playing sound {} ({})",sfx_id, S_sfx[sfx_id].name );

  // check for bogus sound #
  if (sfx_id < 1 || sfx_id > NUMSFX)
    I_Error("Bad sfx #: %d", sfx_id);

  sfx = &S_sfx[sfx_id];

  // Initialize sound parameters
  if (sfx->link) {
    pitch = sfx->pitch;
    priority = sfx->priority;
    volume += sfx->volume;

    if (volume < 1)
      return;

    if (volume > snd_SfxVolume)
      volume = snd_SfxVolume;
  } else {
    pitch = NORM_PITCH;
    priority = NORM_PRIORITY;
  }

  // Check to see if it is audible,
  //  and if not, modify the params
if (origin && origin != players[consoleplayer].mo) {
    rc = S_AdjustSoundParams(players[consoleplayer].mo, origin, &volume, &sep,
                             &pitch);

    if (origin->x == players[consoleplayer].mo->x &&
        origin->y == players[consoleplayer].mo->y) {
      sep = NORM_SEP;
    }

    if (!rc)
      return;
  } else {
    sep = NORM_SEP;
  }

  // hacks to vary the sfx pitches
  if (sfx_id >= sfx_sawup && sfx_id <= sfx_sawhit) {
    pitch += 8 - (M_Random() & 15);

    if (pitch < 0)
      pitch = 0;
    else if (pitch > 255)
      pitch = 255;
  } else if (sfx_id != sfx_itemup && sfx_id != sfx_tink) {
    pitch += 16 - (M_Random() & 31);

    if (pitch < 0)
      pitch = 0;
    else if (pitch > 255)
      pitch = 255;
  }

  // kill old sound
  S_StopSound(origin);

  // try to find a channel
  cnum = S_getChannel(origin, sfx);

  if (cnum < 0)
    return;

  //
  // This is supposed to handle the loading/caching.
  // For some odd reason, the caching is done nearly
  //  each time the sound is needed?
  //

  // get lumpnum if necessary
  if (sfx->lumpnum < 0)
    sfx->lumpnum = I_GetSfxLumpNum(sfx);

#ifndef SNDSRV
  // cache data if necessary
  if (!sfx->data) {
    fprintf(stderr, "S_StartSoundAtVolume: 16bit and not pre-cached - wtf?\n");

    // DOS remains, 8bit handling
    // sfx->data = (void *) W_CacheLumpNum(sfx->lumpnum);
    // fprintf( stderr,
    //	     "S_StartSoundAtVolume: loading %d (lump %d) : 0x%x\n",
    //       sfx_id, sfx->lumpnum, (int)sfx->data );
  }
#endif

  // increase the usefulness
  if (sfx->usefulness++ < 0)
    sfx->usefulness = 1;

  // Assigns the handle to one of the channels in the
  //  mix/output buffer.
  channels[cnum].handle = I_StartSound(sfx_id,
                                       /*sfx->data,*/
                                       volume, sep, pitch, priority);
}

export void S_StartSound(void *origin, int sfx_id) {
#ifdef SAWDEBUG
  // if (sfx_id == sfx_sawful)
  // sfx_id = sfx_itemup;
#endif

  S_StartSoundAtVolume(origin, sfx_id, snd_SfxVolume);

  // UNUSED. We had problems, had we not?
#ifdef SAWDEBUG
  {
    int i;
    int n;

    static mobj_t *last_saw_origins[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    static int first_saw = 0;
    static int next_saw = 0;

    if (sfx_id == sfx_sawidl || sfx_id == sfx_sawful || sfx_id == sfx_sawhit) {
      for (i = first_saw; i != next_saw; i = (i + 1) % 10)
        if (last_saw_origins[i] != origin)
          fprintf(stderr,
                  "old origin 0x%lx != "
                  "origin 0x%lx for sfx %d\n",
                  last_saw_origins[i], origin, sfx_id);

      last_saw_origins[next_saw] = origin;
      next_saw = (next_saw + 1) % 10;
      if (next_saw == first_saw)
        first_saw = (first_saw + 1) % 10;

      for (n = i = 0; i < numChannels; i++) {
        if (channels[i].sfxinfo == &S_sfx[sfx_sawidl] ||
            channels[i].sfxinfo == &S_sfx[sfx_sawful] ||
            channels[i].sfxinfo == &S_sfx[sfx_sawhit])
          n++;
      }

      if (n > 1) {
        for (i = 0; i < numChannels; i++) {
          if (channels[i].sfxinfo == &S_sfx[sfx_sawidl] ||
              channels[i].sfxinfo == &S_sfx[sfx_sawful] ||
              channels[i].sfxinfo == &S_sfx[sfx_sawhit]) {
            fprintf(stderr,
                    "chn: sfxinfo=0x%lx, origin=0x%lx, "
                    "handle=%d\n",
                    channels[i].sfxinfo, channels[i].origin,
                    channels[i].handle);
          }
        }
        fprintf(stderr, "\n");
      }
    }
  }
#endif
}


//
// Stop and resume music, during game PAUSE.
//
export void S_PauseSound(void) {
  if (mus_playing && !mus_paused) {
    I_PauseSong(mus_playing->handle);
    mus_paused = true;
  }
}

export void S_ResumeSound(void) {
  if (mus_playing && mus_paused) {
    I_ResumeSong(mus_playing->handle);
    mus_paused = false;
  }
}

//
// Updates music & sounds
//
export void S_UpdateSounds(void *listener_p) {
  int audible;
  int cnum;
  int volume;
  int sep;
  int pitch;
  sfxinfo_t *sfx;
  channel_t *c;

  mobj_t *listener = (mobj_t *)listener_p;

  // Clean up unused data.
  // This is currently not done for 16bit (sounds cached static).
  // DOS 8bit remains.
  /*if (gametic > nextcleanup)
  {
      for (i=1 ; i<NUMSFX ; i++)
      {
          if (S_sfx[i].usefulness < 1
              && S_sfx[i].usefulness > -1)
          {
              if (--S_sfx[i].usefulness == -1)
              {
                  Z_ChangeTag(S_sfx[i].data);
                  S_sfx[i].data = 0;
              }
          }
      }
      nextcleanup = gametic + 15;
  }*/

  for (cnum = 0; cnum < numChannels; cnum++) {
    c = &channels[cnum];
    sfx = c->sfxinfo;

    if (c->sfxinfo) {
      if (I_SoundIsPlaying(c->handle)) {
        // initialize parameters
        volume = snd_SfxVolume;
        pitch = NORM_PITCH;
        sep = NORM_SEP;

        if (sfx->link) {
          pitch = sfx->pitch;
          volume += sfx->volume;
          if (volume < 1) {
            S_StopChannel(cnum);
            continue;
          } else if (volume > snd_SfxVolume) {
            volume = snd_SfxVolume;
          }
        }

        // check non-local sounds for distance clipping
        //  or modify their params
        if (c->origin && listener_p != c->origin) {
          audible =
              S_AdjustSoundParams(listener, static_cast<mobj_t *>(c->origin),
                                  &volume, &sep, &pitch);

          if (!audible) {
            S_StopChannel(cnum);
          } else
            I_UpdateSoundParams(c->handle, volume, sep, pitch);
        }
      } else {
        // if channel is allocated but sound has stopped,
        //  free it
        S_StopChannel(cnum);
      }
    }
  }
  // kill music if it is a single-play && finished
  // if (	mus_playing
  //      && !I_QrySongPlaying(mus_playing->handle)
  //      && !mus_paused )
  // S_StopMusic();
}

//
// Starts some music with the music id found in sounds.h.
//
export void S_StartMusic(int m_id) {
  // JONNY TODO
  // S_ChangeMusic(m_id, false);
}


void S_StopChannel(int cnum) {

  int i;
  channel_t *c = &channels[cnum];

  if (c->sfxinfo) {
    // stop the sound playing
    if (I_SoundIsPlaying(c->handle)) {
#ifdef SAWDEBUG
      if (c->sfxinfo == &S_sfx[sfx_sawful])
        fprintf(stderr, "stopped\n");
#endif
      I_StopSound(c->handle);
    }

    // check to see
    //  if other channels are playing the sound
    for (i = 0; i < numChannels; i++) {
      if (cnum != i && c->sfxinfo == channels[i].sfxinfo) {
        break;
      }
    }

    // degrade usefulness of sound data
    c->sfxinfo->usefulness--;

    c->sfxinfo = 0;
  }
}

//
// Changes volume, stereo-separation, and pitch variables
//  from the norm of a sound effect to be played.
// If the sound is not audible, returns a 0.
// Otherwise, modifies parameters and returns 1.
//
int S_AdjustSoundParams(mobj_t *listener, mobj_t *source, int *vol, int *sep,
                        int *pitch) {
  fixed_t approx_dist;
  fixed_t adx;
  fixed_t ady;
  angle_t angle;

  // calculate the distance to sound origin
  //  and clip it if necessary
  adx = abs(listener->x - source->x);
  ady = abs(listener->y - source->y);

  // From _GG1_ p.428. Appox. eucledian distance fast.
  approx_dist = adx + ady - ((adx < ady ? adx : ady) >> 1);

  if (gamemap != 8 && approx_dist > S_CLIPPING_DIST) {
    return 0;
  }

  // angle of source to listener
  angle = R_PointToAngle2(listener->x, listener->y, source->x, source->y);

  if (angle > listener->angle)
    angle = angle - listener->angle;
  else
    angle = angle + (0xffffffff - listener->angle);

  angle >>= ANGLETOFINESHIFT;

  // stereo separation
  *sep = 128 - (FixedMul(S_STEREO_SWING, finesine[angle]) >> FRACBITS);

  // volume calculation
  if (approx_dist < S_CLOSE_DIST) {
    *vol = snd_SfxVolume;
  } else if (gamemap == 8) {
    if (approx_dist > S_CLIPPING_DIST)
      approx_dist = S_CLIPPING_DIST;

    *vol = 15 + ((snd_SfxVolume - 15) *
                 ((S_CLIPPING_DIST - approx_dist) >> FRACBITS)) /
                    S_ATTENUATOR;
  } else {
    // distance effect
    *vol = (snd_SfxVolume * ((S_CLIPPING_DIST - approx_dist) >> FRACBITS)) /
           S_ATTENUATOR;
  }

  return (*vol > 0);
}

//
// S_getChannel :
//   If none available, return -1.  Otherwise channel #.
//
int S_getChannel(void *origin, sfxinfo_t *sfxinfo) {
  // channel number to use
  int cnum;

  channel_t *c;

  // Find an open channel
  for (cnum = 0; cnum < numChannels; cnum++) {
    if (!channels[cnum].sfxinfo)
      break;
    else if (origin && channels[cnum].origin == origin) {
      S_StopChannel(cnum);
      break;
    }
  }

  // None available
  if (cnum == numChannels) {
    // Look for lower priority
    for (cnum = 0; cnum < numChannels; cnum++)
      if (channels[cnum].sfxinfo->priority >= sfxinfo->priority)
        break;

    if (cnum == numChannels) {
      // FUCK!  No lower priority.  Sorry, Charlie.
      return -1;
    } else {
      // Otherwise, kick out lower priority.
      S_StopChannel(cnum);
    }
  }

  c = &channels[cnum];

  // channel is decided to be cnum.
  c->sfxinfo = sfxinfo;
  c->origin = origin;

  return cnum;
}
