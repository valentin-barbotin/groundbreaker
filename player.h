#ifndef PLAYER_H
 #define PLAYER_H

 #include <SDL.h>

 #include "items.h"

 extern SDL_Window*     g_window;
 extern SDL_Renderer*   g_renderer;

 typedef struct     s_player
 {
     char            name[256];
     int             x;
     int             y;
     short           vx;
     short           vy;
     int             xCell;
     int             yCell;
     int             health;
     int             score;
     int             scope;
     t_item          *inventory[NB_ITEMS];
    }t_player;

 t_player   *initPlayer();
 t_player   *getPlayer();

 void       useItem(t_item *item);
 void       initInventory();
 bool       hasItemInInventory(t_item *item);

#endif