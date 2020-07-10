# Chao-s-OS

A 32 bits X86 OS kernel project in GPLv3 license
- The output image of kernel is in Multiboot standard
- You can loaded it by qemu/bochs or other emulator.

## Feature
- Multiboot standard
- Physic memory management
    - support physic memory size up to 896MB (range from ```0xC000000``` to ```0xFFFFFFFF```)
    - buddy system

    ```
        physic Memory Map
        ┌──────────┬────────┬───────────────┬─────────────────────────────────┬──────────┐
        │ reserved ┼ kernel ┼ frames table  ┼       normal memory zone        ┼ reserved │          
        └──────────┴────────┴───────────────┴─────────────────────────────────┴──────────┘
                          ker_end      normal_base
    ```

- Virtual memory management
    - paging
        - linux-like mapping 
- Dynamic kernel memory  allocation
    - kmalloc()
    - kfree()
- Kernel thread: multitask
    - thread_create(&foo, *args)
- VESA video mode

## Tool-Chain

The project uses GNU Compiler Collection.
- gcc: version 10.1.0
    - COLLECT_GCC=i686-elf-gcc
    - c compiler
    - gas
    - ld
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
- ```kernel``` is folder for kernel part
    - ```arch``` is for target architechture distribution
        - ```i386```: source file and linker for i386
    - ```include``` is for kernel part headers
    - ```kernel``` is kernel source file

- ```libc``` is for c runtime lib
    - Some methods are basic for gcc freestanding mode
    - > Most of the compiler support routines used by GCC are present in libgcc, but there are a few exceptions. GCC requires the freestanding environment provide ```memcpy```, ```memmove```, ```memset``` and ```memcmp```. Finally, if __builtin_trap is used, and the target does not implement the trap pattern, then GCC emits a call to ```abort```.
    - https://gcc.gnu.org/onlinedocs/gcc/Standards.html


## Memory Management

1. Memory management layers:
    - PMM: buddy system
    - VMM: paging
    - Dynamic Allocation: kmalloc/free
        - slabs-like mechanism

![image](https://raw.githubusercontent.com/MinchaoZhu/Chao-s-OS/master/img/mem.svg)
2. PMM
- After kernel image, a page_frame array will be set. Each page entry linearly maps to a page in normal zone. 
- Kernel can use method from PMM to request one or more contigious pages.
- PMM uses buddy system.

3. VMM
- Physic memory address space [0, 0x38000000] linearly maps to virtual memory space ranging in [0xC0000000, 0xF8000000] in kernel's sight.

4. Dynamic Memory Allocation
- size > 2048 bytes: directly request pages from PMM
- size <= 2048 bytes: use a slabs-like mechanism
    - size >= 512: bitmap
    - 16bytes <= size < 512 bytes: slabs-like mechanism
- Memory Slice Allocator System: slabs-like mechanism

Memory Slice Allocator System:\

In a Memory Slice Allocator System, there are several types of allocator. Each type allocator is commited to allocate memory slice in a specific size. And each allocator excatly manages one page.
So in order to allocate a size K-bytes memory, MSA System will be invoked and it will select the allocator allocating smallest-size which fits K bytes. Then from the page the allocator manages, a slice will be returned to caller.
Also in order to save space and increase performance, different type allocator with different size will use different strategy to manage free and occupied slices in single page.

- size >= 512 bytes: bitmap. Hence for size = 512 bytes, a single page will be divided into 8 slices with 512 bytes, then the allocator uses the bitmap to manage it. Obviously manage a slice only need no more than 8 manipulate.
- 16bytes <= size < 512 bytes: linked list bucket. Here a slice will be a 8 bits header indicating next free slice appending 64 bytes 'useful' part, if allocator-size is 64 bytes. Obviously page size is hardly a multiple of slice size, so inner fragment will occur. In this way, some space will be wasted because of 8-bits header and no more than 64 bytes inner fragment. But find a free slice and release a allocated slice will be in constant time.

## Multi-Task

- Impelement multi-task of kernel threads in single processor

    - create a new kernel thread: thread_create(thread2, NULL);
    - release a thread: thread_exit(pid);
