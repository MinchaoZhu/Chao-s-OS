#include "kernel/idt.h" //register_interrupt_handler
#include "interrupt/interrupt.h" //isr_handler0~255

interrupt_handler_t interrupt_handlers[256];



void init_interrupt_handlers(){
    memset((void *)&interrupt_handlers, 0, sizeof(interrupt_handler_t) * 256);

    register_interrupt_handler(255, isr_handler255);

}

void register_interrupt_handler(uint8_t n, interrupt_handler_t h){
    interrupt_handlers[n] = h;
}

interrupt_handler_t isr_handler255(pt_regs *regs){

    printf("Interrupt No.255 is processed\n");
}



