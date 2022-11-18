#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>

#include <pthread.h>

#include "config.h"
#include "cache.h"
#include "utils.h"
#include "font.h"
#include "display.h"
#include "timer.h"
#include "menu.h"
#include "loop.h"
#include "map.h"
#include "game.h"
#include "moves.h"
#include "dialog.h"
#include "effects.h"

#include "discord.h"

#define FPS_MAX 60
#define TICKS_PER_FRAME 1000 / FPS_MAX
extern t_gameConfig    *gameConfig;
extern void    assignMenuParents();
extern t_player         *g_bots[MAX_BOTS];
extern short            g_nbBots;
extern t_discord_app    *g_app_discord;

#define DEBUG true

SDL_Color colorWhite = {255, 255, 255, 255};
SDL_Color colorBlack = {0, 0, 0, 255};
SDL_Color colorRed = {255, 0, 0, 255};
SDL_Color colorGreen = {0, 255, 0, 255};
SDL_Color colorBlue = {0, 0, 255, 255};
SDL_Color colorYellow = {255, 255, 0, 255};
SDL_Color colorCyan = {0, 255, 255, 255};
SDL_Color colorMagenta = {255, 0, 255, 255};
SDL_Color colorOrange = {255, 165, 0, 255};
SDL_Color colorPurple = {128, 0, 128, 255};
SDL_Color colorBrown = {165, 42, 42, 255};
SDL_Color colorGray = {128, 128, 128, 255};
SDL_Color colorLightGray = {128, 128, 128, 128};
SDL_Color colorDarkGray = {169, 169, 169, 255};
SDL_Color colorPink = {255, 192, 203, 255};
SDL_Color colorTransparent = {0, 0, 0, 0};

SDL_Window*     g_window = NULL;
SDL_Renderer*   g_renderer = NULL;
TTF_Font*       g_font = NULL;
int             g_currentState;

/**
 * Setup SDL libraries
 * @return {void}
*/
void setupSDL() {
    if (0 != SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER )) {
        #if DEBUG
		    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        #endif
        exit(EXIT_FAILURE);
	}

    // init sdl2_ttf
    if (TTF_Init() == -1)
	{
        #if DEBUG
            fprintf(stderr, "TTF_Init: %s", TTF_GetError());
        #endif
		exit(EXIT_FAILURE);
	}
}

// export DISPLAY=:0.0
int main(int argc, char **argv)
{
    t_gameConfig config;
    gameConfig = &config;
    if (readConfig(&config)) {
        fprintf(stderr, "Error reading config file");
        exit(EXIT_FAILURE);
    }
    time_t t;
    srand((unsigned) time(&t));

    assignMenuParents();

    unsigned int    windowWidth;
    unsigned int    windowHeight;

    g_currentState = GAME_START;

    // init SDL
    setupSDL();
    windowWidth = config.video.width;
    windowHeight = config.video.height;

    const char *windowTitle = "Groundbreaker";
    // Create a resizable window using config's width/heigth
    g_window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!g_window) {
		fprintf(stderr, "Erreur SDL_CreateWindow : %s\n", SDL_GetError());
		return -1;
	}

    // Create the renderer for the window
    // SDL_RENDERER_ACCELERATED -> hardware acceleration
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!g_renderer) {
		fprintf(stderr, "Erreur SDL_CreateRenderer : %s\n", SDL_GetError());
		return -1;
	}

    // window limits, i3
    SDL_Rect windowLimits = {0, 0, windowWidth, windowHeight};

    loadFont(FONT_PATH, 20);

    SDL_RaiseWindow(g_window);


    g_currentState = GAME_MAINMENU;

    pthread_t       th;
    pthread_create(&th, NULL, &setupDiscord, "");

    int running = 1;
    SDL_Event event;
    // Main loop
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            // Handle window events (resize, close, ...)
            if (event.type == SDL_WINDOWEVENT) {
                // printf("window event = %d\n", event.window.event);
                // handleWindowEvent(&event);
                continue;
            }

            switch (event.type)
            {
                // Handle a simple click with the mouse

                case SDL_MOUSEBUTTONUP:
                    puts("Mouse button up");
                    handleMouseButtonUp(&event);
                    break;
                case SDL_TEXTINPUT:
                    handleTextEditing(&event);
                    break;
                // Handle a key press
                case SDL_KEYDOWN:
                    // puts("Key down");
                    handleKeyDown(&event.key);
                    break;
                // Handle a key release
                case SDL_KEYUP:
                        // puts("Key up");
                    handleKeyUp(&event);
                    break;
                case SDL_QUIT:
                    puts("Quit");
                    running = 0;
                    break;
                default:
                    break;
            }
        }
        SDL_RenderClear(g_renderer);

        pickColor(&colorBlack);

        if (inMainMenu()) {
            // No need to render at 1000 fps
            setupMenu();
            SDL_Delay(30);
        } else if (inGame())
        {
            drawMap();
            drawEffects();

            if (isGamePaused()) {
                setupMenu();
                SDL_Delay(30);

            } else {
                // move bots
                for (size_t i = 0; i < g_nbBots; i++)
                {
                    if (g_bots[i]->health) {
                        movePlayer(g_bots[i]);
                    }
                }

                movePlayer(getPlayer());
            }
        }

        if (getEditBox()->active) {
            displayEditBox();
        }

        pickColor(&colorRed);
        SDL_RenderDrawRect(g_renderer, &windowLimits);
        pickColor(&colorBlack);

        SDL_RenderPresent(g_renderer);

        // Give a way to quit the program and display a custom error
        if (g_currentState == GAME_EXIT) {
            running = 0;
        }
    }
    SDL_RenderClear(g_renderer);
    SDL_Quit();
    return EXIT_SUCCESS;
}
