#ifndef TIMER_H
 #define TIMER_H

 #include <stdbool.h>
 
 typedef struct  s_timer
 {
     unsigned int    startTicks;
     unsigned int    pausedTicks;
     unsigned int   duration;
     bool            paused;
     void            (*start)(struct s_timer *timer);
     void            (*stop)(struct s_timer*);
 }               t_timer;

 void           startTimer(t_timer *timer);
 void           stopTimer(t_timer *timer);
 void           pauseTimer(t_timer *timer);
 void           unpauseTimer(t_timer *timer);
 void           printTimer(t_timer *timer);
 unsigned int   getTimerTicks(t_timer *timer);
 unsigned int   getTimerSeconds(t_timer *timer);
 unsigned int   getTimerDuration(t_timer *timer);
 unsigned int   getTimeLeft(t_timer *timer);
 unsigned int   getDuration(t_timer *timer);
 bool           isTimerPaused(t_timer *timer);
 bool           isTimerFinished(t_timer *timer);

#endif
