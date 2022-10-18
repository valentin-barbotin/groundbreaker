#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

#include "map.h"

short       g_nbMap = 0;

void    getMaps() {
    struct dirent *files;
    DIR *dir;

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
            printf("File: %s\n", files->d_name);
            g_nbMap++;
        }
    }

    closedir(dir);
}

t_map   *map_create(int width, int height) {
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
            printf("%c", map->map[i][j]);
        }
        printf("\n");
    }
};

t_map  *getMap() {
    static t_map *map;

    if (map == NULL) {
        map = map_create(10, 10);
        map_fill(map);
    }

    return map;
};