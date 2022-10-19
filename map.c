#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "map.h"
#include "utils.h"
#include "game.h"

short       g_nbMap = 0;

void    *getMaps() {
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
        dir = opendir("../maps");
        if (dir == NULL) {
            printf("Error: Could not open maps directory\n");
            exit(1);
        }
    }

    while ((files = readdir(dir)) != NULL) {
        if (files->d_type == DT_REG) { // regular file
            if (files->d_name[0] == '.') continue;

            if (g_nbMap == 9) break;

            buff = malloc(sizeof(char) * (strlen(files->d_name) + 1));
            sprintf(buff, "maps/%s", files->d_name);
            
            fd = fopen(buff, "rb");
            if (fd == NULL) {
                sprintf(buff, "../maps/%s", files->d_name);
                fd = fopen(buff, "rb");
                if (fd == NULL) {
                    printf("Error: Could not open file %s", files->d_name);
                    exit(1);
                }
            }
            free(buff);

            mapfd = malloc(sizeof(t_map));
            if (mapfd == NULL) {
                fprintf(stderr, "Error malloc map: %s", SDL_GetError());
                exit(1);
            }
            fread(mapfd, sizeof(t_map), 1, fd);
            w = mapfd->width;
            h = mapfd->height;
            free(mapfd);

            map = map_create(w, h);
            if (map == NULL) {
                fprintf(stderr, "Error malloc map: %s", SDL_GetError());
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
        perror("Error randomString");
        exit(1);
    }

    sprintf(buff, "maps/%s.bin", name);

    fd = fopen(buff, "wb");
    if (fd == NULL) {
        sprintf(buff, "../maps/%s.bin", name);
        fd = fopen(buff, "wb");
        if (fd == NULL) {
            printf("Error: Could not open maps directory\n");
            exit(1);
        }
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
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        map->map[i] = malloc(sizeof(char) * width);
        if (map->map[i] == NULL) {
            return NULL;
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
