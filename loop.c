#include <math.h>

#include "config.h"
#include "lobby.h"
#include "loop.h"
#include "map.h"
#include "game.h"
#include "moves.h"
#include "timer.h"
#include "sound.h"

extern SDL_Rect         g_buttonsLocation[4];
extern int              g_currentState;

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
            main_music->file = "./assets/sound/main_music.ogg";
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
        puts("SUBMENU");
        g_currentMenu = g_currentMenu->next[index];

        setupMenu();

        // SDL_RenderPresent(g_renderer); //TODO: check
        return;
    }

    if (g_currentMenu->fct[index] != NULL) {
        puts("FUNCTION");
        g_currentMenu->fct[index](index);
    }
}

/**
 * @brief Handle a mouse button up event
 * @param event the event to handle
 * @return void
 */
void    handleMouseButtonUp(const SDL_Event *event) {
    //loop through buttons
    if (g_currentState == GAME_MAINMENU) { //TODO: check
        for (unsigned short i = 0; i < g_currentMenu->nbButtons; ++i) {
            // get click position
            int xStart = g_buttonsLocation[i].x;
            int yStart = g_buttonsLocation[i].y;

            SDL_Point click = { event->button.x, event->button.y };
            SDL_Rect button = { xStart, yStart, g_buttonsLocation[i].w, g_buttonsLocation[i].h };

            if (SDL_PointInRect(&click, &button))
            {
                makeSelection(i);
            }
        }
    } else if (inGame()) {
        // TODO: handle game events
    }
}

void    handleKeyDown(const SDL_Event *event) {
    switch (g_currentState)
    {
        case GAME_PLAY_PLAYING:
            handleKeyDownPlay(event);
            break;
        
        default:
            break;
    }
}

//TODO: refacto
void    handleKeyUp(const SDL_Event *event) {
    if (inMainMenu()) {
        short index = -1;
        switch (event->key.keysym.sym) {
            case SDLK_ESCAPE:
                if (g_currentState == GAME_MAINMENU_PLAY) {
                    g_currentState = GAME_MAINMENU;
                } else {
                    g_currentState = GAME_EXIT;
                }
                break;
            case SDLK_RETURN:
                switch (g_currentState)
                {
                    case GAME_MAINMENU_PLAY:
                        //TODO: pick a random map from selected ones and launch the game (solo)
                        printf("Launch game\n");
                        t_game *game = getGame();
                        // t_map *maps = getMaps();

                        t_map *tmp[10] = {0};
                        index = -1;
                        for (size_t i = 0; i < g_nbMap; i++)
                        {
                            // printf("i: %d, Map: %d\n", i, game->maps[i].selected);
                            if (game->maps[i].selected) {
                                if (index == -1) index = 0; //used to check if we have at least one map selected
                                tmp[index++] = &game->maps[i];
                            }
                        }
                        if (index == -1) {
                            printf("No map selected\n");
                            return;
                        }

                        index = rand() % index; //pick a random map between the selected ones
                        printf("index = %d\n", index);
                        game->map = tmp[index];
                        // // memcpy(&game->map->map, &maps[index].map, sizeof(game->map->map));

                        spawnPlayer();

                        g_currentState = GAME_PLAY_PLAYING;

                        break;
                    
                    default:
                        index = g_currentMenu->selectedButton;
                        makeSelection(index);
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
                        getGame()->maps[g_nbMap++] = *map;
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
                        if (getGame()->maps[i].selected) nb++;
                    }
                    if (nb > g_nbMap) return;
                    
                    getGame()->maps[g_currentMap].selected = !getGame()->maps[g_currentMap].selected;
                }
                break;
            case SDLK_LEFT:
                switch (g_currentState)
                {
                    case GAME_MAINMENU_PLAY:
                        switch (g_currentOption)
                        {
                            case 0:
                                g_lobby->rows--;
                                if (g_lobby->rows < 1) {
                                    g_lobby->rows = 1;
                                }
                                break;
                            case 1:
                                g_lobby->columns--;
                                if (g_lobby->columns < 1) {
                                    g_lobby->columns = 1;
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
                                g_lobby->rows++;
                                if (g_lobby->rows > 10) {
                                    g_lobby->rows = 10;
                                }
                                break;
                            case 1:
                                g_lobby->columns++;
                                if (g_lobby->columns > 10) {
                                    g_lobby->columns = 10;
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
                puts("A");
                break;
            case SDLK_e:
                if (g_currentState == GAME_MAINMENU_PLAY) {
                    selectMap(1);
                }
                puts("E");
                break;
            default:
                break;
        }
    } else if (inGame()) {
        handleKeyUpPlay(event);
        movePlayer();
    }
}