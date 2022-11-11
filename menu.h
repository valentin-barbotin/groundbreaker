#ifndef MENU_H
 #define MENU_H

 #include <SDL.h>
 #include <SDL_ttf.h>

 #include "lobby.h"

 extern SDL_Window*     g_window;
 extern SDL_Renderer*   g_renderer;
 extern short           g_currentMap;
 extern short           g_nbMap;
 extern t_lobby         *g_lobby;

 #define MAX_MENU_BUTTONS 9

 typedef struct     s_menu {
    char             *name;
    char             *buttons[MAX_MENU_BUTTONS];
    void             (*fct[MAX_MENU_BUTTONS])();
    struct s_menu    *parent;
    struct s_menu    *next[MAX_MENU_BUTTONS];
    unsigned short    selectedButton;
    unsigned short    nbButtons;
 }                  t_menu;

 void    setupMenu();
 void    drawPlayersList();
 void    drawLobbyMenu();
 void    setupMenuButtons();
 void    exitMenu();

#endif