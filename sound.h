#ifndef SOUND_H
 #define SOUND_H

 #include "config.h"
 #include "timer.h"
 #include <SDL_mixer.h>

 extern t_gameConfig    *gameConfig;

 typedef struct  s_sound
 {
     char        *file;
     t_timer     *timer;
     Mix_Music   *music;
 }               t_sound;

 bool    initAudio(t_sound *sound);
 bool    initMusic(t_sound *sound);
 bool    playSound(t_sound *sound);
 bool    playSoundLoop(t_sound *sound);
 bool    stopSound(t_sound *sound);
 bool    pauseSound(t_sound *sound);
 bool    setSoundVolume(t_sound *sound, int volume);
#endif
