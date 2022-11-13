#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "dialog.h"
#include "font.h"
#include "utils.h"

#define DEBUG true

/**
 * @brief Create a Edit Box object
 * 
 * @param text 
 * @param fontSize 
 * @param color 
 * @param backgroundColor 
 */
t_dialog    *createEditBox(const char *text, const int fontSize, SDL_Color color, SDL_Color backgroundColor) {
    t_dialog  *dialog;

    dialog = getEditBox();
    dialog->active = true;
    dialog->text = text;
    dialog->edit[0] = '\0'; // set the first char to null
    dialog->fontSize = fontSize;
    dialog->color = color;
    dialog->backgroundColor = backgroundColor;
    dialog->arg = 0;

    SDL_StartTextInput();

    return dialog;
}

/**
 * @brief Get the Edit Box object
 * 
 * @return t_dialog* 
 */
t_dialog    *getEditBox() {
    static t_dialog     *dialog;

    if (dialog == NULL) {
        dialog = malloc(sizeof(t_dialog));
        if (dialog == NULL) {
            #ifdef DEBUG
                fprintf(stderr, "Error allocating memory for dialog");
            #endif
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game crashed", SDL_GetError(), g_window);
            exit(1);
        }

        dialog->active = false;
        dialog->text = NULL;
        memset(dialog->edit, 0, sizeof(dialog->edit));
    }

    return dialog;
}

void        destroyEditBox() {
    t_dialog     *dialog;

    dialog = getEditBox();
    dialog->active = false;
    SDL_StopTextInput();
}

/**
 * @brief Render the dialog box
 * 
 */
void        displayEditBox() {
    const t_dialog  *dialog;
    SDL_Rect        rect;
    int             x;
    int             y;

    dialog = getEditBox();

    rect.w = gameConfig->video.width / 3;
    rect.h = gameConfig->video.width / 4;
    rect.x = (gameConfig->video.width - rect.w) / 2;
    rect.y = (gameConfig->video.height - rect.h) / 2;

    //background
    SDL_RenderFillRect(g_renderer, &rect);

    pickColor(&dialog->backgroundColor);
    loadFont(FONT_PATH, dialog->fontSize);

    // text
    //TODO: get text width and height
    x = rect.x + (rect.w / 2);
    y = rect.y + dialog->fontSize;
    drawText(&dialog->color, x, y, dialog->text, true, rect.w);

    // user input
    x = rect.x + (rect.w / 2);
    y = rect.y + (rect.h / 2);
    drawText(&dialog->color, x, y, dialog->edit, true, rect.w);

}

void    handleKeyUpDialog(const SDL_Event *event) {
    t_dialog *dialog = getEditBox();

    switch (event->key.keysym.sym) {
        case SDLK_c:
            if (event->key.keysym.mod & KMOD_CTRL) {
                SDL_SetClipboardText(dialog->edit);
            }
            break;
        case SDLK_v:
            if (event->key.keysym.mod & KMOD_CTRL) {
                char *clipboard = SDL_GetClipboardText();
                if (clipboard != NULL) {
                    strcpy(dialog->edit, clipboard);
                    SDL_free(clipboard);
                }
            }
            break;
        case SDLK_RETURN:
            if (dialog->callback != NULL) {
                #ifdef DEBUG
                    puts("CALLBACK");
                #endif
                dialog->callback(dialog->edit);
            }

            break;
    
        default:
            break;
    }
}
