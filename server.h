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
 }             t_message;

 void    handleMessageSrv(char  *buffer, int client);
 void    *handleClient(void *clientSocket);
 void    launchServer();
 void    *createServer();

#endif