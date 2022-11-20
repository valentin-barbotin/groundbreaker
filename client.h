#ifndef CLIENT_H
 #define CLIENT_H

 #include <netinet/in.h>

 #include "game.h"
 #include "player.h"
 #include "colors.h"
 #include "effects.h"

 extern t_gameConfig    *gameConfig;
 extern short           g_playersMultiIndex;
 extern char            **g_messages;
 extern size_t          g_messages_nb;

 void    broadcastMsg(const char *msg, int except);
 void    broadcastMsgUDP(const char *msg, short except);
 void    handleMessageClient(const char  *buffer, int client, const struct sockaddr_in  *sockaddr);
 void    handleMessageClient2(char *type, char *content);
 void    askUsernameCallback();
 void    askServerHost();
 void    askServerHostCallback();
 void    askServerPort();
 void    askServerPortCallback();
 void    joinServer();
 void    *connectToServer();
 void    *connectToServerUDP(void *arg);
 void    updateCell(unsigned short xCell, unsigned short yCell, t_type type);
 void    sendEffect(const t_effect *effect);

#endif