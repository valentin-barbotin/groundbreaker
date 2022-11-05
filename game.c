#include "config.h"
#include "loop.h"
#include "game.h"
#include "map.h"
#include "moves.h"
#include "utils.h"
#include "sound.h"
#include "player.h"
#include "server.h"
#include "assets.h"

#define DEBUG

t_sound *walk, *wall, *unbreakableWall, *bomb, *item;


/**
 * @brief Place the player in his cell instead of a wall..
 * 
 */
void    spawnPlayer(int x, int y, t_player *player) {
    const t_game    *game;
    const t_map     *map;
    unsigned int    cellSizeX;
    unsigned int    cellSizeY;

    game = getGame();
    map = game->map;


    player->xCell = x;
    player->yCell = y;

    cellSizeX = gameConfig->video.width / map->width;
    cellSizeY = gameConfig->video.height / map->height;

    player->x = (player->xCell * cellSizeX) + (cellSizeX / 2);
    player->y = (player->yCell * cellSizeY) + (cellSizeY / 2);
}

bool    inGame() {
    return (g_currentState >= GAME_PLAY_PLAYING);
}

void setPath() {
    walk->file = SOUND_WALK;
    wall->file = SOUND_WALL;
    unbreakableWall->file = SOUND_UNBREAKABLE_WALL;
    //bomb->file = changemeBombPath;
    //item->file = changemeItemPath;
    //life->file = changemeLifePath;
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

        //TODO
        walk = malloc(sizeof(t_sound));
        wall = malloc(sizeof(t_sound));
        unbreakableWall = malloc(sizeof(t_sound));
        //bomb = malloc(sizeof(t_sound));
        //item = malloc(sizeof(t_sound));
        setPath();
        ///////////

        //TMP max players
        //TODO:
        int nb = 4;
        game->players = malloc(sizeof(t_player *) * nb);
        if (game->players == NULL) {
            #ifdef DEBUG
                fprintf(stderr, "Error allocating memory for game->players");
            #endif
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
            exit(1);
        }

        for (int i = 0; i < nb; i++) {
            game->players[i] = initPlayer();
        }

        strcpy(game->players[0]->name, g_username);
        game->nbPlayers = 1;

        // setPath for sound
        setPath();
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
    char            buffer[100];

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

    if (!isAlive()) {
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

            if (Mix_PlayingMusic() == 1) {
                if (!stopSound(wall)) {
                    #ifdef DEBUG
                            fprintf(stderr, "Error: can't stop sound\n");
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                    exit(1);
                }
            } else if (Mix_PlayingMusic() == 0) {
                initMusic(wall);
                if (wall->music == NULL) {
                    #ifdef DEBUG
                            fprintf(stderr, "Error loading sound file: %s\n", Mix_GetError());
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                    exit(1);
                }
                playSound(wall);
            } else if (Mix_PlayingMusic() == 1) {
                if (!stopSound(wall)) {
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
            player->x -= player->vx;
            player->y -= player->vy;

            if (Mix_PlayingMusic() == 1) { stopSound(unbreakableWall); }

            if (Mix_PlayingMusic() == 0) {
                if (walk == NULL) {
                #ifdef DEBUG
                        fprintf(stderr, "Error: malloc failed in movePlayer()\n");
                #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                    exit(1);
                }
                initMusic(unbreakableWall);
                if (unbreakableWall->music == NULL) {
                    #ifdef DEBUG
                                fprintf(stderr, "Error loading sound in moveplayer(): %s\n", Mix_GetError());
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                    exit(1);
                }

                // TODO: Timer to not play the sound every frame
                playSound(unbreakableWall);

            } else if (Mix_PlayingMusic() == 1) {
                if (!stopSound(unbreakableWall)) {
                    #ifdef DEBUG
                        fprintf(stderr, "Error: stopSound() failed in movePlayer()\n");
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                    exit(1);
                }
            }
            break;
        case BOMB:
            // if the player is on a bomb kill him if he is not invicible
            if (player->godMode == 1) {
                break;
            } else if (player->passThroughBomb) {
                // if the player is on a bomb and he has the passThroughBomb powerup so he jumps over the bomb
                break;
            }else if(player->bombKick) {
                if (map->map[player->yCell + player->vy][player->xCell + player->vx] == EMPTY) {
                    // move the bomb to the next cell
                    map->map[player->yCell + player->vy][player->xCell + player->vx] = BOMB;
                    map->map[player->yCell][player->xCell] = EMPTY;
                    // move the player to the next cell
                    player->x += player->vx;
                    player->y += player->vy;
                }
                break;
            } else if (player->canSurviveExplosion) {
                player->canSurviveExplosion = 0;
                player->inventory[ITEM_HEART]->quantity = -1;
                break;
            } else {
                player->x -= player->vx;
                player->y -= player->vy;
                player->health -= 100;
                break;
            }

        case ITEM:
            player->inventory[map->map[player->yCell][player->xCell]]->quantity++;
            map->map[player->yCell][player->xCell] = EMPTY;
            break;
        default:
            if(isMoving(player)) {
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
    player->direction = getDirection(player);

    sendPos();
}

void    posToGridN(int x, int y, int *cellX, int *cellY) {
    unsigned int   cellSizeX;
    unsigned int   cellSizeY;
    t_game         *game;

    game = getGame();

    // can't divide by 0
    if (x) {
        cellSizeX = gameConfig->video.width / game->map->width; // ex: 166 (width of 1000 divided by 6 (nb of cols))
        // avoid xCell to be equal to map->width (segfault, col 6 doesn't exist for a map of 6 cols)
        if (x != gameConfig->video.width) {
            *cellX = (x / cellSizeX);
        }
    }

    if (y) {
        cellSizeY = gameConfig->video.height / game->map->height;
        if (y != gameConfig->video.height) {
            *cellY = (y / cellSizeY);
        }
    }

    // find the nearest multiple of cellSize(s)
    // ex: 768 / 166 = 4.6 => 4
}

void explodeBomb(int xCell, int yCell) {
    t_player    *player;
    t_map* map;

    player = getPlayer();
    map = getGame()->map;
    GETCELL(yCell, xCell) = EMPTY;


    // posToGrid but with 2 int pointers
    posToGrid(player->x, player->y);

    // pour chaque direction (UP, DOWN, LEFT, RIGHT)
    for (int k = 0; k < NB_DIRECTIONS; k++) {
        searchDirectionMap(k);
    }
}


void searchDirectionMap(t_directionMap direction) {
    t_game   *game;
    t_player *player;
    const    t_map     *map;
    game = getGame();
    map = game->map;

    if (map == NULL) {
        #ifdef DEBUG
                fprintf(stderr, "Error: map is NULL in searchDirectionMap()\n");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
        exit(1);
    }

    // ⚠ Temporaire ⚠
    // en attendant que la structure Joueur soit implémentée
    int SCOPE = 2;

    int cell, i;
    int cellX, cellY;
    switch(direction) {
        case UP:
            cellX = player->xCell;
            cellY = player->yCell - i;
            break;
        case DOWN:
            cellX = player->xCell;
            cellY = player->yCell + i;
            break;
        case LEFT:
            cellX = player->xCell - i;
            cellY = player->yCell;
            break;
        case RIGHT:
            cellX = player->xCell + i;
            cellY = player->yCell;
            break;
        default:
            break;
    }

    for (i = 1; i <= SCOPE; i++) {
        cell = GETCELL(cellY, cellX);
        switch (cell) {
            case WALL:
                cell = GRAVEL;
                break;
            case UNBREAKABLE_WALL:
                // on arrête la bombe dans sa course
                i = SCOPE;
                break;
            case PLAYER:
                if(player->godMode == 1) {
                    if (player->inventory[ITEM_HEART]->isActive) {
                        player->inventory[ITEM_HEART]->isActive = false;
                        player->godMode = 0;
                    }
                }
                // TODO : player is on a bomb so the player must die
                //player->health = 0;
                break;
            case BOMB:
                explodeBomb(cellX, cellY);
                break;
            default:
                break;
        }
    }
}
