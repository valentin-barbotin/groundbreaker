#ifndef PLAYER_H
 #define PLAYER_H

 #include <stdbool.h>

 #include <SDL.h>
 #include "items.h"

 #define MAX_BOMBS 40

 extern SDL_Window*     g_window;
 extern SDL_Renderer*   g_renderer;
 extern bool            g_serverRunning;
 extern t_item          g_items[NB_ITEMS];

         extern pthread_t       g_clientThread;

 typedef enum       e_direction
 {
            DIR_IDLE,
            DIR_UP,
            DIR_DOWN,
            DIR_LEFT,
            DIR_RIGHT,
            DIR_UP_LEFT,
            DIR_UP_RIGHT,
            DIR_DOWN_LEFT,
            DIR_DOWN_RIGHT
 }                  t_direction;

 typedef struct     s_player
 {
     short           id;
     char            name[256];
     int             x;
     int             y;
     short           vx;
     short           vy;
     int             xCell;
     int             yCell;
     int             health;
     int             score;
     t_direction     direction;
     int             scope;
     bool            godMode;
     bool            passThroughBomb;
     bool            bombKick;
     bool            canSurviveExplosion;
     struct s_item   *inventory[NB_ITEMS];
     int             selectedSlot;
     bool            isBot;
     unsigned short  lives;
     unsigned short  maxBombs;
     t_bomb          *bombs[MAX_BOMBS];
     bool            bombPlaced;
 }                  t_player;

 char            *getUsername();
 t_player        *initPlayer();
 t_player        *getPlayer();
 t_direction     getDirection(const t_player *player);
 bool            inMultiplayer();
 bool            isMoving(const t_player *player);
 void            sendPos();
 void            doSendPos(const t_player *player);
 bool            hasItemInInventory(const t_player *player, const struct s_item *item);
 void            initInventory(t_player *player);
 bool            isAlive(const t_player *player);
 void            removePlacedBomb(t_player *player, int xCell, int yCell);
 void            storePlacedBomb(t_player *player, int xCell, int yCell);
 t_player        *findBombOwner(int xCell, int yCell);
 bool            searchPlacedBomb(t_player *player, int xCell, int yCell);

#endif