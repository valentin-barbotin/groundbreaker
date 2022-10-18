#ifndef GAME_H
 #define GAME_H

 #include <stdbool.h>

 #include "map.h"

 typedef struct     s_game
 {
     int        x;
     int        y;
     int        lastX;
     int        lastY;
     t_map      *map;
     t_map      maps[10];
 }                  t_game;

 bool    inGame();
 void    game_quit();
 t_game  *getGame();
 void    movePlayer(t_game *game);

#endif
