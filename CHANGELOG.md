# Change Log

## Unreleased
- add global description table
- add idt table
- add frame to register interrupt handler implement
    - defined in kernel/intterrupt.h
    - impelements in kernel/intterrupt.c

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