#ifndef BOMB_H
 #define BOMB_H

 #include <SDL.h>

//  #include "player.h"

 typedef struct     s_bomb
 {
        SDL_Point     pos;
        void          *owner; //tmp, wtf
        // t_player      *owner;
 }                  t_bomb;

#endif