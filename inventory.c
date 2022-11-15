#include <SDL_rect.h>
#include "inventory.h"
#include "game.h"
#include "utils.h"
#include "font.h"
#include "display.h"

#define DEBUG true

void drawInventory() {
    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);

    SDL_Color colorGrey = {128, 128, 128, 128};
    SDL_Color colorWhite = {255, 255, 255, 255};
    const t_player      *player;
    SDL_Rect            rect;
    SDL_Rect            rectItem;
    SDL_Rect            rectDest;

    player = getPlayer();

    pickColor(&colorGrey);

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

    for (int separator = ITEM_BOMB; separator < 11; separator++) {
        SDL_RenderDrawLine(g_renderer, rect.x + (rect.w / 11) * separator, rect.y, rect.x + (rect.w / 11) * separator, rect.y + rect.h);
        // TODO : replace 0 by player->inventory[ITEM_NAME]->quantity
        drawText(&colorWhite, rect.x + (rect.w / 11) * separator + 5, rect.y + 6, "0", false, rect.w);

        SDL_RenderDrawRect(g_renderer, &rect);

        rectDest.x = rect.x + (rect.w / 11) * separator;
        rectDest.y = rect.y;
        rectDest.w = rect.w / 11;
        rectDest.h = rect.h;

        drawTexture(player->inventory[separator]->tex, &rectItem, &rectDest);

    }
    loadFont(FONT_PATH, 20);
}

void drawSelectedItem() {
    SDL_Color           colorYellow = {255, 255, 0, 255};
    SDL_Color           colorBlack = {0, 0, 0, 255};
    SDL_Rect            rect;
    const t_player      *player;


    player = getPlayer();

    pickColor(&colorYellow);

    rect.x = (gameConfig->video.width) * 0.175 + (player->selectedSlot * (gameConfig->video.width) * 0.65 / 11);
    rect.y = (gameConfig->video.height) * 0.92;
    rect.w = (gameConfig->video.width) * 0.65 / 11;
    rect.h = (gameConfig->video.width) * 0.05;

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