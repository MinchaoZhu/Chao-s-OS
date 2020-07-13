#include "interrupt/timer.h"
#include "io/ioport.h"
#include "kernel/kmm.h"



timer_t* timers = 0;

static void timer_schedule(pt_regs* regs);
 
void init_timer(){
    register_interrupt_handler(32, timer_schedule);
    uint32_t divisor = TIMER_FREQUENCY_MAX / TIMER_FRE; // 1.19318 MHz
    outb(0x43, 0x36);

    uint8_t divisor_low = (uint8_t)(divisor & 0xFF);
    uint8_t divisor_hign = (uint8_t)((divisor >> 8) & 0xFF);

    outb(0x40, divisor_low);
    outb(0x40, divisor_hign);

}


void timer_schedule(pt_regs* regs) {
    timer_t* current_timer = timers;
    while(current_timer) {
        if(--(current_timer->elapse) == 0) {
            current_timer->elapse = current_timer->interval;
            (*(current_timer->callback_func))();
        }
        current_timer = current_timer -> next;
    }
}

// frequency: [2, 1193180]
void register_timer(timer_callback callback, uint32_t frequency) {
    if(frequency >= TIMER_FREQUENCY_MIN && frequency <= TIMER_FREQUENCY_MAX) {
        timer_t* new_timer = (timer_t*) kmalloc(sizeof(timer_t));
        uint32_t interval = TIMER_FRE/frequency;
        new_timer -> callback_func = callback;
        new_timer -> elapse = interval;
        new_timer -> interval = interval;
        new_timer -> next = 0;
        if(timers == 0) {
            timers = new_timer;
        }
        else {
            new_timer -> next = timers;
            timers = new_timer;
        }
    }
}

void unregister_timer(timer_t* timer) {
    timer_t* current = timers;
    timer_t* prev = timers;

    while(current!=0 && current!=timer) {
        prev = current;
        current = current->next;
    }

    if(current == timer) {
        if(prev) {
            prev->next = current->next;
        }
        else {
            timers = current->next;
        }
        kfree(timer);
    }

}