#include "config.h"
#include "loop.h"
#include "game.h"
#include "map.h"
#include "moves.h"

bool    inGame() {
    return (g_currentState >= GAME_PLAY_PLAYING);
}

t_game* getGame() {
    static t_game* game;

    if (game == NULL) {
        game = malloc(sizeof(t_game));
        if (game == NULL) {
            fprintf(stderr, "Error: malloc failed in getGame()\n");
            exit(1);
        }
        game->x = 0;
        game->y = 0;

        game->vx = 0;
        game->vy = 0;

        game->xCell = 0;
        game->yCell = 0;
    }
    return game;
}

void    posToGrid() {
    short   cellSizeX;
    short   cellSizeY;

    // can't divide by 0
    if (getGame()->x) {
        cellSizeX = gameConfig->video.width / getGame()->map->width; // ex: 166 (width of 1000 divided by 6 (nb of cols))
        // avoid xCell to be equal to map->width (segfault, col 6 doesn't exist for a map of 6 cols)
        if (getGame()->x != gameConfig->video.width) {
            getGame()->xCell = (getGame()->x / cellSizeX);
        }
    }

    if (getGame()->y) {
        cellSizeY = gameConfig->video.height / getGame()->map->height;
        if (getGame()->y != gameConfig->video.height) {
            getGame()->yCell = (getGame()->y / cellSizeY);
        }
    }

    // find the nearest multiple of cellSize(s)
    // ex: 768 / 166 = 4.6 => 4
}

void    makeOldPosEmpty(short x, short y) {
    t_map* map;

    map = getGame()->map;
    map->map[y][x] = EMPTY;
}

bool  thereIsA(short x, short y, t_type type) {
    t_map* map;

    map = getGame()->map;
    switch (type) {
        case BOMB:
            return (map->map[y][x] == BOMB);
        case WALL:
            return (map->map[y][x] == WALL);
        case UNBREAKABLE_WALL:
            return (map->map[y][x] == UNBREAKABLE_WALL);
        case PLAYER:
            return (map->map[y][x] == PLAYER);
        case EMPTY:
            return (map->map[y][x] == EMPTY);
        default:
            return false;
    }
}


void explodeBomb(short x, short y) {
    t_map* map;

    map = getGame()->map;
    map->map[y][x] = EMPTY;

    // ⚠ Temporaire ⚠
    // en attendant que la structure Joueur soit faite
    int SCOPE = map->scopeBomb = 2;

    for (int i = 1; i <= SCOPE; i++) {
        // up
        if (y - i >= 0) {
            if (thereIsA(x, y - i, WALL)) {
                makeOldPosEmpty(x, y - i);
                break;
            } else if (thereIsA(x, y - i, BOMB)) {
                explodeBomb(x, y - i);
                break;
            } else if (thereIsA(x, y - i, PLAYER)) {
                // kill player
                break;
            }else if (thereIsA(x, y - i, UNBREAKABLE_WALL)) {
                break;
            } else {
                makeOldPosEmpty(x, y - i);
            }
        }

        // down
        if (y + i < map->height) {
            if (thereIsA(x, y + i, WALL)) {
                makeOldPosEmpty(x, y + i);
                break;
            } else if (thereIsA(x, y + i, BOMB)) {
                explodeBomb(x, y + i);
                break;
            } else if (thereIsA(x, y + i, PLAYER)) {
                // kill player
                break;
            }else if (thereIsA(x, y + i, UNBREAKABLE_WALL)) {
                break;
            } else {
                makeOldPosEmpty(x, y + i);
            }
        }

        // left
        if (x - i >= 0) {
            if (thereIsA(x - i, y, WALL)) {
                makeOldPosEmpty(x - i, y);
                break;
            } else if (thereIsA(x - i, y, BOMB)) {
                explodeBomb(x - i, y);
                break;
            } else if (thereIsA(x - i, y, PLAYER)) {
                // kill player
                break;
            }else if (thereIsA(x - i, y, UNBREAKABLE_WALL)) {
                break;
            } else {
                makeOldPosEmpty(x - i, y);
            }
        }

        // right
        if (x + i < map->width) {
            if (thereIsA(x + i, y, WALL)) {
                makeOldPosEmpty(x + i, y);
                break;
            } else if (thereIsA(x + i, y, BOMB)) {
                explodeBomb(x + i, y);
                break;
            } else if (thereIsA(x + i, y, PLAYER)) {
                // kill player
                break;
            }else if (thereIsA(x + i, y, UNBREAKABLE_WALL)) {
                break;
            } else {
                makeOldPosEmpty(x + i, y);
            }
        }
    }
}

void    movePlayer() {
    t_game* game;
    t_map* map;
    short   oldCellX;
    short   oldCellY;

    // save the position on the grid before moving
    oldCellX = getGame()->xCell;
    oldCellY = getGame()->yCell;

    game = getGame();
    map = game->map;

    if (map == NULL) {
        fprintf(stderr, "Error: map is NULL in movePlayer()\n");
        exit(1);
    }

    checkBorders();

    // make the move
    game->x += game->vx;
    game->y += game->vy;

    // clamp the player position to the map
    if (game->x < 0) {
        game->x = 0;
    }
    if (game->y < 0) {
        game->y = 0;
    }

    if (getGame()->x + 10 > gameConfig->video.width) {
        game->x = gameConfig->video.width;
    }
    if (getGame()->y + 10 > gameConfig->video.height) {
        game->y = gameConfig->video.height;
    }
    posToGrid();

    // printf("x = %d, y = %d , vx = %d, vy = %d, cellPos: (cx:%hu cy:%hu)\n", getGame()->x, getGame()->y, getGame()->vx, getGame()->vy, getGame()->xCell, getGame()->yCell);

    // if (game->x == getGame()->lastX && game->y == getGame()->lastY) return;

    // the old position is now empty
    // map->map[getGame()->lastY][getGame()->lastX] = EMPTY;

    // if player want to go out of the map then we move him at the other side

    // checkborders();
    // if (game->x >= map->width) {
    //     game->x = 0;
    // } else if (game->x < 0) {
    //     game->x = map->width - 1;
    // } else if (game->y >= map->height) {
    //     game->y = 0;
    // } else if (game->y < 0) {
    //     game->y = map->height - 1;
    // }

    switch (map->map[getGame()->yCell][getGame()->xCell]) {
        case EMPTY:
            map->map[getGame()->yCell][getGame()->xCell] = PLAYER;
            break;
        case WALL:
            map->map[getGame()->yCell][getGame()->xCell] = PLAYER;
            // TODO : player must plant a bomb to break the wall
            break;
        case UNBREAKABLE_WALL:
            map->map[getGame()->yCell][getGame()->xCell] = PLAYER;
            // unbreakable wall
            break;
        case BOMB:
            map->map[getGame()->yCell][getGame()->xCell] = PLAYER;
            // TODO : player is on a bomb so the player must die
            break;
        case ITEM:
            map->map[getGame()->yCell][getGame()->xCell] = PLAYER;
            // TODO : Remove the item from the map and add it to the inventory
            break;
        default:
            break;
    }

    if ((oldCellX != getGame()->xCell) || (oldCellY != getGame()->yCell)) {
        makeOldPosEmpty(oldCellX, oldCellY);
    }

    // // save the last position
    // if (map->map[game->y][game->x] == PLAYER) {
    //     getGame()->lastX = game->x;
    //     getGame()->lastY = game->y;
    // } else {
    //     game->x = getGame()->lastX;
    //     game->y = getGame()->lastY;
    // }
}


