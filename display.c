#include "display.h"
#include "utils.h"
#include "cache.h"

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
		fprintf(stderr, "Error SDL_RenderCopy : %s", SDL_GetError());
        return false;
    }
    return true;
}