#include "items.h"

t_item static items[NB_ITEMS] = {
    {ITEM_BOMB_UP, "Bomb Up", 0, 0,false, 1, 0, 0, NULL, false, false,  NULL},
    {ITEM_BOMB_DOWN, "Bomb Down", 0, 0, false,1,0, 0, NULL, false, false,  NULL},
    {ITEM_YELLOW_FLAME, "Yellow Flame", 0, 0, false, 0,1, 0, NULL, false, false,  NULL},
    {ITEM_BLUE_FLAME, "Blue Flame", 0, 0, false, 0, 1,0, NULL, false, false,   NULL},
    {ITEM_RED_FLAME, "Red Flame", 0, 0, true, 0, 999, 0, NULL, false, false,  NULL},
    {ITEM_PASS_THROUGH_BOMB, "Pass Through Bomb", 0, 0, false, 0, 0, 0, NULL, false, false,  NULL},
    {ITEM_BOMB_KICK, "Bomb Kick", 0, 0, false, 0, 0, 0, NULL, false, false,  NULL},
    {ITEM_INVINCIBILITY, "Invincibility", 0, 0, false, 0, 0, 1000, NULL, false, false,  NULL},
    {ITEM_HEART, "Heart", 0, 0, false, 0, 0, 0, NULL, false, false,  NULL},
    {ITEM_LIFE, "Life", 0, 0, false, 0, 0, 0, NULL, false, false,  NULL}
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