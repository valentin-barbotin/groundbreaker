#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "map.h"
#include "utils.h"
#include "game.h"
#include "display.h"
#include "player.h"
#include "inventory.h"
#include "font.h"
#include "tchat.h"
#include "client.h"

#define DEBUG true

short       g_nbMap = 0;

/**
 * @brief Update a cell for others players
 * 
 * @param content 
 */
void     cellUpdate(const char* content) {
    const t_map        *map;
    t_type             type;
    unsigned short     x;
    unsigned short     y;

    map = getGame()->map;

    sscanf(content, "%hu %hu %u", &x, &y, &type);
    GETCELL(x, y) = type;
}

void     getMaps() {
    struct dirent   *files;
    DIR             *dir;
    FILE            *fd;
    t_game          *game;
    t_map           *mapfd;
    t_map           *map;
    char            *buff;
    char            **grid;
    unsigned short  w;
    unsigned short  h;

    game = getGame();

    g_nbMap = 0;
    dir = opendir("maps");
    if (dir == NULL) {
        #if DEBUG
            fprintf(stderr, "Error: Can't open maps directory\n");
            perror("opendir");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Can't open maps directory", g_window);
        exit(1);
    }

    while ((files = readdir(dir)) != NULL) {
        if (files->d_type == DT_REG) { // regular file
            if (files->d_name[0] == '.') continue;

            if (g_nbMap == 9) break;

            // We can strdup because d_name array is 256 bytes long
            buff = strdup(files->d_name);
            if (buff == NULL) {
                #if DEBUG
                    fprintf(stderr, "Error: Could not allocate memory for buff in getMaps()\n");
                #endif
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
                exit(1);
            }
            sprintf(buff, "maps/%s", files->d_name);

            fd = fopen(buff, "rb");
            if (fd == NULL) {
                #if DEBUG
                    fprintf(stderr, "Error: Can't open map file %s\n", files->d_name);
                #endif
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Can't open map file", g_window);
                exit(1);
            }
            free(buff);

            mapfd = malloc(sizeof(t_map));
            if (mapfd == NULL) {
                #if DEBUG
                    fprintf(stderr, "Error: Could not allocate memory for mapfd in getMaps()\n");
                #endif
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
                exit(1);
            }
            fread(mapfd, sizeof(t_map), 1, fd);
            w = mapfd->width;
            h = mapfd->height;
            free(mapfd);

            map = map_create(w, h);
            if (map == NULL) {
                #if DEBUG
                    fprintf(stderr, "Error: Could not allocate memory for map in getMaps()\n");
                #endif
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
                exit(1);
            }
            grid = map->map;

            rewind(fd);
            fread(map, sizeof(t_map), 1, fd);
            map->map = grid;
            for (int i = 0; i < map->height; i++) {
                fread(map->map[i], sizeof(char), map->width, fd);
            }

            memcpy(&game->maps[g_nbMap++], map, sizeof(t_map));
            fclose(fd);
            free(map);
        }
    }
}

void    saveMap(const t_map *map) {
    FILE    *fd;
    char    buff[50];
    char    *name;

    name = randomString(20);
    if (name == NULL) {
        #if DEBUG
            fprintf(stderr, "Error: Could not allocate memory for name in saveMap()\n");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
        exit(1);
    }

    sprintf(buff, "maps/%s.bin", name);

    fd = fopen(buff, "wb");
    if (fd == NULL) {
        #if DEBUG
            fprintf(stderr, "Error: Can't open map file %s\n", name);
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Can't write map", g_window);
        exit(1);
    }
    free(name);

    fwrite(map, sizeof(t_map), 1, fd);
    for (int i = 0; i < map->height; i++) {
        fwrite(map->map[i], sizeof(char), map->width, fd);
    }
    fclose(fd);
}

t_map   *map_create(unsigned short width, unsigned short height) {
    t_map   *map;

    map = malloc(sizeof(t_map));
    if (map == NULL) {
        return NULL;
    }

    map->width = width;
    map->height = height;
    map->map = malloc(sizeof(char *) * height);
    if (map->map == NULL) {
        #if DEBUG
            fprintf(stderr, "Error: Could not allocate memory for map->map in map_create()\n");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
        exit(1);
    }

    for (int i = 0; i < height; i++) {
        map->map[i] = malloc(sizeof(char) * width);
        if (map->map[i] == NULL) {
            #if DEBUG
                fprintf(stderr, "Error: Could not allocate memory for map->map[i] in map_create()\n");
            #endif
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
            exit(1);
        }
    }

    return map;
};

void    map_fill(const t_map *map) {
    int    random;
    if (map == NULL) return;

    // Fill the map with empty tiles
    for (int i = 0; i < map->height; i++) {
        for (int j = 0; j < map->width; j++) {
            map->map[i][j] = EMPTY;
        }
    }
    for (int i = 0; i < map->height; i++) {
        for (int j = 0; j < map->width; j++) {
            map->map[map->height - 1][j] = UNBREAKABLE_WALL;
            map->map[i][0] = UNBREAKABLE_WALL;
            map->map[i][map->width - 1] = UNBREAKABLE_WALL;
            map->map[0][j] = UNBREAKABLE_WALL;
        }
    }

    // Place the unbreakables walls
    for (int i = 2; i < map->height-2; i++) {
        for (int j = 2; j < map->width-2; j++) {
            if (i % 2 == 0 && j % 2 == 0) {
                map->map[i][j] = UNBREAKABLE_WALL;
            }
        }
    }

    // if 1, holes are placed at the top of the map and at the bottom
    if (rand() % 2) {
        random = rand() % ((map->width - 2) + 1);
        if (random == 0) random++;
        if (random == map->width - 1) random--;
        map->map[0][random] = EMPTY;
        map->map[map->height - 1][random] = EMPTY;
    } else {
        // if 0, holes are placed at the left of the map and at the right
        random = rand() % ((map->height - 2) + 1);
        if (random == 0) random++;
        if (random == map->height- 1) random--;
        map->map[random][0] = EMPTY;
        map->map[random][map->width - 1] = EMPTY;
    }
}





void    map_destroy(t_map *map) {
    for (int i = 0; i < map->height; i++) {
        free(map->map[i]);
    }
    free(map->map);
    free(map);
};

void    map_print(const t_map *map) {
    for (int i = 0; i < map->height; i++) {
        for (int j = 0; j < map->width; j++) {
            printf("%d ", map->map[i][j]);
        }
        printf("\n");
    }
    printf("\n");
};

void    drawMapInRect(const SDL_Rect *rectList, size_t index) {
    SDL_Rect        rect;
    SDL_Rect        rectdest;
    const char      *tex;
    unsigned int    cellSizeX;
    unsigned int    cellSizeY;
    const t_game    *game;
    const t_map     *map;
    
    game = getGame();
    map = &game->maps[index];
    cellSizeX = rectList->w / map->width; // ex: 166 (width of 1000 divided by 6 (nb of cols))
    cellSizeY = rectList->h / map->height;

    for (int i = 0; i < map->height; i++) {
        for (int j = 0; j < map->width; j++) {
            switch (map->map[i][j])
            {
                case UNBREAKABLE_WALL:
                    tex = TEX_UNBREAKABLE_WALL;
                    break;
                default:
                    tex = TEX_DIRT; //player & empty
                    break;
            }

            rect.x = 0;
            rect.y = 0;
            rect.w = 288; //TODO: dynamic
            rect.h = 288;

            rectdest.x = j * cellSizeX + rectList->x;
            rectdest.y = i * cellSizeY + rectList->y;
            rectdest.w = cellSizeX;
            rectdest.h = cellSizeY;
            drawTexture(tex, &rect, &rectdest);
        }
    }
}


void    drawMap() {
    SDL_Rect        rect;
    SDL_Rect        rectdest;
    const char      *tex;
    unsigned int    cellSizeX;
    unsigned int    cellSizeY;
    const t_game    *game;
    const t_map     *map;
    
    game = getGame();
    map = game->map;
    cellSizeX = gameConfig->video.width / game->map->width; // ex: 166 (width of 1000 divided by 6 (nb of cols))
    cellSizeY = gameConfig->video.height / game->map->height;

    for (int i = 0; i < map->height; i++) {
        for (int j = 0; j < map->width; j++) {

            rect.x = 0;
            rect.y = 0;
            rect.w = 288; //TODO: dynamic
            rect.h = 288;

            switch (GETCELL(j, i))
            {
                case WALL:
                    tex = TEX_WALL;
                    break;
                case LOOT:
                    tex = TEX_LOOT;
                    break;
                case UNBREAKABLE_WALL:
                    tex = TEX_UNBREAKABLE_WALL;
                    break;
                case GRAVEL:
                    tex = TEX_GRAVEL;
                    break;
                default:
                    tex = TEX_DIRT; //player & empty
                    break;
            }

            rectdest.x = j * cellSizeX;
            rectdest.y = i * cellSizeY;
            rectdest.w = cellSizeX;
            rectdest.h = cellSizeY;

            drawTexture(tex, &rect, &rectdest);

            tex = NULL;

            // Draw items
            switch (GETCELL(j, i))
            {
                case BOMB:
                    rect.w = 758; //TODO: dynamic
                    rect.h = 980;
                    tex = TEX_BOMB_BOUM;
                    break;
                
                //items
                case ITEM_BOMB:
                    rect.w = 758;
                    rect.h = 980;
                    tex = TEX_BOMB;
                    break;
                case ITEM_BOMB_UP:
                    rect.w = 758;
                    rect.h = 980;
                    tex = TEX_BOMB_UP;
                    break;
                case ITEM_BOMB_DOWN:
                    rect.w = 758;
                    rect.h = 980;
                    tex = TEX_BOMB_DOWN;
                    break;
                case ITEM_YELLOW_FLAME:
                    rect.w = 758;
                    rect.h = 980;
                    tex = TEX_YELLOW_FLAME;
                    break;
                case ITEM_BLUE_FLAME:
                    rect.w = 758;
                    rect.h = 980;
                    tex = TEX_BLUE_FLAME;
                    break;
                case ITEM_RED_FLAME:
                    rect.w = 758;
                    rect.h = 980;
                    tex = TEX_RED_FLAME;
                    break;
                case ITEM_PASS_THROUGH_BOMB:
                    rect.w = 758;
                    rect.h = 980;
                    tex = TEX_PASS_THROUGH;
                    break;
                case ITEM_BOMB_KICK:
                    rect.w = 758;
                    rect.h = 980;
                    tex = TEX_BOMB_KICK;
                    break;
                case ITEM_INVINCIBILITY:
                    rect.w = 758;
                    rect.h = 980;
                    tex = TEX_INVINCIBILITY;
                    break;
                case ITEM_HEART:
                    rect.w = 758;
                    rect.h = 980;
                    tex = TEX_HEART;
                    break;
                case ITEM_LIFE:
                    rect.w = 758;
                    rect.h = 980;
                    tex = TEX_LIFE;
                    break;
                
                default:
                    break;
            }

            if (tex) {
                drawTexture(tex, &rect, &rectdest);
            }
        }
    }

    if (inMultiplayer()) {
        drawTchat();
        drawTchatMessages();
    }

    drawInventory();
    drawSelectedItem();

    for (size_t i = 0; i < game->nbPlayers; i++)
    {
        drawPlayer(game->players[i]);
    }

    // bots
    for (size_t i = 0; i < g_nbBots; i++)
    {
        drawPlayer(g_bots[i]);
    }
}

void getPlayerDirection(SDL_Rect *rect, const t_direction *direction) {

    // position of the sprite in the texture
    switch (*direction)
    {
        case DIR_UP_RIGHT:
        case DIR_UP_LEFT:
        case DIR_UP:
            rect->x = 110;
            break;
        case DIR_DOWN_RIGHT:
        case DIR_DOWN_LEFT:
        case DIR_DOWN:
            rect->x = 47;
            break;
        case DIR_LEFT:
            rect->x = 229;
            break;
        case DIR_RIGHT:
            rect->x = 174;
            break;
        default:
            rect->x = 47; // default: down
            break;
    }
}

/**
 * @brief Get the player stance (moving or not) and render the player
 * 
 */

void    drawPlayer(const t_player *player) {
    short           spriteW;
    short           spriteH;
    SDL_Rect        rect;
    SDL_Rect        rectdest;
    SDL_Color       textColor = {255, 255, 255, 255};
    char            buffer[256];

    //draw player
    spriteW = 50;
    spriteH = 75;

    // position of the player (centered)

    rectdest.x = player->x - (spriteW/2);
    rectdest.y = player->y - (spriteH/2);
    rectdest.w = PLAYER_WIDTH;
    rectdest.h = PLAYER_HEIGHT;

    // position of the sprite in the texture
    getPlayerDirection(&rect, &player->direction);
    rect.y = 13;
    rect.w = spriteW;
    rect.h = spriteH;

    drawTexture(player->health ? TEX_PLAYER : TEX_TOMBSTONE, &rect, &rectdest);

    if (!player->health) {
        rect.w = 900;
        rect.h = 600;
        drawTexture(TEX_TOMBSTONE, &rect, &rectdest);
    } else {
        drawTexture(TEX_PLAYER, &rect, &rectdest);
    }

    // draw player name
    if (player->lives && !player->isBot) {
        sprintf(buffer, "%s%s lives: %hu", player->name, player->health ? "" : " (dead)", player->lives);
    } else if (player->lives && player->isBot) {
        strcpy(buffer, "Bot");
    } else {
        sprintf(buffer, "%s (dead)", player->name);
    }
    drawText(&textColor, player->x, player->y + gameConfig->video.height * 0.08, buffer, true, 0);
}

void    spawnRandomItem(int xCell, int yCell) {
    t_item  *item;
    int     random;
    t_map   *map;

    map = getGame()->map;

    item = &g_items[rand() % NB_ITEMS];
    random = rand() % 100;
    
    if (random < 50) {
        GETCELL(xCell, yCell) = item->type;
        updateCell(xCell, yCell, (t_type) item->type);
    } else {
        GETCELL(xCell, yCell) = EMPTY;
        updateCell(xCell, yCell, EMPTY);
    }
}


bool    canMoveOnNextCell(char cell) {
    switch (cell)
    {
        case ITEM_BOMB:
        case ITEM_BOMB_UP:
        case ITEM_BOMB_DOWN:
        case ITEM_YELLOW_FLAME:
        case ITEM_BLUE_FLAME:
        case ITEM_RED_FLAME:
        case ITEM_PASS_THROUGH_BOMB:
        case ITEM_BOMB_KICK:
        case ITEM_INVINCIBILITY:
        case ITEM_HEART:
        case ITEM_LIFE:
        case EMPTY:
        case GRAVEL:
            return true;
            break;
        
        default:
            return false;
            break;
    }
}