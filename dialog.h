#ifndef DIALOG_H
 #define DIALOG_H
 
 #include <SDL.h>
 
 #include "game.h"

 extern t_gameConfig    *gameConfig;

 typedef struct  s_dialog
 {
     bool           active;
     SDL_Rect       rect;
     const char     *text;
     char           edit[256];
     int            fontSize;
     SDL_Color      color;
     SDL_Color      backgroundColor;
     void           (*callback)();
 }               t_dialog;

 t_dialog    *createEditBox(const char *text, const int fontSize, SDL_Color color, SDL_Color backgroundColor);
 t_dialog    *getEditBox();
 void        destroyEditBox() ;
 void        displayEditBox();

#endif