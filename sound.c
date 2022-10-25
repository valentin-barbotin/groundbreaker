#include "sound.h"
#include <SDL_mixer.h>
#include <SDL.h>

/**
 * @brief Initialize the audio system
 * @param sound
 * @return  0 on success, -1 on error
 */
bool     initAudio(t_sound *sound) {
    int flags;

    // get the 3 last characters of sound->file
    char *fileType = sound->file + strlen(sound->file) - 3;

    if (strcmp(fileType, "ogg") == 0) {
        flags = MIX_INIT_OGG;
    } else {
        flags = MIX_INIT_MP3;
    }

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_Log("Failed to init SDL");
        return false;
    }

    if (flags != (Mix_Init(flags))) {
        printf("Could not initialize mixer (result: %d).\n", Mix_Init(flags));
        printf("Mix_Init: %s\n", Mix_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
        return false;
    }

    return true;
}

/**
 * @brief Initialize the music
 * @param sound
 * @return  0 on success or -1 on error
 */
bool    initMusic(t_sound *sound) {
    if (sound->file == NULL) return false;

    initAudio(sound);

    if (sound->music == NULL) {
        sound->music = Mix_LoadMUS(sound->file);

        if (sound->music == NULL) {
            SDL_Log("Failed to load music: %s", Mix_GetError());
            return false;
        }
    }
    return true;
}
/**
 * @brief Play the sound
 * @param sound
 * @return  0 on success or -1 on error
 */
bool    playSound(t_sound *sound) {
    if (initMusic(sound) == -1) return -1;

    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(sound->music, 0);
        return true;
    }

    if (pauseSound(sound)) {
        Mix_ResumeMusic();
        return true;
    }else{
        SDL_Log("La musique n'est pas en pause");
        return false;
    }
}

bool    playSoundLoop(t_sound *sound) {
    if (initMusic(sound) == -1) return false;

    Mix_PlayMusic(sound->music, -1);

    while (Mix_PlayingMusic()) {
        SDL_Delay(100);
    }

    Mix_FreeMusic(sound->music);

    sound->music = NULL;

    Mix_CloseAudio();

    return true;
}

bool    stopSound(t_sound *sound) {
    if (initMusic(sound) == -1) return false;
    Mix_HaltMusic();
    Mix_FreeMusic(sound->music);
    sound->music = NULL;
    Mix_CloseAudio();

    return true;
}

bool    pauseSound(t_sound *sound) {
    if (initMusic(sound) == -1) return false;

    if (Mix_PlayingMusic() == 0) {
        SDL_Log("Music is not playing");
        return false;
    }

    switch (Mix_PausedMusic()) {
        case 0:
            Mix_PauseMusic();
            return true;
        case 1:
            Mix_ResumeMusic();
            return false;
        default:
            SDL_Log("Music is not playing");
            return false;
    }
}

bool    setSoundVolume(t_sound *sound, int volume) {
    if (initMusic(sound) == -1) return false;

    if (Mix_PlayingMusic() == 0) {
        SDL_Log("Music is not playing");
        return false;
    }

    if (Mix_VolumeMusic(volume) == -1) {
        SDL_Log("Failed to set volume: %s", Mix_GetError());
        return false;
    }

    Mix_VolumeMusic(volume);

    return true;
}
