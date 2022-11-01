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
    PLAYERDAT,
    MYNAME
 }             t_message;

 typedef struct     s_peer
 {
    int                         socket;
    const struct sockaddr_in    *clientAddr;
    char                        name[256];
 }                  t_peer;
 

 void    sendToAll(const char *msg);
 void    handleMessageSrv(char  *buffer, int client, const struct sockaddr_in *clientAddr);
 void    handleClientUDP(int socket);
 void    *handleClient(void *clientSocket);
 void    launchServer();
 void    *createServerUDP(void *arg);
 void    *createServer();
 void    multiplayerStart();

#endif