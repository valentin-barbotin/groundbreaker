#ifndef MAP_H
 #define MAP_H

 typedef struct     s_map
 {
     int         width;
     int         height;
     char        **map;
 }                  t_map;

 t_map   *map_create(int width, int height);
 void    map_fill(const t_map *map);
 void    map_destroy(t_map *map);
 void    map_print(const t_map *map);

#endif