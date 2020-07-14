#ifndef INTERRUPT_TIMER_H_
#define INTERRUPT_TIMER_H_

#include <stdint.h>
#include "interrupt/interrupt.h"

#define TIMER_FREQUENCY_MAX 1193180
#define TIMER_FREQUENCY_MIN 2

#define TIMER_FRE 1000 // testing frequency, in republish version, it shouble be 1193180


typedef void (*timer_callback)(void);

// kernel timer
typedef struct timer {
    timer_callback callback_func;  // call back function
    uint32_t interval;
    uint32_t elapse;
    struct timer *next;            // linked list next 
} timer_t;





void init_timer();
void register_timer(timer_callback callback, uint32_t frequency);
void unregister_timer(timer_t* timer);

#endif