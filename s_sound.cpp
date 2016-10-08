#define SAMPLERATE		11025	// Hz
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <cstring>

#define _USE_MATH_DEFINES
#include <cmath>

//check if M_PI is defined or not
#ifndef M_PI
#define M_PI std::atan(1)*4
#endif

#include "i_system.hpp"
#include "i_sound.hpp"
#include "sounds.hpp"
#include "s_sound.hpp"

#include "z_zone.hpp"
#include "m_random.hpp"
#include "w_wad.hpp"

#include "doomdef.hpp"
#include "p_local.hpp"

#include "doomstat.hpp"

#define S_MAX_VOLUME		127

// when to clip out sounds
// Does not fit the large outdoor areas.
#define S_CLIPPING_DIST		(1200*0x10000)

// Distance tp origin when sounds should be maxed out.
// This should relate to movement clipping resolution
// (see BLOCKMAP handling).
// Originally: (200*0x10000).
#define S_CLOSE_DIST		(160*0x10000)


#define S_ATTENUATOR		((S_CLIPPING_DIST-S_CLOSE_DIST)>>FRACBITS)

// Adjustable by menu.
#define NORM_VOLUME    		snd_MaxVolume

#define NORM_PITCH     		128
#define NORM_PRIORITY		64
#define NORM_SEP		128

#define S_PITCH_PERTURB		1
#define S_STEREO_SWING		(96*0x10000)

// percent attenuation from front to back
#define S_IFRACVOL		30

#define NA			0
#define S_NUMCHANNELS		2


// Current music/sfx card - index useless
//  w/o a reference LUT in a sound module.
extern int snd_MusicDevice;
extern int snd_SfxDevice;
// Config file? Same disclaimer as above.
extern int snd_DesiredMusicDevice;
extern int snd_DesiredSfxDevice;

// These are not used, but should be (menu).
// Maximum volume of a sound effect.
// Internal default is max out of 0-15.
int 		snd_SfxVolume = 15;

// Maximum volume of music. Useless so far.
int 		snd_MusicVolume = 15; 



// whether songs are mus_paused
static bool		mus_paused;	

// music currently being played
static musicinfo_t*	mus_playing=0;

//
// Internals.
//
int
S_AdjustSoundParams
( mobj_t*	listener,
  mobj_t*	source,
  int*		vol,
  int*		sep,
  int*		pitch );

//
// Initializes sound stuff, including volume
// Sets channels, SFX and music volume,
//  allocates channel buffer, sets S_sfx lookup.
//
void S_Init
( int		sfxVolume,
  int		musicVolume )
{  
  int		i;

  fprintf( stderr, "S_Init: default sfx volume %d\n", sfxVolume);

  // Whatever these did with DMX, these are rather dummies now.
  I_SetChannels();
  
  S_SetSfxVolume(sfxVolume);
  // No music with Linux - another dummy.
  S_SetMusicVolume(musicVolume);
  
  // no sounds are playing, and they are not mus_paused
  mus_paused = 0;

  // Note that sounds have not been cached (yet).
  for (i=1 ; i<NUMSFX ; i++)
    S_sfx[i].lumpnum = S_sfx[i].usefulness = -1;
}




//
// Per level startup code.
// Kills playing sounds at start of level,
//  determines music if any, changes music.
//
void S_Start(void)
{
  int cnum;
  int mnum;
  
  // start new music for the level
  mus_paused = 0;
  
  if (gamemode == commercial)
    mnum = mus_runnin + gamemap - 1;
  else
  {
    int spmus[]=
    {
      // Song - Who? - Where?
      
      mus_e3m4,	// American	e4m1
      mus_e3m2,	// Romero	e4m2
      mus_e3m3,	// Shawn	e4m3
      mus_e1m5,	// American	e4m4
      mus_e2m7,	// Tim 	e4m5
      mus_e2m4,	// Romero	e4m6
      mus_e2m6,	// J.Anderson	e4m7 CHIRON.WAD
      mus_e2m5,	// Shawn	e4m8
      mus_e1m9	// Tim		e4m9
    };
    
    if (gameepisode < 4)
      mnum = mus_e1m1 + (gameepisode-1)*9 + gamemap-1;
    else
      mnum = spmus[gamemap-1];
    }	
  
  // HACK FOR COMMERCIAL
  //  if (commercial && mnum > mus_e3m9)	
  //      mnum -= mus_e3m9;
  
  S_ChangeMusic(mnum, true);
}	



void
S_StartSoundAtVolume
(void*		origin_p,
	int		sfx_id,
	int		volume)
{

	int		rc;
	int		sep;
	int		pitch;
	int		priority;
	sfxinfo_t*	sfx;
	int		cnum;

	mobj_t*	origin = (mobj_t *)origin_p;


	// Debug.
	/*fprintf( stderr,
		 "S_StartSoundAtVolume: playing sound %d (%s)\n",
		 sfx_id, S_sfx[sfx_id].name );*/

		 // check for bogus sound #
	if (sfx_id < 1 || sfx_id > NUMSFX)
		I_Error("Bad sfx #: %d", sfx_id);

	sfx = &S_sfx[sfx_id];

	// Initialize sound parameters
	if (sfx->link)
	{
		pitch = sfx->pitch;
		priority = sfx->priority;
		volume += sfx->volume;

		if (volume < 1)
			return;

		if (volume > snd_SfxVolume)
			volume = snd_SfxVolume;
	}
	else
	{
		pitch = NORM_PITCH;
		priority = NORM_PRIORITY;
	}


	// Check to see if it is audible,
	//  and if not, modify the params
	if (origin && origin != players[consoleplayer].mo)
	{
		rc = S_AdjustSoundParams(players[consoleplayer].mo,
			origin,
			&volume,
			&sep,
			&pitch);


		if (origin->x == players[consoleplayer].mo->x
			&& origin->y == players[consoleplayer].mo->y)
		{
			sep = NORM_SEP;
		}

		if (!rc)
			return;
	}
	else
	{
		sep = NORM_SEP;
	}

	// hacks to vary the sfx pitches
	if (sfx_id >= sfx_sawup
		&& sfx_id <= sfx_sawhit)
	{
		pitch += 8 - (M_Random() & 15);

		if (pitch < 0)
			pitch = 0;
		else if (pitch > 255)
			pitch = 255;
	}
	else if (sfx_id != sfx_itemup
		&& sfx_id != sfx_tink)
	{
		pitch += 16 - (M_Random() & 31);

		if (pitch < 0)
			pitch = 0;
		else if (pitch > 255)
			pitch = 255;
	}

	// kill old sound
	S_StopSound(origin);

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
	if (!sfx->data)
	{
		fprintf(stderr,
			"S_StartSoundAtVolume: 16bit and not pre-cached - wtf?\n");

		// DOS remains, 8bit handling
		//sfx->data = (void *) W_CacheLumpNum(sfx->lumpnum, PU_MUSIC);
		// fprintf( stderr,
		//	     "S_StartSoundAtVolume: loading %d (lump %d) : 0x%x\n",
		//       sfx_id, sfx->lumpnum, (int)sfx->data );

	}
#endif
	if (soundBuffers.find(sfx->name) == soundBuffers.end())
	{
		//not loaded yet, set it up
		auto dataSize(W_LumpLength(sfx->lumpnum));
		unsigned char* data((unsigned char*)sfx->data);
		std::vector<sf::Int16> newData;
		auto lastSample = 0;
		int i = 0;
		while (i < dataSize)
		{
			sf::Int16 newSample(data[i] << 8);	//8bit to 16bit
			newSample ^= 0x8000;	//flip the sign
			newData.push_back(newSample);
			i++;
		}

		if (!soundBuffers[sfx->name].loadFromSamples(newData.data(), dataSize, 1, SAMPLERATE))
			fprintf(stderr, "Failed to load sound");
	}

	sounds.emplace_back(soundBuffers[sfx->name]);
	sounds.back().play();
	if (origin && origin != players[consoleplayer].mo)
	{
		sounds.back().setPosition(origin->x, origin->y, origin->z);

		//some numbers to frig it a little
		sounds.back().setMinDistance(S_CLOSE_DIST);
		sounds.back().setAttenuation(1.f);
	}
	else
	{
		sounds.back().setRelativeToListener(true);
	}
	 // increase the usefulness
  if (sfx->usefulness++ < 0)
    sfx->usefulness = 1;
}	

void
S_StartSound
( void*		origin,
  int		sfx_id )
{
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
	
    static mobj_t*      last_saw_origins[10] = {1,1,1,1,1,1,1,1,1,1};
    static int		first_saw=0;
    static int		next_saw=0;
	
    if (sfx_id == sfx_sawidl
	|| sfx_id == sfx_sawful
	|| sfx_id == sfx_sawhit)
    {
	for (i=first_saw;i!=next_saw;i=(i+1)%10)
	    if (last_saw_origins[i] != origin)
		fprintf(stderr, "old origin 0x%lx != "
			"origin 0x%lx for sfx %d\n",
			last_saw_origins[i],
			origin,
			sfx_id);
	    
	last_saw_origins[next_saw] = origin;
	next_saw = (next_saw + 1) % 10;
	if (next_saw == first_saw)
	    first_saw = (first_saw + 1) % 10;
	    
	for (n=i=0; i<numChannels ; i++)
	{
	    if (channels[i].sfxinfo == &S_sfx[sfx_sawidl]
		|| channels[i].sfxinfo == &S_sfx[sfx_sawful]
		|| channels[i].sfxinfo == &S_sfx[sfx_sawhit]) n++;
	}
	    
	if (n>1)
	{
	    for (i=0; i<numChannels ; i++)
	    {
		if (channels[i].sfxinfo == &S_sfx[sfx_sawidl]
		    || channels[i].sfxinfo == &S_sfx[sfx_sawful]
		    || channels[i].sfxinfo == &S_sfx[sfx_sawhit])
		{
		    fprintf(stderr,
			    "chn: sfxinfo=0x%lx, origin=0x%lx, "
			    "handle=%d\n",
			    channels[i].sfxinfo,
			    channels[i].origin,
			    channels[i].handle);
		}
	    }
	    fprintf(stderr, "\n");
	}
    }
}
#endif
 
}




void S_StopSound(void *origin)
{
}









//
// Stop and resume music, during game PAUSE.
//
void S_PauseSound(void)
{
    if (mus_playing && !mus_paused)
    {
	I_PauseSong(mus_playing->handle);
	mus_paused = true;
    }
}

void S_ResumeSound(void)
{
    if (mus_playing && mus_paused)
    {
	I_ResumeSong(mus_playing->handle);
	mus_paused = false;
    }
}


//
// Updates music & sounds
//
void S_UpdateSounds(void* listener_p)
{
    int		audible;
    int		cnum;
    int		volume;
    int		sep;
    int		pitch;
    sfxinfo_t*	sfx;
    
    mobj_t*	listener = (mobj_t*)listener_p;

	if (listener)
	{
		sf::Listener::setPosition(listener->x, listener->y, 0);
		auto angle(static_cast<float>(listener->angle) / std::numeric_limits<unsigned int>::max());
		auto degrees = angle*360.f;

		//flip 180 degrees because...
		degrees += 180.f;
		if (degrees > 360.f)
			degrees -= 360.f;

		auto radians = degrees / (180.f / M_PI);

		sf::Listener::setDirection(std::cos(radians),0, std::sin(radians));
	}

	for (auto sound = sounds.begin();sound != sounds.end();sound++)
	{
		if (sound->getStatus() == sf::Sound::Stopped)
		{
			sounds.erase(sound);
			break;
		}
	}
}


void S_SetMusicVolume(int volume)
{
    if (volume < 0 || volume > 127)
    {
	I_Error("Attempt to set music volume at %d",
		volume);
    }    

    I_SetMusicVolume(127);
    I_SetMusicVolume(volume);
    snd_MusicVolume = volume;
}



void S_SetSfxVolume(int volume)
{

    if (volume < 0 || volume > 127)
	I_Error("Attempt to set sfx volume at %d", volume);

    sf::Listener::setGlobalVolume(volume/15.f*100.f);

}

//
// Starts some music with the music id found in sounds.h.
//
void S_StartMusic(int m_id)
{
    S_ChangeMusic(m_id, false);
}

void
S_ChangeMusic
( int			musicnum,
  int			looping )
{
    musicinfo_t*	music;
    char		namebuf[9];

    if ( (musicnum <= mus_None)
	 || (musicnum >= NUMMUSIC) )
    {
	I_Error("Bad music number %d", musicnum);
    }
    else
	music = &S_music[musicnum];

    if (mus_playing == music)
	return;

    // shutdown old music
    S_StopMusic();

    // get lumpnum if neccessary
    if (!music->lumpnum)
    {
	sprintf(namebuf, "d_%s", music->name);
	music->lumpnum = W_GetNumForName(namebuf);
    }

    // load & register it
    music->data = (void *) W_CacheLumpNum(music->lumpnum, PU_MUSIC);
    music->handle = I_RegisterSong(music->data);

	//first 4 bytes should be "MUS"
	/*char 	ID[4];
	std::memcpy(ID, music->data, 4);
	auto scoreLength = static_cast<sf::Uint16*>(music->data) + 2;
	auto scoreStart = static_cast<sf::Uint16*>(music->data) + 3;
	auto channels = static_cast<sf::Uint16*>(music->data) + 4;
	auto secondaryChannels = static_cast<sf::Uint16*>(music->data) + 5;
	auto instrumentCount = static_cast<sf::Uint16*>(music->data) + 6;
	auto dummy = static_cast<sf::Uint16*>(music->data) + 7;
	auto instruments = static_cast<sf::Uint16*>(malloc(*instrumentCount));
	std::memcpy(instruments,static_cast<sf::Uint16*>(music->data) + 8, *instrumentCount*sizeof(sf::Uint16));
	//load the data into the music
	if (sfMusics.find(music->name) == sfMusics.end())
	{
		//not loaded yet, set it up
		auto dataSize(W_LumpLength(music->lumpnum));
		unsigned char* data((unsigned char*)music->data);
		std::vector<sf::Int16> newData;
		auto lastSample = 0;
		int i = 0;
		while (i<dataSize)
		{
			newData.push_back(static_cast<sf::Int16>(std::pow(data[i], 2)));
			i++;
		}

		if (!sfSounds[music->name].buffer.loadFromSamples(newData.data(), dataSize, 1, SAMPLERATE))
			fprintf(stderr, "Failed to load sound");
		sfSounds[music->name].sound.setBuffer(sfSounds[music->name].buffer);
	}

	sfSounds[music->name].sound.play();*/

    // play it
    I_PlaySong(music->handle, looping);

    mus_playing = music;
}


void S_StopMusic(void)
{
    if (mus_playing)
    {
	if (mus_paused)
	    I_ResumeSong(mus_playing->handle);

	I_StopSong(mus_playing->handle);
	I_UnRegisterSong(mus_playing->handle);
	
	mus_playing->data = 0;
	mus_playing = 0;
    }
}

//
// Changes volume, stereo-separation, and pitch variables
//  from the norm of a sound effect to be played.
// If the sound is not audible, returns a 0.
// Otherwise, modifies parameters and returns 1.
//
int
S_AdjustSoundParams
( mobj_t*	listener,
  mobj_t*	source,
  int*		vol,
  int*		sep,
  int*		pitch )
{
    int	approx_dist;
    int	adx;
    int	ady;
    angle_t	angle;

    // calculate the distance to sound origin
    //  and clip it if necessary
    adx = abs(listener->x - source->x);
    ady = abs(listener->y - source->y);

    // From _GG1_ p.428. Appox. eucledian distance fast.
    approx_dist = adx + ady - ((adx < ady ? adx : ady)>>1);
    
    if (gamemap != 8
	&& approx_dist > S_CLIPPING_DIST)
    {
	return 0;
    }
    
    // angle of source to listener
    angle = R_PointToAngle2(listener->x,
			    listener->y,
			    source->x,
			    source->y);

    if (angle > listener->angle)
	angle = angle - listener->angle;
    else
	angle = angle + (0xffffffff - listener->angle);

    angle >>= ANGLETOFINESHIFT;

    // stereo separation
    *sep = 128 - (FixedMul(S_STEREO_SWING,finesine[angle])>>FRACBITS);

    // volume calculation
    if (approx_dist < S_CLOSE_DIST)
    {
	*vol = snd_SfxVolume;
    }
    else if (gamemap == 8)
    {
	if (approx_dist > S_CLIPPING_DIST)
	    approx_dist = S_CLIPPING_DIST;

	*vol = 15+ ((snd_SfxVolume-15)
		    *((S_CLIPPING_DIST - approx_dist)>>FRACBITS))
	    / S_ATTENUATOR;
    }
    else
    {
	// distance effect
	*vol = (snd_SfxVolume
		* ((S_CLIPPING_DIST - approx_dist)>>FRACBITS))
	    / S_ATTENUATOR; 
    }
    return (*vol > 0);
}
