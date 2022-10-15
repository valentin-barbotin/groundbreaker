#include <math.h>
#include "config.h"
#include "loop.h"
#include "game.h"
#include "map.h"

static t_game *game;
static t_map *map;

t_game *getGame() {
    if (game == NULL) {
        game = malloc(sizeof(t_game));
        game_init(game);
    }
    return game;
}

void game_init (t_game *game) {
    if(game->x == 0 && game->y == 0) {
        game->x = 0;
        game->y = 0;
    }
}

int lastX = 0;
int lastY = 0;
void movePlayer(t_game *game, t_map *map) {
    if (game->x == lastX && game->y == lastY) { return; }

    // the old position is now empty
    map->map[lastY][lastX] = 'X';

    if (game->x >= map->width) {
        game->x = 0;
    }else if (game->x < 0) {
        game->x = map->width - 1;
    } else if (game->y >= map->height) {
        game->y = 0;
    } else if (game->y < 0) {
        game->y = map->height - 1;
    }

    switch (map->map[game->y][game->x]) {
        case 'X':
            map->map[game->y][game->x] = 'P';
            break;
        case 'M':
            game->x = lastX;
            game->y = lastY;
            // TODO : player must plant a bomb to break the wall
            break;
        case 'I':
            game->x = lastX;
            game->y = lastY;
            // unbreakable wall
            break;
        case 'B':
            map->map[game->y][game->x] = 'P';
            // TODO : player is on a bomb so if the bomb explodes the player dies
            break;
        case 'O':
            map->map[game->y][game->x] = 'P';
            // TODO : Remove the item from the map and add it to the inventory
            break;
        default:
            break;
    }

    lastX = game->x;
    lastY = game->y;

    printf("__________debut__________\n");
    map_print(map);
    printf("_________fin__________\n");
}


