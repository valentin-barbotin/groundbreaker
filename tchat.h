#ifndef TCHAT_H
 #define TCHAT_H

 #include "player.h"
 extern int              g_serverSocket;

 void       drawTchat();
 void       askTchatMessage();
 void       askTchatMessageCallback(const char *msg);
 void       putMessageInTchat(t_player *player, const char *msg);
 void       drawTchatMessages();
 void       updateTchatMessages(char *msg);

#endif
