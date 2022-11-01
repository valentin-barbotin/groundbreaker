#include <stdlib.h>
#include <stdio.h>

#include "player.h"
#include "game.h"

#define DEBUG true

char                g_username[256] = {0};
short               g_playersMultiIndex = 0;


char       *getUsername() {
    const t_game      *game;

    game = getGame();

    if (g_clientThread) {
        return game->players[g_playersMultiIndex]->name;
    } else {
        return g_username;
    }
}

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
    const t_game      *game;

    game = getGame();

    // if player is a client, player != [0]
    if (g_clientThread) {
        return game->players[g_playersMultiIndex];
    } else {
        return game->players[0];
    }
}
