#include "items.h"
#include "player.h"
#include "game.h"

#define DEBUG true

// Declare all items
t_item g_items[NB_ITEMS] = {
        {ITEM_BOMB, 0, 0, false, ITEM_BOMB_NB, 4000, false, TEX_BOMB},
        {ITEM_BOMB_UP, 0, 0, false, 0, 0, false, TEX_BOMB_UP},
        {ITEM_BOMB_DOWN, 0, 0, false, 0, 0, false, TEX_BOMB_DOWN},
        {ITEM_YELLOW_FLAME, 0, 0, false, 0, 0, false, TEX_YELLOW_FLAME},
        {ITEM_BLUE_FLAME, 0, 0, false, 0, 0, false, TEX_BLUE_FLAME},
        {ITEM_RED_FLAME, 0, 0, true, 0, 0, false, TEX_RED_FLAME},
        {ITEM_PASS_THROUGH_BOMB, 0, 0, false, 0, 0, false, TEX_PASS_THROUGH},
        {ITEM_BOMB_KICK, 0, 0, false, 0, 0, false, TEX_BOMB_KICK},
        {ITEM_INVINCIBILITY, 0, 0, false, 0, 10000, false, TEX_INVINCIBILITY},
        {ITEM_HEART, 0, 0, false, 0, 0, false, TEX_HEART},
        {ITEM_LIFE, 0, 0, false, 0, 0, false, TEX_LIFE}
};

/**
 * @brief      Use a item on the player
 * @param {t_item *} item
 * @return   {void}
 */
void   useItem(t_item *item) {
    t_player        *player;
    SDL_TimerID     timer_bomb_id;
    SDL_TimerID     timer_invicibility_id;

    player = getPlayer();
    if (!hasItemInInventory(player, item)) return;
    if (item->isActive) return;

    switch (item->type) {
        case ITEM_BOMB: {
            placeBomb(player->xCell, player->yCell, player);

            break;
        }
        case ITEM_BOMB_UP:
            player->inventory[ITEM_BOMB]->quantity--;
            break;
        case ITEM_YELLOW_FLAME:
            player->scope++;
            break;
        case ITEM_BLUE_FLAME:
            player->scope--;
            break;
        case ITEM_RED_FLAME:
            player->scope = 999;
            break;
        case ITEM_PASS_THROUGH_BOMB:
            if(player->bombKick) {
                player->bombKick = false;
            }else {
                player->passThroughBomb = true;
            }
            break;
        case ITEM_BOMB_KICK:
            if(player->passThroughBomb) {
                player->passThroughBomb = false;
            }else{
                player->bombKick = true;
            }
            break;
        case ITEM_INVINCIBILITY:
            player->godMode = true;
            timer_bomb_id = SDL_AddTimer(item[ITEM_INVINCIBILITY].duration, invincibilityTimer, NULL);
            break;
        case ITEM_HEART:
            player->canSurviveExplosion = true;
            break;
        case ITEM_LIFE:
            player->health += 1;
            break;
        default:
            break;
    }
}

/**
 * @brief      Timer for the bomb item
 * @param {Uint32} interval
 * @param {t_bomb *} param
 * @return   {Uint32}
 */
Uint32 bombTimer(Uint32 interval, t_bomb *param) {
    t_player    *player = ((t_player *) param->owner);

    t_map *map = getGame()->map;

    // if someone already triggered the bomb
    if (GETCELL(param->pos.x, param->pos.y) == BOMB) {
        explodeBomb(param->pos.x, param->pos.y, param->owner);
    }

    removePlacedBomb(player, param->pos.x, param->pos.y);

    free(param);
    return 0;
}

/**
 * @brief      Timer for the invincibility item
 * @param {Uint32} interval
 * @param {t_bomb *} param
 * @return   {Uint32}
 */
Uint32 invincibilityTimer(Uint32 interval, void *param) {
    t_player *player;
    player = getPlayer();
    player->godMode = false;
    return 0;
}