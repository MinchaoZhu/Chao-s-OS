#include "interrupt/timer.h"
#include "kernel/ioport.h"
#include "kernel/kmm.h"

// frequency: [2, 1193180] 
void init_timer(uint32_t frequency, interrupt_handler_t timer_callback){
    
    register_interrupt_handler(32, timer_callback);
    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);

    uint8_t divisor_low = (uint8_t)(divisor & 0xFF);
    uint8_t divisor_hign = (uint8_t)((divisor >> 8) & 0xFF);

    outb(0x40, divisor_low);
    outb(0x40, divisor_hign);

}
