#ifndef SERVER_H
 #define SERVER_H

 #include "game.h"
 extern t_gameConfig    *gameConfig;

 void    *handleClient(void *clientSocket);
 void    createServer();

#endif