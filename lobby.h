#ifndef LOBBY_H
 #define LOBBY_H

 #include <SDL.h>
 #include <SDL_ttf.h>
 #include <stdio.h>
 #include <stdlib.h>

 #include "map.h"

 extern short       g_nbMap;

 typedef struct     s_lobby {
    unsigned short    rows;
    unsigned short    columns;
    unsigned short    players;
    t_map             maps[10];
 }                  t_lobby;

 void       selectMap(short index);

#endif