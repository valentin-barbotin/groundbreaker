#ifndef MAP_H
 #define MAP_H
 
 #include <stdbool.h>

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

 t_map   *map_create(int width, int height);
 void    map_fill(const t_map *map);
 void    map_destroy(t_map *map);
 void    map_print(const t_map *map);
 t_map   *getMap();
 void    getMaps();

#endif