#include "config.h"
#include "loop.h"
#include "game.h"
#include "map.h"

static t_map *map;
int lastX, lastY = 0;

t_game  *getGame() {
    static t_game *game;

    if (game == NULL) {
        game = malloc(sizeof(t_game));
        game_init(game);
    }
    return game;
}

void    game_init (t_game *game) {
    if (game->x == 0 && game->y == 0) {
        game->x = 0;
        game->y = 0;
    }
}

void    movePlayer(t_game *game, t_map *map) {
    if (game->x == lastX && game->y == lastY)  return;

    // the old position is now empty
    map->map[lastY][lastX] = EMPTY;

    // if player want to go out of the map then we move him at the other side
    if (game->x >= map->width) {
        game->x = 0;
    } else if (game->x < 0) {
        game->x = map->width - 1;
    } else if (game->y >= map->height) {
        game->y = 0;
    } else if (game->y < 0) {
        game->y = map->height - 1;
    }

    switch (map->map[game->y][game->x]) {
        case EMPTY:
            map->map[game->y][game->x] = PLAYER;
            break;
        case WALL:
            map->map[lastY][lastX] = PLAYER;
            // TODO : player must plant a bomb to break the wall
            break;
        case UNBREAKABLE_WALL:
            map->map[lastY][lastX] = PLAYER;
            // unbreakable wall
            break;
        case BOMB:
            map->map[game->y][game->x] = PLAYER;
            // TODO : player is on a bomb so the player must die
            break;
        case ITEM:
            map->map[game->y][game->x] = PLAYER;
            // TODO : Remove the item from the map and add it to the inventory
            break;
        default:
            break;
    }

    // save the last position
    if (map->map[game->y][game->x] == PLAYER) {
        lastX = game->x;
        lastY = game->y;
    }else{
        game->x = lastX;
        game->y = lastY;
    }
}


