#ifndef INCLUDE_TIMER_H_
#define INCLUDE_TIMER_H_

#include <stdint.h>
#include "interrupt/interrupt.h"

void init_timer(uint32_t frequency, interrupt_handler_t timer_callback);


#endif