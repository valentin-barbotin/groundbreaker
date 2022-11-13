#ifndef GAME_H
 #define GAME_H

 #include <stdbool.h>

 #include "map.h"
 #include "player.h"
 #include "assets.h"
 #include "menu.h"

 extern char            g_username[256];
 extern short           g_playersMultiIndex;

 #define PLAYER_WIDTH   50
 #define PLAYER_HEIGHT  73

 extern t_menu      menuPause;
 extern SDL_Rect    g_buttonsLocation[MAX_MENU_BUTTONS];

 typedef struct     s_game
 {
     t_map               *map;
     t_map               maps[10];
     t_player            **players;
     unsigned short      nbPlayers;
 }                  t_game;

 void    spawnPlayer(int x, int y, t_player *player);
 bool    inGame();
 bool    isMoving();
 bool    isGamePaused();
 void    resumeGame();
 void    pauseGame();
 void    setPath();
 void    game_quit();
 void    launchGame();
 t_game  *getGame();
 void    movePlayer();
 void    posToGrid();
 void    handleMouseButtonUpPlaying(const SDL_Event *event);

#endif
