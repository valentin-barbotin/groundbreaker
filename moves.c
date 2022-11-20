#include "config.h"
#include "game.h"
#include "moves.h"
#include "player.h"
#include "inventory.h"
#include "loop.h"
#include "settings.h"

#define DEBUG true

void    handleKeyDownPlay(const SDL_Event *event) {
    t_player    *player;

    player = getPlayer();

     if (event->key.repeat == 0) {

        switch (getActionFor(event->key.keysym.sym)) {
            case ACTION_KEY_UP:
                player->vy -= VELOCITY;
                break;
            case ACTION_KEY_DOWN:
                player->vy += VELOCITY;
                break;
            case ACTION_KEY_LEFT:
                player->vx -= VELOCITY;
                break;
            case ACTION_KEY_RIGHT:
                player->vx += VELOCITY;
                break;
        }
    }
    
    if (player->vx < -VELOCITY) {
        player->vx = -VELOCITY;
    } else if (player->vx > VELOCITY) {
        player->vx = VELOCITY;
    }

    if (player->vy < -VELOCITY) {
        player->vy = -VELOCITY;
    } else if (player->vy > VELOCITY) {
        player->vy = VELOCITY;
    }
}
void    handleKeyUpPlay(const SDL_Event *event) {
    t_player    *player;

    player = getPlayer();

    if (!isGamePaused()) {
        // disable keys if player is dead (spectator mode or waiting for respawn)
        if (!player->health) return;

        printf("11 %d\n", (event->key.keysym.sym));
        if (event->key.repeat == 0) {

            switch (getActionFor(event->key.keysym.sym))
            {
                case ACTION_KEY_USE_ITEM:
                    useItem(player->inventory[player->selectedSlot]);
                    break;
                case ACTION_KEY_UP:
                    player->vy += VELOCITY;
                    break;
                case ACTION_KEY_DOWN:
                    player->vy -= VELOCITY;
                    break;
                case ACTION_KEY_LEFT:
                    player->vx += VELOCITY;
                    break;
                case ACTION_KEY_RIGHT:
                    player->vx -= VELOCITY;
                    break;
                case ACTION_KEY_ITEM_1:
                    player->selectedSlot = 0;
                    drawSelectedItem();
                    break;
                case ACTION_KEY_ITEM_2:
                    player->selectedSlot = 1;
                    drawSelectedItem();
                    break;
                case ACTION_KEY_ITEM_3:
                    player->selectedSlot = 2;
                    drawSelectedItem();
                    break;
                case ACTION_KEY_ITEM_4:
                    player->selectedSlot = 3;
                    drawSelectedItem();
                    break;
                case ACTION_KEY_ITEM_5:
                    player->selectedSlot = 4;
                    drawSelectedItem();
                    break;
                default:
                    break;
            }

            if (event->key.keysym.sym == SDLK_ESCAPE) {
                pauseGame();
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