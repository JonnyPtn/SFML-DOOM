////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file   src\i_sound.cpp.
///
/// \brief  Declares the i_sound interface.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "doomstat.hpp"
#include "i_sound.hpp"
#include "m_random.hpp"
#include "r_main.hpp"
#include "w_wad.hpp"
#include "timidity/controls.h"
#include "g_game.hpp"
#include "i_system.hpp"

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

std::unique_ptr<I_Sound> I_Sound::instance;

////////////////////////////////////////////////////////////////////////////////////////////////////
 void I_Sound::initialise()
{
    fprintf(stderr, "I_InitSound: ");

    // Initialize Timidity
    Timidity_Init(MIDI_RATE, MIDI_FORMAT, MIDI_CHANNELS, MIDI_RATE, "eawpats/gravis.cfg");
    fprintf(stderr, " initialised timidity\n");

    //create music objects
    musicSound.reset(new sf::Sound());
    musicSoundBuffer.reset(new sf::SoundBuffer());

    fprintf(stderr, " default sfx volume %d\n", snd_SfxVolume);
    fprintf(stderr, " default music volume %d\n", snd_MusicVolume);

    setSfxVolume(snd_SfxVolume);

    setMusicVolume(snd_MusicVolume);

    // Finished initialization.
    fprintf(stderr, "I_InitSound: sound module ready\n");
}

 ////////////////////////////////////////////////////////////////////////////////////////////////////
 void I_Sound::shutdown()
 {
     musicSound->stop();
     Timidity_Shutdown();
     instance.release();
 }

////////////////////////////////////////////////////////////////////////////////////////////////////
void I_Sound::setSfxVolume(int volume)
{
  snd_SfxVolume = volume;
  for (auto& s : sounds)
  {
      s.first->setVolume(volume / 15.f*100.f);
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int I_Sound::getSfxVolume()
{
    return snd_SfxVolume;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void I_Sound::setMusicVolume(int volume)
{
  // Internal state variable.
  snd_MusicVolume = volume;
  musicSound->setVolume(volume / 15.f*100.f);
}
int I_Sound::getMusicVolume()
{
    return snd_MusicVolume;
}
void I_Sound::stopAllSounds()
{
    sounds.clear();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void I_Sound::startSound(void * origin_p, int sfx_id, int volume)
{
    int		pitch = NORM_PITCH;
    sfxinfo_t*	sfx;

    mobj_t*	origin = (mobj_t *)origin_p;

    // check for bogus sound #
    if (sfx_id < 1 || sfx_id > NUMSFX)
        printf("Bad sfx #: %d", sfx_id);

    sfx = &S_sfx[sfx_id];

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
    stopSound(origin);

    //
    // This is supposed to handle the loading/caching.
    // For some odd reason, the caching is done nearly
    //  each time the sound is needed?
    //

    // get lumpnum if necessary
    if (sfx->lumpnum < 0)
        sfx->lumpnum = I_Sound::getSfxLumpNum(sfx);

    if (soundBuffers.find(sfx->name) == soundBuffers.end())
    {
        //not loaded yet, set it up
        auto dataSize(WadManager::getLumpLength(sfx->lumpnum) - 8);
        unsigned char* data((unsigned char*)WadManager::getLump(sfx->lumpnum) + 8);
        std::vector<sf::Int16> newData(dataSize);
        int i = 0;
        while (i < dataSize)
        {
            sf::Int16 newSample(data[i] << 8);	//8bit to 16bit
            newSample ^= 0x8000;	//flip the sign
            newData[i] = newSample;
            i++;
        }
        
        soundBuffers[sfx->name] = std::make_unique<sf::SoundBuffer>();
        if (!soundBuffers[sfx->name]->loadFromSamples(newData.data(), dataSize, 1, SAMPLERATE))
            fprintf(stderr, "Failed to load sound");
    }

    sounds.push_back(std::make_pair( std::make_unique<sf::Sound>() , origin));
    sounds.back().first->setBuffer(*soundBuffers[sfx->name]);
    sounds.back().first->play();
    if (origin && origin != players[consoleplayer].mo)
    {
        sounds.back().first->setPosition(static_cast<float>(origin->x), static_cast<float>(origin->y), static_cast<float>(origin->z));

        //some numbers to frig it a little
        sounds.back().first->setMinDistance(S_CLOSE_DIST);
        sounds.back().first->setAttenuation(S_ATTENUATOR);
        sounds.back().first->setPitch(float(pitch) / 255.f);
    }
    else
    {
        sounds.back().first->setRelativeToListener(true);
    }
    sounds.back().first->setVolume(snd_SfxVolume / 15.f*100.f);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void I_Sound::pauseSound()
{
    //pause all sounds
    for (auto& sound : sounds)
        sound.first->pause();

    musicSound->pause();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void I_Sound::resumeSound()
{
    //resume all sounds
    for (auto& sound : sounds)
        sound.first->play();

    musicSound->play();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void I_Sound::stopSound(void * origin)
{
    if (!origin)
        return;

    for (auto s = sounds.begin(); s !=sounds.end();s++)
    {
        if (s->second == origin)
        {
            sounds.erase(s);
            return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void I_Sound::S_UpdateSounds(mobj_t * listener_p)
{
    //remove any finished sounds
    /*sounds.erase(std::remove_if(sounds.begin(), sounds.end(), [](const std::pair<sf::Sound, void*>& sound)
    {
        return sound.first.getStatus() == sf::Sound::Stopped;
    }),sounds.end());*/

    if (listener_p)
    {
        sf::Listener::setPosition(static_cast<float>(listener_p->x), static_cast<float>(listener_p->y), static_cast<float>(listener_p->z));
        auto angle(static_cast<float>(listener_p->angle) / std::numeric_limits<unsigned int>::max());
        auto degrees = angle*360.f;

        //flip 180 degrees because...
        degrees += 180.f;
        if (degrees > 360.f)
            degrees -= 360.f;

        auto radians = degrees / (180.f / atan(1.f)*4); //pi

        sf::Listener::setDirection(cos(radians), 0, sin(radians));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int I_Sound::getSfxLumpNum(sfxinfo_t* sfx)
{
    auto namebuf = "ds" +  std::string(sfx->name);
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
    printf("I_Sound::playMusic: Playing new track: '%s'\n", songname.c_str());

    //Stop if it's not already
    if (musicSound->getStatus() != sf::Sound::Status::Stopped)
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

////////////////////////////////////////////////////////////////////////////////////////////////////
void I_Sound::playMusic(const int musicNum, bool looping)
{
    //don't play music for shareware
    if (Game::gamemode == GameMode_t::shareware)
        return;
    if ((musicNum <= mus_None) || (musicNum >= NUMMUSIC))
    {
        printf("Bad music number %d", musicNum);
        return;
    }

    auto& music = S_music[musicNum];

    playMusic(music.name, looping);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
I_Sound* I_Sound::getInstance()
{
    if (!instance)
        instance = std::make_unique<I_Sound>();
    
    return instance.get();
}
