//
// Created by James Abib on 19/10/2022.
//

#ifndef SOUND_H
 #define SOUND_H

 #include "config.h"
 #include "timer.h"
 #include <SDL_mixer.h>

 extern t_gameConfig    *gameConfig;

 typedef struct  s_sound
 {
     char   *file;
     t_timer     *timer;
     Mix_Music   *music;
 }               t_sound;

 int    initAudio(t_sound *sound);
 int    initMusic(t_sound *sound);
 int    playSound(t_sound *sound);
 int    playSoundLoop(t_sound *sound);
 int    stopSound(t_sound *sound);
 int    pauseSound(t_sound *sound);
 int    unpauseSound(t_sound *sound);
 int    setSoundVolume(t_sound *sound, int volume);


#endif
