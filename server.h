#ifndef SERVER_H
 #define SERVER_H

 #include "game.h"
 #include "player.h"
 extern t_gameConfig    *gameConfig;

 typedef enum   e_message
 {
    BROADCAST,
    JOIN,
    MOVE,
    CHAT,
    READY,
    START,
    QUIT,
    PLAYERDAT,
    MYNAME,
    TCHAT,
    CELL,
    EFFECT,
    DAMAGE,
    RESPAWN,
    LIFE,
    END
 }             t_message;

 typedef struct     s_peer
 {
    int                         socket;
    const struct sockaddr_in    *clientAddr;
    char                        name[256];
    unsigned short              id;
 }                  t_peer;
 

 void    sendToAll(const char *msg, int except);
 void    sendToAllUDP(const char *msg, short except);
 void    addPeer(int socket, const struct sockaddr_in *clientAddr, const char *name, unsigned short id);
 void    handleMessageSrv(char  *buffer, int client, const struct sockaddr_in *clientAddr);
 void    handleMessageSrv2(char *type, char *content, int client, const struct sockaddr_in *clientAddr);
 void    handleClientUDP(int socket);
 void    *handleClient(void *clientSocket);
 void    launchServer();
 void    *createServerUDP(const void *arg);
 void    *createServer(const void *arg);
 void    multiplayerStart();
 void    multiplayerEnd(short winner);
 void    sendPlayersToAll();

#endif