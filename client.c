#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "client.h"
#include "server.h"
#include "utils.h"
#include "config.h"
#include "dialog.h"
#include "player.h"
#include "game.h"

#define DEBUG true

pthread_t   g_clientThread = NULL;
pthread_t   g_clientThreadUDP = NULL;
int         g_serverSocket = 0;
int         g_serverSocketUDP = 0;

t_serverConfig g_serverConfig;

void    handleMessageClient(char  *buffer, int server) {
    char        *pos;
    char        type[128];
    char        *content;
    t_message   action;
    t_game      *game;

    pos = strchr(buffer, ':');
    if (pos == NULL) {
        #ifdef DEBUG
            puts("Invalid message");
        #endif
        return;
    }

    *pos = '\0';
    strcpy(type, buffer);

    content = pos + 1;
    #ifdef DEBUG
        printf("type: %s, msg: [%s]\n", type, content);
    #endif

    if (strcmp(type, "JOIN") == 0) {
        action = JOIN;
    } else if (strcmp(type, "MOVE") == 0) {
        action = MOVE;
    } else if (strcmp(type, "CHAT") == 0) {
        action = CHAT;
    } else if (strcmp(type, "READY") == 0) {
        action = READY;
    } else if (strcmp(type, "START") == 0) {
        action = START;
    } else if (strcmp(type, "QUIT") == 0) {
        action = QUIT;
    } else {
        #ifdef DEBUG
            puts("Invalid message type");
        #endif
    }

    t_player    *player;
    game = getGame();
    switch (action)
    {
        case START: {
            //launch game
            unsigned short h;
            unsigned short w;
            t_map          *map;
            char           *ptr;
            sscanf(content, "%hu %hu $", &h, &w);
            printf("h: %hu, w: %hu\n", h, w);

            if (!h || !w) {
                puts("Invalid map size");
                return;
            }

            ptr = strchr(content, '$');
            if (ptr == NULL) {
                #ifdef DEBUG
                    puts("Invalid message");
                #endif
                return;
            }
            ptr++;

            map = map_create(w, h);
            // note: players

            for (int i = 0; i < h; i++) {
                strncat(map->map[i], ptr + w * i, w);
            }

            game->map = map;
            game->map->players = 4; //TODO

            puts("Game started");
            map_print(game->map);

            spawnPlayer();
            g_currentState = GAME_PLAY_PLAYING;
        }
            break;
        
        default:
            break;
    }
    
    memset(buffer, 0, 1024);
}

void    askUsernameCallback() {
    //TODO: check if host is valid
    t_player    *player;

    player = getPlayer();

    if (strlen(getEditBox()->edit) == 0) {
        #ifdef DEBUG
            puts("No username entered");
        #endif
        return;
    }

    strcpy(player->name, getEditBox()->edit);
    destroyEditBox();
}

void    askServerHost() {
    t_dialog  *dialog;

    dialog = getEditBox();

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->text = "Enter server IP/host:";
    dialog->callback = askServerHostCallback;
}

void    askServerHostCallback() {
    //TODO: check if host is valid
    strcpy(g_serverConfig.host, getEditBox()->edit);
    askServerPort();
}

void    askServerPort() {
    t_dialog  *dialog;

    dialog = getEditBox();

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->text = "Enter server PORT:";
    dialog->callback = askServerPortCallback;
}

void    askServerPortCallback() {
    //TODO: check if port is valid

    strcpy(g_serverConfig.port, getEditBox()->edit);

    destroyEditBox();

    pthread_create(&g_clientThread, NULL, &connectToServer, "client");
    pthread_create(&g_clientThreadUDP, NULL, &connectToServerUDP, "client");
}




void    joinServer() {
    bool    valid = false;
    if (g_clientThread != NULL) {
        #ifdef DEBUG
            puts("Client already running");
        #endif
        return;
    }

    valid = checkUsername();
    if (!valid) return;

    createEditBox("", 20, (SDL_Color) {0, 255, 0, 255}, (SDL_Color) {255, 255, 255, 255});
    SDL_StartTextInput();
    askServerHost();
}

/**
 * @brief Connect to the server
 * This function should be used in a separate thread
 * 
 */
void    *connectToServer(void *arg) {
    struct sockaddr_in  cl;
    char                buffer[1024];

    // thread does not need to be joined
    pthread_detach(pthread_self());

    printf("Connecting to server %s:%s\n", g_serverConfig.host, g_serverConfig.port);

    puts("Connecting to server...");
    g_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    cl.sin_family = AF_INET;
    cl.sin_addr.s_addr = inet_addr(g_serverConfig.host);
    cl.sin_port = htons((uint16_t) atoi(g_serverConfig.port));

    int res = connect(g_serverSocket, (struct sockaddr *)&cl, sizeof(cl));
    if (res < 0) {
        #ifdef DEBUG
            perror("Error connecting to server");
            fprintf(stderr, "Error connecting to server: %s", strerror(res));
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Can't connect", "Can't connect to server", g_window);
        g_clientThread = NULL;
        return NULL;
    }

    puts("Connected to server");

    sprintf(buffer, "JOIN:%s", getPlayer()->name);
    sendMsg(buffer, g_serverSocket);
    printf("Sent message to server: %s\n", buffer);

    do
    {
        // receive message from client, wait if no message
        #ifdef DEBUG
            puts("Waiting for message from server");
        #endif

        receiveMsg(buffer, g_serverSocket);

        handleMessageClient(&buffer, g_serverSocket);
    } while (true);
    

}


void    *connectToServerUDP(void *arg) {
    struct sockaddr_in  cl;
    char                buffer[1024];

    // thread does not need to be joined
    pthread_detach(pthread_self());

    printf("(UDP) Connecting to server %s:%s\n", g_serverConfig.host, g_serverConfig.port);

    puts("(UDP) Connecting to server...");
    g_serverSocketUDP = socket(AF_INET, SOCK_DGRAM, 0);
    cl.sin_family = AF_INET;
    cl.sin_addr.s_addr = inet_addr(g_serverConfig.host);
    cl.sin_port = htons((uint16_t) atoi(g_serverConfig.port));

    int res = connect(g_serverSocketUDP, (struct sockaddr *)&cl, sizeof(cl));
    if (res < 0) {
        #ifdef DEBUG
            perror("(UDP) Error connecting to server");
            fprintf(stderr, "(UDP) Error connecting to server: %s", strerror(res));
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Can't connect", "Can't connect to server", g_window);
        g_clientThreadUDP = NULL;
        return NULL;
    }

    puts("Connected to server");

    do
    {
        // receive message from client, wait if no message
        #ifdef DEBUG
            puts("Waiting for message from server");
        #endif

        receiveMsgUDP(buffer, g_serverSocket, &cl);

        handleMessageClient(&buffer, g_serverSocket);
    } while (true);
}
