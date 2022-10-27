#ifndef GAME_H
 #define GAME_H

 #include <stdbool.h>

 #include "map.h"

 typedef struct     s_game
 {
     int                 x;
     int                 y;
     short               vx;
     short               vy;
     unsigned short      xCell;
     unsigned short      yCell;
     t_map               *map;
     t_map               maps[10];
 }                  t_game;

 bool    inGame();
 bool    isThereA(short x, short y, t_type type);
 void    game_quit();
 t_game  *getGame();
 void    makeOldPosEmpty(short x, short y);
 void    movePlayer();
 void    explodeBomb(short x, short y);

#endif
