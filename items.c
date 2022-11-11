#include "items.h"
#include "player.h"
#include "game.h"

t_item items[NB_ITEMS] = {
        {ITEM_BOMB, 0, 0, false, 1, 10000, false},
        {ITEM_BOMB_UP, 0, 0, false, 1, 10000, false},
        {ITEM_BOMB_DOWN, 0, 0, false, 1, 10000, false},
        {ITEM_YELLOW_FLAME, 0, 0, false, 1, 10000, false},
        {ITEM_BLUE_FLAME, 0, 0, false, 1, 10000, false},
        {ITEM_RED_FLAME, 0, 0, false, 1, 10000, false},
        {ITEM_PASS_THROUGH_BOMB, 0, 0, false, 1, 10000, false},
        {ITEM_BOMB_KICK, 0, 0, false, 1, 10000, false},
        {ITEM_INVINCIBILITY, 0, 0, false, 1, 10000, false},
        {ITEM_HEART, 0, 0, false, 1, 10000, false},
        {ITEM_LIFE, 0, 0, false, 1, 10000, false}
};


t_item     *getItem(t_item_type type) {
    return &items[type];
}

void   useItem(t_item *item) {
    t_player *player;
    player = getPlayer();
    SDL_TimerID timer_bomb_id, timer_invicibility_id;


    if (!hasItemInInventory(player, item)) return;
    if (item->isActive) return;

    item->type == ITEM_BOMB_UP ? player->inventory[ITEM_BOMB]->quantity++ : player->inventory[item->type]->quantity--;
    switch (item->type) {
        case ITEM_BOMB:
            // TODO : START A TIMER OF 10 SECONDS
            // SDL TIMER
            timer_bomb_id = SDL_AddTimer(item[ITEM_BOMB].duration, bombTimer, NULL);
            // t_timer *timerBomb = malloc(sizeof(t_timer));
            // timer->startTicks = SDL_GetTicks();
            // timer->duration = 10000;
            // timer->isPaused = false;
            // startTimer(timerBomb);
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
    }
}

Uint32 bombTimer(Uint32 interval, void *param) {
    explodeBomb(items[ITEM_BOMB].xCell, items[ITEM_BOMB].yCell);
    return 0;
}

Uint32 invincibilityTimer(Uint32 interval, void *param) {
    t_player *player;
    player = getPlayer();
    player->godMode = false;
    return 0;
}