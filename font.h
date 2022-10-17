#ifndef FONT_H
 #define FONT_H

 #include <SDL.h>
 #include <SDL_ttf.h>

 extern SDL_Window*     g_window;
 extern SDL_Renderer*   g_renderer;

 void               loadFont(const char* fontPath, const int size);
 SDL_Texture*       getTextureFromString(const char* src, const SDL_Color* color);

 extern TTF_Font*       g_font; 

#endif