#ifndef DISPLAY_H
 #define DISPLAY_H
 
 #include "stdbool.h"

 #include <SDL.h>

 extern SDL_Window*     g_window;
 extern SDL_Renderer*   g_renderer;

 int    setBackgroundColor(const SDL_Color* color);
 int    setBackgroundImage(const char *name);
 bool   drawTexture(const char* image, const SDL_Rect *srcrect, const SDL_Rect *dstrect) ;
 
#endif