#include <stdio.h>

#include "kernel/tty.h"
#include "kernel/gdt.h"
#include "kernel/idt.h"
#include "interrupt/timer.h"
#include "kernel/multiboot.h"
#include "kernel/pmm.h"
#include "kernel/vmm.h"

extern normal_mem_t normal_mem;
extern page_directory_t pgd_k[1024];
extern page_t tables[KERNEL_NORMAL_ZONE_DIRECTORY_SIZE + 1][1024];
__attribute__((aligned(4096))) char kernel_stack[STACK_SIZE];



__attribute__((section(".init.data"))) uint32_t* pgd_tmp = (uint32_t*) 0x1000;

__attribute__((section(".init.data"))) uint32_t* pt0 = (uint32_t*) 0x2000;
__attribute__((section(".init.data"))) uint32_t* pt1 = (uint32_t*) 0x3000;

__attribute__((section(".init.text"))) void kernel_main_init(void) {
	pgd_tmp[0] = ((uint32_t)pt0) | PAGE_PRESENT | PAGE_WRITE;
	pgd_tmp[PGD_INDEX(PAGE_OFFSET)] = ((uint32_t)pt1) | PAGE_PRESENT | PAGE_WRITE;

	for(uint32_t i = 0; i < 1024; ++i) {
		pt0[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
		pt1[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}

	asm volatile ("mov %0, %%cr3" : : "r" (pgd_tmp));

	uint32_t cr0;

	asm volatile ("mov %%cr0, %0" : "=r" (cr0));
    cr0 |= 0x80000000;
    asm volatile ("mov %0, %%cr0" : : "r" (cr0));

	uint32_t kernel_stack_top = (uint32_t) (&kernel_stack) + STACK_SIZE;
    asm volatile ("mov %0, %%esp\n\t"
            "xor %%ebp, %%ebp" : : "r" (kernel_stack_top));

	global_multiboot_ptr = (multiboot_t*)((uint32_t)global_multiboot_ptr_tmp + PAGE_OFFSET);
	init_vmm();
	// printf("xxx\n");
	kernel_main();
}




void kernel_main(void) {
	pgd_k[0] = 0; // reset the tmp directory
	switch_pgd(((uint32_t)&pgd_k) - PAGE_OFFSET);

	asm volatile ("call _init");  // call global constructor
	init_gdt();
	init_idt();
	terminal_initialize();

	init_normal_mem_zone();

	// printf("%d\n", 12461638);
	// printf("%x\n", 12461638);
	// printf("%X\n", 12461638);
	// printf("%0x\n", 12461638);
	// printf("%0X\n", 12461638);
	// printf("%8x\n", 12461638);
	// printf("%8X\n", 12461638);
	// printf("%08x\n", 12461638);
	// printf("%08X\n", 12461638);
	// printf("%#x\n", 12461638);
	// printf("%#X\n", 12461638);
	// printf("%#0x\n", 12461638);
	// printf("%#0X\n", 12461638);
	// printf("%#8x\n", 12461638);
	// printf("%#8X\n", 12461638);
	// printf("%#08x\n", 12461638);
	// printf("%#08X\n", 12461638);
	// printf("12461638\n", 12461638);

	asm volatile ("sti"); // active interrupt
	while(1);
}
