#pragma once

#include <SFML/Audio.hpp>
#include <map>

//sounds, sfx, etc.
static std::map<std::string, sf::SoundBuffer> soundBuffers;
static std::vector<sf::Sound>	sounds;

//
// Initializes sound stuff, including volume
// Sets channels, SFX and music volume,
//  allocates channel buffer, sets S_sfx lookup.
//
void
S_Init
( int		sfxVolume,
  int		musicVolume );




//
// Per level startup code.
// Kills playing sounds at start of level,
//  determines music if any, changes music.
//
void S_Start(void);


//
// Start sound for thing at <origin>
//  using <sound_id> from sounds.h
//
void
S_StartSound
( void*		origin,
  int		sound_id );



// Will start a sound at a given volume.
void
S_StartSoundAtVolume
( void*		origin,
  int		sound_id,
  int		volume );


// Stop sound for thing at <origin>
void S_StopSound(void* origin);


// Start music using <music_id> from sounds.h
void S_StartMusic(int music_id);

// Start music using <music_id> from sounds.h,
//  and set whether looping
void
S_ChangeMusic
( int		music_id,
  int		looping );

// Stops the music fer sure.
void S_StopMusic(void);

// Stop and resume music, during game PAUSE.
void S_PauseSound(void);
void S_ResumeSound(void);


//
// Updates music & sounds
//
void S_UpdateSounds(void* listener);

void S_SetMusicVolume(int volume);
void S_SetSfxVolume(int volume);