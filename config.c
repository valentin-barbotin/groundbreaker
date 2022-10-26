#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "config.h"
#include "utils.h"

t_gameConfig    *gameConfig = NULL;


/**
 * Fill the gameConfig struct with default values
 * @param {gameConfig*} game configuration of the game
 * @return void
 */
void    defaultConfig(t_gameConfig *config) {
    strcpy(config->global.lang, "en");
}


/**
 * Parse global settings from file descriptor and update the globalConfig struct using pointers
 * @param {FILE*} File descriptor
 * @param {char*} char*data
 * @param {t_globalConfig*} video configuration of the game
 * @return bool true if success
 */
bool    setupGlobal(const FILE* fd, char* data, t_globalConfig *globalConfig) {
    char key[SIZE_DATA];
    char value[SIZE_DATA];

    //check if key is blacklisted
    while (fgets(data, SIZE_DATA, fd), !feof(fd))
    {
        removeLineFeed(data);
        char firstChar = *data; // *data = data[0]
        if (firstChar == '[' || firstChar == '\0')
            break;
        // get key and value
        // read all until the first '='
        sscanf(data, "%[^=]=%s", key, value);

        switch (hash(key))
        {
            case c_lang:
                strcpy(globalConfig->lang, value);
                break;
            
            default:
                break;
        }
    }
    return true;
}


/**
 * Parse server settings from file descriptor and update the globalConfig struct using pointers
 * @param {FILE*} File descriptor
 * @param {char*} char*data
 * @param {t_serverConfig *} video configuration of the game
 * @return bool true if success
 */
bool    setupServer(const FILE* fd, char* data, t_serverConfig *config) {
    char key[SIZE_DATA];
    char value[SIZE_DATA];

    //check if key is blacklisted
    while (fgets(data, SIZE_DATA, fd), !feof(fd))
    {
        removeLineFeed(data);
        char firstChar = *data; // *data = data[0]
        if (firstChar == '[' || firstChar == '\0')
            break;
        // get key and value
        // read all until the first '='
        sscanf(data, "%[^=]=%s", key, value);

        switch (hash(key))
        {
            case c_host:
                strcpy(config->host, value);
                break;
            case c_port:
                strcpy(config->port, value);
                break;
            
            default:
                break;
        }
    }
    return true;
}

/**
 * Parse video settings from file descriptor and update the globalConfig struct using pointers
 * @param {FILE*} File descriptor
 * @param {char*} char*data
 * @param {t_videoConfig *} video configuration of the game
 * @return bool true if success
 */
bool    setupVideo(const FILE* fd, char* data, t_videoConfig *config) {
    char key[SIZE_DATA];
    char value[SIZE_DATA];

    //check if key is blacklisted
    while (fgets(data, SIZE_DATA, fd), !feof(fd))
    {
        removeLineFeed(data);
        char firstChar = *data; // *data = data[0]
        if (firstChar == '[' || firstChar == '\0')
            break;
        // get key and value
        // read all until the first '='
        sscanf(data, "%[^=]=%s", key, value);

        switch (hash(key))
        {
            case c_fullscreen:
                config->fullscreen = atoi(value);
                break;
            case c_width:
                config->width = atoi(value);
                break;
            case c_height:
                config->height = atoi(value);
                break;
            case c_vsync:
                config->vsync = atoi(value);
                break;
            
            default:
                break;
        }
    }
    return true;
}


/**
 * Parse audio settings from file descriptor and update the globalConfig struct using pointers
 * @param {FILE*} File descriptor
 * @param {char*} char*data
 * @param {t_audioConfig *} audio configuration of the game
 * @return bool true if success
 */
bool    setupAudio(const FILE* fd, char* data, t_audioConfig *config) {
    char key[SIZE_DATA];
    char value[SIZE_DATA];

    //check if key is blacklisted
    while (fgets(data, SIZE_DATA, fd), !feof(fd))
    {
        removeLineFeed(data);
        char firstChar = *data; // *data = data[0]
        if (firstChar == '[' || firstChar == '\0')
            break;
        // get key and value
        // read all until the first '='
        sscanf(data, "%[^=]=%s", key, value);

        switch (hash(key))
        {
            case c_volume:
                config->volume = atoi(value);
                break;
            case c_music_vol:
                config->music_volume = atoi(value);
                break;
            case c_sound_vol:
                config->music_volume = atoi(value);
                break;
            
            default:
                break;
        }
    }
    return true;
}



/**
 * Parse commands settings from file descriptor and update the globalConfig struct using pointers
 * @param {FILE*} File descriptor
 * @param {char*} char*data
 * @param {t_commandsConfig *} commands configuration of the game
 * @return bool true if success
 */
bool    setupCommands(const FILE* fd, char* data, t_commandsConfig *config) {
    char key[SIZE_DATA];
    char value[SIZE_DATA];

    //check if key is blacklisted
    while (fgets(data, SIZE_DATA, fd), !feof(fd))
    {
        removeLineFeed(data);
        char firstChar = *data; // *data = data[0]
        if (firstChar == '[' || firstChar == '\0')
            break;
        // get key and value
        // read all until the first '='
        sscanf(data, "%[^=]=%s", key, value);

        switch (hash(key))
        {
            case c_up:
                config->up = *value;
                break;
            case c_down:
                config->down = *value;
                break;
            case c_left:
                config->left = *value;
                break;
            case c_right:
                config->right = *value;
                break;
            
            default:
                break;
        }
    }
    return true;
}

/**
 * Try to read game configuration file, then parse to parse it
 * @return {int} 0 on success, and 1 in case of failure
 */
int     readConfig(t_gameConfig *config) {
    FILE    *fd;

    fd = fopen("./config.ini", "r");
    if (fd == NULL) {
        fd = fopen("../config.ini", "r");
        if (fd == NULL) {
            return EXIT_FAILURE;
        }
    }

    // to define
    char* data = calloc(SIZE_DATA, sizeof(char));
    if (data == NULL) {
        #ifdef DEBUG
                fprintf(stderr, "Error: calloc failed\n");
        #endif
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
        exit(1);
    }

    bool success = false;
    while (!feof(fd))
    {
        fgets(data, SIZE_DATA, fd);
        removeLineFeed(data);
        switch (hash(data))
        {
            case Server:
                puts("Setup server");
                success = setupServer(fd, data, &config->server);
                break;
            case Video:
                puts("Setup video");
                success = setupVideo(fd, data, &config->video);
                break;
            case Audio:
                puts("Setup audio");
                success = setupAudio(fd, data, &config->audio);
                break;
            case Commands:
                puts("Setup commands");
                success = setupCommands(fd, data, &config->commands);
                break;
            case Global:
                puts("Setup global");
                success = setupGlobal(fd, data, &config->global);
                break;
            default:
                puts("DEFAULT");
                break;
        }
        if (!success) {
            return EXIT_FAILURE;
        }
    }

    // if !fd, then recreate config file
    // recreateConfig

    free(data);
    fclose(fd);
    return 0;
}