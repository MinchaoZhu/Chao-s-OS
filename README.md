# Chao-s-OS

A simple OS kernel project in GPLv3 license
- The output image of kernel is in Multiboot standard
- You can loaded it by qemu/bochs or other simulator.

## Tool-Chain

The project uses GNU Compiler Collection.
- gcc: version 10.1.0
    - COLLECT_GCC=i686-elf-gcc
    - which including c compiler, asm compiler, linker and freestanding libc
    - all assembly source files use AT&T Assembly Syntax surported by gcc as
- GNU Make 4.1
- grub-file (GRUB) 2.02-2ubuntu8.15
    - for multiboot image building

For cross gcc compiler, please refers to https://wiki.osdev.org/Building_GCC


## Build

- clone the whole repositery
- build the iso image
    ```
    $ export PATH="YOUR_I686_ELF_GCC_PATH/bin:$PATH"
    $ chomd +x clean.sh
    $ chomd +x headers.sh
    $ chomd +x iso.sh
    $ ./clean.sh
    $ ./headers.sh
    $ ./iso.sh
    ```
- run chao's os by using qemu
    ```
    $ ./qemu.sh
    ```


## File Organization
```kernel``` is folder for kernel part
    ```arch``` is for target architechture distribution
    ```include``` is for kernel part headers
    ```kernel``` is kernel source file

```libc``` is for c runtime lib
