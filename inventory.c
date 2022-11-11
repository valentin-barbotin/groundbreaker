#include <SDL_rect.h>
#include "inventory.h"
#include "game.h"
#include "utils.h"
#include "font.h"
#include "display.h"

#define DEBUG true

void drawInventory() {
    SDL_Color colorRed = {255, 0, 0, 255};
    SDL_Color colorWhite = {255, 255, 255, 255};
    t_game      *game;
    t_player      *player;
    SDL_Rect    rect, rectItem, rectDest;

    player = getPlayer();

    player->inventory.x = NULL;

    pickColor(&colorRed);

    rect.x = (gameConfig->video.width) * 0.175;
    rect.y = (gameConfig->video.height) * 0.92;
    rect.w = (gameConfig->video.width) * 0.65;
    rect.h = (gameConfig->video.width) * 0.05;

    rectItem.x = 0;
    rectItem.y = 0;
    rectItem.w = 758;
    rectItem.h = 988;

    SDL_RenderFillRect(g_renderer, &rect);
    loadFont(FONT_PATH, 10);
    pickColor(&colorWhite);

    // TODO : replace 0 by 'ITEM_BOMB' and replace 10 by 'NB_ITEMS'
    // NB : ITEM BOMB replace 0 because it's the first item of the enum
    for (int separator = 0; separator < 10; separator++) {
        SDL_RenderDrawLine(g_renderer, rect.x + (rect.w / 10) * separator, rect.y, rect.x + (rect.w / 10) * separator, rect.y + rect.h);
        // TODO : replace 0 by player->inventory[ITEM_NAME]->quantity
        drawText(&colorWhite, rect.x + (rect.w / 10) * separator + 5, rect.y + 6, "0", false, rect.w);

        player->inventory.x = rect.x + (rect.w / 10) * separator;
        player->inventory.y = rect.y;
        player->inventory.w = rect.w / 10;
        player->inventory.h = rect.h;

        SDL_RenderDrawRect(g_renderer, &rect);


        rectDest.x = rect.x + (rect.w / 10) * separator;
        rectDest.y = rect.y;
        rectDest.w = player->inventory.w;
        rectDest.h = rect.h;

        //TODO : replace TEX_BLUE_FLAME by player->inventory[ITEM_NAME]->texture
        drawTexture(TEX_BOMB_UP, &rectItem, &rectDest);

    }
}

// TODO : add 1 parameter to the function to know which item is selected (example : ITEM_BOMB is the first slot because ITEM_BOMB is the first item of the enum)
void drawSelectedItem() {
    SDL_Color   colorYellow = {255, 255, 0, 255};
    SDL_Rect    rect;
    t_game      *game;
    t_player    *player;
    player = getPlayer();

    game = getGame();

    pickColor(&colorYellow);

    rect.x = player->inventory.x;
    rect.y = player->inventory.y;
    rect.w = player->inventory.w;
    rect.h = player->inventory.h;

    // Increase the width of the rectangle
    for (int i = 0; i < 3; i++) {
        rect.x -= 1;
        rect.y -= 1;
        rect.w += 2;
        rect.h += 2;
        SDL_RenderDrawRect(g_renderer, &rect);
    }
}

// TODO : create a function named getSlot() which return the number of the slot where the item is with (rect.x and rect.w) - rect.x change according to the slot
