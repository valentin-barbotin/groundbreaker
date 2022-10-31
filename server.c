#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "server.h"
#include "utils.h"
#include "game.h"
#include "player.h"

#define DEBUG true

bool        g_serverRunning = false;
pthread_t   g_serverThread = NULL;
int         g_socketsList[3]; // 4 players max
int         g_socketsListNb = 0;

void    sendToAll(const char *msg) {
    for (int i = 0; i < g_socketsListNb; i++) {
        sendMsg(msg, g_socketsList[i]);
    }
}

void    handleMessageSrv(char  *buffer, int client) {
    char        *pos;
    char        type[128];
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
    #ifdef DEBUG
        printf("type: %s, msg: [%s]\n", type, pos + 1);
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
        case JOIN:
            player = initPlayer();
            strcpy(player->name, pos + 1);

            //TODO: mutex
            game->players[game->nbPlayers++] = player;
            break;
        
        default:
            break;
    }
    
    memset(buffer, 0, 1024);
}

/**
 * @brief handle the client and react to his messages
 * 
 * @param clientSocket 
 * @return void* 
 */
void   *handleClient(void *clientSocket) {
    // thread does not need to be joined
    pthread_detach(pthread_self());
    char    buffer[1024];

    int     client = *(int *)clientSocket;

    #ifdef DEBUG
        printf("Client socket: %d\n", client);
    #endif

    do
    {
        // receive message from client, wait if no message
        #ifdef DEBUG
            printf("Waiting for message from client %d", client);
        #endif
        receiveMsg(buffer, client);

        #ifdef DEBUG
            printf("Received message from client %d: %s\n", client, buffer);
        #endif

        handleMessageSrv(&buffer, client);
    } while (true);
    return NULL;
}

void    launchServer() {
    if (g_serverRunning) {
        #ifdef DEBUG
            puts("Server already running");
        #endif
        return;
    }

    pthread_create(&g_serverThread, NULL, &createServer, "server");
}

/**
 * @brief Main function of the server
 * We use a multi-threaded server to handle multiple clients in the same process
 * Must be used in a separate thread
 * @todo close the socket when the thread is going to be closed
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
void    *createServer(void *arg) {
    if (g_serverRunning) {
        #ifdef DEBUG
            puts("Server already running");
        #endif
        return NULL;
    }

    int                 serverSocket;
    struct sockaddr_in  serverAddress;
    struct sockaddr_in  clientAddress;
    int                 clientSocket;
    socklen_t           clientAddressLength;
    t_game              *game;           

    game = getGame();


    puts("Starting server...");

    // create socket with IPv4 and TCP
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        #ifdef DEBUG
            perror("Error opening socket");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Can't open server", "Can't create socket", g_window);
        g_serverThread = NULL;
        return NULL;
    }

    g_serverRunning = true;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(gameConfig->server.host);
    serverAddress.sin_port = htons((uint16_t) atoi(gameConfig->server.port));

    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        #ifdef DEBUG
            perror("Error on binding");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Can't open server", "Can't bind", g_window);
        g_serverThread = NULL;
        return NULL;
    }

    listen(serverSocket, 5);

    // create thread for each client
    while (true)
    {
        int thstate;

        puts("Waiting for client...");
        // wait for client to connect
        clientAddressLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientAddressLength);
        if (clientSocket < 0) {
            perror("Error on accept");
            exit(EXIT_FAILURE);
        }

        if (clientSocket < 0) {
            perror("Error on accept");
            exit(EXIT_FAILURE);
        }

        puts("Received connection from client");

        g_socketsList[g_socketsListNb++] = clientSocket;

        // create thread and handle client (infinite loop with recv and send)
        pthread_t thread;
        thstate = pthread_create(&thread, NULL, handleClient, &clientSocket);
        
        if (thstate != 0) {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }

    }
}