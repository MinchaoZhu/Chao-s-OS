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
- pages table
- physic memory management: buddy system
    - kmalloc(size_t size_in_bytes) : acquire a contigious physic memory
    - kfree(void* pointer) : free a allocated memory from kmalloc 
- support for any memory size less than 4 GiB

### Fixed
- bug: int tty.c, output bug of terminal_roll()
}
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