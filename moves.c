#include "config.h"
#include "game.h"
#include "moves.h"
#include "player.h"
#include "inventory.h"

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

    if (!isGamePaused()) {
        printf("11 %d\n", (event->key.keysym.sym));
        if (event->key.repeat == 0) {
            switch (event->key.keysym.sym) {
                case SDLK_ESCAPE:
                    pauseGame();
                    break;
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
                case SDLK_1:
                    player->selectedSlot = 0;
                    drawSelectedItem();
                    break;
                case SDLK_2:
                    player->selectedSlot = 1;
                    drawSelectedItem();
                    break;
                case SDLK_3:
                    player->selectedSlot = 2;
                    drawSelectedItem();
                    break;
                case SDLK_4:
                    player->selectedSlot = 3;
                    drawSelectedItem();
                    break;
                case SDLK_5:
                    player->selectedSlot = 4;
                    drawSelectedItem();
                    break;
                case SDLK_6:
                    player->selectedSlot = 5;
                    drawSelectedItem();
                    break;
                case SDLK_7:
                    player->selectedSlot = 6;
                    drawSelectedItem();
                    break;
                case SDLK_8:
                    player->selectedSlot = 7;
                    drawSelectedItem();
                    break;
                case SDLK_9:
                    player->selectedSlot = 8;
                    drawSelectedItem();
                    break;
                case SDLK_0:
                    player->selectedSlot = 9;
                    drawSelectedItem();
                    break;
                case SDLK_RIGHTPAREN:
                     player->selectedSlot = 10;
                     drawSelectedItem();
                     break;
                default:
                    break;
            }
        }
    } else {
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