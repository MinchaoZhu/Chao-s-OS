#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/gdt.h>

void kernel_main(void) {
	init_gdt();

	terminal_initialize();
	printf("Hello, kernel World!\n");
	printf("'\\n' represents new line now!\n");
	for(int i = 3; i<=28; ++i){	
		printf("Rolling test: line %d\n", i);
	}
	printf("%d\n", 10000000);
	printf("%d\n", 0);
	
	printf("%d\n", -10000000);
	printf("%d\n", -2147483648);
	printf("%d\n", 2147483647);

}
