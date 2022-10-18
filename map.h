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

 void    saveMap(const t_map *map);
 t_map   *map_create(unsigned short width, unsigned short height, unsigned short players);
 void    map_fill(const t_map *map);
 void    map_destroy(t_map *map);
 void    map_print(const t_map *map);
 void    getMaps();

#endif