#include "kernel/idt.h" //register_interrupt_handler
#include "interrupt/interrupt.h" //isr_handler0~255
#include <stdio.h>
#include <string.h>
interrupt_handler_t interrupt_handlers[256];



void init_interrupt_handlers() {
    memset((void *)&interrupt_handlers, 0, sizeof(interrupt_handler_t) * 256);

    register_interrupt_handler(255, isr_handler255);
    register_interrupt_handler(14, page_fault);

}

void register_interrupt_handler(uint8_t n, interrupt_handler_t h) {
    interrupt_handlers[n] = h;
}

void isr_handler255(pt_regs *regs){

    printf("Interrupt No.255 is processed\n");
}

void page_fault(pt_regs *regs) {
    uint32_t cr2;
    asm volatile ("mov %%cr2, %0" : "=r" (cr2));

    printf("Page fault at 0x%x, virtual faulting address 0x%x\n", regs->eip, cr2);
    printf("Error code: %x\n", regs->err_code);

    // bit 0 not present
    if ( !(regs->err_code & 0x1)) {
        printf("Because the page wasn't present.\n");
    }
    // bit 1 Write or read error
    if (regs->err_code & 0x2) {
        printf("Write error.\n");
    } else {
        printf("Read error.\n");
    }
    // bit 2
    if (regs->err_code & 0x4) {
        // in user mode
        printf("In user mode.\n");
    } else {
        // in kernel mode
        printf("In kernel mode.\n");
    }
    // bit 3
    if (regs->err_code & 0x8) {
        printf("Reserved bits being overwritten.\n");
    }
    // bit 4
    if (regs->err_code & 0x10) {
        printf("The fault occurred during an instruction fetch.\n");
    }

    while (1);


}

