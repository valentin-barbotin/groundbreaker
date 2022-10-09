#include "stdio.h"
#include "stdlib.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>

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
    SDL_Window*     window = NULL;
    SDL_Renderer*   renderer = NULL;
    SDL_Surface*    surface = NULL;
    SDL_Texture*    texture = NULL;

    // init SDL
    setupSDL();
    

    windowWidth = 1024;
    windowHeight = 1024;

    char* windowTitle = "Bandersnatch";
    // Create a resizable window using config's width/heigth
    window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!window) {
		fprintf(stderr, "Erreur SDL_CreateWindow : %s", SDL_GetError());
		return -1;
	}

    // Create the renderer for the window
    // SDL_RENDERER_ACCELERATED -> hardware acceleration
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		fprintf(stderr, "Erreur SDL_CreateRenderer : %s", SDL_GetError());
		return -1;
	}

    SDL_RaiseWindow(window);

    SDL_RenderPresent(renderer);

    int exit = 0;
    SDL_Event event;
    // Main loop
    while (SDL_WaitEvent(&event), !exit) {
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

    SDL_RenderClear(renderer);
    SDL_Quit();
	return EXIT_SUCCESS;
}
