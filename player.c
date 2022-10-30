#include <stdlib.h>
#include <stdio.h>

#include "player.h"

#define DEBUG true

t_player   *initPlayer() {
    t_player    *player;

    player = malloc(sizeof(t_player));
    if (player == NULL) {
        #ifdef DEBUG
            fprintf(stderr, "Error allocating memory for player");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Error while creating player", g_window);
        exit(1);
    }
    *player->name = '\0';
    player->score = 0;
    player->x = 0;
    player->y = 0;
    player->vx = 0;
    player->vy = 0;
    player->xCell = 0;
    player->yCell = 0;

    return player;
}

t_player   *getPlayer() {
    static t_player *player;

    if (player == NULL) {
        player = initPlayer();
    }

    return player;
}
