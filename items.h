#ifndef ITEMS_H
    #define ITEMS_H

    #include <SDL.h>
    #include <SDL_ttf.h>
    #include <stdio.h>
    #include "config.h"

    #define ITEM_BOMB_UP 0
    #define ITEM_BOMB_DOWN 1
    #define ITEM_YELLOW_FLAME 2
    #define ITEM_BLUE_FLAME 3
    #define ITEM_RED_FLAME 4
    #define ITEM_PASS_THROUGH_BOMB 5
    #define ITEM_BOMB_KICK 6
    #define ITEM_INVINCIBILITY 7
    #define ITEM_HEART 8
    #define ITEM_LIFE 9
    #define NB_ITEMS 10

    typedef struct s_item {
        int     type;
        char    *name;
        int     x;
        int     y;
        bool    isRare;
        int nb_bomb;
        int range;
        int     duration;
        struct s_item *next;
        bool    active;
        bool    isUsed;
        SDL_Texture *texture;
    } t_item;

    t_item *getItem(int type);

    void    drawItem(t_item *item);

    void    updateItem(t_item *item);

    void    destroyItem(t_item *item);

    void    useItem(t_item *item);

    void    activateItem(t_item *item);

    void    deactivateItem(t_item *item);

    void    useItem(t_item *item);






#endif
