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
#include "utils.h"
#include "config.h"
#include "dialog.h"
#include "player.h"

#define DEBUG true

pthread_t   g_clientThread = NULL;
int         g_serverSocket = 0;

t_serverConfig g_serverConfig;

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
    g_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    cl.sin_family = AF_INET;
    cl.sin_addr.s_addr = inet_addr(g_serverConfig.host);
    cl.sin_port = htons((uint16_t) atoi(g_serverConfig.port));

    int res = connect(g_serverSocket, (struct sockaddr *)&cl, sizeof(cl));
    if (res < 0) {
        #ifdef DEBUG
            fprintf(stderr, "Error connecting to server: %s", strerror(res));
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Can't connect", "Can't connect to server", g_window);
        return NULL;
    }

    puts("Connected to server");

    sprintf(buffer, "JOIN:%s", getPlayer()->name);
    sendMsg(buffer, g_serverSocket);
    printf("Sent message to server: %s\n", buffer);

    do
    {
        // send message to server
        // receive message from server
        // handle message ('switch case')
    } while (true);
    

}