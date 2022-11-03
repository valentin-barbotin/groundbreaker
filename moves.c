#include "config.h"
#include "game.h"
#include "moves.h"

void    handleKeyDownPlay(const SDL_Event *event) {
     if (event->key.repeat == 0) {
        switch (event->key.keysym.sym) {
            case SDLK_UP:
                getGame()->vy -= VELOCITY;
                break;
            case SDLK_DOWN:
                getGame()->vy += VELOCITY;
                break;
            case SDLK_LEFT:
                getGame()->vx -= VELOCITY;
                break;
            case SDLK_RIGHT:
                getGame()->vx += VELOCITY;
                break;
            default:
                break;
        }
    }
}

void    handleKeyUpPlay(const SDL_Event *event) {
     if (event->key.repeat == 0) {
        switch (event->key.keysym.sym) {
            case SDLK_UP:
                getGame()->vy += VELOCITY;
                break;
            case SDLK_DOWN:
                getGame()->vy -= VELOCITY;
                break;
            case SDLK_LEFT:
                getGame()->vx += VELOCITY;
                break;
            case SDLK_RIGHT:
                getGame()->vx -= VELOCITY;
                break;
            default:
                break;
        }
    }

}

bool isMoving() {
    return (getGame()->vx != 0 || getGame()->vy != 0);
}

void    checkBorders() {
    
    if (getGame()->x + PLAYER_WIDTH/2 > gameConfig->video.width) {
        getGame()->x -= VELOCITY;
    } else if (getGame()->x < 0) {
        getGame()->x += VELOCITY;
    }

    if (getGame()->y + PLAYER_HEIGHT/2 > gameConfig->video.height) {
        getGame()->y -= VELOCITY;
    } else if (getGame()->y < 0) {
        getGame()->y += VELOCITY;
    }
}