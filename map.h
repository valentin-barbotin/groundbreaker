#ifndef MAP_H
 #define MAP_H
 
 #include <stdbool.h>
 #include <SDL.h>

 #include "config.h"

 #define TEX_WALL "../assets/wall.png"
 #define TEX_UNBREAKABLE_WALL "../assets/unbreak.png"
 #define TEX_DIRT "../assets/dirt.png"

 extern int              g_currentState;
 extern t_gameConfig     *gameConfig;

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
         PLAYER = 'P'
 }               t_type;

 t_map   *map_create(unsigned short width, unsigned short height);
 void    map_fill(const t_map *map);
 void    map_destroy(t_map *map);
 void    map_print(const t_map *map);
 void    saveMap(const t_map *map);
 void    *getMaps();
 void    drawMap(const t_map *map);
 void    getPlayerDirection(SDL_Rect *rect);
 void    drawPlayer();

#endif