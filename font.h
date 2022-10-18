#ifndef FONT_H
 #define FONT_H

 #include <SDL.h>
 #include <SDL_ttf.h>

 extern SDL_Window*     g_window;
 extern SDL_Renderer*   g_renderer;
 extern TTF_Font*       g_font; 

 void                   loadFont(const char* fontPath, const int size);
 SDL_Texture*           getTextureFromString(const char* src, const SDL_Color* color, const bool wrapped);
 void                   drawText(const SDL_Color *color, const unsigned int x, const unsigned int y, const char *text, const bool centered);


#endif