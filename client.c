#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "client.h"
#include "utils.h"


/**
 * @brief Connect to the server
 * This function should be used in a separate thread
 * 
 */
void    connectToServer() {
    int                 sd;
    struct sockaddr_in  cl;

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    cl.sin_family = AF_INET;
    cl.sin_addr.s_addr = inet_addr(gameConfig->server.host);
    cl.sin_port = htons((uint16_t) atoi(gameConfig->server.port));

    if (connect(sd, (struct sockaddr *) &cl, sizeof(cl)) < 0) {
        perror("Error on binding");
        exit(EXIT_FAILURE);
    }

    do
    {
        // send message to server
        // receive message from server
        // handle message ('switch case')
    } while (true);
    

}