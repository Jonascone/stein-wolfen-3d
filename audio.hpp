#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <iostream>
#include <ctime>

#include <AL/alut.h>
#include "vec.hpp"

// Sources
ALuint SOURCE_MENU = 0, SOURCE_MUSIC = 0, SOURCE_DOOR = 0, SOURCE_PICKUP = 0, SOURCE_NPC = 0,

// Sounds

       // Menu
       MENU_SELECT = 0, MENU_SWITCH = 0, MENU_BACK = 0,

       // Door
       SOUND_DOPEN = 0, SOUND_DCLOSE = 0, SOUND_EXIT = 0,

       // Treasure
       SOUND_TCUP = 0, SOUND_TCROSS = 0, SOUND_TCHEST = 0, SOUND_TCROWN = 0,

       // Pickups
       SOUND_HMEDKIT = 0,

       // Weapons
       SOUND_WKNIFE = 0, SOUND_WPISTOL = 0, SOUND_WMACH = 0, SOUND_WCHAIN = 0,
       SOUND_PMACH = 0, SOUND_PCHAIN = 0, SOUND_PAMMO = 0,

       // NPCs
       SOUND_NGUARD_AGG = 0, SOUND_NGUARD_DED[7] = {0},

// Music
       MUSIC_SPLASH = 0, MUSIC_MENU = 0, MUSIC_WIN = 0;

void playSound(ALuint Sound, ALuint Source, const vec3 &Pos, bool Looping = false, bool Everywhere = false) {

    alSourceStop(Source); // Stop the source playing, if at all.
    alSourcei(Source, AL_BUFFER, Sound); // Load the specified sound buffer into the source.
    alSource3f(Source, AL_POSITION, Pos.x, Pos.y, Pos.z); // Set the position where the source will play in 3D space.
    if (Everywhere) { alSourcei(Source, AL_SOURCE_RELATIVE, AL_TRUE); } // Determine whether the position should be in relation to the listener (player).
    else { alSourcei(Source, AL_SOURCE_RELATIVE, AL_FALSE); }
    alSourcei(Source, AL_LOOPING, Looping); // Set whether it'll loop the sound or not.
    alSourcePlay(Source); // Play the source!

}
void playMusic(ALuint Music, bool Looping = true) { // By default we would want music to loop, no?

    alSourceStop(SOURCE_MUSIC);
    alSourcei(SOURCE_MUSIC, AL_BUFFER, Music);
    alSource3f(SOURCE_MUSIC, AL_POSITION, 0.f, 0.f, 0.f);
    alSourcei(SOURCE_MUSIC, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcei(SOURCE_MUSIC, AL_LOOPING, Looping);
    alSourcePlay(SOURCE_MUSIC);

}

#endif // AUDIO_HPP
