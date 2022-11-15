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
#include "ia.h"

#define DEBUG

t_sound *walk, *wall, *unbreakableWall, *bomb, *item;


void    injectItems(const t_map *map) {
    for (size_t i = 1; i < map->height - 1; i++)
    {
        for (size_t j = 1; j < map->width - 1; j++)
        {
            if (map->map[i][j] == EMPTY && rand() % 100 < 30)
            {
                map->map[i][j] = (rand() % 100 < 50) ? LOOT : WALL;
            }
        }
    }
}

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

    map->map[player->yCell][player->xCell] = EMPTY;
}

bool    inGame() {
    return (g_currentState >= GAME_PLAY_PLAYING);
}

bool    isGamePaused() {
    return (g_currentState == GAME_PLAY_PAUSE);
}

void    resumeGame() {
    g_currentState = GAME_PLAY_PLAYING;
}

void    pauseGame() {
    g_currentState = GAME_PLAY_PAUSE;
    g_currentMenu = &menuPause;
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
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
            exit(1);
        }

        game->map = NULL;

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
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
            exit(1);
        }

        for (int i = 0; i < nb; i++) {
            game->players[i] = initPlayer();
        }

        game->nbPlayers = 1;

        // setPath for sound
        setPath();
    }
    return game;
}

void    posToGrid(t_player *player) {
    unsigned int    cellSizeX;
    unsigned int    cellSizeY;
    const t_game    *game;

    game = getGame();

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


void    movePlayer(t_player *player) {
    t_game          *game;
    const t_map     *map;
    char            buffer[100];
    bool            stopped;

    game = getGame();
    map = game->map;

    if (map == NULL) {
        #ifdef DEBUG
            fprintf(stderr, "Error: map is NULL in movePlayer()\n");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Game error", g_window);
        exit(1);
    }

    if (!isAlive(player)) {
        exit(1);
    }

    // TODO: if the timer is finished, the bomb explodes
    // if(getTimeLeft(timerBomb) == 0) {
    //     explodeBomb(player->xCell, player->yCell);
    // }

    // TODO: if the timer is finished, the item disappears
    // if(getTimeLeft(timerInvincible) == 0) {
    //     player->invincible = false;
    // }

    checkBorders();

    stopped = false;

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
    posToGrid(player);

    //if the player is moving out of the map then we move him at the other side if possible
    if (player->x >= (gameConfig->video.width - PLAYER_WIDTH/2)) {
        if(GETCELL(0, player->yCell) == EMPTY) {
            // move the player to the other side
            player->x = 0;
        }
    } else if (player->x == 0 && player->vx < 0) {
        // if the player is on the left side of the map and he is moving left then we move him to the right side of the map
        
        // check if the player can be placed on the next cell
        if (GETCELL(game->map->width-1, player->yCell) == EMPTY) {
            // move the player to the other side
            player->x = gameConfig->video.width - PLAYER_WIDTH/2;
        }
    }

    if (player->y >= (gameConfig->video.height - PLAYER_HEIGHT/2)) {
        if (GETCELL(player->xCell, 0) == EMPTY) {
            // move the player to the other side
            player->y = 0;
        }
    } else if (player->y == 0 && player->vy < 0) {
        // if the player is on the top side of the map and he is moving up then we move him to the bottom side of the map
        
        // check if the player can be placed on the next cell
        if (GETCELL(player->xCell, game->map->height-1) == EMPTY) {
            // move the player to the other side
            player->y = gameConfig->video.height - PLAYER_HEIGHT/2;
        }
    }


    switch (GETCELL(player->xCell, player->yCell)) {
        case WALL:
            // if the player is on a wall then we move him back to the old position
            player->x -= player->vx;
            player->y -= player->vy;
            stopped = true;

            if (Mix_PlayingMusic() == 1) {
                if (!stopSound(wall)) {
                    #ifdef DEBUG
                            fprintf(stderr, "Error: can't stop sound\n");
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Can't stop sound", g_window);
                    exit(1);
                }
            } else if (Mix_PlayingMusic() == 0) {
                initMusic(wall);
                if (wall->music == NULL) {
                    #ifdef DEBUG
                            fprintf(stderr, "Error loading sound file: %s\n", Mix_GetError());
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", Mix_GetError(), g_window);
                    exit(1);
                }
                playSound(wall);
            } else if (Mix_PlayingMusic() == 1) {
                if (!stopSound(wall)) {
                    #ifdef DEBUG
                            fprintf(stderr, "Error: can't stop sound\n");
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Can't stop sound", g_window);
                    exit(1);
                }
            }

            break;
        case LOOT:
            player->x -= player->vx;
            player->y -= player->vy;
            stopped = true;
            break;
        case UNBREAKABLE_WALL:
            // if the player is on a wall then we move him back to the old position
            player->x -= player->vx;
            player->y -= player->vy;
            stopped = true;

            if (Mix_PlayingMusic() == 1) { stopSound(unbreakableWall); }

            if (Mix_PlayingMusic() == 0) {
                if (walk == NULL) {
                #ifdef DEBUG
                        fprintf(stderr, "Error: malloc failed in movePlayer()\n");
                #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
                    exit(1);
                }
                initMusic(unbreakableWall);
                if (unbreakableWall->music == NULL) {
                    #ifdef DEBUG
                                fprintf(stderr, "Error loading sound in moveplayer(): %s\n", Mix_GetError());
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", Mix_GetError(), g_window);
                    exit(1);
                }

                // TODO: Timer to not play the sound every frame
                playSound(unbreakableWall);

            } else if (Mix_PlayingMusic() == 1) {
                if (!stopSound(unbreakableWall)) {
                    #ifdef DEBUG
                        fprintf(stderr, "Error: stopSound() failed in movePlayer()\n");
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Can't stop sound", g_window);
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
                searchDirectionMap(getDirection(player), 2);
                break;
            }else if(player->bombKick) {
                // search in direction of the player and kick the bomb
                searchDirectionMap(player->direction, 999);
                break;
            } else if (player->canSurviveExplosion) {
                player->canSurviveExplosion = false;
                player->inventory[ITEM_HEART]->quantity = -1;
                break;
            } else {
                player->x -= player->vx;
                player->y -= player->vy;
                player->health -= 100;
                break;
            }

        case ITEM:
            player->inventory[GETCELL(player->xCell, player->yCell)]->quantity++;
            GETCELL(player->xCell, player->yCell) = EMPTY;
            break;
        default:
            if(isMoving(player)) {
                if (Mix_PlayingMusic() == 0) {
                    initMusic(walk);
                    if(walk->music == NULL) {
                        #ifdef DEBUG
                            fprintf(stderr, "Error loading sound : %s\n", Mix_GetError());
                        #endif
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", Mix_GetError(), g_window);
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
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Can't stop sound", g_window);
                        exit(1);
                    }
                }
            }
            break;
    }
    posToGrid(player);

    if (player->isBot && stopped) {
        // inverse the direction of the bot
        switch (player->direction)
        {
            case DIR_LEFT:
                player->vx = BOT_SPEED;
                break;
            case DIR_RIGHT:
                player->vx = -BOT_SPEED;
                break;

            case DIR_UP:
                player->vy = BOT_SPEED;
                break;
            case DIR_DOWN:
                player->vy = -BOT_SPEED;
                break;
            
            default:
                break;
        }
    }

    player->direction = getDirection(player);

    if (!player->isBot) {
        sendPos();
    }
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
    const t_map*      map;
    t_player          *player;

    player = getPlayer();
    map = getGame()->map;
    GETCELL(yCell, xCell) = EMPTY;

    // pour chaque direction (UP, DOWN, LEFT, RIGHT)
    searchDirectionMap(DIR_UP, player->scope);
    searchDirectionMap(DIR_DOWN, player->scope);
    searchDirectionMap(DIR_LEFT, player->scope);
    searchDirectionMap(DIR_RIGHT, player->scope);
}

void    handleMouseButtonUpPlaying(const SDL_Event *event) {
    for (unsigned short i = 0; i < g_currentMenu->nbButtons; ++i) {
        // get click position
        int xStart = g_buttonsLocation[i].x;
        int yStart = g_buttonsLocation[i].y;

        SDL_Point click = { event->button.x, event->button.y };
        SDL_Rect button = { xStart, yStart, g_buttonsLocation[i].w, g_buttonsLocation[i].h };

        if (SDL_PointInRect(&click, &button))
        {
            makeSelection(i);
        }
    }
}

void searchDirectionMap(t_direction direction, int scope) {
    t_game   *game;
    t_player *player;
    const    t_map     *map;
    game = getGame();
    player = getPlayer();
    map = game->map;
    int cell, i, cellX, cellY;

    if (map == NULL) {
        #ifdef DEBUG
                fprintf(stderr, "Error: map is NULL in searchDirectionMap()\n");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Map error", g_window);
        exit(1);
    }

    switch(direction) {
        case DIR_UP:
            cellX = player->xCell;
            cellY = player->yCell - i;
            break;
        case DIR_DOWN:
            cellX = player->xCell;
            cellY = player->yCell + i;
            break;
        case DIR_LEFT:
            cellX = player->xCell - i;
            cellY = player->yCell;
            break;
        case DIR_RIGHT:
            cellX = player->xCell + i;
            cellY = player->yCell;
            break;
        default:
            break;
    }

    for (i = 1; i <= scope; i++) {
        cell = GETCELL(cellY, cellX);
        switch (cell) {
            case WALL:
                cell = GRAVEL;
                if(player->bombKick) {
                    // on met la bombe sur la case précédente
                    GETCELL(cellY - player->vy, cellX - player->vx) = BOMB;
                    return;
                }
                break;
            case UNBREAKABLE_WALL:
                // on arrête la bombe dans sa course
                if(player->bombKick) {
                    // on met la bombe sur la case précédente
                    GETCELL(cellY - player->vy, cellX - player->vx) = BOMB;
                    return;
                }
                i = scope;
                break;
            case PLAYER:
                // if the player is on a bomb kill him if he is not invicible
                if (player->godMode == 1) {
                    break;
                } else if (player->passThroughBomb) {
                    // if the player is on a bomb and he has the passThroughBomb powerup so he jumps over the bomb
                    break;
                }else if(player->bombKick) {
                    if (GETCELL(player->xCell + player->vx, player->yCell + player->vy) == EMPTY) {
                        // move the bomb to the next cell
                        GETCELL(player->xCell + player->vx, player->yCell + player->vy) = BOMB;
                        GETCELL(player->xCell, player->yCell) = EMPTY;
                        // move the player to the next cell
                        player->x += player->vx;
                        player->y += player->vy;
                    }
                    break;
                } else if (player->canSurviveExplosion) {
                    player->canSurviveExplosion = false;
                    player->inventory[ITEM_HEART]->quantity = -1;
                    break;
                } else {
                    player->x -= player->vx;
                    player->y -= player->vy;
                    player->health -= 100;
                    break;
                }
            case BOMB:
                if(player->passThroughBomb) {
                    player->x += player->vx;
                    break;
                }

                explodeBomb(cellX, cellY);
                break;
            default:
                break;
        }
    }
}


void    launchGame() {
    t_game            *game;
    short             index;
    t_map             *tmp[10] = {0};

    game = getGame();

    if (g_serverRunning && (game->nbPlayers < g_lobby->players)) {
        #ifdef DEBUG
            puts("Not enough players");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lobby", "Not enough player", g_window);
        return;
    }

    index = -1;
    for (size_t i = 0; i < g_nbMap; i++)
    {
        if (game->maps[i].selected) {
            if (index == -1) index = 0; //used to check if we have at least one map selected
            tmp[index++] = &game->maps[i];
        }
    }
    if (index == -1) {
        printf("No map selected\n");
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lobby", "You must select a map", g_window);
        return;
    }

    index = rand() % index; //pick a random map between the selected ones
    printf("index = %d\n", index);
    game->map = tmp[index];

    injectItems(game->map);

    spawnPlayer(1, 1, getPlayer());

    if (g_serverRunning) {

        if (game->nbPlayers == game->map->players) {
            // not enough players to start the game
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lobby", "Not enough player", g_window);
            return;
        }

        // send start game message
        multiplayerStart();
    } else {
        searchBotsPos(game->map);
    }

    g_currentState = GAME_PLAY_PLAYING;
}
