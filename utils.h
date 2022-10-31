#ifndef UTILS_H
 #define UTILS_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>

 #include <SDL.h>

 extern SDL_Window*     g_window;
 extern SDL_Renderer*   g_renderer;

 char                *randomString(unsigned short size);
 void                removeLineFeed(char* str);
 const unsigned long hash(const char* str);
 int                 doesInclude(char** array, const char* str, const int size);
 char*               readFile(const char* src);
 bool                pickColor(const SDL_Color *color);
 bool                stringIsEqual(const char *str1, const char *str2);
 SDL_Texture*        textureFromFile(const char* src);
 char*               removeSuffix(const char* src, char* suffix);
 void                sendMsgUDP(const char *msg, int socket, struct sockaddr *serverAddress);
 void                receiveMsgUDP(char *buffer, int socket, struct sockaddr *serverAddress);
 void                sendMsg(const char *msg, int socket);
 void                receiveMsg(char *buffer, int socket);
 bool                checkUsername();

#endif /* !FOO_H */
