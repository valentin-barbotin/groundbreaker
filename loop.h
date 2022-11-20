#ifndef LOOP_H
 #define LOOP_H

 #include <SDL.h>
 #include <SDL_ttf.h>

 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>

 #include "menu.h"
 #include "sound.h"

 extern SDL_Window     *g_window;
 extern SDL_Renderer   *g_renderer;
 extern t_menu         *g_currentMenu;
 extern t_lobby        *g_lobby;
 extern short          g_currentOption;
 extern bool           g_serverRunning;
 extern bool           g_serverRunningUDP;

 extern t_sound     *walk;
 extern t_sound     *wall;
 extern t_sound     *unbreakableWall;
 extern t_sound     *bombExplosion;
 extern t_sound     *item;
 extern t_sound     *hurt;

 typedef enum       e_command
 {
     ACTION_OTHER,
     ACTION_KEY_UP,
     ACTION_KEY_DOWN,
     ACTION_KEY_LEFT,
     ACTION_KEY_RIGHT,
     ACTION_KEY_USE_ITEM,
     ACTION_KEY_ITEM_1,
     ACTION_KEY_ITEM_2,
     ACTION_KEY_ITEM_3,
     ACTION_KEY_ITEM_4,
     ACTION_KEY_ITEM_5,
 }                  t_command;


 bool   inMainMenu();
 void   makeSelection(unsigned short);
 void   handleMouseButtonUp(const SDL_Event *event);
 void   handleKeyDown(const SDL_KeyboardEvent *event);
 void   handleKeyUp(const SDL_Event *event);
 void   handleTextEditing(const SDL_Event *event);

#endif