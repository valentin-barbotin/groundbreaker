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
    player->scope = 2;
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

void       initInventory() {
    t_player    *player;
    player = getPlayer();


    // TODO : init inventore, example : player->inventory[ITEM_BOMB] = initItem(ITEM_BOMB);



    /*
    for(int k = 1; k < NB_ITEMS; k++) {
        player->inventory[k] = initItem(k);
    }
    */

}


bool        hasItemInInventory(t_item *item) {
    t_player    *player;
    player = getPlayer();

    if (player->inventory[item->type] != NULL && player->inventory[item->type]->quantity > 0) {
        return true;
    }
    return false;
}

// printInventory() is a function I use to debug my inventory
void        printInventory() {
    t_player    *player;
    player = getPlayer();

    /*
    for(int k = 0; k < NB_ITEMS; k++) {
        if (player->inventory[k] != NULL) {
            printf("Item %d: %d", k, player->inventory[k]->quantity);
        }
    }
     */
}