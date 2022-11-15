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
#include <netinet/tcp.h>

#include "client.h"
#include "server.h"
#include "utils.h"
#include "config.h"
#include "dialog.h"
#include "player.h"
#include "game.h"

#define DEBUG true

pthread_t           g_clientThread = 0;
pthread_t           g_clientThreadUDP = 0;
int                 g_serverSocket = 0;
int                 g_serverSocketUDP = 0;
struct sockaddr_in  *g_serverAddrUDP = NULL;


t_serverConfig g_serverConfig;

void    broadcastMsg(const char *msg) {
    char    buffer[1024];

    sprintf(buffer, "BROADCAST:%s", msg);
    sendMsg(buffer, g_serverSocket);
}

void    broadcastMsgUDP(const char *msg) {
    char    buffer[1024];

    sprintf(buffer, "BROADCAST:%s", msg);
    sendMsgUDP(buffer, g_serverSocketUDP, g_serverAddrUDP);
}

/**
 * @brief Handle the message received from the server
 * if sockaddr is not null, it's a UDP message, else it's a TCP message
 * 
 * @param buffer 
 * @param server 
 * @param sockaddr 
 */
void    handleMessageClient(const char  *buffer, int server, const struct sockaddr_in  *sockaddr) {
    char        *pos;
    char        type[128];
    char        *content;
    t_message   action;
    t_game      *game;

    pos = strchr(buffer, ':');
    if (pos == NULL) {
        #ifdef DEBUG
            puts("Invalid message (: client)");
            exit(1);
        #endif
    }

    *pos = '\0';
    pos++;
    strcpy(type, buffer);

    content = pos;
    #ifdef DEBUG
        printf("type: %s, msg: [%s]\n", type, content);
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
    } else if (stringIsEqual(type, "PLAYERDAT")) {
        action = PLAYERDAT;
    } else {
        #ifdef DEBUG
            puts("Invalid message type");
        #endif
    }

    t_player    *player;
    game = getGame();
    switch (action)
    {
        case MOVE:
            receiveMove(content);
            break;
        case START: {
            //launch game
            unsigned short  h;
            unsigned short  w;
            unsigned short  players;
            t_map           *map;
            const char      *ptr;
            sscanf(content, "%hu %hu %hu $", &h, &w, &players);
            printf("h: %hu, w: %hu, nb: %hu\n", h, w, players);

            if (!h || !w) {
                puts("Invalid map size");
                return;
            }

            ptr = strchr(content, '$');
            if (ptr == NULL) {
                #ifdef DEBUG
                    puts("Invalid message ($ client)");
                    exit(1);
                #endif
            }
            ptr++;

            map = map_create(w, h);
            // note: players

            for (int i = 0; i < h; i++) {
                strncat(map->map[i], ptr + w * i, w);
            }

            game->map = map;
            // amount of players
            game->map->players = players; //TODO

            game->nbPlayers = players;

            for (size_t i = 0; i < players; i++)
            {
                player = game->players[i];
                printf("(init) spawned player %s at %d, %d\n", player->name, player->xCell, player->yCell);
                spawnPlayer(player->xCell, player->yCell, player);
            }
            
            g_currentState = GAME_PLAY_PLAYING;
        }
            break;
        
        case PLAYERDAT: {
            // n, index in the players list is shared by the server
            short           n;
            int             xCell;
            int             yCell;
            char            name[256];
            sscanf(content, "%s %d %d %hu", name, &xCell, &yCell, &n);

            if (*name == 0) {
                puts("Name empty");
                exit(1);
            }

            printf("Params received: %s %d %d %hu\n", name, xCell, yCell, n);

            player = game->players[n];

            if (stringIsEqual(g_username, name)) {
                printf("set g_playersMultiIndex to %hu\n", n);
                g_playersMultiIndex = n;
                strcpy(game->players[n]->name, g_username);
            }

            game->nbPlayers++;

            strcpy(player->name, name);
            player->xCell = xCell;
            player->yCell = yCell;
            player->direction = DIR_IDLE;

            printf("player[%hu] %s at %d, %d\n", n, player->name, player->xCell, player->yCell);

            // if the player is the current player, spawn it
            printf("PLAYER name: %s\n", player->name);
        }
            break;
        
        default:
            break;
    }
}

void    askUsernameCallback() {
    //TODO: check if host is valid

    if (strlen(getEditBox()->edit) == 0) {
        #ifdef DEBUG
            puts("No username entered");
        #endif
        return;
    }

    printf("put username [%s] in g_username\n", getEditBox()->edit);
    strcpy(getUsername(), getEditBox()->edit);

    strcpy(getGame()->players[0]->name, getUsername());
    printf("SET PLAYER 0 NAME TO %s\n", g_username);

    printf("g_username: %s\n", getUsername());
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

    //Put client in the lobby
    g_currentState = GAME_MAINMENU_PLAY;
}




void    joinServer() {
    bool    valid = false;
    if (g_clientThread != 0) {
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
    const char          *ptr;
    size_t              total;
    size_t              len;

    ptr = NULL;
    // thread does not need to be joined
    pthread_detach(pthread_self());

    printf("Connecting to server %s:%s\n", g_serverConfig.host, g_serverConfig.port);

    puts("Connecting to server...");
    g_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    cl.sin_family = AF_INET;
    cl.sin_addr.s_addr = inet_addr(g_serverConfig.host);
    cl.sin_port = htons((uint16_t) atoi(g_serverConfig.port));

    // setsockopt(g_serverSocket, IPPROTO_TCP, TCP_NODELAY, &(int){0}, sizeof(int));

    int res = connect(g_serverSocket, (struct sockaddr *)&cl, sizeof(cl));
    if (res < 0) {
        #ifdef DEBUG
            perror("Error connecting to server");
            fprintf(stderr, "Error connecting to server: %s", strerror(res));
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Can't connect", "Can't connect to server", g_window);
        g_clientThread = 0;
        return NULL;
    }

    puts("Connected to server");

    sprintf(buffer, "JOIN:%s%c", g_username, '\0');
    sendMsg(buffer, g_serverSocket);
    do
    {
        // receive message from client, wait if no message
        #ifdef DEBUG
            puts("Waiting for message from server");
        #endif

        total = receiveMsg(buffer, g_serverSocket);
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

            handleMessageClient(ptr, g_serverSocket, NULL);

        } while (len != total);
        

        memset(buffer, 0, 1024);
    } while (true);
    

}


void    *connectToServerUDP(void *arg) {
    struct sockaddr_in  cl;
    unsigned            short  port;
    char                buffer[1024];
    char                *ptr = NULL;
    size_t              total;
    size_t              len;

    port = (unsigned short) atoi(g_serverConfig.port);

    // thread does not need to be joined
    pthread_detach(pthread_self());

    printf("(UDP) Connecting to server %s:%d\n", g_serverConfig.host, port);

    puts("(UDP) Connecting to server...");
    g_serverSocketUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    cl.sin_family = AF_INET;
    cl.sin_addr.s_addr = inet_addr(g_serverConfig.host);
    cl.sin_port = htons(port);

    g_serverAddrUDP = &cl;

    puts("(UDP) Connected to server");

    sprintf(buffer, "MYNAME:%s%c", g_username, '\0');
    sendMsgUDP(buffer, g_serverSocketUDP, &cl);
    memset(buffer, 0, 1024);

    do
    {
        struct sockaddr_in  sockaddr;

        // receive message from client, wait if no message
        #ifdef DEBUG
            puts("Waiting for message from server");
        #endif

        total = receiveMsgUDP(buffer, g_serverSocketUDP, &sockaddr);
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

            printf("len: %lu, total: %lu\n\n", len, total);
            printf("ptr: %s\n", ptr);
            handleMessageClient(ptr, g_serverSocketUDP, &sockaddr);

        } while (len != total);

        memset(buffer, 0, 1024);
    } while (true);
}
