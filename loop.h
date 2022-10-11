#ifndef LOOP_H
 #define LOOP_H

 #include <SDL.h>
 #include <SDL_ttf.h>

 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>

 extern SDL_Window*     g_window;
 extern SDL_Renderer*   g_renderer;

 void   handleMouseButtonUp(SDL_Event *event);

#endif