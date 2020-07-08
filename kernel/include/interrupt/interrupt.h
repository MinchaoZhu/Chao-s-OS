#ifndef INTERRUPT_INT_H_
#define INTERRUPT_INT_H_

#include "kernel/idt.h"

// interrupt handler process method pointer
typedef void (*interrupt_handler_t)(pt_regs *);

void init_interrupt_handlers();

void register_interrupt_handler(uint8_t n, interrupt_handler_t h);

// call interrupt handler funtion
interrupt_handler_t isr_handler255(pt_regs *regs);
interrupt_handler_t page_fault(pt_regs *regs);

#endif