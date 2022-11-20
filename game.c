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
#include "effects.h"
#include "client.h"

#define DEBUG true

/**
 * @brief Kills the bots if they are on the given cell
 * Bots are immuned from other bot's bombs
 *
 * @param xCell
 * @param yCell
 */
void    killBots(int xCell, int yCell) {
    t_player        *bot;
    bool            allDead;

    if (g_currentState == GAME_MAINMENU_PLAY) return;

    allDead = true;
    for (short i = 0; i < g_nbBots; i++) {
        bot = g_bots[i];
        if (bot->health && bot->xCell == xCell && bot->yCell == yCell) {
            bot->health = 0; // kill bot
            bot->vx = 0;
            bot->vy = 0;
        }

        if (bot->health) {
            allDead = false;
        }
    }

    if (allDead) {
        if (!inMultiplayer()) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game finished", "You won the game", g_window);
        }

        // puts the player back in the lobby
        g_currentState = GAME_MAINMENU_PLAY;
        resetBots();
        clearEffects();
    }
}

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


t_game* getGame() {
    static t_game*  game;

    if (game == NULL) {
        game = malloc(sizeof(t_game));
        if (game == NULL) {
            #if DEBUG
                fprintf(stderr, "Error allocating memory for game");
            #endif
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
            exit(1);
        }

        game->map = NULL;

        //TMP max players
        //TODO:
        int nb = 4;
        game->players = malloc(sizeof(t_player *) * nb);
        if (game->players == NULL) {
            #if DEBUG
                fprintf(stderr, "Error allocating memory for game->players");
            #endif
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
            exit(1);
        }

        for (int i = 0; i < nb; i++) {
            game->players[i] = initPlayer();
        }

        game->nbPlayers = 1;
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
    const t_game    *game;
    const t_map     *map;
    bool            stopped;
    char            currentCell;
    char            nextCell;

    game = getGame();
    map = game->map;

    if (map == NULL) {
        #if DEBUG
            fprintf(stderr, "Error: map is NULL in movePlayer()\n");
        #endif

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Game error", g_window);
        exit(1);
    }

    if (!isAlive(player)) {
        return; // don't move dead players/bots
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
        nextCell = GETCELL(0, player->yCell);
        if (canMoveOnNextCell(nextCell)) {
            // move the player to the other side
            player->x = 0;
        }
    } else if (player->x == 0 && player->vx < 0) {
        // if the player is on the left side of the map and he is moving left then we move him to the right side of the map

        // check if the player can be placed on the next cell
        nextCell = GETCELL(game->map->width-1, player->yCell);
        if (canMoveOnNextCell(nextCell)) {
            // move the player to the other side
            player->x = gameConfig->video.width - PLAYER_WIDTH/2;
        }
    }

    if (player->y >= (gameConfig->video.height - PLAYER_HEIGHT/2)) {
        nextCell = GETCELL(player->xCell, 0);
        if (canMoveOnNextCell(nextCell)) {
            // move the player to the other side
            player->y = 0;
        }
    } else if (player->y == 0 && player->vy < 0) {
        // if the player is on the top side of the map and he is moving up then we move him to the bottom side of the map

        // check if the player can be placed on the next cell
        nextCell = GETCELL(player->xCell, game->map->height-1);
        if (canMoveOnNextCell(nextCell)) {
            // move the player to the other side
            player->y = gameConfig->video.height - PLAYER_HEIGHT/2;
        }
    }

    currentCell = GETCELL(player->xCell, player->yCell);
    switch (currentCell) {
        case WALL:
            // if the player is on a wall then we move him back to the old position
            player->x -= player->vx;
            player->y -= player->vy;
            stopped = true;

            if (player->wallChannel == -1) {
                player->wallChannel = playSound(wall);
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

            if (player->wallChannel == -1) {
                player->wallChannel = playSound(unbreakableWall);
            }

            break;
        case BOMB: {
            // if the bomb is placed by the player then we don't explode it

            t_player    *owner;
            owner = findBombOwner(player->xCell, player->yCell);
            
            // local player placed the bomb, owner can't trigger
            if (owner && owner == player) break;

            // if a bot moved on it and the owner is a bot then we don't explode it
            if (!inMultiplayer() && owner && owner->isBot && player->isBot) break;

            // trigger the bomb if the player is on it
            if (player->passThroughBomb) {
                break;
            }
            if (player->bombKick) {
                //TODO: kick the bomb

                kickBomb();
                break;
            }

            // null allow use to kill all the players
            // the owner of the bomb can be killed by others players using his bomb
            explodeBomb(player->xCell, player->yCell, owner); 

            break;
        }
        case ITEM_BOMB:
            if (player->inventory[ITEM_BOMB]->quantity == player->maxBombs) {
                break;
            }

            player->inventory[currentCell]->quantity++;
            GETCELL(player->xCell, player->yCell) = EMPTY;
            updateCell(player->xCell, player->yCell, EMPTY);
            break;
        case ITEM_BOMB_UP:
            player->maxBombs++;
            GETCELL(player->xCell, player->yCell) = EMPTY;
            updateCell(player->xCell, player->yCell, EMPTY);
            break;
        case ITEM_BOMB_DOWN:
            player->maxBombs--;

            if (player->maxBombs < 1) {
                player->maxBombs = 1;
            }

            if (player->inventory[ITEM_BOMB]->quantity > player->maxBombs) {
                player->inventory[ITEM_BOMB]->quantity = player->maxBombs;
            }

            GETCELL(player->xCell, player->yCell) = EMPTY;
            updateCell(player->xCell, player->yCell, EMPTY);
            break;
        case ITEM_YELLOW_FLAME:
            player->scope++;
            GETCELL(player->xCell, player->yCell) = EMPTY;
            updateCell(player->xCell, player->yCell, EMPTY);
            break;
        case ITEM_BLUE_FLAME:
            player->scope--;

            if (player->scope < 1) {
                player->scope = 1;
            }
            GETCELL(player->xCell, player->yCell) = EMPTY;
            updateCell(player->xCell, player->yCell, EMPTY);
            break;
        case ITEM_RED_FLAME:
            player->scope = 999;
            GETCELL(player->xCell, player->yCell) = EMPTY;
            updateCell(player->xCell, player->yCell, EMPTY);
            break;
        case ITEM_PASS_THROUGH_BOMB:
            player->passThroughBomb = true;
            player->bombKick = false;

            GETCELL(player->xCell, player->yCell) = EMPTY;
            updateCell(player->xCell, player->yCell, EMPTY);
            break;
        case ITEM_BOMB_KICK:
            player->passThroughBomb = false;
            player->bombKick = true;

            GETCELL(player->xCell, player->yCell) = EMPTY;
            updateCell(player->xCell, player->yCell, EMPTY);
            break;
        case ITEM_INVINCIBILITY:
            // only one godmode
            if (player->godMode) break;

            player->godMode = true;
            GETCELL(player->xCell, player->yCell) = EMPTY;
            updateCell(player->xCell, player->yCell, EMPTY);
            break;
        case ITEM_HEART:
            // only one heart
            if (player->canSurviveExplosion) break;

            player->canSurviveExplosion = true;
            GETCELL(player->xCell, player->yCell) = EMPTY;
            updateCell(player->xCell, player->yCell, EMPTY);
            break;
        case ITEM_LIFE: {
            char    buffer[128];
            player->lives++;
            GETCELL(player->xCell, player->yCell) = EMPTY;
            updateCell(player->xCell, player->yCell, EMPTY);

            sprintf(buffer, "LIFE: %hd %d", player->id, player->lives);

            sendToAll(buffer, getPlayer()->id);
        } 
            break;
        default:
            if (!player->isBot) {
                if (isMoving(player)) {
                    if (player->walkChannel == -1) {
                        player->walkChannel = playSoundLoop(walk);
                    }
                } else {
                    if (player->walkChannel != -1) {
                        Mix_HaltChannel(player->walkChannel);
                        player->walkChannel = -1;
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
    const t_game   *game;

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

void explodeBomb(int xCell, int yCell, t_player *owner) {
    const t_map      *map;
    const t_game     *game;
    const t_player    *player;
    t_effect          *effect;

    player = getPlayer();
    game = getGame();
    map = game->map;

    // if the bomb already exploded, return
    if (GETCELL(xCell, yCell) == GRAVEL) return;

    GETCELL(xCell, yCell) = GRAVEL;
    updateCell(xCell, yCell, GRAVEL);

    effect = addEffect(xCell, yCell, BOMB_EXPLOSION);
    if (effect) {
        sendEffect(effect);
    }

    playSound(bombExplosion);

    printf("cell destroyed at x:%d y:%d\n", xCell, yCell);


    // pour chaque direction (UP, DOWN, LEFT, RIGHT)
    searchDirectionMap(xCell, yCell, DIR_UP, player->scope, owner);
    searchDirectionMap(xCell, yCell, DIR_DOWN, player->scope, owner);
    searchDirectionMap(xCell, yCell, DIR_LEFT, player->scope, owner);
    searchDirectionMap(xCell, yCell, DIR_RIGHT, player->scope, owner);

    if (player->isBot) {
        removePlacedBomb(player, xCell, yCell);
    }
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

void    handleDamage(t_player *player) {
    char    buffer[256];
    if (player->lives && !player->godMode && !player->canSurviveExplosion) {
        //TODO: spawn tombstone and send it to all

        if (inMultiplayer()) {
            sprintf(buffer, "DAMAGE:%hu %d %d", player->id, player->xCell, player->yCell);
            sendToAll(buffer, getPlayer()->id);
        } else {
            sprintf(buffer, "DAMAGE:%hu %d %d", player->id, player->xCell, player->yCell);
            receiveDamage(buffer);
        }
    }

    if (player->canSurviveExplosion) {
        player->canSurviveExplosion = false;
    }
}

void    searchDirectionMap(int xCellBase, int yCellBase, t_direction direction, int scope, t_player *owner) {
    const t_game        *game;
    t_player            *player;
    const t_map         *map;
    int                 cellX;
    int                 cellY;
    const t_effect      *effect;

    game = getGame();
    player = getPlayer();
    map = game->map;

    if (map == NULL) {
        #if DEBUG
            fprintf(stderr, "Error: map is NULL in searchDirectionMap()\n");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Map error", g_window);
        exit(1);
    }

    // no bots in multiplayer
    // in case of a bot on the map itself
    if (!inMultiplayer() && owner) {
        if (!owner->isBot) {
            killBots(xCellBase, yCellBase);
        } else {
            if (player->health && player->xCell == xCellBase && player->yCell == yCellBase) {
                handleDamage(player);
            }
        }
    } else {
        for (int j = 0; j < game->nbPlayers; j++) {
            // disable this only if the player can receive damage of its own bomb
            if (owner && game->players[j] == owner) continue;

            player = game->players[j];
            if (player->xCell == xCellBase && player->yCell == yCellBase) {
                handleDamage(player);
            }
        }
    }

    for (int i = 1; i <= scope; i++) {
        switch (direction) {
            case DIR_UP:
                cellX = xCellBase;
                cellY = yCellBase - i;
                break;
            case DIR_DOWN:
                cellX = xCellBase;
                cellY = yCellBase + i;
                break;
            case DIR_LEFT:
                cellX = xCellBase - i;
                cellY = yCellBase;
                break;
            case DIR_RIGHT:
                cellX = xCellBase + i;
                cellY = yCellBase;
                break;
            default:
                break;
        }

        //TODO: explosion over border
        if (cellX < 0 || cellX >= map->width || cellY < 0 || cellY >= map->height) {
            return;
        }

        // no bots in multiplayer
        if (!inMultiplayer() && owner) {
            if (!owner->isBot) {
                killBots(cellX, cellY);
            } else {
                if (player->health && player->xCell == cellX && player->yCell == cellY) {
                    handleDamage(player);
                }
            }
        }

        switch (GETCELL(cellX, cellY)) {
            case WALL:
                GETCELL(cellX, cellY) = GRAVEL;
                updateCell(cellX, cellY, GRAVEL);
                printf("Wall destroyed at x:%d y:%d\n", cellX, cellY);
                break;
            case LOOT:
                printf("Loot destroyed at x:%d y:%d\n", cellX, cellY);
                spawnRandomItem(cellX, cellY);
                break;
            case UNBREAKABLE_WALL:
                // on arrête la bombe dans sa course
                return;
            case BOMB:
                if (!player->passThroughBomb) break;

                explodeBomb(cellX, cellY, owner);
                break;
            default:
                break;
        }

        // search for players on the cell
        if (inMultiplayer()) {
            for (int j = 0; j < game->nbPlayers; j++) {
                // disable this only if the player can receive damage of its own bomb
                // if (j == g_playersMultiIndex) continue;
                if (game->players[j] == owner) continue;

                player = game->players[j];
                if (player->xCell == cellX && player->yCell == cellY) {
                    handleDamage(player);
                }
            }
        }

        effect = addEffect(cellX, cellY, BOMB_EXPLOSION);
        if (effect) {
            sendEffect(effect);
        }
    }
}


void    launchGame() {
    t_game            *game;
    short             index;
    t_map             *tmp[10] = {0};

    game = getGame();
    index = -1;

    for (size_t i = 0; i < g_nbMap; i++)
    {
        if (game->maps[i].selected) {
            if (index == -1) index = 0; //used to check if we have at least one map selected
            tmp[index++] = &game->maps[i];
        }
    }

    getMaps(tmp);

    if (g_serverRunning && (game->nbPlayers < g_lobby->players)) {
        #if DEBUG
            puts("Not enough players");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lobby", "Not enough player", g_window);
        return;
    }

    if (index == -1) {
        printf("No map selected\n");
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lobby", "You must select a map", g_window);
        return;
    }

    index = rand() % index; //pick a random map between the selected ones
    printf("index = %d\n", index);
    game->map = tmp[index];

    resetPlayer(getPlayer());

    injectItems(game->map);

    putPlayerInFreeCell(getPlayer());

    for (size_t i = 0; i < MAX_BOMBS; i++)
    {
        if (getPlayer()->bombs[i] != NULL) {
            free(getPlayer()->bombs[i]);
        }
        getPlayer()->bombs[i] = NULL;
    }

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

 void    putPlayerInFreeCell(t_player *player) {
    const t_game    *game;
    const t_map     *map;
    int             x;
    int             y;

    game = getGame();
    map = game->map;

    do {
        x = rand() % map->width;
        y = rand() % map->height;
    } while (GETCELL(x, y) != EMPTY);

    spawnPlayer(x, y, player);
 }

Uint32   timedRespawn(Uint32 interval, char *param) {
    t_player    *player;

    if (inMultiplayer()) {
        sendToAll(param, getPlayer()->id);
        free(param);
    }
    
    player = getPlayer();

    player->health = 100;
    putPlayerInFreeCell(player);
    if (inMultiplayer()) {
        doSendPos(player);
    }
    return 0;
}

void    receiveDamage(const char *content) {
    short       id;
    short       winner;
    int         xCell;
    int         yCell;
    char        *buff;
    t_player    *player;

    player = getPlayer();

    sscanf(content, "%hd %d %d", &id, &xCell, &yCell);
    if (inMultiplayer() && id != player->id) return;

    player->health = 0;
    player->vx = 0;
    player->vy = 0;

    puts("You died");

    if (--player->lives == 0) {
        // check if everyone is dead
        if (inMultiplayer() && checkIfEveryoneIsDead(&winner)) {
            // send end game message
            multiplayerEnd(winner);
        }

        if (!inMultiplayer()) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game finished", "You lost", g_window);
            g_currentState = GAME_MAINMENU_PLAY;
            resetBots();
            clearEffects();
        }
        return;
        //TODO: handle endgame in solo
    }

    //TODO: static tombstone

    buff = malloc(sizeof(char) * 100);
    if (!buff) {
        #if DEBUG
            fprintf(stderr, "Error allocating memory");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
        exit(1);
    }

    // respawn after n seconds
    sprintf(buff, "RESPAWN:%hd %d %d", id, player->xCell, player->yCell);
    //TODO: respawn time in config
    SDL_AddTimer(3000, timedRespawn, buff);
}


void    receiveLife(const char *content) {
    short           id;
    unsigned short  lifes;
    t_player    *player;

    sscanf(content, "%hd %hu", &id, &lifes);

    player = getGame()->players[id];

    player->lives = lifes;
}

bool    checkIfEveryoneIsDead(short *winner) {
    const t_game      *game;
    const t_player    *player;
    
    *winner = 0;

    game = getGame();

    for (int i = 0; i < game->nbPlayers; i++) {
        player = game->players[i];
        if (player->lives) {
            // if there is at least two players alive, the game is not over
            if (*winner) return false;
            
            // if we have a winner, we store it
            *winner = player->id;
        }
    }

    // everyone is dead
    return true;
}


void    receiveEndGame(const char* content) {
    short           winner;
    const t_game    *game;
    const t_player  *player;
    char            buff[256];

    sscanf(content, "%hd", &winner);

    game = getGame();
    player = game->players[winner];

    sprintf(buff, "%s won the game", player->name);

    // the winner can still move until he click on the button
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game finished", buff, g_window);
    clearEffects();

    if (g_serverRunning) {
        // reset the game
        getMaps();
    }

    // puts the player back in the lobby
    g_currentState = GAME_MAINMENU_PLAY;
}


void    kickBomb() {
    const t_player    *player;
    const t_map       *map;
    t_game            *game;
    short             xCellNew;
    short             yCellNew;

    player = getPlayer();
    game = getGame();
    map = game->map;

    xCellNew = player->xCell;
    yCellNew = player->yCell;

    switch (player->direction)
    {
        case DIR_UP_RIGHT:
        case DIR_UP_LEFT:
        case DIR_UP:
            do
            {
                yCellNew++;

                if (searchPlayerToExplode(xCellNew, yCellNew)) return;

            } while (GETCELL(xCellNew, yCellNew + 1) == EMPTY || GETCELL(xCellNew + 1, yCellNew) == GRAVEL);
            break;
        case DIR_DOWN_RIGHT:
        case DIR_DOWN_LEFT:
        case DIR_DOWN:
            do
            {
                yCellNew--;

                if (searchPlayerToExplode(xCellNew, yCellNew)) return;

            } while (GETCELL(xCellNew, yCellNew - 1) == EMPTY || GETCELL(xCellNew, yCellNew - 1) == GRAVEL);
            break;
        case DIR_LEFT:
            do
            {
                xCellNew++;

                if (searchPlayerToExplode(xCellNew, yCellNew)) return;

            } while (GETCELL(xCellNew + 1, yCellNew) == EMPTY || GETCELL(xCellNew + 1, yCellNew) == GRAVEL);
            break;
        case DIR_RIGHT:
            do
            {
                xCellNew--;

                if (searchPlayerToExplode(xCellNew, yCellNew)) return;

            } while (GETCELL(xCellNew - 1, yCellNew) == EMPTY || GETCELL(xCellNew - 1, yCellNew) == GRAVEL);
            break;
        default:
            break;
    }

    if (xCellNew != player->xCell || yCellNew != player->yCell) {
        placeBomb(xCellNew, yCellNew, player); // change bomb owner

        GETCELL(player->xCell, player->yCell) = EMPTY;
        updateCell(player->xCell, player->yCell, EMPTY);
    }
}


bool    searchPlayerToExplode(int xCell, int yCell) {
    const t_game    *game;
    const t_player  *player;

    game = getGame();

    for (int j = 0; j < game->nbPlayers; j++) {
        player = game->players[j];
        if (player->xCell == xCell && player->yCell == yCell) {
            explodeBomb(xCell, yCell, getPlayer()); //function only used in multiplayer, bomb owner is us because we kicked it
            return true;
        }
    }

    return false;
}

void    placeBomb(int xCell, int yCell, t_player *player) {
    const t_game    *game;
    const t_map     *map;
    t_bomb          *bomb2;

    game = getGame();
    map = game->map;

    storePlacedBomb(player, xCell, yCell);

    bomb2 = malloc(sizeof(t_bomb));
    if (!bomb2) {
        #if DEBUG
            fprintf(stderr, "Malloc error in placeBomb()");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
        exit(1);
    }

    GETCELL(xCell, yCell) = BOMB;
    updateCell(xCell, yCell, BOMB);

    bomb2->owner = player;
    bomb2->pos.x = xCell;
    bomb2->pos.y = yCell;
    // printf("Placed bomb at %d %d, detonation in %lu ms\n", xCell, yCell, g_items[ITEM_BOMB].duration);
    SDL_AddTimer(g_items[ITEM_BOMB].duration, bombTimer, bomb2);
}


void    resetBots() {
    for (size_t i = 0; i < g_nbBots; i++)
    {
        for (size_t j = 0; j < MAX_BOMBS; j++)
        {
            // if (g_bots[i]->bombs[j] != NULL) {
            //     free(g_bots[i]->bombs[j]);
            // }
            // g_bots[i]->bombs[j] = NULL;
        }

        if (g_bots[i]->walkChannel != -1) {
            Mix_HaltChannel(g_bots[i]->walkChannel);
            g_bots[i]->walkChannel = -1;
        }

        if (g_bots[i]->wallChannel != -1) {
            Mix_HaltChannel(g_bots[i]->wallChannel);
            g_bots[i]->wallChannel = -1;
        }
        free(g_bots[i]);
        g_bots[i] = NULL;
    }

    g_nbBots = 0;
}