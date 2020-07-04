#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

/**
 * Memory Map
 * ┌──────────┬────────┬───────────────┬──────────────────────────────────┬──────────┐
 * │ reserved ┼ kernel ┼ frames table  ┼        memory for kmalloc        ┼ reserved │          
 * └──────────┴────────┴───────────────┴──────────────────────────────────┴──────────┘
 *                   ker_end      normal_base
 * */

#include "kernel/multiboot.h"
// maximum memory allocation size is 2^10 pages = 4 MiB
#define BUDDY_MAXLEVEL 11

// page size in byte
#define PAGE_FRAME_SIZE_POWER 12
#define PAGE_FRAME_SIZE (1<<PAGE_FRAME_SIZE_POWER)

// number of pages
#define MAX_PAGE_FRAME_NUM (1<<(32 - PAGE_FRAME_SIZE_POWER))

#define PAGE_FRAME_BLOCK_HEAD (1<<1) // this page is head of contigous pages block
#define PAGE_FRAME_ALLOCATED  1 // this page is allocated

#define ALIGN_MASK 0xFFFFF000
#define ALIGN_MASK_HELPER 0xFFF


typedef struct page_frame_t {
    uint8_t flags; // indicate status of the page
    uint8_t level; // indicate size: 2^level after it belongs to it
    uint32_t physic_addr; // physic address of this frame
    uint32_t index; // indicate relative index in pages array
    uint32_t prev_frame; //indicate index of previous unallocate pages block in the free list
    uint32_t next_frame; //indicate index of next unallocate pages block in the free list
    uint32_t allocator; // address of the slice alloc, not empty if this page is assigned for an allocator
} __attribute__((packed)) page_frame_t;

typedef struct normal_mem_t {
    uint32_t base; // physic start address
    uint32_t length; // size in bytes
    uint32_t frames_number; // size in pages
    uint32_t free_lists[BUDDY_MAXLEVEL];  // free list
    // page_t  pages[MAX_PAGES_NUM]; // pages table, fixed
    page_frame_t*  frames; // pages table, dynamic
} __attribute__((packed)) normal_mem_t;

void print_free_buddy_blocks();

// start and end pisition in memory of kernel file
extern uint8_t kernel_start[];
extern uint8_t kernel_end[];


// print physical memory map
void show_memory_map();

// init normal memory zone
void init_normal_mem_zone();

// allocate physic memory, return physic addr of the first page
void* physic_alloc(uint32_t byte_size);
// allocate pages, return physic page_t address
page_frame_t* frame_alloc(uint32_t page_size);


// free physic memory
uint32_t physic_free(void* physic_addr);
// free pages
uint32_t frame_free(page_frame_t* page);


inline uint32_t is_power_of_2 (uint32_t n);
inline uint32_t next_power_of_2 (uint32_t n);
inline uint32_t power_of_2 (uint32_t n);

// buddy_system

/** 
 * return first page index
 * */
inline page_frame_t* buddy_system_get_frame_page_block(uint32_t level);

inline uint32_t buddy_system_free_frame_page_block(uint32_t page_index);


#endif // INCLUDE_PMM_H