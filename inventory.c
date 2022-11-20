#include <SDL_rect.h>
#include "inventory.h"
#include "game.h"
#include "utils.h"
#include "font.h"
#include "display.h"

#define DEBUG true

void    drawInventoryItem(SDL_Rect *rect, int separator, int item) {
    const t_player      *player;
    SDL_Rect            rectItem;
    SDL_Rect            rectDest;
    char                buff[8];

    player = getPlayer();

    rectItem.x = 0;
    rectItem.y = 0;

    rectItem.w = 758;
    rectItem.h = 988;

    SDL_RenderDrawLine(g_renderer, rect->x + (rect->w / INVENTORY_ITEM_NB) * separator, rect->y, rect->x + (rect->w / INVENTORY_ITEM_NB) * separator, rect->y + rect->h);
    // TODO : replace 0 by player->inventory[ITEM_NAME]->quantity
    sprintf(buff, "%d", player->inventory[item]->quantity);
    drawText(&colorWhite, rect->x + (rect->w / INVENTORY_ITEM_NB) * separator + 5, rect->y + 6, buff, false, rect->w);

    SDL_RenderDrawRect(g_renderer, &rect);

    rectDest.x = rect->x + (rect->w / INVENTORY_ITEM_NB) * separator;
    rectDest.y = rect->y;
    rectDest.w = rect->w / INVENTORY_ITEM_NB;
    rectDest.h = rect->h;

    drawTexture(player->inventory[item]->tex, &rectItem, &rectDest);
}

void drawInventory() {
    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect            rect;

    pickColor(&colorLightGray);

    rect.x = (gameConfig->video.width) * 0.175;
    rect.y = (gameConfig->video.height) * 0.87;
    rect.w = (gameConfig->video.width) * 0.65;
    rect.h = (gameConfig->video.width) * 0.10;

    SDL_RenderFillRect(g_renderer, &rect);
    loadFont(FONT_PATH, 10);
    pickColor(&colorWhite);

    drawInventoryItem(&rect, 0, ITEM_BOMB);
    drawInventoryItem(&rect, 1, ITEM_PASS_THROUGH_BOMB);
    drawInventoryItem(&rect, 2, ITEM_BOMB_KICK);
    drawInventoryItem(&rect, 3, ITEM_INVINCIBILITY);
    drawInventoryItem(&rect, 4, ITEM_HEART);

    loadFont(FONT_PATH, 20);
}

void drawSelectedItem() {
    SDL_Color           colorYellow = {255, 255, 0, 255};
    SDL_Color           colorBlack = {0, 0, 0, 255};
    SDL_Rect            rect;
    const t_player      *player;


    player = getPlayer();

    pickColor(&colorYellow);

    rect.x = (gameConfig->video.width) * 0.175 + (player->selectedSlot * (gameConfig->video.width) * 0.65 / INVENTORY_ITEM_NB);
    rect.y = (gameConfig->video.height) * 0.87;
    rect.w = (gameConfig->video.width) * 0.65 / INVENTORY_ITEM_NB;
    rect.h = (gameConfig->video.width) * 0.10;

    // Increase the width of the rectangle
    for (int i = 0; i < 3; i++) {
        rect.x -= 1;
        rect.y -= 1;
        rect.w += 2;
        rect.h += 2;
        SDL_RenderDrawRect(g_renderer, &rect);
    }

    pickColor(&colorBlack);
}