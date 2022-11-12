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
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
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
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
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
                if(!stopSound(wall)) {
                    #ifdef DEBUG
                        fprintf(stderr, "Error: can't stop sound\n");
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                    exit(1);
                }
            } else if(Mix_PlayingMusic() == 0) {
                initMusic(wall);
                if(wall->music == NULL) {
                    #ifdef DEBUG
                        fprintf(stderr, "Error loading sound file: %s\n", Mix_GetError());
                    #endif
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                    exit(1);
                }
                playSound(wall);
            } else if(Mix_PlayingMusic() == 1) {
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
            player->x -= player->vx;
            player->y -= player->vy;
            
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
    }

    g_currentState = GAME_PLAY_PLAYING;
}