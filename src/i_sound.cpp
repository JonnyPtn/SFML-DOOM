////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file   src\i_sound.cpp.
///
/// \brief  Declares the i_sound interface.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "i_sound.hpp"
#include "s_sound.hpp"
#include "w_wad.hpp"
#include "timidity/controls.h"

#define	MIDI_CHANNELS		2
#if 1 //8bit
#define MIDI_RATE			22050
#define MIDI_SAMPLETYPE		XAUDIOSAMPLETYPE_8BITPCM
#define MIDI_FORMAT			AUDIO_U8
#define MIDI_FORMAT_BYTES	1
#else //16bit
#define MIDI_RATE			48000
#define MIDI_SAMPLETYPE		XAUDIOSAMPLETYPE_16BITPCM
#define MIDI_FORMAT			AUDIO_S16MSB
#define MIDI_FORMAT_BYTES	2
#endif

#define SAMPLERATE		11025	// Hz
#define SAMPLESIZE		2   	// 16bit	

//STATICS
/// \brief  The doom music
 MidiSong*				            I_Sound::doomMusic;
/// \brief  The music sound
 std::unique_ptr<sf::Sound>         I_Sound::musicSound;
/// \brief  Buffer for music sound data
 std::unique_ptr<sf::SoundBuffer>   I_Sound::musicSoundBuffer;
/// \brief  Buffer for music data
 char*                              I_Sound::musicBuffer;
/// \brief  The sound sfx volume
 int                                I_Sound::snd_SfxVolume;
/// \brief  The sound music volume
 int                                I_Sound::snd_MusicVolume;

 //
 // This function loads the sound data from the WAD lump,
 //  for single sound.
 //
 void* getsfx(char* sfxname)
 {
     unsigned char*      sfx;
     int                 i;
     int                 size;
     char                name[20];
     int                 sfxlump;


     // Get the sound data from the WAD, allocate lump
     //  in zone memory.
     sprintf(name, "ds%s", sfxname);

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
     if (WadManager::checkNumForName(name) == -1)
         sfxlump = WadManager::getNumForName("dspistol");
     else
         sfxlump = WadManager::getNumForName(name);

     size = WadManager::WadManager::getLumpLength(sfxlump);

     sfx = (unsigned char*)WadManager::WadManager::getLump(sfxlump);

     // Return allocated padded data.
     return (void *)(sfx);
 }

////////////////////////////////////////////////////////////////////////////////////////////////////
 void I_Sound::initialise()
{
    fprintf(stderr, "I_InitSound: ");

    // Initialize Timidity
    Timidity_Init(MIDI_RATE, MIDI_FORMAT, MIDI_CHANNELS, MIDI_RATE, "eawpats/gravis.cfg");
    fprintf(stderr, " initialised timidity\n");

    //create music objects
    musicSound = std::make_unique<sf::Sound>();
    musicSoundBuffer = std::make_unique<sf::SoundBuffer>();

    // Finished initialization.
    fprintf(stderr, "I_InitSound: sound module ready\n");
}

 ////////////////////////////////////////////////////////////////////////////////////////////////////
 void I_Sound::shutdown()
 {
     musicSound->stop();
     Timidity_Shutdown();
 }

////////////////////////////////////////////////////////////////////////////////////////////////////
void I_Sound::I_SetSfxVolume(int volume)
{
  snd_SfxVolume = volume;
  for (auto& s : sounds)
  {
      s.setVolume(volume / 15.f*100.f);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void I_Sound::I_SetMusicVolume(int volume)
{
  // Internal state variable.
  snd_MusicVolume = volume;
  musicSound->setVolume(volume / 15.f*100.f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int I_Sound::I_GetSfxLumpNum(sfxinfo_t* sfx)
{
    char namebuf[9];
    sprintf(namebuf, "ds%s", sfx->name);
    return WadManager::getNumForName(namebuf);
}

//nothing to see here
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace 
{
    const int MaxMidiConversionSize = 1024 * 1024;
    unsigned char midiConversionBuffer[MaxMidiConversionSize];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void I_Sound::playMusic(const std::string& songname, bool looping)
{
    //Stop if it's not already
    if (musicSound->getStatus() != sf::Sound::Stopped)
    {
        musicSound->stop();
    }

    std::string lumpName = "d_";
    lumpName += songname.c_str();

    unsigned char * musFile = static_cast< unsigned char * >(WadManager::getLump(const_cast<char*>(lumpName.c_str())));

    int length = 0;
    Mus2Midi(musFile, midiConversionBuffer, &length);

    doomMusic = Timidity_LoadSongMem(midiConversionBuffer, length);

    if (doomMusic) 
    {
        musicBuffer = (char *)malloc(MIDI_CHANNELS * MIDI_FORMAT_BYTES * doomMusic->samples);
        Timidity_Start(doomMusic);

        int		rc = RC_NO_RETURN_VALUE;
        int		num_bytes = 0;
        int		offset = 0;

        do 
        {
            rc = Timidity_PlaySome(musicBuffer + offset, MIDI_RATE, &num_bytes);
            offset += num_bytes;
        } while (rc != RC_TUNE_END);

        //convert to correct format for SFML
        std::vector<sf::Int16> newData(MIDI_CHANNELS * MIDI_FORMAT_BYTES * doomMusic->samples);
        auto lastSample = 0;
        int i = 0;
        while (i < MIDI_CHANNELS * MIDI_FORMAT_BYTES * doomMusic->samples)
        {
            sf::Int16 newSample(musicBuffer[i] << 8);	//8bit to 16bit
            newSample ^= 0x8000;	//flip the sign
            newData[i]=(newSample);
            i++;
        }
        //load it into the sound
        musicSoundBuffer->loadFromSamples(newData.data(), newData.size(),MIDI_CHANNELS,MIDI_RATE);
        musicSound->setBuffer(*musicSoundBuffer);
        musicSound->play();
        musicSound->setLoop(looping);

        //music has no position
        musicSound->setRelativeToListener(true);

        Timidity_Stop();
        Timidity_FreeSong(doomMusic);
    }
}