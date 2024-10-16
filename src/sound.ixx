//-----------------------------------------------------------------------------
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
//	System interface for sound.
//
//-----------------------------------------------------------------------------
module;
#include "g_game.h"
#include "m_random.h"
#include "r_main.h"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Listener.hpp>
#include <algorithm>
#include <string>
#include <ranges>
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
struct sfxinfo_t
{
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
	sf::SoundBuffer data;

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
struct musicinfo_t
{
	// up to 6-character name
	std::string name;

	// lump number of music
	int lumpnum;

	// music data
	void *data;

	// music handle once registered
	int handle;

};

//
// Identifiers for all sfx in game.
//
export enum sfxenum_t
{
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
};

//
// Information about all the sfx
//
sfxinfo_t S_sfx[] = {
	{},
	{"pistol", false, 64, 0, -1, -1, {}},
	{"shotgn", false, 64, 0, -1, -1, {}},
	{"sgcock", false, 64, 0, -1, -1, {}},
	{"dshtgn", false, 64, 0, -1, -1, {}},
	{"dbopn", false, 64, 0, -1, -1, {}},
	{"dbcls", false, 64, 0, -1, -1, {}},
	{"dbload", false, 64, 0, -1, -1, {}},
	{"plasma", false, 64, 0, -1, -1, {}},
	{"bfg", false, 64, 0, -1, -1, {}},
	{"sawup", false, 64, 0, -1, -1, {}},
	{"sawidl", false, 118, 0, -1, -1, {}},
	{"sawful", false, 64, 0, -1, -1, {}},
	{"sawhit", false, 64, 0, -1, -1, {}},
	{"rlaunc", false, 64, 0, -1, -1, {}},
	{"rxplod", false, 70, 0, -1, -1, {}},
	{"firsht", false, 70, 0, -1, -1, {}},
	{"firxpl", false, 70, 0, -1, -1, {}},
	{"pstart", false, 100, 0, -1, -1, {}},
	{"pstop", false, 100, 0, -1, -1, {}},
	{"doropn", false, 100, 0, -1, -1, {}},
	{"dorcls", false, 100, 0, -1, -1, {}},
	{"stnmov", false, 119, 0, -1, -1, {}},
	{"swtchn", false, 78, 0, -1, -1, {}},
	{"swtchx", false, 78, 0, -1, -1, {}},
	{"plpain", false, 96, 0, -1, -1, {}},
	{"dmpain", false, 96, 0, -1, -1, {}},
	{"popain", false, 96, 0, -1, -1, {}},
	{"vipain", false, 96, 0, -1, -1, {}},
	{"mnpain", false, 96, 0, -1, -1, {}},
	{"pepain", false, 96, 0, -1, -1, {}},
	{"slop", false, 78, 0, -1, -1, {}},
	{"itemup", true, 78, 0, -1, -1, {}},
	{"wpnup", true, 78, 0, -1, -1, {}},
	{"oof", false, 96, 0, -1, -1, {}},
	{"telept", false, 32, 0, -1, -1, {}},
	{"posit1", true, 98, 0, -1, -1, {}},
	{"posit2", true, 98, 0, -1, -1, {}},
	{"posit3", true, 98, 0, -1, -1, {}},
	{"bgsit1", true, 98, 0, -1, -1, {}},
	{"bgsit2", true, 98, 0, -1, -1, {}},
	{"sgtsit", true, 98, 0, -1, -1, {}},
	{"cacsit", true, 98, 0, -1, -1, {}},
	{"brssit", true, 94, 0, -1, -1, {}},
	{"cybsit", true, 92, 0, -1, -1, {}},
	{"spisit", true, 90, 0, -1, -1, {}},
	{"bspsit", true, 90, 0, -1, -1, {}},
	{"kntsit", true, 90, 0, -1, -1, {}},
	{"vilsit", true, 90, 0, -1, -1, {}},
	{"mansit", true, 90, 0, -1, -1, {}},
	{"pesit", true, 90, 0, -1, -1, {}},
	{"sklatk", false, 70, 0, -1, -1, {}},
	{"sgtatk", false, 70, 0, -1, -1, {}},
	{"skepch", false, 70, 0, -1, -1, {}},
	{"vilatk", false, 70, 0, -1, -1, {}},
	{"claw", false, 70, 0, -1, -1, {}},
	{"skeswg", false, 70, 0, -1, -1, {}},
	{"pldeth", false, 32, 0, -1, -1, {}},
	{"pdiehi", false, 32, 0, -1, -1, {}},
	{"podth1", false, 70, 0, -1, -1, {}},
	{"podth2", false, 70, 0, -1, -1, {}},
	{"podth3", false, 70, 0, -1, -1, {}},
	{"bgdth1", false, 70, 0, -1, -1, {}},
	{"bgdth2", false, 70, 0, -1, -1, {}},
	{"sgtdth", false, 70, 0, -1, -1, {}},
	{"cacdth", false, 70, 0, -1, -1, {}},
	{"skldth", false, 70, 0, -1, -1, {}},
	{"brsdth", false, 32, 0, -1, -1, {}},
	{"cybdth", false, 32, 0, -1, -1, {}},
	{"spidth", false, 32, 0, -1, -1, {}},
	{"bspdth", false, 32, 0, -1, -1, {}},
	{"vildth", false, 32, 0, -1, -1, {}},
	{"kntdth", false, 32, 0, -1, -1, {}},
	{"pedth", false, 32, 0, -1, -1, {}},
	{"skedth", false, 32, 0, -1, -1, {}},
	{"posact", true, 120, 0, -1, -1, {}},
	{"bgact", true, 120, 0, -1, -1, {}},
	{"dmact", true, 120, 0, -1, -1, {}},
	{"bspact", true, 100, 0, -1, -1, {}},
	{"bspwlk", true, 100, 0, -1, -1, {}},
	{"vilact", true, 100, 0, -1, -1, {}},
	{"noway", false, 78, 0, -1, -1, {}},
	{"barexp", false, 60, 0, -1, -1, {}},
	{"punch", false, 64, 0, -1, -1, {}},
	{"hoof", false, 70, 0, -1, -1, {}},
	{"metal", false, 70, 0, -1, -1, {}},
	{"chgun", false, 64, &S_sfx[sfx_pistol], 150, 0, {}},
	{"tink", false, 60, 0, -1, -1, {}},
	{"bdopn", false, 100, 0, -1, -1, {}},
	{"bdcls", false, 100, 0, -1, -1, {}},
	{"itmbk", false, 100, 0, -1, -1, {}},
	{"flame", false, 32, 0, -1, -1, {}},
	{"flamst", false, 32, 0, -1, -1, {}},
	{"getpow", false, 60, 0, -1, -1, {}},
	{"bospit", false, 70, 0, -1, -1, {}},
	{"boscub", false, 70, 0, -1, -1, {}},
	{"bossit", false, 70, 0, -1, -1, {}},
	{"bospn", false, 70, 0, -1, -1, {}},
	{"bosdth", false, 70, 0, -1, -1, {}},
	{"manatk", false, 70, 0, -1, -1, {}},
	{"mandth", false, 70, 0, -1, -1, {}},
	{"sssit", false, 70, 0, -1, -1, {}},
	{"ssdth", false, 70, 0, -1, -1, {}},
	{"keenpn", false, 70, 0, -1, -1, {}},
	{"keendt", false, 70, 0, -1, -1, {}},
	{"skeact", false, 70, 0, -1, -1, {}},
	{"skesit", false, 70, 0, -1, -1, {}},
	{"skeatk", false, 70, 0, -1, -1, {}},
	{"radio", false, 60, 0, -1, -1, {}} };

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
	{"dm2ttl", 0}, {"dm2int", 0} };

//
// Identifiers for all music in game.
//
export enum musicenum_t
{
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
};


// These are not used, but should be (menu).
// Maximum volume of a sound effect.
// Internal default is max out of 0-15.
export int snd_SfxVolume = 8;

// Maximum volume of music. Useless so far.
export int snd_MusicVolume = 15;

const auto SAMPLERATE = 11025; // Hz

struct channel_t
{
  channel_t(sfxinfo_t* info, void* origin, int vol, int pitch ) : 
  sfxinfo(info),
  origin(origin),
  sound(info->data)
  {
    sound.play();
  };
	// sound information (if null, channel avail.)
	sfxinfo_t *sfxinfo;

	// origin of sound
	void *origin;

	// sound being played
	sf::Sound sound;

};

// The channels used to play sounds
std::vector<channel_t> channels;

//
// Retrieve the raw data lump index
//  for a given SFX name.
//
export int I_GetSfxLumpNum( const std::string& name ) {

	// Sounds are all prefixed with "ds"
	auto lump_name = "ds" + name;

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
	if ( W_CheckNumForName( lump_name ) == -1 )
	{
		I_Log( "{} sound not found, using pistol placeholder", name );
		return W_GetNumForName( "dspistol" );
	}
	else
	{
		return W_GetNumForName( lump_name );
	}
}


void I_SetSfxVolume( int volume ) {
	// Identical to DOS.
	// Basically, this should propagate
	//  the menu/config file setting
	//  to the state variable used in
	//  the mixing.
	snd_SfxVolume = volume;
}

// MUSIC API - dummy. Some code from DOS version.
export void I_SetMusicVolume( int volume ) {
	// Internal state variable.
	snd_MusicVolume = volume;
	// Now set volume on output device.
	// Whatever( snd_MusciVolume );
}

export void I_StopSound( int handle ) {
	// You need the handle returned by StartSound.
	// Would be looping all channels,
	//  tracking down the handle,
	//  an setting the channel to zero.

	// UNUSED.
	handle = 0;
}

export int I_SoundIsPlaying( int handle ) {
	// Ouch.
	return gametic < handle;
}

struct dmx_header
{
  uint16_t format;
  uint16_t sample_rate;
  uint32_t sample_count;
};
static_assert(sizeof(dmx_header) == 8);

export void I_InitSound();

//
// MUSIC API.
// Still no music done.
// Remains. Dummies.
//
void I_InitMusic( void ) {}

void I_ShutdownMusic( void ) {}

static int looping = 0;
static int musicdies = -1;

export void I_PlaySong( int handle, int looping ) {
	// UNUSED.
	handle = looping = 0;
	musicdies = gametic + TICRATE * 30;
}

export void I_PauseSong( int handle ) {
	// UNUSED.
	handle = 0;
}

export void I_ResumeSong( int handle ) {
	// UNUSED.
	handle = 0;
}

export void I_StopSong( int handle ) {
	// UNUSED.
	handle = 0;

	looping = 0;
	musicdies = 0;
}

export void I_UnRegisterSong( int handle ) {
	// UNUSED.
	handle = 0;
}

export int I_RegisterSong( void *data ) {
	// UNUSED.
	data = NULL;

	return 1;
}

// Is the song playing?
int I_QrySongPlaying( int handle ) {
	// UNUSED.
	handle = 0;
	return looping || musicdies > gametic;
}


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

export void S_SetSfxVolume( int volume ) {

	if ( volume < 0 || volume > 127 )
		I_Error( "Attempt to set sfx volume at %d", volume );

	snd_SfxVolume = volume;
}


export void S_SetMusicVolume( int volume ) {
	if ( volume < 0 || volume > 127 )
	{
		I_Error( "Attempt to set music volume at %d", volume );
	}

	I_SetMusicVolume( 127 );
	I_SetMusicVolume( volume );
	snd_MusicVolume = volume;
}

//
// Initializes sound stuff, including volume
// Sets channels, SFX and music volume,
//  allocates channel buffer, sets S_sfx lookup.
//
export void S_Init( int sfxVolume, int musicVolume ) {
	int i;

	fprintf( stderr, "S_Init: default sfx volume %d\n", sfxVolume );

	S_SetSfxVolume( sfxVolume );
	// No music with Linux - another dummy.
	S_SetMusicVolume( musicVolume );

	// Allocating the internal channels for mixing
	// (the maximum numer of sounds rendered
	// simultaneously) within zone memory.
	//channels = (channel_t *)malloc(numChannels * sizeof(channel_t));

	// Free all channels for use
  channels = {};

	// no sounds are playing, and they are not mus_paused
	mus_paused = 0;

	// Note that sounds have not been cached (yet).
	for ( i = 1; i < NUMSFX; i++ )
		S_sfx[i].lumpnum = S_sfx[i].usefulness = -1;
}
void S_StopMusic( void ) {
	if ( mus_playing )
	{
		if ( mus_paused )
			I_ResumeSong( mus_playing->handle );

		I_StopSong( mus_playing->handle );
		I_UnRegisterSong( mus_playing->handle );

		mus_playing->data = 0;
		mus_playing = 0;
	}
}

export void S_ChangeMusic( int musicnum, int looping ) {
	musicinfo_t *music{};
	char namebuf[9];

	if ( (musicnum <= mus_None) || (musicnum >= NUMMUSIC) )
	{
		I_Error( "Bad music number %d", musicnum );
	}
	else
		music = &S_music[musicnum];

	if ( mus_playing == music )
		return;

	// shutdown old music
	S_StopMusic();

	// get lumpnum if neccessary
	if ( !music->lumpnum )
	{
		music->lumpnum = W_GetNumForName( music->name );
	}

	// load & register it
	music->data = (void *)W_CacheLumpNum( music->lumpnum );
	music->handle = I_RegisterSong( music->data );

	// play it
	I_PlaySong( music->handle, looping );

	mus_playing = music;
}

//
// Per level startup code.
// Kills playing sounds at start of level,
//  determines music if any, changes music.
//
export void S_Start( void ) {

	// kill all playing sounds at start of level
	//  (trust me - a good idea)
  channels = {};

	// start new music for the level
	mus_paused = 0;

	auto mnum = mus_runnin + gamemap - 1;
	if ( gamemode != commercial )
	{
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

		if ( gameepisode < 4 )
			mnum = mus_e1m1 + (gameepisode - 1) * 9 + gamemap - 1;
		else
			mnum = spmus[gamemap - 1];
	}

	// HACK FOR COMMERCIAL
	//  if (commercial && mnum > mus_e3m9)
	//      mnum -= mus_e3m9;

	S_ChangeMusic( mnum, true );
}

void S_StartSoundAtVolume( void *origin_p, int sfx_id, int volume ) {
	mobj_t *origin = (mobj_t *)origin_p;
	I_Debug( "S_StartSoundAtVolume: playing sound {} ({})", sfx_id, S_sfx[sfx_id].name );

	// check for bogus sound #
	if ( sfx_id < 1 || sfx_id > NUMSFX)
		I_Error( "Bad sfx #: %d", sfx_id );

	auto sfx = &S_sfx[sfx_id];

  auto pitch = NORM_PITCH;

	// Initialize sound parameters
	if ( sfx->link )
	{
		pitch = sfx->pitch;
		volume += sfx->volume;

		if ( volume < 1 )
			return;

		if ( volume > snd_SfxVolume )
			volume = snd_SfxVolume;
	}

	// hacks to vary the sfx pitches
	if ( sfx_id >= sfx_sawup && sfx_id <= sfx_sawhit )
	{
		pitch += 8 - (M_Random() & 15);

		if ( pitch < 0 )
			pitch = 0;
		else if ( pitch > 255 )
			pitch = 255;
	}
	else if ( sfx_id != sfx_itemup && sfx_id != sfx_tink )
	{
		pitch += 16 - (M_Random() & 31);

		if ( pitch < 0 )
			pitch = 0;
		else if ( pitch > 255 )
			pitch = 255;
	}

	//
	// This is supposed to handle the loading/caching.
	// For some odd reason, the caching is done nearly
	//  each time the sound is needed?
	//

	// get lumpnum if necessary
	if ( sfx->lumpnum < 0 )
		sfx->lumpnum = I_GetSfxLumpNum( sfx->name );

	// cache data if necessary
	if ( !sfx->data.getSampleCount() )
	{
		I_Error( "S_StartSoundAtVolume: 16bit and not pre-cached - wtf?\n" );
	}

	// increase the usefulness
	if ( sfx->usefulness++ < 0 )
		sfx->usefulness = 1;

	channels.emplace_back( sfx, origin, volume, pitch );
}

export void S_StartSound( void *origin, int sfx_id ) {
#ifdef SAWDEBUG
	// if (sfx_id == sfx_sawful)
	// sfx_id = sfx_itemup;
#endif

	S_StartSoundAtVolume( origin, sfx_id, snd_SfxVolume );

	// UNUSED. We had problems, had we not?
#ifdef SAWDEBUG
	{
		int i;
		int n;

		static mobj_t *last_saw_origins[10] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
		static int first_saw = 0;
		static int next_saw = 0;

		if ( sfx_id == sfx_sawidl || sfx_id == sfx_sawful || sfx_id == sfx_sawhit )
		{
			for ( i = first_saw; i != next_saw; i = (i + 1) % 10 )
				if ( last_saw_origins[i] != origin )
					fprintf( stderr,
							 "old origin 0x%lx != "
							 "origin 0x%lx for sfx %d\n",
							 last_saw_origins[i], origin, sfx_id );

			last_saw_origins[next_saw] = origin;
			next_saw = (next_saw + 1) % 10;
			if ( next_saw == first_saw )
				first_saw = (first_saw + 1) % 10;

			for ( n = i = 0; i < numChannels; i++ )
			{
				if ( channels[i].sfxinfo == &S_sfx[sfx_sawidl] ||
					 channels[i].sfxinfo == &S_sfx[sfx_sawful] ||
					 channels[i].sfxinfo == &S_sfx[sfx_sawhit] )
					n++;
			}

			if ( n > 1 )
			{
				for ( i = 0; i < numChannels; i++ )
				{
					if ( channels[i].sfxinfo == &S_sfx[sfx_sawidl] ||
						 channels[i].sfxinfo == &S_sfx[sfx_sawful] ||
						 channels[i].sfxinfo == &S_sfx[sfx_sawhit] )
					{
						fprintf( stderr,
								 "chn: sfxinfo=0x%lx, origin=0x%lx, "
								 "handle=%d\n",
								 channels[i].sfxinfo, channels[i].origin,
								 channels[i].handle );
					}
				}
				fprintf( stderr, "\n" );
			}
		}
	}
#endif
}

export void S_StopSound(void *origin)
{
  std::erase_if(channels,[origin](channel_t& channel){return channel.origin == origin;}); 
}

//
// Stop and resume music, during game PAUSE.
//
export void S_PauseSound( void ) {
	if ( mus_playing && !mus_paused )
	{
		I_PauseSong( mus_playing->handle );
		mus_paused = true;
	}
}

export void S_ResumeSound( void ) {
	if ( mus_playing && mus_paused )
	{
		I_ResumeSong( mus_playing->handle );
		mus_paused = false;
	}
}

//
// Updates music & sounds
//
export void S_UpdateSounds( void *listener_p ) {

	mobj_t *listener = (mobj_t *)listener_p;
  //sf::Listener::setPosition...

  auto stopped = [](channel_t& channel){ return channel.sound.getStatus() == sf::Sound::Status::Stopped;};
  std::erase_if(channels,stopped);
}

//
// Starts some music with the music id found in sounds.h.
//
export void S_StartMusic( int m_id ) {
	// JONNY TODO
	// S_ChangeMusic(m_id, false);
}

module : private;

void I_InitSound() {
	I_Log( "I_InitSound: " );

	for ( auto& sound : S_sfx)
	{
		// Alias? Example is the chaingun sound linked to pistol.
		if ( !sound.link )
		{
			// Load data from WAD file.
			auto sfxlump = I_GetSfxLumpNum( sound.name);
			auto sfx = W_CacheLumpNum( sfxlump );
			auto length = W_LumpLength( sfxlump );

      // Sound lumps are stored in some format with a small header and samples
      auto header = *static_cast<dmx_header*>(sfx);
      sfx = static_cast<char*>(sfx) + sizeof(dmx_header);

      // But the samples are unsigned 8 bit samples, whereas SFML requires signed 16 bit, so we must convert
      std::vector<int16_t> samples(header.sample_count);
      for (auto i : std::ranges::iota_view{0u,samples.size()})
      {
        samples[i] = (static_cast<uint8_t*>(sfx)[i] * 256) ^ 0x8000;
      }
			if (!sound.data.loadFromSamples( samples.data(), samples.size(), 1, header.sample_rate, {sf::SoundChannel::Mono} ))
      {
        I_Error("Failed to load sound: {}", sound.name);
      }
		}
		else
		{
			// Previously loaded already?
			sound.data = sound.link->data;
		}
	}

	I_Log("Sounds initialised");
}