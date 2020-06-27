#include <stdio.h>

#include "kernel/tty.h"
#include "kernel/gdt.h"
#include "kernel/idt.h"
#include "interrupt/timer.h"

interrupt_handler_t timer_test(pt_regs * regs){
	static int count = 0;
	printf("Timer tick: %d\n", count++);
}

void kernel_main(void) {
	init_gdt();
	init_idt();

	terminal_initialize();
    // register_interrupt_handler(32, &timer_test);


	init_timer(20 , timer_test);
	
	asm volatile ("sti");


	while(1);
}
