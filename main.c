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
    // init SDL
    setupSDL();
    puts("Hello World!");


    SDL_Quit();
	return EXIT_SUCCESS;
}
