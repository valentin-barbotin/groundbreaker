#ifndef MENU_H
 #define MENU_H

 #include <SDL.h>
 #include <SDL_ttf.h>

 extern SDL_Window*     g_window;
 extern SDL_Renderer*   g_renderer;
 extern short           g_currentMap;
 extern short           g_nbMap;

 typedef struct     s_menu {
    char             *name;
    char             *buttons[4];
    void             (*fct[4])();
    struct s_menu    *parent;
    struct s_menu    *next[4];
    unsigned short    selectedButton;
    unsigned short    nbButtons;
 }                  t_menu;

 void    setupMenu();
 void    drawLobbyMenu();
 void    setupMenuButtons();

#endif