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








// irq0 - irq15
// isr32- isr47
// void isr32(); // irq0    system timer (cannot be changed)
// void isr33(); // irq1    keyboard controller (cannot be changed)
// void isr34(); // irq2    cascaded signals from IRQs 8–15 (any devices configured to use IRQ 2 will actually be using IRQ 9)
// void isr35(); // irq3    serial port controller for serial port 2 (shared with serial port 4, if present)
// void isr36(); // irq4    serial port controller for serial port 1 (shared with serial port 3, if present)
// void isr37(); // irq5    parallel port 2 and 3  or  sound card
// void isr38(); // irq6    floppy disk controller
// void isr39(); // irq7    parallel port 1.
// void isr40(); // irq8    real-time clock (RTC)
// void isr41(); // irq9    Advanced Configuration and Power Interface (ACPI) system control interrupt on Intel chipsets.
// void isr42(); // irq10   The Interrupt is left open for the use of peripherals (open interrupt/available, SCSI or NIC)
// void isr43(); // irq11   The Interrupt is left open for the use of peripherals (open interrupt/available, SCSI or NIC)
// void isr44(); // irq12   mouse on PS/2 connector
// void isr45(); // irq13   CPU co-processor  or  integrated floating point unit  or  inter-processor interrupt (use depends on OS)
// void isr46(); // irq14   primary ATA channel (ATA interface usually serves hard disk drives and CD drives)
// void isr47(); // irq15   secondary ATA channel



#endif