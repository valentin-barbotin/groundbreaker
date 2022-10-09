#ifndef CACHE_H
 #define CACHE_H

 #include <SDL.h>
 #include <SDL_image.h>

 extern SDL_Window*     g_window;
 extern SDL_Renderer*   g_renderer;

 typedef struct  s_image
 {
     SDL_Texture     *texture;
     char            *name;
 }               t_image;

 typedef struct  s_cache
 {
     size_t          size;
     t_image         *images;

 }               t_cache;

 t_cache      *getImageCache();
 SDL_Texture  *getImage(const char *name);
 SDL_Texture  *addImageToCache(const char *name, SDL_Texture *texture);

#endif /* !FOO_H */