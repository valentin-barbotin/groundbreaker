#ifndef GAME_H
 #define GAME_H

#include "map.h"

typedef struct     s_game
{
    int x;
    int y;
}                  t_game;

void    game_init(t_game *game);
void    game_quit();
t_game  *getGame();
void    movePlayer(t_game *game, t_map *map);

#endif
