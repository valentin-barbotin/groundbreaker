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
void    handleKeyUpPlay(const SDL_Event *event) {
    t_player    *player;

    player = getPlayer();

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
                case SDLK_1:
                //   drawSelectedItem(ITEM_BOMB);
                    break;
                case SDLK_2:
                  //   drawSelectedItem(ITEM_BOMB_UP);
                    break;
                case SDLK_3:
                  //   drawSelectedItem(ITEM_BOMB_DOWN);
                    break;
                case SDLK_4:
                  //   drawSelectedItem(ITEM_YELLOW_FLAME);
                    break;
                case SDLK_5:
                  //   drawSelectedItem(ITEM_BLUE_FLAME);
                    break;
                case SDLK_6:
                  //   drawSelectedItem(ITEM_RED_FLAME);
                    break;
                case SDLK_7:
                  //   drawSelectedItem(ITEM_THROUGH_BOMB);
                    break;
                case SDLK_8:
                  //   drawSelectedItem(ITEM_BOMB_KICK);
                    break;
                case SDLK_9:
                  //   drawSelectedItem(ITEM_INVINCIBILITY);
                    break;
                case SDLK_0:
                  //   drawSelectedItem(ITEM_HEART);
                    break;
                case SDLK_RIGHTPAREN:
                  //  drawSelectedItem(ITEM_LIFE);
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