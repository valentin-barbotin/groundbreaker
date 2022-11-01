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

pthread_t           g_clientThread = NULL;
pthread_t           g_clientThreadUDP = NULL;
int                 g_serverSocket = 0;
int                 g_serverSocketUDP = 0;

t_serverConfig g_serverConfig;

/**
 * @brief Handle the message received from the server
 * if sockaddr is not null, it's a UDP message, else it's a TCP message
 * 
 * @param buffer 
 * @param server 
 * @param sockaddr 
 */
void    handleMessageClient(char  *buffer, int server, const struct sockaddr_in  *sockaddr) {
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
        return;
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
        case START: {
            //launch game
            unsigned short h;
            unsigned short w;
            unsigned short players;
            t_map          *map;
            char           *ptr;
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
                return;
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

            spawnPlayer(0, 0);
            printf("spawned player at %d, %d\n", 0, 0);

        }
            break;
        
        case PLAYERDAT: {
            player = game->players[game->nbPlayers++];
            sscanf(content, "%s %d %d", player->name, &player->xCell, &player->yCell);

            printf("player %s at %d, %d\n", player->name, player->xCell, player->yCell);

            if (game->nbPlayers == game->map->players) {
                puts("All players are in the game");
                printf("game->nbPlayers: %d, game->map->players: %d\n", game->nbPlayers, game->map->players);
                g_currentState = GAME_PLAY_PLAYING;
            }
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

    setsockopt(g_serverSocket, IPPROTO_TCP, TCP_NODELAY, &(int){0}, sizeof(int));

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

    sprintf(buffer, "JOIN:%s%c", getPlayer()->name, '\0');
    sendMsg(buffer, g_serverSocket);

    do
    {
        // receive message from client, wait if no message
        #ifdef DEBUG
            puts("Waiting for message from server");
        #endif

        receiveMsg(buffer, g_serverSocket);

        handleMessageClient(&buffer, g_serverSocket, NULL);
        memset(buffer, 0, 1024);
    } while (true);
    

}


void    *connectToServerUDP(void *arg) {
    struct sockaddr_in  cl;
    char                buffer[1024];
    unsigned            short  port;

    port = (unsigned short) atoi(g_serverConfig.port);

    // thread does not need to be joined
    pthread_detach(pthread_self());

    printf("(UDP) Connecting to server %s:%d\n", g_serverConfig.host, port);

    puts("(UDP) Connecting to server...");
    g_serverSocketUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    cl.sin_family = AF_INET;
    cl.sin_addr.s_addr = inet_addr(g_serverConfig.host);
    cl.sin_port = htons(port);

    // int res = connect(g_serverSocketUDP, (struct sockaddr *)&cl, sizeof(cl));
    // if (res < 0) {
    //     #ifdef DEBUG
    //         perror("(UDP) Error connecting to server");
    //         fprintf(stderr, "(UDP) Error connecting to server: %s", strerror(res));
    //     #endif
    //     SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Can't connect", "Can't connect to server", g_window);
    //     g_clientThreadUDP = NULL;
    //     return NULL;
    // }

    puts("(UDP) Connected to server");

    sprintf(buffer, "MYNAME:%s%c", getPlayer()->name, '\0');
    sendMsgUDP(buffer, g_serverSocketUDP, &cl);
    memset(buffer, 0, 1024);

    do
    {
        struct sockaddr_in  sockaddr;

        // receive message from client, wait if no message
        #ifdef DEBUG
            puts("Waiting for message from server");
        #endif

        receiveMsgUDP(buffer, g_serverSocketUDP, &sockaddr);

        handleMessageClient(&buffer, g_serverSocketUDP, &sockaddr);
        memset(buffer, 0, 1024);
    } while (true);
}
