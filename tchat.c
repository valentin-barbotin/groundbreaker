#include "tchat.h"
#include "game.h"
#include "utils.h"
#include "font.h"
#include "dialog.h"

char   **messages[];


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

    if (msg) {
        putMessageInTchat(player, msg);
    }
    destroyEditBox();
}

void    putMessageInTchat(t_player *player, const char *msg) {
    printf("%s", msg);
}