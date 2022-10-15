#include <math.h>

#include "config.h"
#include "loop.h"
#include "game.h"

extern SDL_Rect         g_buttonsLocation[4];
extern int              g_currentState;

bool    inMainMenu() {
    return (g_currentState >= GAME_MAINMENU && g_currentState < GAME_MAINMENU_END);
}

bool    inGame() {
    return (g_currentState >= GAME_PLAY_PLAYING);
}

void    makeSelection(unsigned short index) {
    if (g_currentMenu->next[index] != NULL) {
        puts("SUBMENU");
        g_currentMenu = g_currentMenu->next[index];

        setupMenu();

        // SDL_RenderPresent(g_renderer); //TODO: check
        return;
    }

    if (g_currentMenu->fct[index] != NULL) {
        puts("FUNCTION");
        g_currentMenu->fct[index](index);
    }
}

/**
 * @brief Handle a mouse button up event
 * @param event the event to handle
 * @return void
 */
void    handleMouseButtonUp(const SDL_Event *event) {
    //loop through buttons
    if (inMainMenu()) {
        for (unsigned short i = 0; i < g_currentMenu->nbButtons; ++i) {
            // get click position
            int xStart = g_buttonsLocation[i].x;
            int yStart = g_buttonsLocation[i].y;

            SDL_Point click = { event->button.x, event->button.y };
            SDL_Rect button = { xStart, yStart, g_buttonsLocation[i].w, g_buttonsLocation[i].h };

            if (SDL_PointInRect(&click, &button))
            {
                makeSelection(i);
            }
        }
    }
    else if (inGame()) {
        // TODO: handle game events
    }
}

void    handleKeyDown(const SDL_Event *event) {
}

void    handleKeyUp(const SDL_Event *event) {
    t_game *game = getGame();

    if (inMainMenu()) {
        short index = -1;
        switch (event->key.keysym.sym) {
            case SDLK_ESCAPE:
                g_currentState = GAME_EXIT;
                break;
            case SDLK_RETURN:
                index = g_currentMenu->selectedButton;
                makeSelection(index);
                break;
            case SDLK_UP:
                index = (g_currentMenu->selectedButton - 1);
                if (index < 0) {
                    index = g_currentMenu->nbButtons - 1;
                }
                g_currentMenu->selectedButton = index;
                printf("UP: %d\n", g_currentMenu->selectedButton);
                break;
            case SDLK_DOWN:
                index = (g_currentMenu->selectedButton + 1);
                if (index > g_currentMenu->nbButtons - 1) {
                    index = 0;
                }
                g_currentMenu->selectedButton = index;
                printf("DOWN: %d\n", g_currentMenu->selectedButton);
                break;
            default:
                break;
        }
    }
    else if (inGame()) {
        switch (event->key.keysym.sym) {
            case SDLK_ESCAPE:
                g_currentState = GAME_MAINMENU;
                break;
            case SDLK_LEFT:
                game->x--;
                break;
            case SDLK_RIGHT:
                game->x++;
                break;
            case SDLK_UP:
                game->y--;
                break;
            case SDLK_DOWN:
                game->y++;
                break;
            default:
                break;
        }

        movePlayer(game, getMap());


    }

}