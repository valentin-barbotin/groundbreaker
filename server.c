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

#define DEBUG true

bool        g_serverRunning = false;
pthread_t   g_serverThread = NULL;

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
        printf("Client socket: %d", client);
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

        // handle message ('switch case')

        // send message to client
        #ifdef DEBUG
            printf("Sending message to client %d", client);
        #endif
        strcpy(buffer, "Message received");
        sendMsg(buffer, client);
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

    g_serverRunning = true;

    puts("Starting server...");

    // create socket with IPv4 and TCP
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(gameConfig->server.host);
    serverAddress.sin_port = htons((uint16_t) atoi(gameConfig->server.port));

    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error on binding");
        exit(EXIT_FAILURE);
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

        // create thread and handle client (infinite loop with recv and send)
        pthread_t thread;
        thstate = pthread_create(&thread, NULL, handleClient, &clientSocket);
        
        if (thstate != 0) {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }

    }
}