#include "kernel/pmm.h" // show_memory_map
#include <stdio.h> // printf

normal_mem_t normal_mem; // global variable storing normal memory zone information

/**
 * Memory Map
 * ┌──────────┬────────┬─────────────┬──────────────────────────────────┬──────────┐
 * │ reserved ┼ kernel ┼ pages table ┼        memory for kmalloc        ┼ reserved │          
 * └──────────┴────────┴─────────────┴──────────────────────────────────┴──────────┘
 *                   ker_end      normal_base
 * */

/**
 * Buddy System
 * 1. get page block
 * 2. free page block
 * 
 * 
 * */
void show_memory_map(){
    // get mmap info from multiboot
    uint32_t mmap_addr = global_multiboot_ptr -> mmap_addr;
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
    page_t* pages = normal_mem.pages;
    uint32_t len = normal_mem.pages_number;
    uint32_t total_pages_count = 0;
    for(int i = 0; i < BUDDY_MAXLEVEL; ++i){
        uint32_t cur = free_lists[i];
        printf("Level %d:", i);
        while(cur!= len){
            total_pages_count += (1 << i);
            printf("%X ", cur);
            cur = pages[cur].next_page;
        }
        printf("\n");
    }
    // printf("Total pages is %X\n", total_pages_count);
}

void init_normal_mem_zone() {
    uint32_t mmap_addr = global_multiboot_ptr -> mmap_addr;
    uint32_t mmap_length = global_multiboot_ptr -> mmap_length;

    mmap_entry_t *mmap = (mmap_entry_t *) mmap_addr;
    for (; mmap < mmap_addr + mmap_length; ++mmap) {
        if(mmap -> base_addr_low == 0x100000 && mmap -> type == 1){
            uint32_t base_addr = mmap -> base_addr_high;
            base_addr = (base_addr << 16) + mmap -> base_addr_low;

            uint32_t length = mmap -> length_high;
            length = (length << 16) + mmap -> length_low;

            uint32_t end_addr = base_addr + length;
            base_addr = (uint32_t )kernel_end;

            // base_addr = base_addr & ALIGN_MASK; // align for 4k
            // end_addr  = end_addr &  ALIGN_MASK; // align for 4k
            length = end_addr - base_addr;
            // [base_addr, end_addr)
            // [0x00F15000, 0x07FE0000)
            // availiable is [0x00F15000, 0x07FDFFFF]
            

            uint32_t pages_number = length/(PAGE_SIZE + sizeof(page_t)) - 1;
            
            page_t* pages = (page_t*) base_addr; // page table follows kernel end

            normal_mem.pages = pages;

            uint32_t mem_base = base_addr + pages_number * sizeof(page_t); // allocate memory for pages table
            mem_base = (mem_base + ALIGN_MASK_HELPER) & ALIGN_MASK; // next sector is availiable normal memory, align for 4K

            normal_mem.base = mem_base;
            normal_mem.length = pages_number * PAGE_SIZE;
            normal_mem.pages_number = pages_number; 



            for (uint32_t i = 0; i < BUDDY_MAXLEVEL; ++i){
                normal_mem.free_lists[i] = pages_number;
            }

            for (uint32_t i = 0; i < pages_number; ++i) {
                pages[i].flags = 0 | PAGE_ALLOCATED;
                pages[i].level = 0;
                pages[i].index = i;
                pages[i].next_page = pages_number;
                pages[i].prev_page = pages_number;
            } 
            
            uint32_t block_page_size = 1 << (BUDDY_MAXLEVEL - 1);
            uint32_t block_level = BUDDY_MAXLEVEL - 1;
            for (uint32_t i = 0; i < pages_number; i += block_page_size) {
                while(i > pages_number - block_page_size){
                    block_page_size /= 2;
                    --block_level;
                }
                
                pages[i].flags = 0 | PAGE_BLOCK_HEAD;
                pages[i].level = block_level;

                uint32_t tmp = normal_mem.free_lists[block_level];
                pages[i].next_page = tmp;
                pages[tmp].prev_page = i;
                normal_mem.free_lists[block_level] = i;

            }

            break;
        }
    }    

}

void* kmalloc(uint32_t byte_size) {
    uint32_t page_size = byte_size / PAGE_SIZE + ((byte_size % PAGE_SIZE) > 0);
    uint32_t start_page = palloc(page_size);
    if(start_page == normal_mem.pages_number) {
        return (void*) 0 ;
    }
    else {
        return (void*) (normal_mem.base + start_page * PAGE_SIZE);
    }
}

inline uint32_t palloc(uint32_t page_size) {
    uint32_t level = next_power_of_2(page_size);
    level = power_of_2(level);
    return buddy_system_get_page_block(level);
}


// get page block in specific level
inline uint32_t buddy_system_get_page_block(uint32_t level) {
    uint32_t current_level = level;
    uint32_t* free_lists = normal_mem.free_lists;
    uint32_t pages_number = normal_mem.pages_number;
    page_t* pages = normal_mem.pages;
    // find availiable level
    while(current_level < BUDDY_MAXLEVEL && free_lists[current_level] == pages_number)
        ++current_level;
    if(current_level == BUDDY_MAXLEVEL)
        return pages_number;

    uint32_t page_index = free_lists[current_level];
    free_lists[current_level] = pages[page_index].next_page;
    // printf("current level: %d\n", current_level);
    while(current_level > level) {
        --current_level;
        uint32_t right = page_index ^ (1 << current_level);
        pages[right].level = current_level;
        pages[free_lists[current_level]].prev_page = right;
        pages[right].next_page = free_lists[current_level];
        pages[right].flags = 0 | PAGE_BLOCK_HEAD;
        free_lists[current_level] = right;
    }

    pages[page_index].flags = PAGE_ALLOCATED + PAGE_BLOCK_HEAD;
    pages[page_index].level = level;
    
    pages[page_index].prev_page = pages_number;
    pages[page_index].next_page = pages_number;

    return page_index;
}




uint32_t kfree(void* addr) {
    if(addr == 0) return 0;
    uint32_t base = normal_mem.base;
    uint32_t page_index = ((uint32_t)addr - base) / PAGE_SIZE;
    
    return buddy_system_free_page_block(page_index);
}

inline uint32_t buddy_system_free_page_block(uint32_t page_index) {
    uint32_t pages_number = normal_mem.pages_number;
    page_t* pages = normal_mem.pages;
    uint32_t* free_lists = normal_mem.free_lists;
    
    if(page_index >= pages_number || (pages[page_index].flags & PAGE_BLOCK_HEAD) == 0 || (pages[page_index].flags & PAGE_BLOCK_HEAD) == 0)
        return 0;
    uint32_t level = pages[page_index].level;

    uint32_t buddy_index = page_index ^ (1<<level);

    while(level < BUDDY_MAXLEVEL - 1 
    && level == pages[buddy_index].level 
    && pages[buddy_index].flags ==  PAGE_BLOCK_HEAD) {
        uint32_t prev = pages[buddy_index].prev_page;
        uint32_t next = pages[buddy_index].next_page;
        if(prev == pages_number){
            free_lists[level] = next;
        }
        else{
            pages[prev].next_page = next;
        }
        if(next < pages_number){
            pages[next].prev_page = prev;
        }
    

        if( page_index > buddy_index) {
            uint32_t tmp = buddy_index;
            buddy_index = page_index;
            page_index = tmp;
        }
        pages[buddy_index].flags =  PAGE_ALLOCATED;
        pages[buddy_index].level = 0;

        pages[page_index].level = ++level;
        buddy_index = page_index ^ (1<<level);
    }

    pages[page_index].flags = PAGE_BLOCK_HEAD;
    pages[page_index].level = level;
    uint32_t tmp = free_lists[level];
    pages[page_index].next_page = free_lists[level];
    if(tmp < pages_number)
        pages[tmp].prev_page = page_index;
    pages[page_index].prev_page = pages_number;
    free_lists[level] = page_index;

    return 1;
}



inline uint32_t is_power_of_2 (uint32_t n) {
    return !((n-1) & n) && n;
}


inline uint32_t next_power_of_2 (uint32_t n) {
    if(is_power_of_2(n))
        return n;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;

    return n + 1;
}

inline uint32_t power_of_2 (uint32_t n) {
    uint32_t i = 0;
    while(! (n & (uint32_t)1)) {
        n = n >> 1;
        ++i;
    }
    return i;
}