#include <math.h>
#include <pthread.h>

#include "config.h"
#include "lobby.h"
#include "loop.h"
#include "map.h"
#include "game.h"
#include "moves.h"
#include "timer.h"
#include "sound.h"
#include "utils.h"
#include "dialog.h"
#include "client.h"
#include "server.h"
#include "assets.h"

#define DEBUG true

extern int              g_currentState;
extern int              g_serverSocket;

t_sound                *main_music = NULL;

/**
 * @brief Check if we are in the main menu or a sub menu
 * 
 * @return bool
 */
bool    inMainMenu() {
    if((g_currentState >= GAME_MAINMENU && g_currentState < GAME_MAINMENU_END)) {
        if (Mix_PlayingMusic() == 0) {
            main_music = malloc(sizeof(t_sound));
            if(main_music == NULL) {
                #ifdef DEBUG
                    fprintf(stderr, "Error allocating memory for main_music");
                #endif
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                exit(1);
            }
            main_music->file = SOUND_MUSIC_MAIN;
            initMusic(main_music);
            if (main_music->music == NULL) {
                #ifdef DEBUG
                                fprintf(stderr, "Error loading music: %s\n", Mix_GetError());
                #endif
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
                exit(1);
            }
            Mix_VolumeMusic(25);
            playSoundLoop(main_music);

        }
    }else if(Mix_PlayingMusic() == 1 && main_music->music != NULL) {
        if (!stopSound(main_music)) {
            #ifdef DEBUG
                fprintf(stderr, "Error: Can't open stop the music\n");
            #endif
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
            exit(1);
        }
    }
    return (g_currentState >= GAME_MAINMENU && g_currentState < GAME_MAINMENU_END);
}

void    makeSelection(unsigned short index) {
    if (g_currentMenu->next[index] != NULL) {
        g_currentMenu = g_currentMenu->next[index];

        setupMenu();

        return;
    }

    if (g_currentMenu->fct[index] != NULL) {
        g_currentMenu->fct[index](index);
    }
}

/**
 * @brief Handle a mouse button up event
 * @param event the event to handle
 * @return void
 */
void    handleMouseButtonUp(const SDL_Event *event) {
    switch (g_currentState)
    {
        case GAME_MAINMENU:
            break;
        case GAME_PLAY_PLAYING:
            handleMouseButtonUpPlaying(event);
            break;
        
        default:
            break;
    }
}

void    handleKeyDown(const SDL_Event *event) {
    t_dialog *dialog = getEditBox();

    if (dialog->active) {
        switch (event->key.keysym.sym)
        {
           case SDLK_BACKSPACE:
                if (dialog->edit[0] != '\0') {
                    dialog->edit[strlen(dialog->edit) - 1] = '\0';
                }
                break;
        
            default:
                break;
        }
        return;
    }

    switch (g_currentState)
    {
        case GAME_MAINMENU:
            break;
        case GAME_PLAY_PLAYING:
            handleKeyDownPlay(event);
            break;
        
        default:
            break;
    }
}

/**
 * @brief Append a character to the edit box
 * 
 * @param event 
 */
void    handleTextEditing(const SDL_Event *event) {
    t_dialog *dialog = getEditBox();

    // limit text length
    if (strlen(dialog->edit) >= 30) return;


    if (dialog->active) {
        strcat(dialog->edit, event->text.text);
    }
}

//TODO: refacto
void    handleKeyUp(const SDL_Event *event) {
    t_game          *game;
    const t_dialog        *dialog;

    game = getGame();
    dialog = getEditBox();

    if (dialog->active) {
        handleKeyUpDialog(event);
        return;
    }

    if (inGame()) {
        handleKeyUpPlay(event);
        t_player *player = getPlayer();
    
        player->vx = 0;
        player->vy = 0;
    }

    if (inMainMenu() || isGamePaused()) {
        short index = -1;
        switch (event->key.keysym.sym) {
            case SDLK_ESCAPE:
                g_currentState = (g_currentState == GAME_MAINMENU_PLAY) ? GAME_MAINMENU : GAME_EXIT;
                break;
            case SDLK_RETURN:
                switch (g_currentState)
                {
                    case GAME_MAINMENU_PLAY:
                        //TODO: pick a random map from selected ones and launch the game (solo)

                        launchGame();

                        break;
                    
                    default:
                        index = g_currentMenu->selectedButton;
                        makeSelection(index);
                        break;
                }
                break;
            case SDLK_h:
                switch (g_currentState)
                {
                    case GAME_MAINMENU_PLAY:
                        launchServer();
                        break;
                    
                    default:
                        break;
                }
                break;
            case SDLK_n:
                switch (g_currentState)
                {
                    case GAME_MAINMENU_PLAY:
                        if (g_nbMap == 9) break;

                        printf("create map with %d players, %d cols, %d rows\n", g_lobby->players, g_lobby->columns, g_lobby->rows);
                        const t_map *map = map_create(g_lobby->columns, g_lobby->rows);
                        map_fill(map);
                        saveMap(map);
                        map_print(map);
                        game->maps[g_nbMap++] = *map;
                        break;
                    
                    default:
                        index = g_currentMenu->selectedButton;
                        makeSelection(index);
                        break;
                }
                break;
            case SDLK_SPACE:
                if (g_currentState == GAME_MAINMENU_PLAY) {
                    short nb;

                    nb = 0;
                    for (size_t i = 0; i < g_nbMap; i++)
                    {
                        if (game->maps[i].selected) nb++;
                    }
                    if (nb > g_nbMap) return;
                    
                    game->maps[g_currentMap].selected = !game->maps[g_currentMap].selected;
                }
                break;
            case SDLK_LEFT:
                switch (g_currentState)
                {
                    case GAME_MAINMENU_PLAY:
                        switch (g_currentOption)
                        {
                            case 0:
                                g_lobby->rows -= 2;
                                if (g_lobby->rows < 3) {
                                    g_lobby->rows = 3;
                                }
                                break;
                            case 1:
                                g_lobby->columns -= 2;
                                if (g_lobby->columns < 3) {
                                    g_lobby->columns = 3;
                                }
                                break;
                            case 2:
                                g_lobby->players--;
                                if (g_lobby->players < 2) {
                                    g_lobby->players = 2;
                                }
                                break;
                            
                            default:
                                break;
                        }
                        break;
                    
                    default:
                        break;
                }

                break;
            case SDLK_RIGHT:
                switch (g_currentState)
                {
                    case GAME_MAINMENU_PLAY:
                        switch (g_currentOption)
                        {
                            case 0:
                                g_lobby->rows += 2;
                                if (g_lobby->rows > 9) {
                                    g_lobby->rows = 9;
                                }
                                break;
                            case 1:
                                g_lobby->columns += 2;
                                if (g_lobby->columns > 9) {
                                    g_lobby->columns = 9;
                                }
                                break;
                            case 2:
                                g_lobby->players++;
                                if (g_lobby->players > 4) {
                                    g_lobby->players = 4;
                                }
                                break;
                            
                            default:
                                break;
                        }
                        break;
                    
                    default:
                        break;
                }
                break;
            case SDLK_UP:
                switch (g_currentState)
                {
                    case GAME_MAINMENU_PLAY:
                        index = g_currentOption - 1;
                        if (index < 0) {
                            index = 2;
                        }
                        g_currentOption = index;
                        break;
                    
                    default:
                        index = (g_currentMenu->selectedButton - 1);
                        if (index < 0) {
                            index = g_currentMenu->nbButtons - 1;
                        }
                        g_currentMenu->selectedButton = index;
                        printf("UP: %d\n", g_currentMenu->selectedButton);
                        break;
                }

                break;
            case SDLK_DOWN:
                switch (g_currentState)
                {
                    case GAME_MAINMENU_PLAY:
                        index = g_currentOption + 1;
                        if (index > 2) {
                            index = 0;
                        }
                        g_currentOption = index;
                        break;
                    
                    default:
                        index = (g_currentMenu->selectedButton + 1);
                        if (index > g_currentMenu->nbButtons - 1) {
                            index = 0;
                        }
                        g_currentMenu->selectedButton = index;
                        printf("DOWN: %d\n", g_currentMenu->selectedButton);
                        break;
                }
                break;
            case SDLK_a:
                if (g_currentState == GAME_MAINMENU_PLAY) {
                    selectMap(-1);
                }
                break;
            case SDLK_e:
                if (g_currentState == GAME_MAINMENU_PLAY) {
                    selectMap(1);
                }
                break;
            default:
                break;
        }
    }
}