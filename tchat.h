#ifndef TCHAT_H
 #define TCHAT_H

#include "player.h"

 void       drawTchat();
 void       askTchatMessage();
 void       askTchatMessageCallback(const char *msg);
 void       putMessageInTchat(t_player *player, const char *msg);
 void       drawTchatMessages();
 size_t     *getTchatMessagesNb();
 char       **getTchatMessages();


#endif
