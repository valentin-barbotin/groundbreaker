#ifndef CONFIG_H
 #define CONFIG_H
 
 typedef enum e_game_modes {
    GAME_EXIT,
    GAME_START,

    GAME_MAINMENU,
        GAME_MAINMENU_PLAY,
            GAME_MAINMENU_PLAY_NEWGAME,
            GAME_MAINMENU_PLAY_CHOOSE, // maps

    GAME_MAINMENU_END, // Used by inMainMenu to determine if the game is in the main menu or not
    
    GAME_PLAY_PLAYING,
} GAME_MODES;

#endif