#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "menu.h"
#include "font.h"

SDL_Rect g_buttonsLocation[4];

t_menu menuMain = {
    "Main",
    {"Play", "Settings", "Online", "Exit", NULL},
    {NULL, NULL, NULL, NULL, NULL},
    NULL,
    {NULL, NULL, NULL, NULL, NULL}
    // {&menuPlay, &menuSettings, &menuOnline, NULL, NULL}
};


void    setupMenu() {
    static t_menu   *currentMenu;
    int             op;
    SDL_Texture     *tex;
    SDL_Color       *color;
    SDL_Rect        *target;
    int             textWidth;
    int             textHeight;
    int             windowWidth;
    int             windowHeight;

    if (currentMenu == NULL) {
        currentMenu = malloc(sizeof(t_menu));
        if (currentMenu == NULL) {
            fprintf(stderr, "Error malloc : %s", TTF_GetError());
            return;
        }
    }

    color = malloc(sizeof(SDL_Color));
    if (color == NULL) {
        fprintf(stderr, "Error malloc : %s", TTF_GetError());
        return;
    }

    target = malloc(sizeof(SDL_Rect));
    if (target == NULL) {
        fprintf(stderr, "Error malloc : %s", TTF_GetError());
        free(color);
        return;
    }

    color->r = 255;
    color->g = 255;
    color->b = 0;
    color->a = 255;
    for (int i = 0; i < 5; i++)
    {
        if (menuMain.buttons[i] == NULL) {
            continue;
        }

        tex = getTextureFromString(menuMain.buttons[i], color);
        op = SDL_QueryTexture(tex, NULL, NULL, &textWidth, &textHeight);
        if (op != 0) {
            fprintf(stderr, "Erreur SDL_QueryTexture : %s", SDL_GetError());
        }

        SDL_GetWindowSize(g_window, &windowWidth, &windowHeight);
        target->x = windowWidth/2 - textWidth/2;
        target->y = 100 * i;
        target->w = 100;
        target->h = 100;

        op = SDL_RenderCopy(g_renderer, tex, NULL, target);
        if (op < 0) { //TODO: handle
            fprintf(stderr, "Error SDL_RenderCopy : %s", TTF_GetError());
            break;
        }
        g_buttonsLocation[i] = *target;
    }

    free(color);
    free(target);

}
