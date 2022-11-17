#ifndef EFFECTS_H
 #define EFFECTS_H

 #include <stdlib.h>
 #include <stdbool.h>

 #include <SDL.h>
 
 #include "assets.h"
 #include "utils.h"
 #include "display.h"

 #define MAX_EFFECTS 100
 #define TICKETS_PER_EFFECT 100

 extern SDL_Window*     g_window;
 extern SDL_Renderer*   g_renderer;
 extern t_gameConfig    *gameConfig;

 typedef enum       e_effects
 {
    BOMB_EXPLOSION,
 }                  t_effects;
 
 typedef struct     s_effect
 {
     int                x;
     int                y;
     int                xCell;
     int                yCell;
     Uint64             startTicks;
     int                frameMax;
     t_effects          type;
     int                rows;
     int                cols;
     bool               active;
 }                  t_effect;

 void       initEffects();
 void       findAvailableEffect(t_effect **effect);
 void       drawEffects();
 t_effect   *addEffect(int xCell, int yCell, t_effects type);
 void       drawSprite(const char *tex, int x, int y, int row, int col);
 void       receiveEffect(const char *content);

// w 71 h 61
#endif