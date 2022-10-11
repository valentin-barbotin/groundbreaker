#ifndef MENU_H
 #define MENU_H

 #include <SDL.h>
 #include <SDL_ttf.h>

 extern SDL_Window*     g_window;
 extern SDL_Renderer*   g_renderer;

 typedef struct     s_menu {
    char    *name;
    char    *buttons[4];
    void    (*fct[4])(void);
    struct s_menu  *last;
    struct s_menu  *next[5];
 }                  t_menu;

 void    setupMenu();

#endif