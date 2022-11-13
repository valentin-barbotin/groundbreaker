#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "map.h"
#include "utils.h"
#include "game.h"
#include "display.h"
#include "player.h"

#define DEBUG true
#define GETCELL(i,j)(map->[i][j])


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
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
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
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                exit(1);
            }
            sprintf(buff, "maps/%s", files->d_name);

            fd = fopen(buff, "rb");
            if (fd == NULL) {
                #ifdef DEBUG
                    fprintf(stderr, "Error: Can't open map file %s\n", files->d_name);
                #endif
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                exit(1);
            }
            free(buff);

            mapfd = malloc(sizeof(t_map));
            if (mapfd == NULL) {
                #ifdef DEBUG
                    fprintf(stderr, "Error: Could not allocate memory for mapfd in getMaps()\n");
                #endif
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
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
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
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
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
        exit(1);
    }

    sprintf(buff, "maps/%s.bin", name);
    free(name);

    fd = fopen(buff, "wb");
    if (fd == NULL) {
        #ifdef DEBUG
            fprintf(stderr, "Error: Can't open map file %s\n", name);
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
        exit(1);
    }

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
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
        exit(1);
    }

    for (int i = 0; i < height; i++) {
        map->map[i] = malloc(sizeof(char) * width);
        if (map->map[i] == NULL) {
            #ifdef DEBUG
                fprintf(stderr, "Error: Could not allocate memory for map->map[i] in map_create()\n");
            #endif
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
            exit(1);
        }
    }

    return map;
};

void    map_fill(const t_map *map) {
    if (map == NULL) return;

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
    for (int i = 2; i < map->height-2; i++) {
        for (int j = 2; j < map->width-2; j++) {
            map->map[i][(j % 2) * j] = WALL;
            map->map[(i %2)*i][j] = EMPTY;
            map->map[i][0] = UNBREAKABLE_WALL;
            map->map[0][j] = UNBREAKABLE_WALL;
            map->map[1][1] = PLAYER;
            map->map[map->height - 2][map->width - 2] = PLAYER;
        }
    }
    for (int i = 2; i < map->height-2; i++) {
        for (int j = 2; j < map->width-2; j++) {
            map->map[1][0] = EMPTY;
            map->map[1][map->width - 1] = EMPTY;

        }
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

            rectdest.x = j * cellSizeX;
            rectdest.y = i * cellSizeY;
            rectdest.w = cellSizeX;
            rectdest.h = cellSizeY;
            drawTexture(tex, &rect, &rectdest);

            switch (map->map[i][j]){
                case BOMB:
                    tex = TEX_BOMB;
                    break;
                case BOMB_UP:
                    tex = TEX_BOMB_UP;
                    break;
                case BOMB_DOWN:
                    tex = TEX_BOMB_DOWN;
                    break;
                case YELLOW:
                    tex = TEX_YELLOW_FLAME;
                    break;
                case RED:
                    tex = TEX_RED_FLAME;
                    break;
                case BLUE:
                    tex = TEX_BLUE_FLAME;
                    break;
                case BOMB_KICK:
                    tex = TEX_BOMB_KICK;
                    break;
                case PASS_THROUGH_BOMB:
                    tex = TEX_PASS_THROUGH_BOMB;
                    break;
                case INVINCIBILITY:
                    tex=TEX_INVINCIBILITY;
                    break;
                case HEART:
                    tex=TEX_HEART;
                    break;
                case LIFE:
                    tex=TEX_LIFE;
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


    for (size_t i = 0; i < game->nbPlayers; i++)
    {
        drawPlayer(game->players[i]);
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
}