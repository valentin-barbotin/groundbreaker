#ifndef SOUND_H
 #define SOUND_H

 #include "config.h"
 #include "timer.h"
 #include <SDL_mixer.h>
 #define IS_FILE_TYPE(type) (strcmp(fileType, type) == 0)

 extern t_gameConfig    *gameConfig;

 typedef struct  s_sound
 {
     char        *file;
     Mix_Chunk   *chunk;
     int         channel;
 }               t_sound;

 bool    initAudio(t_sound *sound);
 bool    playSound(t_sound *sound);
 bool    playSoundLoop(t_sound *sound);
 bool    stopSound(t_sound *sound);
 bool    pauseSound(t_sound *sound);
 bool    setSoundVolume(t_sound *sound, int volume);
 bool    isSoundPlaying(t_sound *sound);

#endif
