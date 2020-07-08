#ifndef KERNEL_KMM_H_
#define KERNEL_KMM_H_

#include <stdint.h>
#include "kernel/pmm.h"
#include "kernel/vmm.h"
// manage kernel memory allocation
// kmalloc()/kfree()
// 1. if size > 2048 bytes, directly call frame_alloc or physic_alloc
// 2. if size is smaller than 2048, use page slice allocator


// current slices size(in bytes):
// 16 32 64 128 256 512 1024 2048
// [16, 256] use free-block-linked list
// [512, 2048] use bit map


// [16, 256] sliced piece structure
// one page
//    ┌──────────────────────┬────────────┬──────────────────┐─────┐────────────┬──────────────────┐
//    │   allocator_ptr      ┼ next_avail ┼ allocated memory │ ... │ next_avail ┼ allocated memory │          
//    └──────────────────────┴────────────┴──────────────────┘─────┘────────────┴──────────────────┘
//     page_slice_allocator_t*   uint8_t        fixed_size              uint8_t        fixed_size            

#define PAGE_SLICE_LEVEL 8
#define ALLOC_END 0xFFFF
#define MAX_ALLOC ((1 << (BUDDY_MAXLEVEL - 1)) * PAGE_FRAME_SIZE)

#define KMA_MODE_LINKED 0
#define KMA_MODE_BITMAP 1


// page_slice_allocator, also doubly linked-list node for itself
// this slice allocator is to manage a single page for allocating and free a small fixed piece
typedef struct page_slice_allocator {
    uint16_t prev_allocator;      // previous allocator index in list
    uint16_t next_allocator;      // next allocator index in list
    uint8_t  first_avai;          // first avai , it is also bit map for slice bigger than or equal to 512 bytes
    uint8_t  avai_num;            // number of free slices
    page_frame_t* target_page;    // the physic page assigned for this allocator
} __attribute__((packed)) page_slice_allocator_t;

#define NUM_OF_ALLOC_PER_MANAGER ((PAGE_FRAME_SIZE - 2 - 2 - 2 - 2 - sizeof(void*))/sizeof(page_slice_allocator_t))
#define END_FLAG NUM_OF_ALLOC_PER_MANAGER

#define SLICE_END 0xFF
// One of this managers is in size of 1 PAGE;
// A manager only alloc/free small memory slice in excatly one fixed length
// for example, if slice_size = 64, all the allocators belonged to this manager
// only allocate memory between 33 bytes to 64 bytes ;
// One manager contains 453 allocators which can manage 453 pages;
typedef struct page_slice_allocator_manager {
    uint16_t slice_size;                    // the slice size this manager domains 
    uint16_t free;                          // free allocator list
    uint16_t p_full;                        // partially full allocator list
    uint16_t full;                          // full allocator list
    struct page_slice_allocator_manager* next;     // next 
    page_slice_allocator_t allocs[NUM_OF_ALLOC_PER_MANAGER]; // real manager allocators 
} __attribute__((packed)) page_slice_allocator_manager_t;

// entry for those allocator managers
typedef struct kmalloc_manager_t {
    page_slice_allocator_manager_t* alloc_ms[PAGE_SLICE_LEVEL];
} kmalloc_manager_t;


void init_kmm(void);

// kernel: dynamically allocate a contigious memory slice 
void* kmalloc(uint32_t byte_size);

// kernel: free a memory slice which is allocated by kmalloc
void  kfree(void* addr);

#endif //INCLUDE_KMM_H_