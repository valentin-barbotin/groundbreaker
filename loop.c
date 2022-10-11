#include "loop.h"

extern SDL_Rect g_buttonsLocation[4];

/**
 * @brief Handle a mouse button up event
 * @param event the event to handle
 * @return void
 */
void handleMouseButtonUp(SDL_Event *event) {
    //loop through buttons
    if (true) {
    // if (inMainMenu()) {
        for (int i = 0; i < 4; ++i) {
            // get click position
            int xStart = g_buttonsLocation[i].x;
            int yStart = g_buttonsLocation[i].y;

            SDL_Point click = { event->button.x, event->button.y };
            SDL_Rect button = { xStart, yStart, g_buttonsLocation[i].w, g_buttonsLocation[i].h };

            if (SDL_PointInRect(&click, &button))
            {
                printf("BUTTON %d CLICKED\n", i);
            }
        }
    }
}