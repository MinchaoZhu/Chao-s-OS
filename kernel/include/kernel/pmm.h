#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

#include "kernel/multiboot.h"

// maximum memory allocation size is 2^10 pages = 4 MiB
#define BUDDY_MAXLEVEL 11

// page size in byte
#define PAGE_SIZE_POWER 12
#define PAGE_SIZE (1<<PAGE_SIZE_POWER)

// number of pages
#define MAX_PAGES_NUM (1<<(32 - PAGE_SIZE_POWER))

#define PAGE_BLOCK_HEAD (1<<1) // this page is head of contigous pages block
#define PAGE_ALLOCATED  1 // this page is allocated

#define ALIGN_MASK 0xFFFFF000
#define ALIGN_MASK_HELPER 0xFFF


typedef struct page_t {
    uint8_t flags; // indicate status of the page
    uint8_t level; // indicate size: 2^level after it belongs to it
    uint32_t index; // indicate relative index in pages array
    uint32_t next_page; //indicate index of next unallocate pages block in the free list
    uint32_t addr; // physic address
} __attribute__((packed)) page_t;


typedef struct normal_mem_t {
    uint32_t base; // physic start address
    uint32_t length; // size in bytes
    uint32_t pages_number; // size in pages
    uint32_t free_lists[BUDDY_MAXLEVEL];  // free list
    page_t  pages[MAX_PAGES_NUM]; // pages table
} __attribute__((packed)) normal_mem_t;








// start and end pisition in memory of kernel file
extern uint8_t kernel_start[];
extern uint8_t kernel_end[];



// print physical memory map
void show_memory_map();

// init normal memory zone
void init_normal_mem_zone();


#endif // INCLUDE_PMM_H