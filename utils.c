#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>


#include <SDL.h>
#include <SDL_image.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "utils.h"
#include "cache.h"
#include "dialog.h"
#include "player.h"
#include "client.h"

#define DEBUG true

/**
 * Description
 * @todo handle differents breaklines
 * @param {char*} take a string and remove the break line
 * @return {void}
 */
void removeLineFeed(char* str) {
    if (str == NULL) return;

    str[strcspn(str, "\n")] = '\0';
}

char   *randomString(unsigned short size) {
    char          *str;
    const char    *charset;

    charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    str = malloc(sizeof(char) * size + 1);
    if (str == NULL) {
        #ifdef DEBUG
            fprintf(stderr, "Error allocating memory for cache");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
        exit(1);
    }

    if (str) {
        for (unsigned short i = 0; i < size; i++) {
            int key = rand() % (int) (strlen(charset) - 1);
            str[i] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

// https://theartincode.stanis.me/008-djb2/
/**
 * Generate an "hash" of numbers from a string
 * Used to make switch case with string
 * @param {const char* str} the source string
 * @return {const unsigned long} the immutable generated hash
 */
const unsigned long hash(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
    
}
/**
 * Check if a string array contains a specific string
 * @param char** string array
 * @param char* the string to search
 * @param const int size of the array
 * @return {any}
 */
int doesInclude(char** array, const char* str, const int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(array[i], str) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Read an entire file, and store the content in the heap
 * @param src filePath
 * @return char* content location
 */
char* readFile(const char* src) {
    FILE* fd = fopen(src, "rb");
    if (fd == NULL) {
        return NULL;
    }

    fseek(fd, 0, SEEK_END);
    long size = ftell(fd);
    rewind(fd);
    char* data = malloc(size);
    if (data == NULL) {
        #ifdef DEBUG
            fprintf(stderr, "Error allocating memory for cache");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
        exit(1);
    }

    fread(data, sizeof(char), size, fd);
    data[size - 1] = '\0';

    fclose(fd);
    return data;
}


/**
 * Pick a color from the colors struct
 * @param {const SDL_Color*} constSDL_Color*color
 * @return {bool}
 */
bool pickColor(const SDL_Color *color) {
    bool success = SDL_SetRenderDrawColor(g_renderer, color->r, color->g, color->b, color->a);
    if (success == 0) {
        return true;
    }
    fprintf(stderr, "Erreur SDL_SetRenderDrawColor : %s\n", SDL_GetError());
    return false;
}

bool stringIsEqual(const char *str1, const char *str2) {
    return strcmp(str1, str2) == 0;
}

/**
 * Load a texture from a file
 * @param const char* src
 * @return SDL_Texture* texture
 */
SDL_Texture* textureFromFile(const char* src) {
    SDL_Texture* tex;
    tex = IMG_LoadTexture(g_renderer, src);
    if (tex == NULL) {
        fprintf(stderr, "Erreur IMG_LoadTexture : %s\n", SDL_GetError());
        return NULL;
    }

    return addImageToCache(src, tex);
}

/**
 * Remove suffix from a string
 * @param src char* src string
 * @param char* suffix to remove
 * @return char* suffix position in src
 */
char* removeSuffix(const char* src, char* suffix) {
    char *pos = strstr(src, suffix);
    if (pos == NULL) {
        #ifdef DEBUG
            fprintf(stderr, "Error removing suffix from string");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
        exit(1);
    }
    *pos = '\0';
    return pos;
}

/**
 * @brief Send a message to a socket
 * 
 * @param buffer 
 * @param socket 
 */
void   sendMsg(const char *msg, int socket) {
    send(socket, msg, strlen(msg) + 1, 0);
}

/**
 * @brief Receive a message from a socket
 * 
 * @param buffer 
 * @param socket 
 */
void    receiveMsg(char *buffer, int socket) {
    int     len = 0;
    char    c = 0;

    do {
        len += recv(socket, &c, 1, 0);
        buffer[len - 1] = c;
    } while (c != '\0');

    #ifdef DEBUG
        printf("Received: %s  size: %d\n", buffer, len);
    #endif
}


/**
 * @brief Send a message to a socket
 * 
 * @param buffer 
 * @param socket 
 */
void   sendMsgUDP(const char *msg, int socket, struct sockaddr *serverAddress) {
    sendto(socket, msg, strlen(msg) + 1, 0, serverAddress, sizeof(serverAddress));
}

/**
 * @brief Receive a message from a socket
 * 
 * @param buffer 
 * @param socket 
 */
void    receiveMsgUDP(char *buffer, int socket, struct sockaddr *serverAddress) {
    int     len = 0;
    char    c = 0;

    do {
        len += recvfrom(socket, &c, 1, 0, serverAddress, sizeof(serverAddress));
        buffer[len - 1] = c;
    } while (c != '\0');

    #ifdef DEBUG
        printf("Received: %s  size: %d\n", buffer, len);
    #endif
}

bool    checkUsername() {
    // get user name
    t_dialog            *dialog;
    const t_player      *player;

    player = getPlayer();

    // create dialog
    // if the dialog was just created, set props
    if (*player->name == 0) {
        puts("Creating dialog");
        dialog = getEditBox();
        if (dialog->text == NULL) {
            createEditBox("Enter your name", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});
            dialog->callback = askUsernameCallback;
        }
        return false;
    }

    return true;
}
