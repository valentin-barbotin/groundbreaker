#ifndef GAME_H
 #define GAME_H

 #include <stdbool.h>

 #include "map.h"
 #include "player.h"

 #define TEX_PLAYER     "./assets/bomber.png"
 #define PLAYER_WIDTH   50
 #define PLAYER_HEIGHT  73

 typedef struct     s_game
 {
     t_map               *map;
     t_map               maps[10];
     t_player            **players;
     unsigned short      nbPlayers;
 }                  t_game;

 void    spawnPlayer(int x, int y);
 bool    inGame();
 void    game_quit();
 t_game  *getGame();
 void    movePlayer();
 void    posToGrid();

#endif
