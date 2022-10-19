#include <SDL_timer.h>
#include "timer.h"

void    initTimer(t_timer *timer) {
    timer->paused = false;
    timer->started = false;
    timer->startTicks = 0;
    timer->pausedTicks = 0;
}

void    startTimer(t_timer *timer) {
    timer->started = true;
    timer->paused = false;
    timer->startTicks = SDL_GetTicks64();
    timer->pausedTicks = 0;
}

void    stopTimer(t_timer *timer) {
    timer->started = false;
    timer->paused = false;
    timer->startTicks = 0;
    timer->pausedTicks = 0;
}

void    pauseTimer(t_timer *timer) {
    if (timer->started && !timer->paused) {
        timer->paused = true;
        timer->pausedTicks = SDL_GetTicks64() - timer->startTicks;
    }
}

void    unpauseTimer(t_timer *timer) {
    if (timer->started && timer->paused) {
        timer->paused = false;
        timer->startTicks = SDL_GetTicks64() - timer->pausedTicks;
        timer->pausedTicks = 0;
    }
}

// Print the timer in the console [DEBUG ONLY]
void    printTimer(t_timer *timer) {
    printf("Timer Milliseconds: %d\n", getTimerTicks(timer));
    printf("Timer seconds: %d\n", getTimerSeconds(timer));
}

// Return the number of ticks since the timer started in milliseconds (s)
unsigned int    getTimerTicks(t_timer *timer) {
    unsigned int time = 0;
    if (timer->started) {
        if (timer->paused)
            time = timer->pausedTicks;
        else
            time = SDL_GetTicks64() - timer->startTicks;
    }
    return time;
}

// transform milliseconds ticks to seconds
unsigned int   getTimerSeconds(t_timer *timer) {
    return getTimerTicks(timer) / 1000;
}

bool    isTimerStarted(t_timer *timer) {
    return timer->started;
}

bool    isTimerPaused(t_timer *timer) {
    return timer->paused;
}

