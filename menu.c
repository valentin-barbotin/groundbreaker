#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "menu.h"
#include "font.h"
#include "config.h"
#include "display.h"
#include "map.h"

SDL_Rect                g_buttonsLocation[4];
t_menu                  *g_currentMenu;
short                   g_currentOption = 0;
extern int              g_currentState;
extern t_gameConfig     *gameConfig;



void openLobby(unsigned int value) {
    printf("value: %d\n", value);
    getMaps();
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
    {&openLobby, &test2, NULL, &exitGame},
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
    SDL_Color   colorRed = {255, 0, 0, 255};
    char        buff[7];
    int         gap;
    short       j;
    short       nbMaps;
    short       fromGap;

    if (g_lobby == NULL) {
        g_lobby = malloc(sizeof(t_lobby));
        if (g_lobby == NULL) {
            fprintf(stderr, "Error malloc lobby: %s", SDL_GetError());
            return;
        }
        g_lobby->columns = 4;
        g_lobby->rows = 4;
        g_lobby->players = 2;
    }

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

    pickColor(&colorBlack);

    for (size_t i = fromGap; (i < (nbMaps + fromGap)) && i < g_nbMap; i++)
    {
        int x = (int) ((i - (nbMaps * j)) * gameConfig->video.width / nbMaps) + gap/2;
        int y = (int) (gameConfig->video.height * 0.75);
        int w = (gameConfig->video.width / nbMaps) - gap;
        int h = (int) (gameConfig->video.height * 0.20);
        // printf("i: %d j: %d\n", i, j);
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;

        if (i == g_currentMap) {
            // printf("i: %d, sel: %lu\n", i, g_lobby->maps[i].selected);
            pickColor(&colorBlue);
        } else {
            if (g_lobby->maps[i].selected) {
                pickColor(&colorRed);
            } else {
                pickColor(&colorBlack);
            }
        }
        
        SDL_RenderFillRect(g_renderer, &rect);
        sprintf(buff, "%lu", i + 1);
        drawText(&colorBlack, x + (w/2), y + h + (h * 0.13), buff, true);
    }

    if (g_nbMap == 0) {
        drawText(&colorBlack, gameConfig->video.width / 2, gameConfig->video.height * 0.80, "No map found", true);
    }

    loadFont("../DejaVuSansMono.ttf", 30);
    drawText(&colorBlack, (gameConfig->video.width) * 0.15, (gameConfig->video.height) * 0.1, "Rows :", true);
    drawText(&colorBlack, (gameConfig->video.width) * 0.15, (gameConfig->video.height) * 0.15, "Columns :", true);
    drawText(&colorBlack, (gameConfig->video.width) * 0.15, (gameConfig->video.height) * 0.20, "Players :", true);

    sprintf(buff, "%d", g_lobby->rows);
    drawText(g_currentOption == 0 ? &colorBlue : &colorBlack, (gameConfig->video.width) * 0.30, (gameConfig->video.height) * 0.1, buff, true);

    sprintf(buff, "%d", g_lobby->columns);
    drawText(g_currentOption == 1 ? &colorBlue : &colorBlack, (gameConfig->video.width) * 0.30, (gameConfig->video.height) * 0.15, buff, true);

    sprintf(buff, "%d", g_lobby->players);
    drawText(g_currentOption == 2 ? &colorBlue : &colorBlack, (gameConfig->video.width) * 0.30, (gameConfig->video.height) * 0.20, buff, true);

    loadFont("../DejaVuSansMono.ttf", 20);
    drawText(&colorBlack, (gameConfig->video.width) * 0.30, (gameConfig->video.height) * 0.25, "You can pick a maximum of 10 maps", true);
    
    drawText(&colorBlack, (gameConfig->video.width) * 0.06, (gameConfig->video.height) * 0.66, "(A) Previous (E) Next (space) Select (Enter) play (N) New", false);
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
        
        tex = getTextureFromString(g_currentMenu->buttons[i], color, false);
        op = SDL_QueryTexture(tex, NULL, NULL, &textWidth, &textHeight);
        if (op != 0) {
            fprintf(stderr, "Erreur SDL_QueryTexture : %s", SDL_GetError());
        }

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
