#include <stdlib.h>
#include <stdio.h>

#include "menu.h"
#include "settings.h"
#include "dialog.h"

void    editSettingCallback() {
    const t_dialog  *dialog;
    const char      *value;

    dialog = getEditBox();
    value = dialog->edit;
    switch (dialog->arg)
    {
        case SETTING_VIDEO_FULLSCREEN:
            break;
        case SETTING_VIDEO_WIDTH:
            break;
        case SETTING_VIDEO_HEIGHT:
            break;
        case SETTING_VIDEO_VSYNC:
            break;
        case SETTING_AUDIO_GLOBAL_VOL:
            break;
        case SETTING_AUDIO_MUSICS_VOL:
            break;
        case SETTING_AUDIO_SOUNDS_VOL:
            break;
        case SETTING_CONTROLS_UP:
            break;
        case SETTING_CONTROLS_DOWN:
            break;
        case SETTING_CONTROLS_LEFT:
            break;
        case SETTING_CONTROLS_RIGHT:
            break;
        case SETTING_CONTROLS_USE_ITEM:
            break;
        case SETTING_CONTROLS_ITEM_1:
            break;
        case SETTING_CONTROLS_ITEM_2:
            break;
        case SETTING_CONTROLS_ITEM_3:
            break;
        case SETTING_CONTROLS_ITEM_4:
            break;
        case SETTING_CONTROLS_ITEM_5:
            break;
        case SETTING_CONTROLS_ITEM_6:
            break;
        case SETTING_CONTROLS_ITEM_7:
            break;
        case SETTING_CONTROLS_ITEM_8:
            break;
        case SETTING_CONTROLS_ITEM_9:
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
