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
    uint32_t prev_page; //indicate index of previous unallocate pages block in the free list
    uint32_t next_page; //indicate index of next unallocate pages block in the free list
} __attribute__((packed)) page_t;


typedef struct normal_mem_t {
    uint32_t base; // physic start address
    uint32_t length; // size in bytes
    uint32_t pages_number; // size in pages
    uint32_t free_lists[BUDDY_MAXLEVEL];  // free list
    // page_t  pages[MAX_PAGES_NUM]; // pages table, fixed
    page_t*  pages; // pages table, dynamic
} __attribute__((packed)) normal_mem_t;

void print_free_buddy_blocks();

// start and end pisition in memory of kernel file
extern uint8_t kernel_start[];
extern uint8_t kernel_end[];


// print physical memory map
void show_memory_map();

// init normal memory zone
void init_normal_mem_zone();

// allocate physic memory
void* kmalloc(uint32_t byte_size);

// allocate pages
uint32_t palloc(uint32_t page_size);


// free physic memory
uint32_t kfree(void* addr);


inline uint32_t is_power_of_2 (uint32_t n);
inline uint32_t next_power_of_2 (uint32_t n);
inline uint32_t power_of_2 (uint32_t n);

// buddy_system

/** 
 * return first page index
 * */
uint32_t buddy_system_get_page_block(uint32_t level);

uint32_t buddy_system_free_page_block(uint32_t page_index);


#endif // INCLUDE_PMM_H