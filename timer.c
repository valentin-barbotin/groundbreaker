#include <SDL_timer.h>
#include "timer.h"

void    startTimer(t_timer *timer) {
    timer->startTicks = SDL_GetTicks64();
}

void    stopTimer(t_timer *timer) {
    timer->paused = false;
    timer->startTicks = 0;
    timer->pausedTicks = 0;
}

void    pauseTimer(t_timer *timer) {
    if (!isTimerPaused(timer) && !isTimerFinished(timer)) {
        timer->paused = true;
        timer->pausedTicks = getTimerDuration(timer);
    }
}

void    unpauseTimer(t_timer *timer) {
    if (isTimerPaused(timer) && !isTimerFinished(timer)) {
        timer->paused = false;
        timer->startTicks = getTimerDuration(timer);
        timer->pausedTicks = 0;
    }
}

// Print timer info in console
void    printTimer(t_timer *timer) {
    printf("Timer Milliseconds: %d, Timer Seconds: %d, Duration: %d, Timer Paused: %d, Timer Finished: %d\n",
           getTimerTicks(timer), getTimerSeconds(timer),getDuration(timer), isTimerPaused(timer), isTimerFinished(timer));
}

// Get duration of the timer
unsigned int    getTimerDuration(t_timer *timer) {
    if (isTimerPaused(timer)) {
        return SDL_GetTicks64() - timer->startTicks;
    }
    return SDL_GetTicks64() - timer->pausedTicks;
}

// Return the number of ticks since the timer started in milliseconds (s)
unsigned int    getTimerTicks(t_timer *timer) {
    if (isTimerPaused(timer)) {
        return timer->pausedTicks;
    }
    return SDL_GetTicks64() - timer->startTicks;
}

// transform milliseconds ticks to seconds
unsigned int   getTimerSeconds(t_timer *timer) {
    return getTimerTicks(timer) / 1000;
}

// return duration max of the timer
unsigned int    getDuration(t_timer *timer) {
    return timer->duration;
}

// Return the number of seconds left before the timer is finished
unsigned int   getTimeLeft(t_timer *timer) {
    return getDuration(timer) - getTimerSeconds(timer);
}

bool    isTimerPaused(t_timer *timer) {
    return timer->paused;
}

bool    isTimerFinished(t_timer *timer) {
    return getTimerSeconds(timer) >= getDuration(timer);
}
