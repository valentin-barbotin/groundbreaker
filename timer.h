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

#endif /* !FOO_H */
