#ifndef ITEMS_H
#define ITEMS_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include "config.h"

#define ITEM_BOMB 0
#define ITEM_BOMB_UP 1
#define ITEM_BOMB_DOWN 2
#define ITEM_YELLOW_FLAME 3
#define ITEM_BLUE_FLAME 4
#define ITEM_RED_FLAME 5
#define ITEM_PASS_THROUGH_BOMB 6
#define ITEM_BOMB_KICK 7
#define ITEM_INVINCIBILITY 8
#define ITEM_HEART 9
#define ITEM_LIFE 10
#define NB_ITEMS 11

typedef struct s_item {
    int     type;
    int     xCell;
    int     yCell;
    bool    isRare;
    int     quantity;
    int     duration;
    bool    isActive;
} t_item;

t_item *getItem(int type);

void    drawItem(t_item *item);

void    updateItem(t_item *item);

void    destroyItem(t_item *item);

void    activateItem(t_item *item);

void    deactivateItem(t_item *item);

void    useItem(t_item *item);


#endif
