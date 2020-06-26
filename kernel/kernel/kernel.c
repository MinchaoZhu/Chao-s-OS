#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>

void kernel_main(void) {
	init_gdt();
	init_idt();

	terminal_initialize();

	printf("0x3");
	asm volatile ("int $0x3");
	printf("255");
    asm volatile ("int $255");


}
