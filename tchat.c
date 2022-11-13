#include "tchat.h"
#include "game.h"
#include "utils.h"
#include "font.h"
#include "dialog.h"

char   **g_messages = NULL;
size_t  g_messages_nb = 0;

// strdup
// TODO : player in green and other in white
void    drawTchat() {
    t_game *game;
    SDL_Rect rect;

    game = getGame();

    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);

    SDL_Color color = {0, 0, 0, 128};

    pickColor(&color);
    rect.x = 0;
    rect.y = 0;
    rect.w = (gameConfig->video.width) * 0.25;
    rect.h = gameConfig->video.height * 0.4;


    SDL_RenderFillRect(g_renderer, &rect);
}

void    askTchatMessage() {
    t_dialog  *dialog;

    dialog = createEditBox("Send a message :", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});
    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = askTchatMessageCallback;
}

void    askTchatMessageCallback(const char *msg) {
    t_game *game;
    t_player *player;

    game = getGame();
    player = getPlayer();

    if (msg) {
        putMessageInTchat(player, msg);
    }
    destroyEditBox();
}

void    putMessageInTchat(t_player *player, const char *msg) {
    char *buffer;
    if(g_messages == NULL) {
        g_messages = malloc(sizeof(char*));
        g_messages_nb++;
    } else {
        g_messages = realloc(g_messages , sizeof(char*) * ++g_messages_nb);
    }
    buffer = malloc(sizeof(char) * (strlen(msg) + 1 + strlen(player->name) + 1));
    sprintf(buffer, "%s: %s", player->name, msg);
    g_messages[g_messages_nb - 1] = buffer;
    buffer = NULL;
    free(buffer);

}


void    drawTchatMessages() {
    char *buffer;
    int i, limit;

    SDL_Rect rect;
    SDL_Color color = {255, 255, 255, 255};

    rect.x = 0;
    rect.y = 0;

    if(g_messages_nb == 0) {
        return;
    }

    i = g_messages_nb;
    limit = g_messages_nb - 5;


    // TODO : get the last 5 messages
    while((i &&  g_messages[i - 1] != NULL && i > limit)) {
        // TODO : retrieve new technic to get last...
        buffer = g_messages[i-1];
        drawText(&color, rect.x + 10, rect.y + 10, buffer, false, 0);
        rect.y += 20;
        i--;
    }

}

/*
t_game *game;
SDL_Rect rect;
SDL_Color color = {255, 255, 255, 255};
game = getGame();


size_t nbMessages = getTchatMessagesNb();
char **messages = getTchatMessages();
pickColor(&color);

for (size_t i = 0; i < nbMessages; i++) {
    drawText(&color, 0, gameConfig->video.height * 0.4 + i * 20, messages[i], false, 0);
}
*/

