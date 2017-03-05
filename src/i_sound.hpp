////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file   src\i_sound.hpp.
///
/// \brief  Declares the i_sound interface.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <map>
#include <list>

#include <SFML/Audio.hpp>

#include "timidity/timidity.h"
#include "sounds.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ISound
///
/// \brief  Sound Interface
////////////////////////////////////////////////////////////////////////////////////////////////////
class I_Sound
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void I_Sound::initialise();
    ///
    /// \brief  Initializes the sound module.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void initialise();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void I_Sound::shutdown();
    ///
    /// \brief  Shutdown sound module.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void shutdown();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void I_Sound::playMusic( const std::string& handle, bool looping);
    ///
    /// \brief  Called by anything that wishes to start music. plays a song, and when the song is
    ///         done, starts playing it again in an endless loop. Horrible thing to do, considering.
    ///
    /// \param  handle  The music name.
    /// \param  looping Loop flag.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void playMusic( const std::string&	handle, bool looping);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static int I_Sound::I_GetSfxLumpNum(sfxinfo_t* sfxinfo);
    ///
    /// \brief  Retrieve the raw data lump index for a given SFX name.
    ///
    /// \param [in,out] sfxinfo If non-null, the sfxinfo to get the lump number for.
    ///
    /// \return The sfx lump number.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static int I_GetSfxLumpNum(sfxinfo_t* sfxinfo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void I_Sound::I_SetMusicVolume(int volume);
    ///
    /// \brief  Sets music volume.
    ///
    /// \param  volume  The volume.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void I_SetMusicVolume(int volume);

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static int I_Sound::I_StartSound( int id,int vol,int pitch);
    ///
    /// \brief  Starts a sound.
    ///
    /// \param  id      The identifier.
    /// \param  vol     The volume.
    /// \param  pitch   The pitch.
    ///
    /// \return An int.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static int I_StartSound( int id,int vol,int pitch);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void I_Sound::I_UpdateSoundParams (int handle, int vol, int sep, int pitch);
    ///
    /// \brief  Updates the sound parameters ( Volume, location, remove finished sounds, etc.).
    ///
    /// \author Jonny
    /// \date   05/03/2017
    ///
    /// \param  handle  The handle.
    /// \param  vol     The volume.
    /// \param  sep     The separator.
    /// \param  pitch   The pitch.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void I_UpdateSoundParams (int handle, int vol, int sep, int pitch);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void I_Sound::I_SetSfxVolume(int volume);
    ///
    /// \brief  Sets sfx volume.
    ///
    /// \author Jonny
    /// \date   05/03/2017
    ///
    /// \param  volume  The volume.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void I_SetSfxVolume(int volume);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn int I_Sound::Mus2Midi(unsigned char* bytes, unsigned char* out, int* len);
    ///
    /// \brief  Converts the MUS format found in original doom WADs into proper midi format
    ///
    /// \author Jonny
    /// \date   05/03/2017
    ///
    /// \param [in,out] bytes   If non-null, the bytes.
    /// \param [in,out] out     If non-null, the out.
    /// \param [in,out] len     If non-null, the length.
    ///
    /// \return An int.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static int Mus2Midi(unsigned char* bytes, unsigned char* out, int* len);


    /// \brief  The doom music
    static MidiSong*				                doomMusic;
    /// \brief  The music sound
    static std::unique_ptr<sf::Sound>              musicSound;
    /// \brief  Buffer for music sound data
    static std::unique_ptr<sf::SoundBuffer>        musicSoundBuffer;
    /// \brief  Buffer for music data
    static char*                                   musicBuffer;
    /// \brief  The sound sfx volume
    static int                                     snd_SfxVolume;
    /// \brief  The sound music volume
    static int                                     snd_MusicVolume;
    /// \brief  The sound buffers
    //static std::map<std::string, sf::SoundBuffer> soundBuffers;
    /// \brief  The sounds
   // static std::list<sf::Sound>	sounds;

};