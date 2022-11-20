#ifndef SOUND_H
 #define SOUND_H

 #include "config.h"
 #include <SDL_mixer.h>
 #define NB_CHANNELS 128

 extern t_gameConfig    *gameConfig;

 typedef struct  s_sound
 {
     char        *file;
     Mix_Chunk   *chunk;
 }               t_sound;

 void    loadSounds();
 bool    initAudio(t_sound *sound);
 int     playSound(t_sound *sound);
 int     playSoundLoop(t_sound *sound);
 bool    stopSound(t_sound *sound);
 bool    pauseSound(t_sound *sound);
 bool    setSoundVolume(t_sound *sound, int volume);
 bool    isChannelAvailable(int channel);
 int     getAvailableChannel();
 void    freeSound(t_sound *sound);
 void    channelFinishedCallback(int channel);

#endif
