#include "display.h"
#include "utils.h"
#include "cache.h"
#include "config.h"

#define DEBUG true

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

    #if DEBUG
        if (success != 0) {
            printf("Error: %s\n", SDL_GetError());
            perror("Error");
        }
    #endif

    return success;
}

int     setBackgroundImage(const char *name) {
    SDL_Rect        source = {0, 0, 0, 0};
    SDL_Rect        target = {0, 0, 0, 0};
    SDL_Texture     *texture;

    texture = getImage(name);
    if (texture == NULL) {
        return 1;
    }

    if (SDL_QueryTexture(texture, NULL, NULL, &source.w, &source.h) < 0) {
        #ifdef DEBUG
            fprintf(stderr, "Erreur SDL_QueryTexture : %s\n", SDL_GetError());
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
        exit(1);
    }

    target.w = gameConfig->video.width;
    target.h = gameConfig->video.height;
    if (!drawTexture(name, &source, &target)) {
        #ifdef DEBUG
            fprintf(stderr, "Error setting background image");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error setting background image", NULL);
        exit(1);
    }

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