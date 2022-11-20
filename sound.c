#include "sound.h"
#include <SDL_mixer.h>
#include <SDL.h>

/**
 * @brief Initialize the audio system
 * @param sound
 * @return  true on success, false on failure
 */
bool     initAudio(t_sound *sound) {
    if (sound->file == NULL) return false;

    // get the 3 last characters of sound->file
    char *fileType = sound->file + strlen(sound->file) - 3;

    if(!IS_FILE_TYPE("wav")) {
        SDL_Log("Error: %s is not a valid audio file", sound->file);
        exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
        SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
        exit(1);
    }

    Mix_AllocateChannels(NB_CHANNELS);
    Mix_Volume(-1, MIX_MAX_VOLUME/2);
    sound->chunk = Mix_LoadWAV(sound->file);
    Mix_VolumeChunk(sound->chunk, MIX_MAX_VOLUME/2);

    if (sound->chunk == NULL) {
        SDL_Log("Mix_LoadWAV: %s\n", Mix_GetError());
        exit(1);
    }

    return true;
}

/**
 * @brief Play the sound
 * @param sound
 * @return true on success or false on error
 */
bool    playSound(t_sound *sound) {
    if (!initAudio(sound)) return false;
    if (isSoundPlaying(sound)) return false;
    if (pauseSound(sound)) {
        Mix_Resume(-1);
        return true;
    }

    return Mix_PlayChannel(-1, sound->chunk, 0) != -1;
}

/**
 * @brief Play the sound in loop
 * @param sound
 * @return true on success or false on error
 */
bool    playSoundLoop(t_sound *sound) {
    if (!initAudio(sound)) return false;
    if (isSoundPlaying(sound)) return false;

    if (pauseSound(sound)) {
        Mix_Resume(-1);
        return true;
    }

    Mix_PlayChannel(-1, sound->chunk, -1);
    if (!isSoundPlaying(sound)) {
        SDL_Log("Failed to play music: %s", Mix_GetError());
        exit(1);
    }
    return true;
}

/**
 * @brief Stop the sound
 * @param sound
 * @return  true on success or false on error
 */
bool    stopSound(t_sound *sound) {
    if (!initAudio(sound)) return false;
    Mix_HaltChannel(getAvailableChannel());
    Mix_FreeChunk(sound->chunk);
    //Mix_CloseAudio();
    return true;
}

/**
 * @brief Pause the sound
 * @param sound
 * @return  true on success or false on error
 */
bool    pauseSound(t_sound *sound) {
    if (!initAudio(sound)) return false;
    if (!isSoundPlaying(sound)) return false;

    switch (Mix_Paused(getAvailableChannel())) {
        case 0:
            Mix_Pause(-1);
            return true;
        case 1:
            Mix_Resume(-1);
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
    if (!initAudio(sound)) return false;
    if (Mix_Playing(getAvailableChannel()) == 0) return false;


    if (Mix_Volume(getAvailableChannel(), volume) == -1) {
        SDL_Log("Failed to set volume: %s", Mix_GetError());
        return false;
    }

    Mix_Volume(getAvailableChannel(), volume);
    Mix_VolumeChunk(sound->chunk, volume);
    return true;
}


bool    isSoundPlaying(t_sound *sound) {
    if (!initAudio(sound)) return false;
    return (Mix_Playing(-1) == 0) ? false : true;
}

/**
* @brief Check if the channel is available
 * @param channel
 * @return  true if available, false if not
 */
bool    isChannelAvailable(int channel) {
    return (Mix_Playing(channel) == 0) ? true : false;
}

/**
 * @brief Get the first available channel
 * @return  the channel number
 */
int     getAvailableChannel() {
    for (int i = 0; i < NB_CHANNELS; i++) {
        if (isChannelAvailable(i)) return i;
    }
    return -1;
}

void    freeSound(t_sound *sound) {
    Mix_FreeChunk(sound->chunk);

    sound->chunk = NULL;
    Mix_CloseAudio();
}

