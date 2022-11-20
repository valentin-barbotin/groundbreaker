#include <SDL_mixer.h>
#include <SDL.h>

#include "sound.h"
#include "assets.h"
#include "utils.h"

t_sound     walkSound;
t_sound     wallSound;
t_sound     unbreakableWallSound;
t_sound     bombExplosionSound;
t_sound     itemSound;
t_sound     hurtSound;

t_sound     *walk = &walkSound;
t_sound     *wall = &wallSound;
t_sound     *unbreakableWall = &unbreakableWallSound;
t_sound     *bombExplosion = &bombExplosionSound;
t_sound     *item = &itemSound;
t_sound     *hurt = &hurtSound;

Mix_Music   *music;

/**
 * @brief Initialize the audio system
 * @param {t_sound *} the sound to initialize
 * @return  true on success, false on failure
 */
bool     initAudio(t_sound *sound) {
    if (sound->file == NULL) return false;

    const char  *fileType;

    // get the 3 last characters of sound->file
    fileType = sound->file + strlen(sound->file) - 3;

    if (!stringIsEqual(fileType, "wav")) {
        SDL_Log("Error: %s is not a valid audio file", sound->file);
        exit(1);
    }

    sound->chunk = Mix_LoadWAV(sound->file);
    if (sound->chunk == NULL) {
        SDL_Log("SDL_mixer Error: %s", Mix_GetError());
        exit(1);
    }
    Mix_VolumeChunk(sound->chunk, MIX_MAX_VOLUME/2);

    if (sound->chunk == NULL) {
        SDL_Log("Mix_LoadWAV: %s\n", Mix_GetError());
        exit(1);
    }

    return true;
}

/**
 * @brief Play a sound on a channel
 * @param {Mix_Chunk *} chunk
 * @param {int} loops
 * @return {int} (the channel on which the sound is played)
 */
static int playSoundWrapper(Mix_Chunk *chunk, int loops) {
    int     channel;

    channel = Mix_PlayChannel(-1, chunk, loops);
    if (channel < 0) {
        SDL_Log("Failed to play sound: %s", Mix_GetError());
        exit(1);
    }

    return channel;
}

/**
 * @brief Play a sound
 * @param {t_sound *} sound
 * @return  {int} playSoundWrapper return channel
 */

int     playSound(t_sound *sound) {
    return playSoundWrapper(sound->chunk, 0);
}

/**
 * @brief Play a sound in loop
 * @param {t_sound *} sound
 * @return  the channel on which the sound is played
 */
int     playSoundLoop(t_sound *sound) {
    return playSoundWrapper(sound->chunk, -1);
}

/**
 * @brief Stop the sound
 * @param {t_sound *} sound
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
 * @param {t_sound *} sound
 * @return  true on success or false on error
 */
bool    pauseSound(t_sound *sound) {
    if (!initAudio(sound)) return false;

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
 * @param {t_sound} sound
 * @param {int} volume
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


/**
* @brief Check if the channel is available
 * @param {channel}
 * @return  true if available, false if not
 */
bool    isChannelAvailable(int channel) {
    return (!Mix_Playing(channel));
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

/**
 * @brief Load all the sounds
 * @return  {void}
 */
void    loadSounds() {
    walk->file =            SOUND_WALK;
    wall->file =            SOUND_WALL;
    unbreakableWall->file = SOUND_UNBREAKABLE_WALL;
    bombExplosion->file =   SOUND_EXPLODE_BOMB;
    hurt->file =            SOUND_HURT;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
        SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
        exit(1);
    }

    Mix_AllocateChannels(NB_CHANNELS);
    Mix_Volume(-1, MIX_MAX_VOLUME/2);

    initAudio(walk);
    initAudio(wall);
    initAudio(unbreakableWall);
    initAudio(bombExplosion);
    initAudio(item);
    initAudio(hurt);

    music = Mix_LoadMUS(SOUND_MUSIC_MAIN);
    if (!music) {
        fprintf(stderr, "Couldn't load %s: %s\n", SOUND_MUSIC_MAIN, SDL_GetError());
        exit(1);
    }

    Mix_ChannelFinished(channelFinishedCallback);
}

/**
 * @brief Callback function called when a channel is finished
 * @param {int} channel
 * @return  {void}
 */
void    channelFinishedCallback(int channel) {
    t_player    *player;

    player = getPlayer();

    if (player->wallChannel == channel) {
        player->wallChannel = -1;
    }
}
