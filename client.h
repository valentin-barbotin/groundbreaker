#ifndef CLIENT_H
 #define CLIENT_H

 #include <netinet/in.h>

 #include "game.h"
 #include "player.h"
 #include "colors.h"
 extern t_gameConfig    *gameConfig;
 extern short           g_playersMultiIndex;

 void    broadcastMsg(const char *msg);
 void    broadcastMsgUDP(const char *msg);
 void    handleMessageClient(const char  *buffer, int client, const struct sockaddr_in  *sockaddr);
 void    askUsernameCallback();
 void    askServerHost();
 void    askServerHostCallback();
 void    askServerPort();
 void    askServerPortCallback();
 void    joinServer();
 void    *connectToServer();
 void    *connectToServerUDP(void *arg);

#endif