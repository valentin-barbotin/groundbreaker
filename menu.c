#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "menu.h"
#include "font.h"
#include "config.h"
#include "display.h"

SDL_Rect        g_buttonsLocation[4];
t_menu          *g_currentMenu;
extern int      g_currentState;


void test(unsigned int value) {
    printf("value: %d\n", value);
}

void test2(unsigned int value) {
    printf("value2: %d\n", value);
}

void    exitGame() {
    g_currentState = GAME_EXIT;
}

t_menu menuMain = {
    "Main",
    {"Play", "Settings", "Online", "Exit"},
    {&test, &test2, NULL, &exitGame},
    NULL,
    {NULL, NULL, NULL, NULL},
    0,
    4
    // {&menuPlay, &menuSettings, &menuOnline, NULL, NULL}
};


void    setupMenu() {
    int             op;
    SDL_Texture     *tex;
    SDL_Color       notSelectedColor;
    SDL_Color       selectedColor;
    SDL_Color       *color;
    SDL_Rect        target;
    int             textWidth;
    int             textHeight;
    int             windowWidth;
    int             windowHeight;

    if (g_currentMenu == NULL) {
        g_currentMenu = &menuMain;
    }

    if (setBackgroundImage("/home/kazuh/Downloads/uwp2625532.jpeg")) {
        fprintf(stderr, "Error setting background image: %s", SDL_GetError());
        // if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "An error occured", g_window) != 0) {
        //     fprintf(stderr, "Error displaying message box: %s", SDL_GetError());
        // };
        return;
    };

    selectedColor.r = 255;
    selectedColor.g = 0;
    selectedColor.b = 0;
    selectedColor.a = 255;

    notSelectedColor.r = 255;
    notSelectedColor.g = 255;
    notSelectedColor.b = 255;
    notSelectedColor.a = 255;

    for (unsigned int i = 0; i < g_currentMenu->nbButtons; i++)
    {
        unsigned short  j;

        if (g_currentMenu->buttons[i] == NULL) {
            continue;
        }

        j = g_currentMenu->selectedButton;

        if ((g_currentMenu->buttons + j) == (g_currentMenu->buttons + i)) {
            color = &selectedColor;
        } else {
            color = &notSelectedColor;
        }
        
        tex = getTextureFromString(g_currentMenu->buttons[i], color);
        op = SDL_QueryTexture(tex, NULL, NULL, &textWidth, &textHeight);
        if (op != 0) {
            fprintf(stderr, "Erreur SDL_QueryTexture : %s", SDL_GetError());
        }

        SDL_GetWindowSize(g_window, &windowWidth, &windowHeight);
        target.x = windowWidth/2 - textWidth/2;
        target.y = 100 * i;
        target.w = 100;
        target.h = 100;

        op = SDL_RenderCopy(g_renderer, tex, NULL, &target);
        if (op < 0) { //TODO: handle
            fprintf(stderr, "Error SDL_RenderCopy : %s", TTF_GetError());
            break;
        }
        g_buttonsLocation[i] = target;
    }
}
