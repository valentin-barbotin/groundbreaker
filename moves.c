#include "config.h"
#include "game.h"
#include "moves.h"
#include "player.h"
#include "tchat.h"

#define DEBUG true

void    handleKeyDownPlay(const SDL_Event *event) {
    t_player    *player;

    player = getPlayer();

     if (event->key.repeat == 0) {
        switch (event->key.keysym.sym) {
            case SDLK_UP:
                player->vy -= VELOCITY;
                break;
            case SDLK_DOWN:
                player->vy += VELOCITY;
                break;
            case SDLK_LEFT:
                player->vx -= VELOCITY;
                break;
            case SDLK_RIGHT:
                player->vx += VELOCITY;
                break;
            default:
                break;
        }
    }
}
void    handleKeyUpPlay(const SDL_Event *event) {
    t_player    *player;

    player = getPlayer();

    puts("key up 2");

    if (!isGamePaused()) {
        printf("11 %d\n", (event->key.keysym.sym));
        if (event->key.repeat == 0) {
            switch (event->key.keysym.sym) {
                case SDLK_ESCAPE:
                    puts("pause");
                    pauseGame();
                case SDLK_UP:
                    player->vy += VELOCITY;
                    break;
                case SDLK_DOWN:
                    player->vy -= VELOCITY;
                    break;
                case SDLK_LEFT:
                    player->vx += VELOCITY;
                    break;
                case SDLK_RIGHT:
                    player->vx -= VELOCITY;
                    break;
                case SDLK_t:
                    askTchatMessage();
                    break;
                default:
                    break;
            }
        }
    } else {
        printf("22 %d\n", (event->key.keysym.sym));
        if (event->key.repeat == 0) {
            switch (event->key.keysym.sym) {
                case SDLK_ESCAPE:
                    puts("resume");
                    resumeGame();
                    break;
                default:
                    break;
            }
        }
    }
    
    movePlayer(player);
    
    // if player stopped moving, send position to other players
    if (inMultiplayer() && !isMoving(player)) {
        doSendPos(player);
    }
}


void    checkBorders() {
    t_player    *player;

    player = getPlayer();
    
    if (player->x + PLAYER_WIDTH/2 > gameConfig->video.width) {
        player->x -= VELOCITY;
    } else if (player->x < 0) {
        player->x += VELOCITY;
    }

    if (player->y + PLAYER_HEIGHT/2 > gameConfig->video.height) {
        player->y -= VELOCITY;
    } else if (player->y < 0) {
        player->y += VELOCITY;
    }
}