#include "items.h"

t_item static items[NB_ITEMS] = {
        {ITEM_BOMB, 0, 0, 0,false, 100, 0, 0, NULL, false, false,  NULL},
        {ITEM_BOMB_UP, 0, 0, 0,false, 1, 0, 0, NULL, false, false,  NULL},
        {ITEM_BOMB_DOWN, 0, 0, 0, false,1,0, 0, NULL, false, false,  NULL},
        {ITEM_YELLOW_FLAME, 0, 0, 0, false, 0,1, 0, NULL, false, false,  NULL},
        {ITEM_BLUE_FLAME, 0, 0, 0, false, 0, 1,0, NULL, false, false,   NULL},
        {ITEM_RED_FLAME, 0, 0, 0, true, 0, 999, 0, NULL, false, false,  NULL},
        {ITEM_PASS_THROUGH_BOMB, 0, 0, 0, false, 0, 0, 0, NULL, false, false,  NULL},
        {ITEM_BOMB_KICK, 0, 0, 0, false, 0, 0, 0, NULL, false, false,  NULL},
        {ITEM_INVINCIBILITY, 0, 0, 0, false, 0, 0, 1000, NULL, false, false,  NULL},
        {ITEM_HEART, 0, 0, 0, false, 0, 0, 0, NULL, false, false,  NULL},
        {ITEM_LIFE, 0, 0, 0, false, 0, 0, 0, NULL, false, false,  NULL}
};


t_item *getItem(int type)
{
    return &items[type];
}


void   useItem(t_item *item)
{
    if (item->isUsed)
        return;
    item->isUsed = true;
    switch (item->type) {
        case ITEM_BOMB_UP:
            //player.nb_bomb += item->nb_bomb;
            break;
        case ITEM_BOMB_DOWN:
            //player.nb_bomb -= item->nb_bomb;
            break;
        case ITEM_YELLOW_FLAME:
            //player.range += item->range;
            break;
        case ITEM_BLUE_FLAME:
            //player.range -= item->range;
            break;
        case ITEM_RED_FLAME:
            //player.range += item->range;
            break;
        case ITEM_PASS_THROUGH_BOMB:
//            if(player.bombKick)
//                player.bombKick = false;
//            else
//                player.passThroughBomb = true;
            break;
        case ITEM_BOMB_KICK:
//            if(player.passThroughBomb) {
//                player.passThroughBomb = false;
//            }else{
//                player.bombKick = true;
//            }
            break;
        case ITEM_INVINCIBILITY:
            //player.invincibility = true;
//            while (item->duration > 0) {
//                item->duration--;
//            }
            break;
        case ITEM_HEART:
            break;
        case ITEM_LIFE:
            //player.life += 1;
            break;
    }
}