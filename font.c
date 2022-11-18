#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "font.h"

#define DEBUG true


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
        fprintf(stderr, "Error TTF_OpenFont : %s\n", TTF_GetError());
        return;
    }
}


/**
 * @brief Get the Texture From string
 * 
 * @param src 
 * @param color 
 * @param wrapped
 * @return SDL_Texture* 
 */
SDL_Texture*    getTextureFromString(const char* src, const SDL_Color* color, const unsigned int *wrapped) {
    SDL_Surface* surface;
    SDL_Texture* texture;

    if (wrapped) {
        // "wrapped" value is the max width of the text in pixels 
        surface = TTF_RenderText_Blended_Wrapped(g_font, src, *color, *wrapped);
    } else {
        surface = TTF_RenderText_Blended(g_font, src, *color);
    }

    if (!surface) {
        fprintf(stderr, "Error TTF_RenderText_Blended_Wrapped : %s\n", TTF_GetError());
        return NULL;
    }
    texture = SDL_CreateTextureFromSurface(g_renderer, surface);
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
 * @param centered 
 * @param max
 */
void            drawText(const SDL_Color *color, const unsigned int x, const unsigned int y, const char *text, const bool centered, const unsigned int max) {
    if (strlen(text) == 0) {
        return;
    }

    SDL_Texture     *tex;
    SDL_Rect        target;
    int             textWidth;
    int             textHeight;
    int             op;
    unsigned int    max2;

    max2 = max ? max * 0.8 : 0;

    tex = getTextureFromString(text, color, max2 ? &max2 : NULL);
    op = SDL_QueryTexture(tex, NULL, NULL, &textWidth, &textHeight);
    if (op != 0) {
        #if DEBUG
            fprintf(stderr, "Erreur SDL_QueryTexture : %s\n", SDL_GetError());
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
        exit(1);
    }

    target.x = centered ? x - textWidth/2 : x;
    target.y = y - textHeight / 2;
    target.w = textWidth;
    target.h = textHeight;

    op = SDL_RenderCopy(g_renderer, tex, NULL, &target);
    if (op < 0) {
        #if DEBUG
            fprintf(stderr, "Erreur SDL_RenderCopy : %s\n", SDL_GetError());
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
        exit(1);
    }
    SDL_DestroyTexture(tex);
}
