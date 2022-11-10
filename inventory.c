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

    player = getPlayer();

    player->inventory.x = NULL;

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

        if(player->inventory.x == NULL) {
            player->inventory.x = rect.x + (rect.w / 10) * separator;
            player->inventory.y = rect.y;
            player->inventory.w = rect.w / 10;
            player->inventory.h = rect.h;
        }
    }
}

void drawSelectedItem() {
    SDL_Color   colorYellow = {255, 255, 0, 255};
    SDL_Rect    rect;
    t_game      *game;
    t_player    *player;
    player = getPlayer();

    game = getGame();

    // TODO: trouver une fonction SDL pour faire ça
    //deleteOldSelectedItem();

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

void deleteOldSelectedItem() {
    // Cette fonction supprime toute la map (tout ce qui a été déssiné précédemment)
    //SDL_RenderClear(g_renderer);
}