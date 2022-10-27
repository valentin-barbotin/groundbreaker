#ifndef GAME_H
 #define GAME_H

 #include <stdbool.h>

 #include "map.h"

 #define TEX_PLAYER     "./assets/bomber.png"
 #define PLAYER_WIDTH   50
 #define PLAYER_HEIGHT  73

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

 void    spawnPlayer();
 bool    inGame();
 void    game_quit();
 t_game  *getGame();
 void    movePlayer();

#endif
