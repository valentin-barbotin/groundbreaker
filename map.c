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

#define DEBUG true

short       g_nbMap = 0;

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
        #ifdef DEBUG
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
                #ifdef DEBUG
                    fprintf(stderr, "Error: Could not allocate memory for buff in getMaps()\n");
                #endif
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
                exit(1);
            }
            sprintf(buff, "maps/%s", files->d_name);

            fd = fopen(buff, "rb");
            if (fd == NULL) {
                #ifdef DEBUG
                    fprintf(stderr, "Error: Can't open map file %s\n", files->d_name);
                #endif
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Can't open map file", g_window);
                exit(1);
            }
            free(buff);

            mapfd = malloc(sizeof(t_map));
            if (mapfd == NULL) {
                #ifdef DEBUG
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
                #ifdef DEBUG
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
        #ifdef DEBUG
            fprintf(stderr, "Error: Could not allocate memory for name in saveMap()\n");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
        exit(1);
    }

    sprintf(buff, "maps/%s.bin", name);

    fd = fopen(buff, "wb");
    if (fd == NULL) {
        #ifdef DEBUG
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
        #ifdef DEBUG
            fprintf(stderr, "Error: Could not allocate memory for map->map in map_create()\n");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
        exit(1);
    }

    for (int i = 0; i < height; i++) {
        map->map[i] = malloc(sizeof(char) * width);
        if (map->map[i] == NULL) {
            #ifdef DEBUG
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
            switch (map->map[i][j])
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

            rect.x = 0;
            rect.y = 0;
            rect.w = 288; //TODO: dynamic
            rect.h = 288;

            rectdest.x = j * cellSizeX;
            rectdest.y = i * cellSizeY;
            rectdest.w = cellSizeX;
            rectdest.h = cellSizeY;
            drawTexture(tex, &rect, &rectdest);
        }
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

    drawTexture(TEX_PLAYER, &rect, &rectdest);

    // draw player name
    drawText(&textColor, player->x, player->y + gameConfig->video.height * 0.08, player->name, true, 0);
}

void    spawnRandomItem(int xCell, int yCell) {
    
}