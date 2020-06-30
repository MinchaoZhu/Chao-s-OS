#include <stdio.h>

#include "kernel/tty.h"
#include "kernel/gdt.h"
#include "kernel/idt.h"
#include "interrupt/timer.h"
#include "kernel/multiboot.h"
#include "kernel/pmm.h"

extern normal_mem_t normal_mem;

interrupt_handler_t timer_test(pt_regs * regs){
	static int count = 0;
	printf("Timer tick: %d\n", count++);
}

void kernel_main(void) {
	init_gdt();
	init_idt();

	terminal_initialize();

	init_timer(20 , timer_test);
	
	// show_memory_map();
	init_normal_mem_zone();


    printf("%X\n", kernel_start);
    // printf("%X\n", kernel_end);
	// printf("avail start: %X\n", normal_mem.base);
	// printf("avail len: %X\n", normal_mem.length);

	// print_free_buddy_blocks();

	// uint32_t* array   = 0; uint32_t* array1  = 0; uint32_t* array2  = 0; uint32_t* array3  = 0; uint32_t* array4  = 0; uint32_t* array5  = 0; uint32_t* array6  = 0; uint32_t* array7  = 0; uint32_t* array8  = 0; uint32_t* array9  = 0; uint32_t* array10 = 0; uint32_t* array11 = 0; uint32_t* array12 = 0; uint32_t* array13 = 0; uint32_t* array14 = 0; uint32_t* array15 = 0; uint32_t* array16 = 0; uint32_t* array17 = 0; uint32_t* array18 = 0;

	// array = (uint32_t*) physic_malloc(sizeof(uint32_t) * 1456);
	// array1 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 542);
	// array2 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 147856);
	// array3 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 5436);
	// array4 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 56);
	// array5 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 146);
	// array6 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 1452);
	// array7 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 475);
	// array8 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 2014);
	// array9 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 54245);
	// array10 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 140056);
	// array11 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 4276);
	// array12 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 5724);
	// array13 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 142456);
	// array14 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 1);
	// array15 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 22);
	// array16 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 48856);
	// array17 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 145216);
	// array18 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 1005127);
	// // print_free_buddy_blocks();

	// // print_free_buddy_blocks();
	// physic_free(array3);
	// physic_free(array2);
	// physic_free(array15);
	// physic_free(array4);
	// physic_free(array9);
	// physic_free(array10);
	// physic_free(array11);
	// physic_free(array7);
	// physic_free(array12);
	// physic_free(array13);
	// physic_free(array1);
	// physic_free(array14);
	// physic_free(array16);
	// physic_free(array17);
	// physic_free(array5);
	// physic_free(array6);
	// physic_free(array8);
	// physic_free(array18);
	// physic_free(array);


	// // print_free_buddy_blocks();




	// array = (uint32_t*) physic_malloc(sizeof(uint32_t) * 1456);
	// array1 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 542);
	// array16 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 48856);
	// array17 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 145216);
	// array18 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 1005127);
	// array5 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 146);
	// array11 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 4276);
	// array12 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 5724);
	// array13 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 142456);
	// array3 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 5436);
	// array4 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 56);
	// array10 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 140056);
	// array14 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 1);
	// array2 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 147856);
	// array15 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 22);
	// array6 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 1452);
	// array7 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 475);
	// array8 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 2014);
	// array9 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 54245);

	// physic_free(array3);
	// physic_free(array14);
	// physic_free(array16);
	// physic_free(array2);
	// physic_free(array1);
	// physic_free(array17);
	// physic_free(array8);
	// physic_free(array18);
	// physic_free(array);
	// physic_free(array15);
	// physic_free(array4);
	// physic_free(array13);
	// physic_free(array9);
	// physic_free(array5);
	// physic_free(array6);
	// physic_free(array10);
	// physic_free(array11);
	// physic_free(array7);
	// physic_free(array12);



	// array = (uint32_t*) physic_malloc(sizeof(uint32_t) * 1456);
	// array1 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 542);
	// array16 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 48856);
	// array14 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 1);
	// array2 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 147856);
	// array15 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 22);
	// array4 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 56);
	// array10 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 140056);
	// array9 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 54245);
	// array6 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 1452);
	// array17 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 145216);
	// array18 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 1005127);
	// array5 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 146);
	// array11 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 4276);
	// array7 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 475);
	// array8 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 2014);
	// array13 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 142456);
	// array3 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 5436);
	// array12 = (uint32_t*) physic_malloc(sizeof(uint32_t) * 5724);

	// physic_free(array18);
	// physic_free(array);
	// physic_free(array15);
	// physic_free(array4);
	// physic_free(array8);
	// physic_free(array5);
	// physic_free(array10);
	// physic_free(array13);
	// physic_free(array9);
	// physic_free(array16);
	// physic_free(array17);
	// physic_free(array11);
	// physic_free(array6);
	// physic_free(array7);
	// physic_free(array12);
	// physic_free(array2);
	// physic_free(array1);
	// physic_free(array3);
	// physic_free(array14);

	// print_free_buddy_blocks();
	// uint32_t* free_list = normal_mem.free_lists;
	// page_t* pages = normal_mem.pages;
	// printf("level: %X\n", pages[0x7B00].level);
	// printf("flags: %X\n", pages[0x7B00].flags);
	// printf("next: %X\n", pages[0x7B00].next_page);
	// printf("prev: %X\n", pages[0x7B00].prev_page);



	// asm volatile ("sti"); // active interrupt
	while(1);
}
