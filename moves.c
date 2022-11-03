#include "config.h"
#include "game.h"
#include "moves.h"
#include "player.h"

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
int k = 0;
void    handleKeyUpPlay(const SDL_Event *event) {
    t_player    *player;

    player = getPlayer();

     if (event->key.repeat == 0) {
        switch (event->key.keysym.sym) {
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
            default:
                break;
        }
    }
    
    movePlayer(player);
    
    // if player stopped moving, send position to other players
    if (inMultiplayer() && !isMoving(player)) {
        doSendPos(player);
    }
}

bool isMoving() {
    return (getGame()->vx != 0 || getGame()->vy != 0);
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