#ifndef GAME_H
 #define GAME_H

 #include <stdbool.h>

 #include "map.h"
 #include "player.h"
 #include "assets.h"

 extern char            g_username[256];
 extern short           g_playersMultiIndex;

 #define PLAYER_WIDTH   50
 #define PLAYER_HEIGHT  73

 typedef struct     s_game
 {
     t_map               *map;
     t_map               maps[10];
     t_player            **players;
     unsigned short      nbPlayers;
 }                  t_game;

 typedef enum      e_directionMap
 {
     UP,
     DOWN,
     LEFT,
     RIGHT,
     NB_DIRECTIONS
 }                 t_directionMap;

void    spawnPlayer(int x, int y, t_player *player);
 bool    inGame();
 bool    isMoving();
 void    setPath();
 void    game_quit();
 t_game  *getGame();
 void    movePlayer();
 void    posToGrid();
 void    posToGridN(int x, int y, int *xCell, int *yCell);
 void    searchDirectionMap(t_directionMap directionMap);
 void    explodeBomb(int xCell, int yCell);

#endif
