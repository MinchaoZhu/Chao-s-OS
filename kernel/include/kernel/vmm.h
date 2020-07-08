#ifndef KERNEL_VMM_H_
#define KERNEL_VMM_H_

#include <stdint.h>
#include "kernel/pmm.h"

#define STACK_SIZE 0x4000 // 16KiB kernel stack 

#define PAGE_OFFSET 0xC0000000
#define PAGE_SIZE   4096
#define PAGE_MASK   ALIGN_MASK


#define PAGE_PRESENT     0x1
#define PAGE_WRITE       0x2
#define PAGE_USER        0x4
#define PAGE_ACCESSED    0x8
#define PAGE_DIRTY       0x16


#define PGD_INDEX(x) (((x) >> 22) & 0x3FF) // page directory entry index
#define PTE_INDEX(x) (((x) >> 12) & 0x3FF) // page table entry index



#define KERNEL_NORMAL_ZONE_SIZE 0x38000000
#define KERNEL_NORMAL_ZONE_DIRECTORY_SIZE (KERNEL_NORMAL_ZONE_SIZE >> 22)



// typedef struct page_t {
//     uint32_t present   : 1;  // Page present in memory
//     uint32_t rw        : 1;  // Read-only if clear, readwrite if set
//     uint32_t user      : 1;  // Supervisor level only if clear
//     uint32_t accessed  : 1;  // Has the page been accessed since last refresh?
//     uint32_t dirty     : 1;  // Has the page been written to since last refresh?
//     uint32_t unused    : 7;  // Amalgamation of unused and reserved bits
//     uint32_t frame     : 20; // Frame address (shifted right 12 bits)
// } page_t;

// typedef struct page_table_t {
//     page_t pages[1024]; // page table entry * 1024
// } page_table_t;

// typedef struct page_directory_t {
//     page_table_t* tables[1024]; // page directory entry * 1024
// } page_directory_t;

typedef uint32_t page_t;

typedef uint32_t page_directory_t;



// active paging by using a temporary page directory
void kernel_main_init(void);

// init paging
void init_vmm(void);

// switch paging directory
void switch_pgd(uint32_t pd);

// build map between a virtual address and a physic address in a page directory
void map(page_directory_t* pd, uint32_t va, uint32_t pa, uint32_t flags);

// release map of a address
void unmap(page_directory_t* pd, uint32_t va);

// get physica address relating to a virtual address in specific page directory 
uint32_t get_mapping(page_directory_t* pd, uint32_t va, uint32_t* pa);

#endif // INCLUDE_VMM_H