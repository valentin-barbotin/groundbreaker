#include "sound.h"
#include <SDL_mixer.h>
#include <SDL.h>

int     initAudio(t_sound *sound) {
    int flags;

    // get the 3 last characters of sound->file
    char *fileType = sound->file + strlen(sound->file) - 3;

    if (strcmp(fileType, "ogg") == 0) {
        flags = MIX_INIT_OGG;
    } else {
        flags = MIX_INIT_MP3;
    }

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Failed to init SDL\n");
        return -1;
    }

    if (flags != (Mix_Init(flags))) {
        printf("Could not initialize mixer (result: %d).\n", Mix_Init(flags));
        printf("Mix_Init: %s\n", Mix_GetError());
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
        return -1;
    }

    return 0;
}

int    initMusic(t_sound *sound) {
    if (sound->file == NULL) return -1;

    initAudio(sound);

    if (sound->music == NULL) {
        sound->music = Mix_LoadMUS(sound->file);

        if (sound->music == NULL) {
            printf("Failed to load music: %s\n", Mix_GetError());
            return -1;
        }
    }
    return 0;
}

int    playSound(t_sound *sound) {
    if (initMusic(sound) == -1) return -1;

    Mix_PlayMusic(sound->music, 1);

    while (Mix_PlayingMusic()) {
        SDL_Delay(100);
    }

    Mix_FreeMusic(sound->music);

    sound->music = NULL;

    Mix_CloseAudio();
    //Mix_Quit();

    return 0;
}

int    playSoundLoop(t_sound *sound) {
    if (initMusic(sound) == -1) return -1;

    Mix_PlayMusic(sound->music, -1);

    while (Mix_PlayingMusic()) {
        SDL_Delay(100);
    }

    Mix_FreeMusic(sound->music);

    sound->music = NULL;

    Mix_CloseAudio();
}

int    stopSound(t_sound *sound) {
    if (initMusic(sound) == -1) return -1;
    Mix_HaltMusic();
    Mix_FreeMusic(sound->music);
    sound->music = NULL;
    Mix_CloseAudio();
}

int    pauseSound(t_sound *sound) {
    if (initMusic(sound) == -1) return -1;

    if (Mix_PlayingMusic() == 0) {
        printf("Music is not playing\n");
        return -1;
    }

    if (Mix_PausedMusic() == 1) {
        printf("Music is already paused\n");
        return -1;
    }

    Mix_PauseMusic();

    return 0;
}

int    unpauseSound(t_sound *sound) {
    if (initMusic(sound) == -1) return -1;

    if (Mix_PlayingMusic() == 0) {
        printf("Music is not playing\n");
        return -1;
    }

    if (Mix_PausedMusic() == 0) {
        printf("Music is not paused\n");
        return -1;
    }

    Mix_ResumeMusic();
    return 0;
}

int    setSoundVolume(t_sound *sound, int volume) {
    if (initMusic(sound) == -1) return -1;

    if (Mix_PlayingMusic() == 0) {
        printf("Music is not playing\n");
        return -1;
    }

    if (Mix_VolumeMusic(volume) == -1) {
        printf("Failed to set volume\n");
        return -1;
    }

    Mix_VolumeMusic(volume);

    return 0;
}
