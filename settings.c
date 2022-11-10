#include <stdlib.h>
#include <stdio.h>

#include "menu.h"
#include "settings.h"
#include "dialog.h"
#include "config.h"

void    editSettingCallback() {
    const t_dialog  *dialog;
    const char      *value;

    dialog = getEditBox();
    value = dialog->edit;
    switch (dialog->arg)
    {
        case SETTING_VIDEO_FULLSCREEN:
            saveSetting("fullscreen", value);
            break;
        case SETTING_VIDEO_WIDTH:
            saveSetting("width", value);
            break;
        case SETTING_VIDEO_HEIGHT:
            saveSetting("height", value);
            break;
        case SETTING_VIDEO_VSYNC:
            saveSetting("vsync", value);
            break;
        case SETTING_AUDIO_GLOBAL_VOL:
            saveSetting("volume", value);
            break;
        case SETTING_AUDIO_MUSICS_VOL:
            saveSetting("music_vol", value);
            break;
        case SETTING_AUDIO_SOUNDS_VOL:
            saveSetting("sound_vol", value);
            break;
        case SETTING_CONTROLS_UP:
            saveSetting("up", value);
            break;
        case SETTING_CONTROLS_DOWN:
            saveSetting("down", value);
            break;
        case SETTING_CONTROLS_LEFT:
            saveSetting("left", value);
            break;
        case SETTING_CONTROLS_RIGHT:
            saveSetting("right", value);
            break;
        case SETTING_CONTROLS_USE_ITEM:
            saveSetting("use_item", value);
            break;
        case SETTING_CONTROLS_ITEM_1:
            saveSetting("item_1", value);
            break;
        case SETTING_CONTROLS_ITEM_2:
            saveSetting("item_2", value);
            break;
        case SETTING_CONTROLS_ITEM_3:
            saveSetting("item_3", value);
            break;
        case SETTING_CONTROLS_ITEM_4:
            saveSetting("item_4", value);
            break;
        case SETTING_CONTROLS_ITEM_5:
            saveSetting("item_5", value);
            break;
        case SETTING_CONTROLS_ITEM_6:
            saveSetting("item_6", value);
            break;
        case SETTING_CONTROLS_ITEM_7:
            saveSetting("item_7", value);
            break;
        case SETTING_CONTROLS_ITEM_8:
            saveSetting("item_8", value);
            break;
        case SETTING_CONTROLS_ITEM_9:
            saveSetting("item_9", value);
            break;
        
        default:
            break;
    }

    destroyEditBox();
}

void    editFullscreen() {
    puts("editFullscreen");
}
void    editWidth() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter width:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_VIDEO_WIDTH;
}
void    editHeight() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter height:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_VIDEO_HEIGHT;
}
void    editVSync() {
    t_dialog  *dialog;

    dialog = createEditBox("Choose VSync (0: off, 1: on):", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_VIDEO_VSYNC;
}


void    editGlobalVol() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter global volume:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_AUDIO_GLOBAL_VOL;
}
void    editMusicsVol() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter musics volume:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_AUDIO_MUSICS_VOL;
}
void    editSoundsVol() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter sounds volume:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_AUDIO_SOUNDS_VOL;
}

void    editUp() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter UP key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_UP;
}
void    editDown() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter DOWN key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_DOWN;
}
void    editLeft() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter LEFT key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_LEFT;
}
void    editRight() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter RIGHT key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_RIGHT;
}

void    editUseItem() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter USE ITEM key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_USE_ITEM;
}

void    editItem1() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter item 1 key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_ITEM_1;
}
void    editItem2() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter item 2 key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_ITEM_2;
}
void    editItem3() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter item 3 key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_ITEM_3;
}
void    editItem4() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter item 4 key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_ITEM_4;
}
void    editItem5() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter item 5 key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_ITEM_5;
}
void    editItem6() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter item 6 key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_ITEM_6;
}
void    editItem7() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter item 7 key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_ITEM_7;
}
void    editItem8() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter item 8 key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_ITEM_8;
}
void    editItem9() {
    t_dialog  *dialog;

    dialog = createEditBox("Enter item 9 key:", 20, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    memset(dialog->edit, 0, sizeof(dialog->edit));
    dialog->callback = editSettingCallback;
    dialog->arg = SETTING_CONTROLS_ITEM_9;
}
