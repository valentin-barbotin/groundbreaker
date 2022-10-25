#include "sound.h"
#include <SDL_mixer.h>
#include <SDL.h>

/**
 * @brief Initialize the audio system
 * @param sound
 * @return  true on success, false on failure
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
 * @return true on success, false on error
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
 * @return true on success or false on error
 */
bool    playSound(t_sound *sound) {
    if (!initMusic(sound)) return false;

    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(sound->music, 0);
        return true;
    }

    if (pauseSound(sound)) {
        Mix_ResumeMusic();
        return true;
    }

    SDL_Log("La musique n'est pas en pause");
    return false;
}

/**
 * @brief Play the sound in loop
 * @param sound
 * @return true on success or false on error
 */
bool    playSoundLoop(t_sound *sound) {
    if (!initMusic(sound)) return false;

    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(sound->music, -1);
        return true;
    }

    if (pauseSound(sound)) {
        Mix_ResumeMusic();
        return true;
    }

    SDL_Log("La musique est déjà en cours de lecture");
    return false;
}

/**
 * @brief Stop the sound
 * @param sound
 * @return  true on success or false on error
 */
bool    stopSound(t_sound *sound) {
    if (!initMusic(sound)) return false;
    Mix_HaltMusic();
    Mix_FreeMusic(sound->music);
    sound->music = NULL;
    Mix_CloseAudio();

    return true;
}

/**
 * @brief Pause the sound
 * @param sound
 * @return  true on success or false on error
 */
bool    pauseSound(t_sound *sound) {
    if (!initMusic(sound)) return false;
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
            break;
        default:
            SDL_Log("Music is not playing");
            break;
    }

    return false;
}

/**
 * @brief Set the sound volume
 * @param sound
 * @param volume
 * @return  true on success or false on error
 */
bool    setSoundVolume(t_sound *sound, int volume) {
    if (!initMusic(sound)) return false;

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
