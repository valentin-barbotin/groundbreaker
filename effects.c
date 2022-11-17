#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "effects.h"
#include "game.h"
#include "map.h"
#include "client.h"

t_effect    **g_effects = NULL;
int         g_nbEffects = 0;

void        initEffects() {
    g_effects = malloc(sizeof(t_effect*) * MAX_EFFECTS);
    if (g_effects == NULL) {
        printf("Error: malloc failed in initEffects");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_EFFECTS; i++) {
        g_effects[i] = malloc(sizeof(t_effect));
        if (g_effects[i] == NULL) {
            printf("Error: malloc failed in initEffects");
            exit(1);
        }
        g_effects[i]->active = false;
    }
}

void        findAvailableEffect(t_effect **effect) {
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (g_effects[i]->active == false) {
            *effect = g_effects[i];
            return;
        }
    }
}

t_effect    *addEffect(int xCell, int yCell, t_effects type) {
    unsigned int      cellSizeX;
    unsigned int      cellSizeY;
    const t_game     *game;
    
    game = getGame();

    if (g_nbEffects >= MAX_EFFECTS) return NULL;

    if (g_effects == NULL) initEffects();

    t_effect *effect = NULL;
    findAvailableEffect(&effect);
    if (effect == NULL) return NULL;

    cellSizeX = gameConfig->video.width / game->map->width;
    cellSizeY = gameConfig->video.height / game->map->height;

    g_nbEffects++;

    effect->x = xCell * cellSizeX;
    effect->y = yCell * cellSizeY;
    effect->xCell = xCell;
    effect->yCell = yCell;
    effect->type = type;

    switch (type)
    {
        case BOMB_EXPLOSION:  // 9 w 7 h
            effect->frameMax = (9 * 5); // 7 rows of 9 frames
            effect->rows = 7;
            effect->cols = 9;
            effect->active = true;
            effect->startTicks = SDL_GetTicks();
            break;
        
        default:
            break;
    }

    return effect;
}

void        drawEffects() {
    if (g_effects == NULL) return;

    Uint64  ticks;
    int     sprite;
    int     row;
    int     col;

    ticks = SDL_GetTicks64();

    for (int i = 0; i < MAX_EFFECTS; i++) {
        t_effect *effect = g_effects[i];
        if (effect->active) {
            switch (effect->type)
            {
                case BOMB_EXPLOSION:
                    sprite = ((ticks - effect->startTicks) / 50) % (effect->rows * effect->cols);
                    if (sprite == effect->rows * effect->cols - 1) {
                        effect->active = false;
                        g_nbEffects--;
                        break;
                    }

                    row = sprite / effect->cols;
                    col = sprite % effect->cols;

                    drawSprite(TEX_BOMB_EXPLOSION, effect->x, effect->y, row, col);
                    break;
                
                default:
                    break;
            }
        }
    }
}

void        drawSprite(const char *tex, int x, int y, int row, int col) {
    unsigned int    cellSizeX;
    unsigned int    cellSizeY;
    const t_game    *game;
    const t_map     *map;

    game = getGame();
    map = game->map;

    cellSizeX = gameConfig->video.width / map->width;
    cellSizeY = gameConfig->video.height / map->height;

    SDL_Rect srcRect = {col * 71, row * 67, 71, 67};
    SDL_Rect dstRect = {x, y, cellSizeX, cellSizeY};

    // printf("row: %d col: %d,  coords: x: %d y: %d w: %d h: %d\n", row, col, col * 71, row * 67, 71, 67);

    drawTexture(tex, &srcRect, &dstRect);
}

void        receiveEffect(const char *content) {
    int             xCell;
    int             yCell;
    int             type;

    sscanf(content, "%d %d %d", &xCell, &yCell, &type);
    addEffect(xCell, yCell, type);
}
