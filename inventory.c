#include <SDL_rect.h>
#include "inventory.h"
#include "game.h"
#include "utils.h"
#include "font.h"

#define DEBUG true

void drawInventory() {
    SDL_Color colorRed = {255, 0, 0, 255};
    SDL_Color colorWhite = {255, 255, 255, 255};
    t_game      *game;
    t_player      *player;
    SDL_Rect    rect;

    game = getGame();

    pickColor(&colorRed);

    rect.x = (gameConfig->video.width) * 0.175;
    rect.y = (gameConfig->video.height) * 0.92;
    rect.w = (gameConfig->video.width) * 0.65;
    rect.h = (gameConfig->video.width) * 0.05;

    SDL_RenderFillRect(g_renderer, &rect);
    loadFont(FONT_PATH, 10);
    pickColor(&colorWhite);

    // TODO : replace 0 by 'ITEM_BOMB' and replace 10 by 'NB_ITEMS'
    // NB : ITEM BOMB replace 0 because it's the first item of the enum
    for (int separator = 0; separator < 10; separator++) {
        SDL_RenderDrawLine(g_renderer, rect.x + (rect.w / 10) * separator, rect.y, rect.x + (rect.w / 10) * separator, rect.y + rect.h);
        // TODO : replace 0 by player->inventory[ITEM_NAME]->quantity
        drawText(&colorWhite, rect.x + (rect.w / 10) * separator + 10, rect.y + 10, "0", false, rect.w);

        //player->inventory[separator]->xCell = rect.x + (rect.w / 10) * separator;
        //player->inventory[separator]->yCell = rect.y;
    }
}

void drawSelectedItem() {
    SDL_Color   colorBlue = {0, 0, 255, 255};
    SDL_Rect    rect;
    t_game      *game;

    game = getGame();

    pickColor(&colorBlue);

    // TODO : create a function to draw a rectangle with yellow color (the color should be changed) bcs it's the selected item

    // First idea but it's not working and i will change it
    /*
    rect.x = (gameConfig->video.width) * 0.175;
    rect.y = (gameConfig->video.height) * 0.92;
    rect.w = (gameConfig->video.width) * 0.05;
    rect.h = (gameConfig->video.width) * 0.05;

    SDL_RenderFillRect(g_renderer, &rect);
    */

}

