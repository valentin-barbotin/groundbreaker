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

    player->inventory[ITEM_BOMB]->type = ITEM_BOMB;
    player->inventory[ITEM_BOMB]->quantity = 2;

    for(int k = 1; k < NB_ITEMS; k++) {
        player->inventory[k] = NULL;
    }

}

void       useItem(t_item *item) {
    t_player    *player;
    player = getPlayer();

    if (!hasItemInInventory(item)) return;

    switch (item->type) {
        case ITEM_BOMB:
            player->inventory[ITEM_BOMB]->quantity--;
            player->inventory[ITEM_BOMB]->isUsed = true;
            break;
        case ITEM_BOMB_UP:
            player->inventory[ITEM_BOMB_UP]->quantity--;
            player->inventory[ITEM_BOMB]->isUsed = true;
            break;
        case ITEM_BOMB_DOWN:
            player->inventory[ITEM_BOMB_DOWN]->quantity--;
            player->inventory[ITEM_BOMB_DOWN]->isUsed = true;
            break;
        case ITEM_YELLOW_FLAME:
            player->inventory[ITEM_YELLOW_FLAME]->quantity--;
            player->inventory[ITEM_YELLOW_FLAME]->isUsed = true;
            player->scope++;
            break;
        case ITEM_BLUE_FLAME:
            player->inventory[ITEM_BLUE_FLAME]->quantity--;
            player->inventory[ITEM_BLUE_FLAME]->isUsed = true;
            player->scope--;
            break;
        case ITEM_RED_FLAME:
            player->inventory[ITEM_RED_FLAME]->quantity--;
            player->inventory[ITEM_RED_FLAME]->isUsed = true;
            player->scope = 999;
            break;
        case ITEM_PASS_THROUGH_BOMB:
            player->inventory[ITEM_PASS_THROUGH_BOMB]->quantity--;
            player->inventory[ITEM_PASS_THROUGH_BOMB]->isActive = true;
            break;
        case ITEM_BOMB_KICK:
            player->inventory[ITEM_BOMB_KICK]->quantity--;
            player->inventory[ITEM_BOMB_KICK]->isActive = true;
            break;
        case ITEM_INVINCIBILITY:
            player->inventory[ITEM_INVINCIBILITY]->quantity--;
            player->inventory[ITEM_INVINCIBILITY]->duration = 10000;
            player->inventory[ITEM_INVINCIBILITY]->isActive = true;
            break;
        case ITEM_HEART:
            player->inventory[ITEM_HEART]->quantity--;
            player->inventory[ITEM_HEART]->isUsed = true;
            player->health = 200;
            break;
        case ITEM_LIFE:
            player->inventory[ITEM_LIFE]->quantity--;
            player->inventory[ITEM_LIFE]->isUsed = true;
            player->health++;
            break;
        default:
            break;
    }
}

bool hasItemInInventory(t_item *item) {
    t_player    *player;
    player = getPlayer();

    if (player->inventory[item->type] != NULL && player->inventory[item->type]->quantity > 0) {
        return true;
    }
    return false;
}