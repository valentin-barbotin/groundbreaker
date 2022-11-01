#ifndef PLAYER_H
 #define PLAYER_H

 #include <SDL.h>

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
 }                  t_player;

 t_player   *initPlayer();
 t_player   *getPlayer();

#endif