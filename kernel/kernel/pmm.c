#include "kernel/pmm.h" // show_memory_map
#include <stdio.h> // printf
#include "kernel/vmm.h"
#include "kernel/math.h"
normal_mem_t normal_mem; // global variable storing normal memory zone information

/**
 * Memory Map
 * ┌──────────┬────────┬───────────────┬───────────────────────────────────┬──────────┐
 * │ reserved ┼ kernel ┼ frames table  ┼        memory for frame_alloc     ┼ reserved │          
 * └──────────┴────────┴───────────────┴───────────────────────────────────┴──────────┘
 *                   ker_end      normal_base
 * */

/**
 * Buddy System
 * 1. get page block
 *      - void* physic_alloc(uint32_t byte_size) : return begin physic addr of meomory block
 *      - page_frame_t* frame_alloc(uint32_t frame_page_number) : return a page_frame_t object indicating the allocated pages 
 * 2. free page block
 *      - uint32_t physic_free(void* physic_addr) : free the pages allocated according to begin addr of pages
 *      - uint32_t frame_free : free the pages allocated by frame_alloc
 * */


// buddy_system

/** 
 * return first page index
 * */
static inline page_frame_t* buddy_system_get_frame_page_block(uint32_t level);

static inline uint32_t buddy_system_free_frame_page_block(uint32_t page_index);

void show_memory_map(){
    // get mmap info from multiboot
    uint32_t mmap_addr = global_multiboot_ptr -> mmap_addr + PAGE_OFFSET;
    uint32_t mmap_length = global_multiboot_ptr -> mmap_length;

    printf("Memory map:\n");

    mmap_entry_t *mmap = (mmap_entry_t *) mmap_addr;
    for(; mmap < mmap_addr + mmap_length; ++mmap){
        printf("base_addr = 0x%X%X, length = 0x%X%X, type = 0x%X\n",
            mmap->base_addr_high, mmap->base_addr_low, mmap->length_high, mmap->length_low,
            mmap->type);
    }

}


void print_free_buddy_blocks(){
    uint32_t* free_lists = normal_mem.free_lists;
    page_frame_t* pages = normal_mem.frames;
    uint32_t len = normal_mem.frames_number;
    uint32_t total_pages_count = 0;
    pages = (page_frame_t*) ((uint32_t) pages + PAGE_OFFSET);

    for(int i = 0; i < BUDDY_MAXLEVEL; ++i){
        uint32_t cur = free_lists[i];
        printf("Level %d:", i);
        while(cur!= len){
            total_pages_count += (1 << i);
            printf("%X ", cur);
            cur = pages[cur].next_frame;
        }
        printf("\n");
    }
    printf("Total pages is %X\n", total_pages_count);
}

void init_normal_mem_zone() {
    uint32_t mmap_addr = global_multiboot_ptr -> mmap_addr + PAGE_OFFSET;
    uint32_t mmap_length = global_multiboot_ptr -> mmap_length;

    mmap_entry_t *mmap = (mmap_entry_t *) mmap_addr;
    for (; mmap < mmap_addr + mmap_length; ++mmap) {
        if(mmap -> base_addr_low == 0x100000 && mmap -> type == 1){
            uint32_t base_addr = mmap -> base_addr_high;
            base_addr = (base_addr << 16) + mmap -> base_addr_low;

            uint32_t length = mmap -> length_high;
            length = (length << 16) + mmap -> length_low;
            uint32_t end_addr = base_addr +  length ;
            base_addr = (uint32_t )kernel_end - PAGE_OFFSET;

            // base_addr = base_addr & ALIGN_MASK; // align for 4k
            // end_addr  = end_addr &  ALIGN_MASK; // align for 4k
            // [base_addr, end_addr)
            // [0x00F15000, 0x07FE0000)
            // availiable is [0x00F15000, 0x07FDFFFF]
            length = end_addr - base_addr;

            uint32_t frames_number = length/(PAGE_FRAME_SIZE + sizeof(page_frame_t)) - 1;
            page_frame_t* pages = (page_frame_t*) base_addr; // physic addr: page table follows kernel end

            normal_mem.frames = pages;

            uint32_t mem_base = base_addr + frames_number * sizeof(page_frame_t); // allocate memory for pages table
            mem_base = (mem_base + ALIGN_MASK_HELPER) & ALIGN_MASK; // next sector is availiable normal memory, align for 4K

            normal_mem.base = mem_base;
            normal_mem.length = frames_number * PAGE_FRAME_SIZE;
            normal_mem.frames_number = frames_number; 

            // printf("base: %#08x\n", mem_base);
            // printf("len: %#08x\n", normal_mem.length);
            // printf("nums: %#08x\n", frames_number);
            // printf("end: %#08x\n", mem_base + normal_mem.length);

            for (uint32_t i = 0; i < BUDDY_MAXLEVEL; ++i){
                normal_mem.free_lists[i] = frames_number;
            }


            pages = (page_frame_t*) ((uint32_t) pages + PAGE_OFFSET); // virtual addr

            for (uint32_t i = 0; i < frames_number; ++i) {
                pages[i].flags = 0 | PAGE_FRAME_ALLOCATED;
                pages[i].level = 0;
                pages[i].index = i;
                pages[i].next_frame = frames_number;
                pages[i].prev_frame = frames_number;
                pages[i].physic_addr = mem_base + i * PAGE_FRAME_SIZE;
                pages[i].allocator = 0;
            } 
            
            uint32_t block_page_size = 1 << (BUDDY_MAXLEVEL - 1);
            uint32_t block_level = BUDDY_MAXLEVEL - 1;
            for (uint32_t i = 0; i < frames_number; i += block_page_size) {
                while(i > frames_number - block_page_size){
                    block_page_size /= 2;
                    --block_level;
                }
                
                pages[i].flags = 0 | PAGE_FRAME_BLOCK_HEAD;
                pages[i].level = block_level;

                uint32_t tmp = normal_mem.free_lists[block_level];
                pages[i].next_frame = tmp;
                pages[tmp].prev_frame = i;
                normal_mem.free_lists[block_level] = i;

            }

            break;
        }
    }    

}

// allocate a contigious memory block size is at least byte_size, return the head page' physic address 
void* physic_alloc(uint32_t byte_size) {
    uint32_t frame_page_number = byte_size / PAGE_FRAME_SIZE + ((byte_size % PAGE_FRAME_SIZE) > 0);
    page_frame_t* ret_page = frame_alloc(frame_page_number);
    if(ret_page == 0)
        return 0;
    ret_page = (page_frame_t*)((uint32_t) ret_page + PAGE_OFFSET);
    return (void*) ret_page->physic_addr;
}

// allocate a contigious pages block, return the struct page_frame_t
page_frame_t* frame_alloc(uint32_t frame_page_number) {
    uint32_t level = next_power_of_2(frame_page_number);
    level = power_of_2(level);
    return buddy_system_get_frame_page_block(level);
}


// get page block in specific level
inline page_frame_t* buddy_system_get_frame_page_block(uint32_t level) {
    uint32_t current_level = level;
    uint32_t* free_lists = normal_mem.free_lists;
    uint32_t frames_number = normal_mem.frames_number;
    page_frame_t* pages = normal_mem.frames;
    pages = (page_frame_t*) ((uint32_t) pages + PAGE_OFFSET);
    // find availiable level
    while(current_level < BUDDY_MAXLEVEL && free_lists[current_level] == frames_number)
        ++current_level;

    // if no free pages block    
    if(current_level >= BUDDY_MAXLEVEL)
        return (page_frame_t*) 0;

    uint32_t page_index = free_lists[current_level];
    free_lists[current_level] = pages[page_index].next_frame;
    // printf("current level: %d\n", current_level);
    while(current_level > level) {
        --current_level;
        uint32_t right = page_index ^ (1 << current_level);
        pages[right].level = current_level;
        pages[free_lists[current_level]].prev_frame = right;
        pages[right].next_frame = free_lists[current_level];
        pages[right].flags = 0 | PAGE_FRAME_BLOCK_HEAD;
        free_lists[current_level] = right;
    }

    pages[page_index].flags = PAGE_FRAME_ALLOCATED + PAGE_FRAME_BLOCK_HEAD;
    pages[page_index].level = level;
    
    pages[page_index].prev_frame = frames_number;
    pages[page_index].next_frame = frames_number;


    return (page_frame_t*) ((uint32_t)&pages[page_index] - PAGE_OFFSET);
}



uint32_t physic_free(void* physic_addr) {
    if(physic_addr == 0) return 0;
    uint32_t base = normal_mem.base;
    uint32_t page_index = ((uint32_t)physic_addr - base) / PAGE_FRAME_SIZE;
    return buddy_system_free_frame_page_block(page_index);
}

uint32_t frame_free(page_frame_t* page) {
    if(page == 0) return 0;
    page = (page_frame_t*) ((uint32_t) page + PAGE_OFFSET); // virtual addr
    return buddy_system_free_frame_page_block(page->index);
}


inline uint32_t buddy_system_free_frame_page_block(uint32_t page_index) {
    uint32_t frames_number = normal_mem.frames_number;
    page_frame_t* pages = normal_mem.frames;
    pages = (page_frame_t*) ((uint32_t) pages + PAGE_OFFSET);
    uint32_t* free_lists = normal_mem.free_lists;
    
    if(page_index >= frames_number || (pages[page_index].flags & PAGE_FRAME_BLOCK_HEAD) == 0 || (pages[page_index].flags & PAGE_FRAME_BLOCK_HEAD) == 0)
        return 0;
    uint32_t level = pages[page_index].level;

    uint32_t buddy_index = page_index ^ (1<<level);

    while(level < BUDDY_MAXLEVEL - 1 
    && level == pages[buddy_index].level 
    && pages[buddy_index].flags ==  PAGE_FRAME_BLOCK_HEAD) {
        uint32_t prev = pages[buddy_index].prev_frame;
        uint32_t next = pages[buddy_index].next_frame;
        if(prev == frames_number){
            free_lists[level] = next;
        }
        else{
            pages[prev].next_frame = next;
        }
        if(next < frames_number){
            pages[next].prev_frame = prev;
        }
    

        if( page_index > buddy_index) {
            uint32_t tmp = buddy_index;
            buddy_index = page_index;
            page_index = tmp;
        }
        pages[buddy_index].flags =  PAGE_FRAME_ALLOCATED;
        pages[buddy_index].level = 0;

        pages[page_index].level = ++level;
        buddy_index = page_index ^ (1<<level);
    }

    pages[page_index].flags = PAGE_FRAME_BLOCK_HEAD;
    pages[page_index].level = level;
    uint32_t tmp = free_lists[level];
    pages[page_index].next_frame = free_lists[level];
    if(tmp < frames_number)
        pages[tmp].prev_frame = page_index;
    pages[page_index].prev_frame = frames_number;
    free_lists[level] = page_index;

    return 1;
}

