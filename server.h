#ifndef SERVER_H
 #define SERVER_H

 #include "game.h"
 extern t_gameConfig    *gameConfig;

 typedef enum   e_message
 {
    JOIN,
    MOVE,
    CHAT,
    READY,
    START,
    QUIT,
    PLAYERDAT
 }             t_message;

 void    sendToAll(const char *msg);
 void    handleMessageSrv(char  *buffer, int client);
 void    handleClientUDP(int socket, struct sockaddr_in *clientAddr);
 void    *handleClient(void *clientSocket);
 void    launchServer();
 void    *createServerUDP(void *arg);
 void    *createServer();

#endif