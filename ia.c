#include "ia.h"
#include "config.h"

#define DEBUG true

t_player    *g_bots[MAX_BOTS];
short        g_nbBots = 0;

void    searchBotsPos(const t_map *map) {
    bool    ok;

    for (short y = 1; y < map->height - 2; y++) {
        for (short x = 1; x < map->width - 2; x++) {
            // find a line of empty cells (min 3)
            if (map->map[y][x] == EMPTY && map->map[y][x + 1] == EMPTY && map->map[y][x + 2] == EMPTY) {
                #ifdef DEBUG
                    printf("found empty line x:%d, y:%d\n", x, y);
                #endif

                // check proximity of bots
                ok = true;
                for (short i = 0; i < g_nbBots; i++)
                {
                    if (g_bots[i]->yCell == y && g_bots[i]->direction == DIR_RIGHT) {
                        ok = false;
                        break;
                    }
                }

                if (ok) {
                    spawnBot(map, x, y, DIR_RIGHT);
                }
                break;
            // find a column of empty cells (min 3)
            } else if (map->map[y][x] == EMPTY && map->map[y + 1][x] == EMPTY && map->map[y + 2][x] == EMPTY) {
                #ifdef DEBUG
                    printf("found empty column x:%d, y:%d\n", x, y);
                #endif
                // check proximity of bots
                ok = true;
                for (short i = 0; i < g_nbBots; i++)
                {
                    if (g_bots[i]->xCell == x && g_bots[i]->direction == DIR_DOWN) {
                        ok = false;
                        break;
                    }
                }

                if (ok) {
                    spawnBot(map, x, y, DIR_DOWN);
                }
                break;
            }
        }
    }

}

void    spawnBot(const t_map *map, short x, short y, t_direction direction) {
    if (x < 0 || y < 0 || x >= map->width || y >= map->height) {
        #ifdef DEBUG
            fprintf(stderr, "spawnBot: invalid position x:%d, y:%d\n", x, y);
        #endif
        return;
    }

    if (x == 1 && y == 1) {
        #ifdef DEBUG
            fprintf(stderr, "spawnBot: invalid position x:%d, y:%d\n", x, y);
        #endif
        return;
    }
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
