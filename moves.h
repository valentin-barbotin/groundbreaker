#ifndef MOVES_H
 #define MOVES_H

 #include <SDL.h>

 extern t_gameConfig    *gameConfig;
 
 #define VELOCITY 5
 void    handleKeyDownPlay(const SDL_Event *event);
 void    handleKeyUpPlay(const SDL_Event *event);
 void    checkBorders();

#endif