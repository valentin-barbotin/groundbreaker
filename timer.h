#ifndef TIMER_H
 #define TIMER_H

 #include <stdbool.h>
 
 typedef struct  s_timer
 {
     unsigned int    startTicks;
     unsigned int    pausedTicks;
     bool            paused;
     bool            started;
     void            (*start)(struct s_timer*);
     void            (*stop)(struct s_timer*);
 }               t_timer;

void           initTimer(t_timer *timer);
void           startTimer(t_timer *timer);
void           stopTimer(t_timer *timer);
void           pauseTimer(t_timer *timer);
void           unpauseTimer(t_timer *timer);
void           printTimer(t_timer *timer);
unsigned int   getTimerTicks(t_timer *timer);
unsigned int   getTimerSeconds(t_timer *timer);
bool           isTimerStarted(t_timer *timer);
bool           isTimerPaused(t_timer *timer);

#endif
