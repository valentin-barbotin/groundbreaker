#ifndef UTILS_H
 #define UTILS_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 #include <netinet/in.h>

 #include <SDL.h>

 #include "player.h"
 #include "colors.h"
 #include "server.h"

 extern SDL_Window*         g_window;
 extern SDL_Renderer*       g_renderer;
 extern bool                g_serverRunningUDP;

 extern t_peer          *g_peersList[4]; // 4 players max
 extern int             g_peersListNb;
 extern t_peer          *g_peersListUDP[4]; // 4 players max
 extern int             g_peersListUDPNb;

 char                *randomString(unsigned short size);
 void                removeLineFeed(char* str);
 const unsigned long hash(const char* str);
 int                 doesInclude(char** array, const char* str, const int size);
 char*               readFile(const char* src);
 bool                pickColor(const SDL_Color *color);
 bool                stringIsEqual(const char *str1, const char *str2);
 SDL_Texture*        textureFromFile(const char* src);
 char*               removeSuffix(const char* src, const char* suffix);
 void                sendMsgUDP(const char *msg, int socket, struct sockaddr_in  *sockaddr);
 size_t              receiveMsgUDP(char *buffer, int socket, struct sockaddr_in  *sockaddr);
 void                sendMsg(const char *msg, int socket);
 size_t              receiveMsg(char *buffer, int socket);
 bool                checkUsername();
 void                receiveMove(const char *content);
 bool                hostToAddr(const char *host, in_addr_t *in_addr);
 bool                isNameAvailable(const char *name, int mode);

#endif /* !FOO_H */
