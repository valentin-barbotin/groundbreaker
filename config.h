#ifndef CONFIG_H
 #define CONFIG_H

 #define SIZE_DATA 256

 #include <stdio.h>
 #include <stdbool.h>
 
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

 //Global
 #define Global 7571859109182318
 #define c_lang 6385431463

 //Server
 #define Server 7571874337131252
 #define c_host 6385303139
 #define c_port 6385590602

 //Video
 #define Video 229450859117364
 #define c_fullscreen 8246329787817902264
 #define c_width 210732560549
 #define c_height 6953582889182
 #define c_vsync 210731756664

 //Audio
 #define Audio 229450051509519
 #define c_volume 6954142767101
 #define c_music_vol 249898635213110838
 #define c_sound_vol 249906820733961246

 //Commands
 #define Commands 8245749069968037039
 #define c_up 5863882
 #define c_down 6385159517
 #define c_left 6385435568
 #define c_right 210726633827
 
 typedef struct     s_globalConfig
 {
     char lang[3];
 }                  t_globalConfig;
 
 typedef struct     s_serverConfig
 {
     char host[SIZE_DATA];
     char port[5];
 }                  t_serverConfig;

  typedef struct    s_videoConfig
  {
     bool fullscreen;
     unsigned int width;
     unsigned int height;
     bool vsync;
 }                  t_videoConfig;

 typedef struct     s_audioConfig
 {
     unsigned int volume;
     unsigned int music_volume;
     unsigned int sound_volume;
 }                  t_audioConfig;

 typedef struct     s_commandsConfig
 {
     char up;
     char down;
     char left;
     char right;
 }                  t_commandsConfig;


 typedef struct     s_gameConfig
 {
     t_globalConfig     global;
     t_serverConfig     server;
     t_videoConfig      video;
     t_audioConfig      audio;
     t_commandsConfig   commands;
 }                  t_gameConfig;


 void    defaultConfig(t_gameConfig *config);
 bool    setupGlobal(const FILE* fd, char *data, t_globalConfig *globalConfig);
 int     readConfig(t_gameConfig *config);

#endif