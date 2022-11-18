#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>


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
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
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
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "Memory error", g_window);
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
char* removeSuffix(const char *src, const char *suffix) {
    char *pos = strstr(src, suffix);
    if (pos == NULL) {
        #ifdef DEBUG
            fprintf(stderr, "Error removing suffix from string");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", "String error", g_window);
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
    size_t      msgLen;

    size_t     total;
    size_t     nb;
    size_t     max;

    msgLen = strlen(msg) + 1; // +1 for the null terminator
    // printf("msgLen: %zu\n", msgLen);

    total = 0;
    while (total != msgLen)
    {
        max = msgLen - total;

        // in case of error, we need to know how many bytes have been sent
        nb = send(socket, msg + total, max, 0);
        // printf("send: %s\n", msg + total);
        if (nb == -1)
        {
            fprintf(stderr, "Error sending message to server: %s\n", strerror(errno));
            exit(1);
        }
        total += nb;
        // printf("debug: [nb: %lu]  [total: %lu]\n", nb, total);
    }

    printf("Sent: (%ld bytes) [%s]\n", total, msg);
}

/**
 * @brief Receive a message from a socket
 * 
 * @param buffer 
 * @param socket 
 */
size_t    receiveMsg(char *buffer, int socket) {
    size_t     total;
    size_t     nb;
    size_t     max;

    max = 1024;

    total = 0;
    while (true)
    {

        nb = recv(socket, buffer + total, max, 0);
        if (nb == -1)
        {
            fprintf(stderr, "Error receiving message from server: %s\n", strerror(errno));
            exit(1);
        }
        // if 0, we have received the whole message
        // TODO: check
        if (nb == 0)
        {
            fprintf(stderr, "Peer closed the connection\n");
            int shut = shutdown(socket, SHUT_RDWR);
            if (shut == -1) {
                fprintf(stderr, "Error shutting down socket: %s\n", strerror(errno));
            } else {
                printf("Socket %d shut down\n", socket);
            }
            
            exit(1);
        }


        if (nb > 0) {
            total += nb;
        } else {
            fprintf(stderr, "Error receiving message from server: %s\n", strerror(errno));
            exit(1);
        }

        // printf("debug: [nb: %lu]  [total: %lu]\n", nb, total);
        //TODO: rework this
        printf("last: %c %d\n", buffer[total - 1], buffer[total - 1]);
        if (buffer[total - 1] == '\0') {
            break;
        }
    }

    printf("Received: (%ld bytes) [%s]\n", total, buffer);
    buffer[total - 1] = '\0';
    return total;
}


/**
 * @brief Send a message to a socket (UDP)
 * 
 * @param buffer 
 * @param socket 
 */
void   sendMsgUDP(const char *msg, int socket, struct sockaddr_in  *sockaddr) {
    size_t      msgLen;
    size_t      total;
    size_t      nb;
    size_t      max;

    msgLen = strlen(msg) + 1; // +1 for the null terminator
    printf("(UDP) msgLen: %zu\n", msgLen);

    total = 0;
    while (total != msgLen)
    {
        max = msgLen - total;

        // in case of error, we need to know how many bytes have been sent
        nb = sendto(socket, msg + total, max, 0, (struct sockaddr*)sockaddr, sizeof(struct sockaddr));
        // printf("(UDP) send: %s\n", msg + total);
        if (nb == -1)
        {
            fprintf(stderr, "Error sending message to server: %s\n", strerror(errno));
            exit(1);
        }
        total += nb;
        // printf("debug: [nb: %lu]  [total: %lu]\n", nb, total);
    }

    printf("(UDP) Sent: (%ld bytes) [%s]\n", total, msg);
}



/**
 * @brief Receive a message from a socket (UDP)
 * 
 * @param buffer 
 * @param socket 
 */
size_t    receiveMsgUDP(char *buffer, int socket, struct sockaddr_in  *clientaddr) {
    size_t     total;
    size_t     nb;
    size_t     max;
    socklen_t  addrlen;

    max = 1024;

    addrlen = sizeof(struct sockaddr);
    total = 0;
    while (true)
    {
        nb = recvfrom(socket, buffer + total, max, 0, (struct sockaddr*)clientaddr, &addrlen);
        if (nb == -1)
        {
            fprintf(stderr, "(UDP) Error receiving message from server: %s\n", strerror(errno));
            exit(1);
        }
        // if 0, we have received the whole message
        // TODO: check
        if (nb == 0)
        {
            fprintf(stderr, "Peer closed the connection\n");
            int shut = shutdown(socket, SHUT_RDWR);
            if (shut == -1) {
                fprintf(stderr, "Error shutting down socket: %s\n", strerror(errno));
            } else {
                printf("Socket %d shut down\n", socket);
            }
            exit(1);
        }


        if (nb > 0) {
            total += nb;
        } else {
            fprintf(stderr, "(UDP) Error receiving message from server: %s\n", strerror(errno));
            exit(1);
        }

        // printf("debug: [nb: %lu]  [total: %lu]\n", nb, total);
        //TODO: rework this
        printf("(UDP) last: %c %d\n", buffer[total - 1], buffer[total - 1]);
        if (buffer[total - 1] == '\0') {
            break;
        }
    }

    printf("(UDP) Received: (%ld bytes) [%s]\n", total, buffer);
    buffer[total - 1] = '\0';
    return total;
}

bool    checkUsername() {
    // get user name
    t_dialog            *dialog;

    // create dialog
    // if the dialog was just created, set props
    if (*getUsername() == 0) {
        puts("Creating dialog");
        dialog = getEditBox();
        if (dialog->text == NULL) {
            createEditBox("Enter your name", 20, colorWhite, colorBlack);
            dialog->callback = askUsernameCallback;
        }
        return false;
    }

    return true;
}

void    receiveMove(const char *content) {
    int             x;
    int             y;
    int             xCell;
    int             yCell;
    short           playerIndex;
    t_player        *player;
    unsigned short  direction; // t_direction

    sscanf(content, "%d %d %d %d %hu %hu", &x, &y, &xCell, &yCell, &direction, &playerIndex);

    player = getGame()->players[playerIndex];
    // //TODO: hashmap to get player by name

    if (player == NULL) {
        #ifdef DEBUG
            puts("Invalid player id");
        #endif
        return;
    }

    player->x = x;
    player->y = y;
    player->xCell = xCell;
    player->yCell = yCell;
    //TODO: remove x and y, use cells instead (differents for each player)
    //TMP
    posToGrid(player);
    player->direction = direction;

    // printf("Player %hu moved to %d %d\n", playerIndex, x, y);
}