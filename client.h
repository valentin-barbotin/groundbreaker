#ifndef CLIENT_H
 #define CLIENT_H

 #include <netinet/in.h>

 #include "game.h"
 extern t_gameConfig    *gameConfig;

 void    broadcastMsg(const char *msg);
 void    broadcastMsgUDP(const char *msg);
 void    handleMessageClient(char  *buffer, int client, const struct sockaddr_in  *sockaddr);
 void    askUsernameCallback();
 void    askServerHost();
 void    askServerHostCallback();
 void    askServerPort();
 void    askServerPortCallback();
 void    joinServer();
 void    *connectToServer();
 void    *connectToServerUDP(void *arg);

#endif