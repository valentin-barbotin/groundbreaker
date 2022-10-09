#include "stdio.h"
#include "stdlib.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>

#include "cache.h"

SDL_Window*     g_window = NULL;
SDL_Renderer*   g_renderer = NULL;

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
    if(TTF_Init()==-1)
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

    // init SDL
    setupSDL();
    

    windowWidth = 256;
    windowHeight = 205;

    const char * windowTitle = "Groundbreaker";
    // Create a resizable window using config's width/heigth
    g_window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!g_window) {
		fprintf(stderr, "Erreur SDL_CreateWindow : %s", SDL_GetError());
		return -1;
	}

    // Create the renderer for the window
    // SDL_RENDERER_ACCELERATED -> hardware acceleration
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
	if (!g_renderer) {
		fprintf(stderr, "Erreur SDL_CreateRenderer : %s", SDL_GetError());
		return -1;
	}

    SDL_Rect srcrect = {0, 0, 256, 205};
    SDL_Rect dstrect = {0, 0, 256, 205};

    SDL_Texture* tex = getImage("../ok.png");

    int success = SDL_RenderCopy(g_renderer, tex, &srcrect, &dstrect);
    if (success != 0) {
        fprintf(stderr, "Erreur SDL_RenderCopy : %s", SDL_GetError());
        return -1;
    }

    SDL_RaiseWindow(g_window);

    SDL_RenderPresent(g_renderer);

    int exit = 0;
    SDL_Event event;
    // Main loop
    // SDL_PollEvent
    while (SDL_PollEvent(&event), !exit) {
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
                // handleMouseButtonUp(&event);
            // Handle a key press
            case SDL_KEYDOWN: {
                puts("Key down");
                // handleKeyDown(&event.key);
                break;
            }
            // Handle a key release
            case SDL_KEYUP: {
                puts("Key up");
                // handleKeyUp(&event);
                break;
            }
            case SDL_QUIT:
                puts("Quit");
                exit = 1;
                break;
            default:
                break;
        }
        if (exit) break;
    }

    SDL_RenderClear(g_renderer);
    SDL_Quit();
	return EXIT_SUCCESS;
}
