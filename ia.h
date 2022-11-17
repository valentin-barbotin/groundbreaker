#ifndef IA_H
 #define IA_H

 #include "map.h"
 #include "player.h"

 void    searchBotsPos(const t_map *map);
 void    spawnBot(const t_map *map, short x, short y, t_direction direction);

#endif