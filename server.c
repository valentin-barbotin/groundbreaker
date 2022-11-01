#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/tcp.h>

#include "server.h"
#include "utils.h"
#include "game.h"
#include "player.h"

#define DEBUG true

bool        g_serverRunning = false;
bool        g_serverRunningUDP = false;
pthread_t   g_serverThread = NULL;
int         g_socketsList[3]; // 4 players max
int         g_socketsListNb = 0;
t_peer      *g_peersListUDP[3]; // 4 players max
int         g_peersListUDPNb = 0;


// TODO: use players list
void    sendToAll(const char *msg) {
    for (int i = 0; i < g_socketsListNb; i++) {
        sendMsg(msg, g_socketsList[i]);
    }
}

void    handleMessageSrv(char  *buffer, int client, const struct sockaddr_in *clientAddr) {
    char        *pos;
    char        type[128];
    char        *content;
    t_message   action;
    t_game      *game;

    pos = strchr(buffer, ':');
    if (pos == NULL) {
        #ifdef DEBUG
            puts("Invalid message (:)");
            puts(buffer);
            // exit(1);
        #endif
        return;
    }

    *pos = '\0';
    pos++;
    strcpy(type, buffer);
    #ifdef DEBUG
        printf("type: %s, msg: [%s]\n", type, pos);
    #endif

    if (stringIsEqual(type, "JOIN")) {
        action = JOIN;
    } else if (stringIsEqual(type, "MOVE")) {
        action = MOVE;
    } else if (stringIsEqual(type, "CHAT")) {
        action = CHAT;
    } else if (stringIsEqual(type, "READY")) {
        action = READY;
    } else if (stringIsEqual(type, "START")) {
        action = START;
    } else if (stringIsEqual(type, "QUIT")) {
        action = QUIT;
    } else if (stringIsEqual(type, "MYNAME")) {
        action = MYNAME;
    } else {
        #ifdef DEBUG
            printf("Invalid message type: [%s]\n", type);
            puts("Invalid message type");
        #endif
    }

    t_player    *player;
    game = getGame();
    switch (action)
    {
        case MYNAME:
            //TODO: check name
            t_peer  *peer;

            peer = malloc(sizeof(t_peer));
            if (peer == NULL) {
                #ifdef DEBUG
                    printf("Error: malloc failed for peer");
                #endif
                exit(1);
            }

            peer->socket = client;
            peer->clientAddr = clientAddr;
            strcpy(peer->name, pos);

            g_peersListUDP[g_peersListUDPNb++] = peer;
            break;
        case JOIN:
            player = initPlayer();
            strcpy(player->name, pos);
            player->socket = client;

            //TODO: mutex
            game->players[game->nbPlayers++] = player;
            break;
        case MOVE:
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
            printf("Waiting for message from client %d\n", client);
        #endif
        receiveMsg(buffer, client);

        #ifdef DEBUG
            printf("Received message from client %d: %s\n", client, buffer);
        #endif

        handleMessageSrv(buffer, client, NULL);
    } while (true);
    return NULL;
}

/**
 * @brief handle the client and react to his messages
 * 
 * @param socket 
 */
void   handleClientUDP(int socket) {
    char    buffer[1024];

    do
    {
        struct sockaddr_in  clientAddr;

        // receive message from client, wait if no message
        #ifdef DEBUG
            puts("(UDP) Waiting for message");
        #endif

        receiveMsgUDP(buffer, socket, &clientAddr);

        #ifdef DEBUG
            printf("(UDP) Received message [%s]\n",  buffer);
        #endif

        handleMessageSrv(buffer, socket, &clientAddr);
    } while (true);
}

void    launchServer() {
    if (g_serverRunning) {
        #ifdef DEBUG
            puts("Server already running");
        #endif
        return;
    }

    pthread_create(&g_serverThread, NULL, &createServer, "server");
    // for position
    pthread_create(&g_serverThread, NULL, &createServerUDP, "server");
}

/**
 * @brief Main function of the server (UDP)
 * We use a multi-threaded server to handle multiple clients in the same process
 * Must be used in a separate thread
 * @todo close the socket when the thread is going to be closed
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
void    *createServerUDP(void *arg) {
    if (g_serverRunningUDP) {
        #ifdef DEBUG
            puts("Server already running");
        #endif
        return NULL;
    }

    int                 serverSocket;
    struct sockaddr_in  serverAddress;
    unsigned short      port;


    port = (unsigned short) atoi(gameConfig->server.port);

    puts("(UDP) Starting server...");

    // create socket with IPv4 and UDP
    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket < 0) {
        #ifdef DEBUG
            perror("Error opening socket");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Can't open server", "Can't create socket", g_window);
        g_serverThread = NULL;
        return NULL;
    }


    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        #ifdef DEBUG
            perror("(UDP) Error on binding");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Can't open server", "Can't bind", g_window);
        g_serverThread = NULL;
        return NULL;
    }
    g_serverRunningUDP = true;

    printf("(UDP) Server started on port %d\n", port);

    handleClientUDP(serverSocket);
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
    unsigned short      port;

    port = atoi(gameConfig->server.port);

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

    setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, &(int){0}, sizeof(int));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        #ifdef DEBUG
            perror("Error on binding");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Can't open server", "Can't bind", g_window);
        g_serverThread = NULL;
        return NULL;
    }

    listen(serverSocket, 5);
    g_serverRunning = true;

    printf("Server started on port %d\n", port);

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


void    multiplayerStart() {
    char            buffer[1024];
    const t_game    *game;

    game = getGame();

    sprintf(buffer, "START:%hu %hu %hu$", game->map->height, game->map->width, game->nbPlayers);

    char *ptr = buffer + strlen(buffer);
    for (size_t i = 0; i < game->map->height; i++)
    {
        for (size_t j = 0; j < game->map->width; j++)
        {
            *ptr++ = game->map->map[i][j];
        }
    }

    *ptr = '\0';
    
    // send map to clients
    sendToAll(buffer);

    //send player to all, except admin (first player)
    for (size_t i = 1; i < game->nbPlayers; i++)
    {
        game->players[i]->x = 2;
        game->players[i]->y = 2;
        sprintf(buffer, "PLAYERDAT:%s %d %d%c", game->players[i]->name, game->players[i]->x, game->players[i]->y, '\0');
        puts(buffer);
        sendMsg(buffer, game->players[i]->socket);
    }
}
