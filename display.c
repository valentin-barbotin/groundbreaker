#include "display.h"
#include "utils.h"
#include "cache.h"
#include "config.h"

extern t_gameConfig    *gameConfig;

/**
 * Clear the background and paint it with given color
 * @param SDL_Color* color
 * @return int success
 */
int     setBackgroundColor(const SDL_Color* color) {
    int success = 0;
    if (pickColor(color)) {
        success = SDL_RenderClear(g_renderer);
    }

    return success;
}

int     setBackgroundImage(const char *name) {
    int w;
    int h;
    SDL_Rect source = {0, 0, 0, 0};
    SDL_Rect target = {0, 0, 0, 0};

    SDL_Texture *tex = getImage(name);
    if (tex == NULL) {
        return 1;
    }
    SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    source.w = w;
    source.h = h;

    target.w = gameConfig->video.width;
    target.h = gameConfig->video.height;
    drawTexture(name, &source, &target);
    return 0;
}

/**
 * Take a texture from file and render copy it on the screen without rendering it
 * @param const char* image
 * @return int success
 */
bool    drawTexture(const char* image, const SDL_Rect *srcrect, const SDL_Rect *dstrect) {
    SDL_Texture* texture;

    texture = getImage(image);
    if (texture == NULL) {
        return false;
    }

    int success = SDL_RenderCopy(g_renderer, texture, srcrect, dstrect);
    if (success != 0) {
		fprintf(stderr, "Error SDL_RenderCopy : %s\n", SDL_GetError());
        return false;
    }
    return true;
}