#include "config.h"
#include "loop.h"
#include "game.h"
#include "map.h"
#include "moves.h"
#include "utils.h"
#include "sound.h"

t_sound *walk, *wall, *unbreakableWall, *bomb, *item;


/**
 * @brief Place the player in his cell instead of a wall..
 *
 */
void    spawnPlayer() {
    t_game          *game;
    const t_map     *map;
    unsigned int    cellSizeX;
    unsigned int    cellSizeY;

    game = getGame();
    map = game->map;

    game->xCell = 1;
    game->yCell = 1;

    cellSizeX = gameConfig->video.width / map->width;
    cellSizeY = gameConfig->video.height / map->height;

    game->x = (game->xCell * cellSizeX) + (cellSizeX / 2);
    game->y = (game->yCell * cellSizeY) + (cellSizeY / 2);
}

bool    inGame() {
    return (g_currentState >= GAME_PLAY_PLAYING);
}

void setPath() {
    walk->file = "./assets/sound/walk.ogg";
    wall->file = "./assets/sound/wall.ogg";
    unbreakableWall->file = "./assets/sound/unbreakable_wall.ogg";
    //bomb->file = changemeBombPath;
    //item->file = changemeItemPath;
    //life->file = changemeLifePath;
}

t_game* getGame() {
    static t_game* game;

    if (game == NULL) {
        game = malloc(sizeof(t_game));
        if (game == NULL) {
            #ifdef DEBUG
                fprintf(stderr, "Error allocating memory for game");
            #endif
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
            exit(1);
        }


        walk = malloc(sizeof(t_sound));
        wall = malloc(sizeof(t_sound));
        unbreakableWall = malloc(sizeof(t_sound));
        //bomb = malloc(sizeof(t_sound));
        //item = malloc(sizeof(t_sound));

        setPath();

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
    unsigned int   cellSizeX;
    unsigned int   cellSizeY;
    t_game         *game;

    game = getGame();

    // can't divide by 0
    if (game->x) {
        cellSizeX = gameConfig->video.width / game->map->width; // ex: 166 (width of 1000 divided by 6 (nb of cols))
        // avoid xCell to be equal to map->width (segfault, col 6 doesn't exist for a map of 6 cols)
        if (game->x != gameConfig->video.width) {
            game->xCell = (game->x / cellSizeX);
        }
    }

    if (game->y) {
        cellSizeY = gameConfig->video.height / game->map->height;
        if (game->y != gameConfig->video.height) {
            game->yCell = (game->y / cellSizeY);
        }
    }

    // find the nearest multiple of cellSize(s)
    // ex: 768 / 166 = 4.6 => 4
}


void    movePlayer() {
    t_game *game;
    const t_map     *map;

    game = getGame();
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
    game->x += game->vx;
    game->y += game->vy;

    // clamp the player position to the map
    if (game->x < 0) {
        game->x = 0;
    }
    if (game->y < 0) {
        game->y = 0;
    }

    if (game->x > gameConfig->video.width) {
        game->x = gameConfig->video.width;
    }
    if (game->y > gameConfig->video.height) {
        game->y = gameConfig->video.height;
    }

    // update the grid position
    posToGrid();

    //if the player is moving out of the map then we move him at the other side if possible
    if (game->x >= (gameConfig->video.width - PLAYER_WIDTH/2)) {
        if (map->map[game->yCell][0] == EMPTY) {
            // move the player to the other side
            game->x = 0;
        }
    } else if (game->x == 0 && game->vx < 0) {
        // if the player is on the left side of the map and he is moving left then we move him to the right side of the map

        // check if the player can be placed on the next cell
        if (map->map[game->yCell][game->map->width - 1] == EMPTY) {
            // move the player to the other side
            game->x = gameConfig->video.width - PLAYER_WIDTH/2;
        }
    }

    if (game->y >= (gameConfig->video.height - PLAYER_HEIGHT/2)) {
        if (map->map[0][game->xCell] == EMPTY) {
            // move the player to the other side
            game->y = 0;
        }
    } else if (game->y == 0 && game->vy < 0) {
        // if the player is on the top side of the map and he is moving up then we move him to the bottom side of the map

        // check if the player can be placed on the next cell
        if (map->map[game->map->height - 1][game->xCell] == EMPTY) {
            // move the player to the other side
            game->y = gameConfig->video.height - PLAYER_HEIGHT/2;
        }
    }

    switch (map->map[game->yCell][game->xCell]) {
        case WALL:
            game->x -= game->vx;
            game->y -= game->vy;

            if(Mix_PlayingMusic() == 1) {
                if(!stopSound(wall)) {
                    #ifdef DEBUG
                        fprintf(stderr, "Error: can't stop sound\n");
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                    exit(1);
                }
            }else if(Mix_PlayingMusic() == 0) {
                initMusic(wall);
                if(wall->music == NULL) {
                    #ifdef DEBUG
                        fprintf(stderr, "Error loading sound file: %s\n", Mix_GetError());
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                    exit(1);
                }
                playSound(wall);
            }else if(Mix_PlayingMusic() == 1) {
                if(!stopSound(wall)) {
                    #ifdef DEBUG
                        fprintf(stderr, "Error: can't stop sound\n");
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                    exit(1);
                }
            }
            break;
        case UNBREAKABLE_WALL:
            // if the player is on a wall then we move him back to the old position
            game->x -= game->vx;
            game->y -= game->vy;

            if(Mix_PlayingMusic() == 1) { stopSound(unbreakableWall); }

            if(Mix_PlayingMusic() == 0) {
                if (walk == NULL) {
                    #ifdef DEBUG
                        fprintf(stderr, "Error: malloc failed in movePlayer()\n");
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                    exit(1);
                }
                initMusic(unbreakableWall);
                if(unbreakableWall->music == NULL) {
                    #ifdef DEBUG
                        fprintf(stderr, "Error loading sound in moveplayer(): %s\n", Mix_GetError());
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                    exit(1);
                }

                // TODO: Timer to not play the sound every frame
                playSound(unbreakableWall);

            }else if(Mix_PlayingMusic() == 1) {
                if (!stopSound(unbreakableWall)) {
                    #ifdef DEBUG
                        fprintf(stderr, "Error: stopSound() failed in movePlayer()\n");
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                    exit(1);
                }
            }
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
            if(isMoving()) {
                if (Mix_PlayingMusic() == 0) {
                    initMusic(walk);
                    if(walk->music == NULL) {
                        #ifdef DEBUG
                            fprintf(stderr, "Error loading sound : %s\n", Mix_GetError());
                        #endif
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                        exit(1);
                    }
                    playSoundLoop(walk);
                }
            }else{
                if(Mix_PlayingMusic() == 1) {
                    if(!stopSound(walk)) {
                        #ifdef DEBUG
                            fprintf(stderr, "Error: stopSound() failed in movePlayer()\n");
                        #endif
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                        exit(1);
                    }
                }
            }
            break;
    }
    posToGrid();
}


