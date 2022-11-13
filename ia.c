#include "ia.h"
#include "config.h"

#define DEBUG true

t_player    *g_bots[MAX_BOTS];
short        g_nbBots = 0;

void    searchBotsPos(const t_map *map) {

    // find a line of empty cells
    for (short i = 1; i < map->height - 2; i++) {
        for (short j = 1; j < map->width - 2; j++) {
            // minimum 3 empty cells
            if (map->map[i][j] == EMPTY && map->map[i][j + 1] == EMPTY && map->map[i][j + 2] == EMPTY) {
                printf("found empty line at %d, %d\n", i, j);
                spawnBot(map, j, i, DIR_RIGHT);
                break;
            }
        }
    }

    // find a column of empty cells
    for (short i = 1; i < map->height - 2; i++) {
        for (short j = 1; j < map->width - 2; j++) {
            // minimum 3 empty cells
            if (map->map[i][j] == EMPTY && map->map[i + 1][j] == EMPTY && map->map[i + 2][j] == EMPTY) {
                printf("found empty column at %d, %d\n", i, j);
                spawnBot(map, j, i, DIR_DOWN);
                break;
            }
        }
    }
}

void    spawnBot(const t_map *map, short x, short y, t_direction direction) {
    unsigned int    cellSizeX;
    unsigned int    cellSizeY;

    if (g_nbBots == MAX_BOTS) {
        #ifdef DEBUG
        printf("Max number of bots reached\n");
        #endif
        return;
    }

    cellSizeX = gameConfig->video.width / map->width;
    cellSizeY = gameConfig->video.height / map->height;
    
    g_bots[g_nbBots] = initPlayer();
    g_bots[g_nbBots]->yCell = y;
    g_bots[g_nbBots]->xCell = x;
    g_bots[g_nbBots]->y = y * cellSizeY + cellSizeY / 2;
    g_bots[g_nbBots]->x = x * cellSizeX + cellSizeX / 2;
    g_bots[g_nbBots]->isBot = true;
    g_bots[g_nbBots]->direction = direction;

    switch (direction)
    {
        case DIR_RIGHT:
            g_bots[g_nbBots]->vx = BOT_SPEED;
            break;
        case DIR_DOWN:
            g_bots[g_nbBots]->vy = BOT_SPEED;
            break;
        
        default:
            break;
    }
    g_nbBots++;
}
