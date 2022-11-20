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
#include "client.h"
#include "tchat.h"

#define DEBUG true

bool        g_serverRunning = false;
bool        g_serverRunningUDP = false;
pthread_t   g_serverThread = 0;
t_peer      *g_peersList[4]; // 4 players max
int         g_peersListNb = 0;
t_peer      *g_peersListUDP[4]; // 4 players max
int         g_peersListUDPNb = 0;


// TODO: use players list
void    sendToAll(const char *msg, int except) {
    if (!inMultiplayer()) return;

    const char    *pos;

    if (g_serverRunning) {
        for (int i = 0; i < g_peersListNb; i++) {
            if (g_peersList[i]->id != except) {
                printf("Sending to [%d, %s]: %s\n", i, g_peersList[i]->name, msg);
                sendMsg(msg, g_peersList[i]->socket);
            }
        }

        if (except != 0) {
            pos = strstr(msg, "BROADCAST");
            if (!pos) {
                printf("TCP LOCAL HANDLE: [%s]\n", msg);
                handleMessageSrv(msg, 0, NULL); // dummy values
            }
        }

        return;
    }
    //let the server broadcast the message
    broadcastMsg(msg, except);
}

void    sendToAllUDP(const char *msg, short except) {
    const char    *pos;

    if (g_serverRunningUDP) {
        for (int i = 0; i < g_peersListUDPNb; i++) {
            if (g_peersListUDP[i]->id != except) {
                // printf("Sending to [%d, %s]: %s\n", i, g_peersListUDP[i]->name, msg);
                sendMsgUDP(msg, g_peersListUDP[i]->socket, g_peersListUDP[i]->clientAddr);
            }
        }

        if (except != 0) {
            //check if the message is a broadcast
            pos = strstr(msg, "BROADCAST");
            if (!pos) {
                // printf("UDP LOCAL HANDLE: [%s]\n", msg);
                handleMessageSrv(msg, 0, NULL); // dummy values
            }
        }

        return;
    }
    //let the server broadcast the message
    broadcastMsgUDP(msg, except);
}

void    addPeer(int socket, const struct sockaddr_in *clientAddr, const char *name, unsigned short id) {
    t_peer  *peer;

    peer = malloc(sizeof(t_peer));
    if (peer == NULL) {
        #if DEBUG
            printf("Error: malloc failed for peer");
        #endif
        exit(1);
    }

    peer->socket = socket;
    strcpy(peer->name, name);
    peer->id = id;

    // UDP peer
    if (clientAddr != NULL) {
        peer->clientAddr = clientAddr;
        g_peersListUDP[g_peersListUDPNb++] = peer;
    } else {
        g_peersList[g_peersListNb++] = peer;
    }

}

void    handleMessageSrv(char  *buffer, int client, const struct sockaddr_in *clientAddr) {
    char        type[128];
    char        *content;

    content = strchr(buffer, ':');
    if (content == NULL) {
        #if DEBUG
            printf("1 Invalid message (:) [%s]\n", buffer);
            // exit(1);
        #endif
        return;
    }

    *content = '\0';
    content++;
    strcpy(type, buffer);
    #if DEBUG
        printf("type: %s, msg: [%s]\n", type, content);
    #endif

    handleMessageSrv2(type, content, client, clientAddr);
}

void    handleMessageSrv2(char *type, char *content, int client, const struct sockaddr_in *clientAddr) {
    t_message   action;
    t_game      *game;
    t_player    *player;

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
    } else if (stringIsEqual(type, "BROADCAST")) {
        action = BROADCAST;
    } else if (stringIsEqual(type, "CELL")) {
        action = CELL;
    } else if (stringIsEqual(type, "EFFECT")) {
        action = EFFECT;
    } else if (stringIsEqual(type, "DAMAGE")) {
        action = DAMAGE;
    } else if (stringIsEqual(type, "RESPAWN")) {
        action = RESPAWN;
    } else if (stringIsEqual(type, "LIFE")) {
        action = LIFE;
    } else if (stringIsEqual(type, "END")) {
        action = END;
    } else if (stringIsEqual(type, "TCHAT")) {
        action = TCHAT;
    } else {
        #if DEBUG
            printf("Invalid message type: [%s]\n", type);
            puts("Invalid message type");
        #endif
    }

    game = getGame();
    switch (action)
    {
        case TCHAT: {
            // we update the tchat to display the message for all players
            updateTchatMessages(content);
        }
        case END:
            receiveEndGame(content);
            break;
        case CELL:
            cellUpdate(content);
            break;
        case LIFE:
            receiveLife(content);
            break;
        case EFFECT:
            receiveEffect(content);
            break;
        case MOVE:
            receiveMove(content);
            break;
        case RESPAWN: {
            short       id;
            int         xCell;
            int         yCell;

            sscanf(content, "%hd %d %d", &id, &xCell, &yCell);
            player = game->players[id];

            player->health = 100;    
        }
            break;
        case DAMAGE: {
            receiveDamage(content);
            break;
        }
        case BROADCAST: {
            char    *content2;
            short   except;

            content2 = strchr(content, ' ');
            if (content2 == NULL) {
                #if DEBUG
                    printf("2 Invalid message (:) [%s]\n", content);
                    // exit(1);
                #endif
                return;
            }

            *content2 = '\0';
            content2++;

            except = atoi(content);
            printf("Broadcasting: (except: %hd) %s\n", except, content2);
            // if clientAddr is not null, it's a UDP message
            clientAddr ? sendToAllUDP(content2, except) : sendToAll(content2, except);
            // handleMessageClient(content, client, NULL);
            break;
        }
        case MYNAME:
            //TODO: check name
            addPeer(client, clientAddr, content, game->nbPlayers); // a client connect to the server in TCP and UDP in the same time, no guarantee that the client has the same id in both protocol.. 
            break;
        case JOIN:
            //TODO: mutex ?
            printf("JOIN: %s\n", content);
            printf("game->nbPlayers: %d\n", game->nbPlayers);
            strcpy(game->players[game->nbPlayers]->name, content);
            // printf("2 - game->nbPlayers: %d\n", game->nbPlayers);

            addPeer(client, clientAddr, content, game->nbPlayers);
            game->nbPlayers++;

            // update players list for each pear
            sendPlayersToAll();

            break;
        case PLAYERDAT:
            //TODO: avoid loopback
            printf("ignore -- PLAYERDAT: [%s]\n", content);
            break;
        
        default:
            break;
    }
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
    char                buffer[1024];
    char                *ptr;
    size_t              total;
    size_t              len;

    int     client = *(int *)clientSocket;
    ptr = NULL;

    #if DEBUG
        printf("Client socket: %d\n", client);
    #endif

    do
    {
        // receive message from client, wait if no message
        #if DEBUG
            puts("Waiting for message from client");
        #endif

        total = receiveMsg(buffer, client);
        // check if there is a single message in the buffer
        // if there is more than one message, we need to split the buffer
        // and process each message separately
        
        ptr = buffer;
        if (ptr == NULL) {
            printf("ptr is NULL, total: %lu\n", total);
        }
        len = 0;
        do
        {
            ptr = buffer + len;
            len += (strlen(ptr) + 1);
            // printf("ptr: char:%d %s\n", *ptr, ptr);
            // printf("len: %lu, total: %lu\n", len, total);

            handleMessageSrv(ptr, client, NULL);

        } while (len != total);
        // printf("len == total, %lu  %lu\n", len, total);
        
        memset(buffer, 0, 1024);
    } while (true);
    return NULL;
}

/**
 * @brief handle the client and react to his messages
 * 
 * @param socket 
 */
void   handleClientUDP(int socket) {
    char                buffer[1024];
    char                *ptr;
    size_t              total;
    size_t              len;

    ptr = NULL;
    do
    {
        struct sockaddr_in  clientAddr;

        // receive message from client, wait if no message
        #if DEBUG
            puts("(UDP) Waiting for message");
        #endif

        total = receiveMsgUDP(buffer, socket, &clientAddr);
        // check if there is a single message in the buffer
        // if there is more than one message, we need to split the buffer
        // and process each message separately
        
        ptr = buffer;
        if (ptr == NULL) {
            printf("ptr is NULL, total: %lu\n", total);
        }
        len = 0;
        do
        {
            ptr = buffer + len;
            len += (strlen(ptr) + 1);

            handleMessageSrv(ptr, socket, &clientAddr);

        } while (len != total);

        // #if DEBUG
        //     printf("(UDP) Received message [%s]\n",  buffer);
        // #endif

        // handleMessageSrv(buffer, socket, &clientAddr);
        memset(buffer, 0, 1024);
    } while (true);
}

void    launchServer() {
    if (g_serverRunning) {
        #if DEBUG
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
void    *createServerUDP(const void *arg) {
    if (g_serverRunningUDP) {
        #if DEBUG
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
        #if DEBUG
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
        #if DEBUG
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
void    *createServer(const void *arg) {
    if (g_serverRunning) {
        #if DEBUG
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
    char                buffer[1024];

    port = atoi(gameConfig->server.port);

    puts("Starting server...");

    // create socket with IPv4 and TCP
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        #if DEBUG
            perror("Error opening socket");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Can't open server", "Can't create socket", g_window);
        g_serverThread = NULL;
        return NULL;
    }

    // setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, &(int){0}, sizeof(int));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        #if DEBUG
            perror("Error on binding");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Can't open server", "Can't bind", g_window);
        g_serverThread = NULL;
        return NULL;
    }

    sprintf(buffer, "Server started on port %d", port);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game", buffer, g_window);

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

    strcpy(game->players[0]->name, getUsername());
    game->players[0]->id = 0;

    for (size_t i = 0; i < game->nbPlayers; i++)
    {
        t_player *player = game->players[i];
        //TODO: set default values
        initInventory(player->inventory);
        player->scope = 2;
        player->maxBombs = 2;
        player->lives = 2;

        putPlayerInFreeCell(player);
        player->id = i;
        printf("(init) spawned player %s at %d, %d\n", player->name, player->xCell, player->yCell);
    }

    // send player to all (except us)
    sendPlayersToAll();

    // send map to all (except us)
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
    sendToAll(buffer, getPlayer()->id);
}


void    multiplayerEnd(short winner) {
    char buffer[256];

    sprintf(buffer, "END:%hd", winner);
    sendToAll(buffer, getPlayer()->id);

    // avoid loopback
    receiveEndGame(buffer + 4); // +4 to skip "END:"
}

void    sendPlayersToAll() {
    const t_game    *game;
    char            buffer[1024];

    game = getGame();

    for (short i = 0; i < game->nbPlayers; i++)
    {
        if (*game->players[i]->name == 0) {
            printf("Invalid name for user %hu\n", i);
            exit(1);
        }
        sprintf(buffer, "PLAYERDAT:%s %d %d %hu%c", game->players[i]->name, game->players[i]->xCell, game->players[i]->yCell, i, '\0');
        puts(buffer);
        sendToAll(buffer, getPlayer()->id);
    }
}