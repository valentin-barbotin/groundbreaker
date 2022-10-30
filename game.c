#include "config.h"
#include "loop.h"
#include "game.h"
#include "map.h"
#include "moves.h"
#include "utils.h"
#include "player.h"

#define DEBUG true

/**
 * @brief Place the player in his cell instead of a wall..
 * 
 */
void    spawnPlayer() {
    t_game          *game;
    t_player        *player;
    const t_map     *map;
    unsigned int    cellSizeX;
    unsigned int    cellSizeY;

    game = getGame();
    player = getPlayer();
    map = game->map;

    player->xCell = 1;
    player->yCell = 1;

    cellSizeX = gameConfig->video.width / map->width;
    cellSizeY = gameConfig->video.height / map->height;

    player->x = (player->xCell * cellSizeX) + (cellSizeX / 2);
    player->y = (player->yCell * cellSizeY) + (cellSizeY / 2);
}

bool    inGame() {
    return (g_currentState >= GAME_PLAY_PLAYING);
}

t_game* getGame() {
    static t_game*  game;

    if (game == NULL) {
        game = malloc(sizeof(t_game));
        if (game == NULL) {
            #ifdef DEBUG
                fprintf(stderr, "Error allocating memory for game");
            #endif
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
            exit(1);
        }

        //TMP max players
        //TODO:
        int nb = 4;
        game->players = malloc(sizeof(t_player) * nb);
        

        game->nbPlayers = 1;
        //put the local player in the game
        game->players[0] = getPlayer();
    }
    return game;
}

void    posToGrid() {
    unsigned int   cellSizeX;
    unsigned int   cellSizeY;
    t_game         *game;
    t_player       *player;

    game = getGame();
    player = getPlayer();

    // can't divide by 0
    if (player->x) {
        cellSizeX = gameConfig->video.width / game->map->width; // ex: 166 (width of 1000 divided by 6 (nb of cols))
        // avoid xCell to be equal to map->width (segfault, col 6 doesn't exist for a map of 6 cols)
        if (player->x != gameConfig->video.width) {
            player->xCell = (player->x / cellSizeX);
        }
    }

    if (player->y) {
        cellSizeY = gameConfig->video.height / game->map->height;
        if (player->y != gameConfig->video.height) {
            player->yCell = (player->y / cellSizeY);
        }
    }

    // find the nearest multiple of cellSize(s)
    // ex: 768 / 166 = 4.6 => 4
}


void    movePlayer() {
    t_game          *game;
    t_player        *player;
    const t_map     *map;

    game = getGame();
    player = getPlayer();
    map = game->map;

    if (map == NULL) {
        #ifdef DEBUG
            fprintf(stderr, "Error: map is NULL in movePlayer()\n");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
        exit(1);
    }

    checkBorders();

    // make the move
    player->x += player->vx;
    player->y += player->vy;

    // clamp the player position to the map
    if (player->x < 0) {
        player->x = 0;
    }
    if (player->y < 0) {
        player->y = 0;
    }

    if (player->x > gameConfig->video.width) {
        player->x = gameConfig->video.width;
    }
    if (player->y > gameConfig->video.height) {
        player->y = gameConfig->video.height;
    }

    // update the grid position
    posToGrid();

    //if the player is moving out of the map then we move him at the other side if possible
    if (player->x >= (gameConfig->video.width - PLAYER_WIDTH/2)) {
        if (map->map[player->yCell][0] == EMPTY) {
            // move the player to the other side
            player->x = 0;
        }
    } else if (player->x == 0 && player->vx < 0) {
        // if the player is on the left side of the map and he is moving left then we move him to the right side of the map
        
        // check if the player can be placed on the next cell
        if (map->map[player->yCell][game->map->width - 1] == EMPTY) {
            // move the player to the other side
            player->x = gameConfig->video.width - PLAYER_WIDTH/2;
        }
    }

    if (player->y >= (gameConfig->video.height - PLAYER_HEIGHT/2)) {
        if (map->map[0][player->xCell] == EMPTY) {
            // move the player to the other side
            player->y = 0;
        }
    } else if (player->y == 0 && player->vy < 0) {
        // if the player is on the top side of the map and he is moving up then we move him to the bottom side of the map
        
        // check if the player can be placed on the next cell
        if (map->map[game->map->height - 1][player->xCell] == EMPTY) {
            // move the player to the other side
            player->y = gameConfig->video.height - PLAYER_HEIGHT/2;
        }
    }


    switch (map->map[player->yCell][player->xCell]) {
        case WALL:
            // if the player is on a wall then we move him back to the old position
            player->x -= player->vx;
            player->y -= player->vy;
            break;
        case UNBREAKABLE_WALL:
            // if the player is on a wall then we move him back to the old position
            player->x -= player->vx;
            player->y -= player->vy;
            break;
        // case BOMB:
        //     map->map[game->yCell][game->xCell] = PLAYER;
        //     // TODO : player is on a bomb so the player must die
        //     break;
        // case ITEM:
        //     map->map[game->yCell][game->xCell] = PLAYER;
        //     // TODO : Remove the item from the map and add it to the inventory
        //     break;
        default:
            break;
    }
    posToGrid();
}


