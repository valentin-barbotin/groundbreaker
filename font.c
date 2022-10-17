#include <stdlib.h>
#include <stdio.h>

#include "font.h"


/**
 * Load a font from a file
 * the font variable is the one in main file
 * @param const char* fontPath
 * @param int size
 * @return void
 */
void    loadFont(const char* fontPath, const int size) {
    if (g_font != NULL) {
        TTF_CloseFont(g_font);
    }
    g_font = TTF_OpenFont(fontPath, size);
    if (g_font == NULL) {
        fprintf(stderr, "Error TTF_OpenFont : %s", TTF_GetError());
        return;
    }
}


/**
 * @brief Get the Texture From string
 * 
 * @param src 
 * @param color 
 * @return SDL_Texture* 
 */
SDL_Texture*    getTextureFromString(const char* src, const SDL_Color* color) {
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(g_font, src, *color, 256); //TODO: check wrap
    if (!surface) {
        fprintf(stderr, "Error TTF_RenderText_Blended_Wrapped : %s\n", TTF_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, surface);
    if (!texture) {
        fprintf(stderr, "Error SDL_CreateTextureFromSurface : %s\n", TTF_GetError());
        return NULL;
    }
    SDL_FreeSurface(surface);
    return texture;
}

/**
 * @brief Draw text on the screen at given position with given color
 * 
 * @param color 
 * @param x 
 * @param y 
 * @param text 
 */
void            drawText(const SDL_Color *color, const unsigned int x, const unsigned int y, const char *text) {
    SDL_Texture *tex;
    SDL_Rect    target;
    int         textWidth;
    int         textHeight;
    int         op;

    tex = getTextureFromString(text, color);
    op = SDL_QueryTexture(tex, NULL, NULL, &textWidth, &textHeight);
    if (op != 0) {
        fprintf(stderr, "Erreur SDL_QueryTexture : %s", SDL_GetError());
        return;
    }

    target.x = x - textWidth / 2;
    target.y = y - textHeight / 2;
    target.w = textWidth;
    target.h = textHeight;

    op = SDL_RenderCopy(g_renderer, tex, NULL, &target);
    if (op < 0) {
        fprintf(stderr, "Error SDL_RenderCopy : %s", TTF_GetError());
        return;
    }
}
