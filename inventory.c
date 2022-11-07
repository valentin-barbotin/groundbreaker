#include <SDL_rect.h>
#include "inventory.h"
#include "game.h"
#include "utils.h"
#include "font.h"

#define DEBUG true

void drawInventory() {
    SDL_Color colorRed = {255, 0, 0, 255};
    t_game      *game;
    SDL_Rect    rect;

    game = getGame();

    pickColor(&colorRed);

    rect.x = (gameConfig->video.width) * 0.175;
    rect.y = (gameConfig->video.height) * 0.92;
    rect.w = (gameConfig->video.width) * 0.65;
    rect.h = (gameConfig->video.width) * 0.05;
    SDL_RenderDrawRect(g_renderer, &rect);

    // TODO : replace 10 by 'NB_ITEMS'
    for (int separator = 0; separator < 10; separator++) {
        SDL_RenderDrawLine(g_renderer, rect.x + (rect.w / 10) * separator, rect.y, rect.x + (rect.w / 10) * separator, rect.y + rect.h);
        // TODO : replace 1 by player->inventory[ITEM_NAME]->quantity
        drawText(&colorRed, rect.x + (rect.w / 10) * separator + 10, rect.y + 10, "1", false, rect.w);
    }


}

