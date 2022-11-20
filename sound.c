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
 * @param sound
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

static int playSoundWrapper(Mix_Chunk *chunk, int loops) {
    int     channel;

    if(!setSoundVolume(chunk, gameConfig->audio.sound_volume)) {
        SDL_Log("Mix_VolumeChunk: %s\n", Mix_GetError());
        exit(1);
    }

    channel = Mix_PlayChannel(-1, chunk, loops);
    if (channel < 0) {
        SDL_Log("Failed to play sound: %s", Mix_GetError());
        exit(1);
    }

    return channel;
}


int     playSound(t_sound *sound) {
    return playSoundWrapper(sound->chunk, 0);
}

int     playSoundLoop(t_sound *sound) {
    return playSoundWrapper(sound->chunk, -1);
}

/**
 * @brief Stop the sound
 * @param sound
 * @return  true on success or false on error
 */
bool    stopSound(t_sound *sound) {
    Mix_HaltChannel(-1);
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
    switch (Mix_Paused(-1)) {
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
bool    setSoundVolume(Mix_Chunk *chunk, int volume) {
    return Mix_VolumeChunk(chunk, volume) != -1;
}

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

void    channelFinishedCallback(int channel) {
    t_player    *player;

    player = getPlayer();

    if (player->wallChannel == channel) {
        player->wallChannel = -1;
    }
}
