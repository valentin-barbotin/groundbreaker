#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "menu.h"
#include "font.h"
#include "config.h"
#include "display.h"

SDL_Rect                g_buttonsLocation[4];
t_menu                  *g_currentMenu;
extern int              g_currentState;
extern t_gameConfig     *gameConfig;



void playTest(unsigned int value) {
    printf("value: %d\n", value);
    g_currentState = GAME_MAINMENU_PLAY;
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
    {&playTest, &test2, NULL, &exitGame},
    NULL,
    {NULL, NULL, NULL, NULL},
    0,
    4
    // {&menuPlay, &menuSettings, &menuOnline, NULL, NULL}
};


void    setupMenu() {
    char            *bg;

    if (g_currentMenu == NULL) {
        g_currentMenu = &menuMain;
    }

    switch (g_currentState)
    {
        case GAME_MAINMENU_PLAY:
            bg = "/home/kazuh/Downloads/uwp2625532.jpeg";
            break;
        
        default:
            bg = "/home/kazuh/Downloads/uwp2625532.jpeg";
            break;
    }

    if (bg && setBackgroundImage(bg)) {
        fprintf(stderr, "Error setting background image: %s", SDL_GetError());
        // if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "An error occured", g_window) != 0) {
        //     fprintf(stderr, "Error displaying message box: %s", SDL_GetError());
        // };
        return;
    }

    switch (g_currentState)
    {
        case GAME_MAINMENU_PLAY:
            drawLobbyMenu();
            break;
        
        default:
            setupMenuButtons();
            break;
    }
}

void    drawLobbyMenu() {
    SDL_Rect    rect;
    SDL_Color   colorWhite = {255, 255, 255, 255};
    SDL_Color   colorYellow = {255, 255, 0, 255};
    SDL_Color   colorBlack = {0, 0, 0, 255};
    SDL_Color   colorBlue = {0, 0, 255, 255};
    int         gap;
    int         j;
    short       nbMaps;
    short       from;
    short       fromGap;

    setBackgroundColor(&colorWhite);

    // Draw the map list
    rect.x = 0;
    rect.y = (int) (gameConfig->video.height * 0.7);
    rect.w = gameConfig->video.width;
    rect.h = (int) (gameConfig->video.height * 0.3);

    pickColor(&colorYellow);
    SDL_RenderFillRect(g_renderer, &rect);

    gap = gameConfig->video.width / 15;

    // Max maps to display
    nbMaps = 4;

    for (short i = 0; i < nbMaps; i++) {
        if ((g_currentMap < (i * nbMaps)) && (g_currentMap >= ((i - 1) * nbMaps))) {
            fromGap = (i - 1) * nbMaps;
            j = i - 1; // j is the current page
        }
    }

    for (size_t i = fromGap; (i < (nbMaps + fromGap)) && i < g_nbMap; i++)
    {
        // printf("i: %d j: %d\n", i, j);
        rect.x = (int) ((i - (nbMaps * j)) * gameConfig->video.width / nbMaps) + gap/2;
        rect.y = (int) (gameConfig->video.height * 0.75);
        rect.w = (gameConfig->video.width / nbMaps) - gap;
        rect.h = (int) (gameConfig->video.height * 0.20);
        
        pickColor((i == g_currentMap) ? &colorBlue : &colorBlack);

        SDL_RenderFillRect(g_renderer, &rect);
    }
    
    // SDL_Rect    rect;
    // SDL_Color   color = {255, 255, 255, 255};
    // SDL_Surface *surface;
    // SDL_Texture *texture;
    // SDL_Rect    location;

    // rect.x = 0;
    // rect.y = 0;
    // rect.w = gameConfig->video.width;
    // rect.h = gameConfig->video.height;

    // SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
    // SDL_RenderFillRect(g_renderer, &rect);

    // surface = TTF_RenderText_Blended(g_font, "Lobby", color);
    // texture = SDL_CreateTextureFromSurface(g_renderer, surface);
    // SDL_FreeSurface(surface);

    // location.x = gameConfig->video.width / 2 - surface->w / 2;
    // location.y = gameConfig->video.height / 2 - surface->h / 2;
    // location.w = surface->w;
    // location.h = surface->h;

    // SDL_RenderCopy(g_renderer, texture, NULL, &location);
    // SDL_DestroyTexture(texture);
};

void    setupMenuButtons() {
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

        // SDL_GetWindowSize(g_window, &windowWidth, &windowHeight);
        target.x = (gameConfig->video.width)/2 - textWidth/2;
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
