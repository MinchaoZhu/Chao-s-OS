# Change Log

## Unreleased
### Added
- global description table
- idt table
- frame to register interrupt handler implement
    - defined in kernel/intterrupt.h
    - impelements in kernel/intterrupt.c
- isr for ISR0 to ISR15
- a timer interrupt handler entry which can be assigned by user
- tty cursor move
- add surport for ```%x``` and ```%X```
- frame table
- physic memory management: buddy system
    - physic_malloc(size_t size_in_bytes) : acquire a contigious physic memory in pages
    - physic_free(void* pointer) : free a allocated memory from physic_free 
- support for any memory size less than 896 MiB
- virtual memory space and paging
- printf: add surport for ```%0x```, ```%0X```, ```%08x```, ```%08X```, ```%8x```, ```%8X```
- printf: add surport for ```%#x```, ```%#X```, ```%#0x```, ```%#0X```, ```%#08x```, ```%#08X```, ```%#8x```, ```%#8X```
- dynamic memory allocation
    - kmalloc(): slabs-like memory slice allocation system
    - kfree()

### Change
- Change the image layout in order to support linux-like virtual memory mapping
    - boot.S
    - linker.ld
    - add kernel_main_init() before kernel_main()
- Port the pmm.c into virtual space, now kernel can allocate pages in virtual space and get the physic address


### Fixed
- bug: int tty.c, output bug of terminal_roll()


## [0.0.1] - 2020-06-22
### Added
- kernel skeleton
- build script
- global constructor
- kernel output to display
- some string.h implementations:
    - memcmp.c
    - memcpy.c
    - memmove.c
    - memset.c
    - strlen.c
- some stdio.h implementations:
    - printf.c
        - only support ```%c```, ```%d```, ```%s```
    - putchar.c
    - puts.c
- some stdlib.h implementations:
    - abort.c