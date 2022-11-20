#ifndef INVENTORY_H
 #define INVENTORY_H

 #include "colors.h"

 #define INVENTORY_ITEM_NB 5

 void    drawInventoryItem(SDL_Rect *rect, int separator, int item);
 void    drawInventory();
 void    drawSelectedItem();

#endif