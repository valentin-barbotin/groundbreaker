#ifndef ITEMS_H
#define ITEMS_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>

#include "config.h"
#include "bomb.h"

#define ITEM_BOMB_NB 2

typedef enum e_item_type {
    ITEM_BOMB,
    ITEM_BOMB_UP,
    ITEM_BOMB_DOWN,
    ITEM_YELLOW_FLAME,
    ITEM_BLUE_FLAME,
    ITEM_RED_FLAME,
    ITEM_PASS_THROUGH_BOMB,
    ITEM_BOMB_KICK,
    ITEM_INVINCIBILITY,
    ITEM_HEART,
    ITEM_LIFE,
    NB_ITEMS
}           t_item_type;

typedef struct s_item {
    t_item_type     type;
    int             xCell;
    int             yCell;
    bool            isRare;
    int             quantity;
    Uint32          duration;
    bool            isActive;
    char            *tex;
}              t_item;

void    useItem(t_item *item);
Uint32  bombTimer(Uint32 interval, t_bomb *param);
Uint32  invincibilityTimer(Uint32 interval, void *param);


#endif
