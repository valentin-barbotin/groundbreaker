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
     int                 xCell;
     int                 yCell;
     t_map               *map;
     t_map               maps[10];
 }                  t_game;

 typedef enum      e_directionMap
 {
     UP,
     DOWN,
     LEFT,
     RIGHT,
     NB_DIRECTIONS
 }                 t_directionMap;

 void    spawnPlayer();
 bool    inGame();
 void    explodeBomb(int xCell, int yCell);
 void    posToGrid();
 void    posToGridN(int x, int y, int *xCell, int *yCell);
 void    game_quit();
 void    searchDirectionMap(t_directionMap directionMap);
 t_game  *getGame();
 void    movePlayer();

#endif
