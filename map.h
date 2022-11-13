#ifndef MAP_H
 #define MAP_H
 
 #include <stdbool.h>
 #include <SDL.h>

 #include "config.h"
 #include "player.h"
 #include "assets.h"
 #define GETCELL(x, y) (map->map[y][x])

 extern int              g_currentState;
 extern t_gameConfig     *gameConfig;

 extern t_player         *g_bots[MAX_BOTS];
 extern short            g_nbBots;

 typedef struct     s_map
 {
     unsigned short     width;
     unsigned short     height;
     unsigned short     players;
     char               **map;
     bool               selected;
 }                  t_map;

 typedef enum    e_type
 {
         BOMB = 'B',
         WALL= 'W',
         UNBREAKABLE_WALL = 'U',
         EMPTY = ' ',
         ITEM = 'I',
         GRAVEL = 'G',
         PLAYER = 'P'
         LOOT = 'L'
 }               t_type;

 t_map   *map_create(unsigned short width, unsigned short height);
 void    map_fill(const t_map *map);
 void    map_destroy(t_map *map);
 void    map_print(const t_map *map);
 void    saveMap(const t_map *map);
 void    getMaps();
 void    drawMapInRect(const SDL_Rect *rectList, size_t index);
 void    drawMap();
 void    getPlayerDirection(SDL_Rect *rect, const t_direction *direction);
 void    drawPlayer(const t_player *player);

#endif