#ifndef INTERRUPT_TIMER_H_
#define INTERRUPT_TIMER_H_

#include <stdint.h>
#include "interrupt/interrupt.h"

#define TIMER_FREQUENCY_MAX 1193180
#define TIMER_FREQUENCY_MIN 2

void init_timer(uint32_t frequency, interrupt_handler_t timer_callback);




#endif