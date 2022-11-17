#include "items.h"
#include "player.h"
#include "game.h"

#define DEBUG true

t_item g_items[NB_ITEMS] = {
        {ITEM_BOMB, 0, 0, false, 2, 2000, false, TEX_BOMB},
        {ITEM_BOMB_UP, 0, 0, false, 0, 0, false, TEX_BOMB_UP},
        {ITEM_BOMB_DOWN, 0, 0, false, 0, 0, false, TEX_BOMB_DOWN},
        {ITEM_YELLOW_FLAME, 0, 0, false, 0, 0, false, TEX_YELLOW_FLAME},
        {ITEM_BLUE_FLAME, 0, 0, false, 0, 0, false, TEX_BLUE_FLAME},
        {ITEM_RED_FLAME, 0, 0, false, 0, 0, false, TEX_RED_FLAME},
        {ITEM_PASS_THROUGH_BOMB, 0, 0, false, 0, 0, false, TEX_PASS_THROUGH},
        {ITEM_BOMB_KICK, 0, 0, false, 0, 0, false, TEX_BOMB_KICK},
        {ITEM_INVINCIBILITY, 0, 0, false, 0, 10000, false, TEX_INVINCIBILITY},
        {ITEM_HEART, 0, 0, false, 0, 0, false, TEX_HEART},
        {ITEM_LIFE, 0, 0, false, 0, 0, false, TEX_LIFE}
};

void   useItem(t_item *item) {
    t_player        *player;
    SDL_TimerID     timer_bomb_id;
    SDL_TimerID     timer_invicibility_id;

    player = getPlayer();
    if (!hasItemInInventory(player, item)) return;
    if (item->isActive) return;

    item->type == ITEM_BOMB_UP ? player->inventory[ITEM_BOMB]->quantity++ : player->inventory[item->type]->quantity--;
    switch (item->type) {
        case ITEM_BOMB: {
            // TODO : START A TIMER OF 10 SECONDS
            // SDL TIMER
            SDL_Point   *pos = malloc(sizeof(SDL_Point));
            if (!pos) {
                #ifdef DEBUG
                    fprintf(stderr, "Malloc error in useItem()");
                #endif
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
                exit(1);
            };

            pos->x = player->xCell;
            pos->y = player->yCell;
            
            printf("POS TIMER x:%d y:%d\n", pos->x, pos->y);
            
            //place the bomb
            timer_bomb_id = SDL_AddTimer(item[ITEM_BOMB].duration, bombTimer, pos);

            t_map *map = getGame()->map;

            GETCELL(player->xCell, player->yCell) = BOMB;
            updateCell(player->xCell, player->yCell, BOMB);
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
            // TODO : START A TIMER OF 10 SECONDS
            timer_bomb_id = SDL_AddTimer(item[ITEM_INVINCIBILITY].duration, invincibilityTimer, NULL);
            // t_timer *timerInvincibility = malloc(sizeof(t_timer));
            // timer->startTicks = SDL_GetTicks();
            // timer->duration = 10000;
            // timer->isPaused = false;
            // startTimer(timerInvincibility);
            break;
        case ITEM_HEART:
            // TODO: on le met en godMode jusqu'à ce qu'il subisse une explosion après il est plus en false
            player->canSurviveExplosion = true;
            break;
        case ITEM_LIFE:
            player->health += 1;
            break;
        default:
            break;
    }
}

Uint32 bombTimer(Uint32 interval, SDL_Point *param) {
    printf("BOMB TIMER x:%d y:%d\n", param->x, param->y);
    explodeBomb(param->x, param->y);
    free(param);
    return 0;
}

Uint32 invincibilityTimer(Uint32 interval, void *param) {
    t_player *player;
    player = getPlayer();
    player->godMode = false;
    return 0;
}