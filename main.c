#include "stdio.h"
#include "stdlib.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>

#include "config.h"
#include "cache.h"
#include "utils.h"
#include "font.h"
#include "timer.h"
#include "menu.h"
#include "loop.h"

#define FPS_MAX 60
#define TICKS_PER_FRAME 1000 / FPS_MAX

SDL_Window*     g_window = NULL;
SDL_Renderer*   g_renderer = NULL;
TTF_Font*       g_font = NULL;
int             g_currentState;

/**
 * Setup SDL libraries
 * @return {void}
*/
void setupSDL() {
    if (0 != SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO )) {
		fprintf(stderr, "SDL_Init: %s", SDL_GetError());
        exit(EXIT_FAILURE);
	}

    // init sdl2_ttf
    if (TTF_Init() == -1)
	{
        fprintf(stderr, "TTF_Init: %s", TTF_GetError());
		exit(EXIT_FAILURE);
	}
}

// export DISPLAY=:0.0
int main(int argc, char **argv)
{
    unsigned int    windowWidth;
    unsigned int    windowHeight;

    g_currentState = GAME_START;

    // init SDL
    setupSDL();
    windowWidth = 1024;
    windowHeight = 1024;

    const char * windowTitle = "Groundbreaker";
    // Create a resizable window using config's width/heigth
    g_window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!g_window) {
		fprintf(stderr, "Erreur SDL_CreateWindow : %s", SDL_GetError());
		return -1;
	}

    // Create the renderer for the window
    // SDL_RENDERER_ACCELERATED -> hardware acceleration
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!g_renderer) {
		fprintf(stderr, "Erreur SDL_CreateRenderer : %s", SDL_GetError());
		return -1;
	}



    // window limits, i3
    SDL_Rect windowLimits = {0, 0, 1024, 1024};

    loadFont("../DejaVuSansMono.ttf", 20);

    SDL_RaiseWindow(g_window);

    SDL_Color windowLimitsColor = { 255, 0, 0, 255 };
    SDL_Color blackColor = { 0, 0, 0, 255 };


    g_currentState = GAME_MAINMENU;

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
                // Handle a key press
            case SDL_KEYDOWN: {
                // puts("Key down");
                handleKeyDown(&event.key);
                break;
            }
                            // Handle a key release
            case SDL_KEYUP: {
                    // puts("Key up");
                handleKeyUp(&event);
                break;
            }
            case SDL_QUIT:
                puts("Quit");
                running = 0;
                break;
            default:
                break;
            }
        }
        SDL_RenderClear(g_renderer);

        pickColor(&windowLimitsColor);
        SDL_RenderDrawRect(g_renderer, &windowLimits);
        pickColor(&blackColor);

        if (true) {
            setupMenu();
        }
        SDL_RenderPresent(g_renderer);
        // No need to render at 1000 fps
        if (inMainMenu()) {
            SDL_Delay(100);
        }

        // Give a way to quit the program and display a custom error
        if (g_currentState == GAME_EXIT) {
            running = 0;
        }
    SDL_RenderClear(g_renderer);
    SDL_Quit();
    return EXIT_SUCCESS;
}
