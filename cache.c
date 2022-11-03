#include "cache.h"
#include "utils.h"

#define DEBUG true

/**
 * @brief Get the Image Cache object
 * 
 * @return s_cache* 
 */
t_cache      *getImageCache() {
    static t_cache  *cache;

    if (cache == NULL) {
        cache = malloc(sizeof(t_cache));
        if (cache == NULL) {
            #ifdef DEBUG
                fprintf(stderr, "Error allocating memory for cache");
            #endif
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
            exit(1);
        }
        cache->size = 0;
        cache->images = NULL;
    }

    return cache;
}

/**
 * @brief Get the Image From Cache object
 * 
 * @param name 
 * @return SDL_Texture* 
 */
SDL_Texture  *getImage(const char *name) {
    const t_cache   *cache;
    SDL_Texture     *res;

    cache = getImageCache();

    // Loop over cache to find the image
    for (size_t i = 0; i < cache->size; i++)
    {
        if (stringIsEqual(cache->images[i].name, name)) {
            return (cache->images[i].texture);
        }
    }

    // If not found, load it from disk
    res = textureFromFile(name);

    // Add it to the cache
    res = addImageToCache(name, res);

    return res;
}

/**
 * @brief add an image to the cache
 * 
 * @param name 
 * @param texture 
 */
SDL_Texture      *addImageToCache(const char *name, SDL_Texture *texture) {
    t_cache     *cache;

    cache = getImageCache();

    cache->images = realloc(cache->images, sizeof(t_image) * (cache->size + 1));
    cache->images[cache->size].name = strdup(name); //strdup
    cache->images[cache->size++].texture = texture;
    return texture;
}
