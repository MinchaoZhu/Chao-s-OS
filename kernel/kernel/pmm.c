#include "kernel/pmm.h" // show_memory_map
#include <stdio.h> // printf

normal_mem_t normal_mem; // global variable storing normal memory zone information



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


void print_init_buddy(){
    uint32_t* free_lists = normal_mem.free_lists;
    page_t* pages = normal_mem.pages;
    uint32_t len = normal_mem.pages_number;
    uint32_t total_pages_count = 0;
    for(int i = 0; i < 11; ++i){
        uint32_t cur = free_lists[i];
        printf("Level %d:", i);
        while(cur!= len){
            total_pages_count += (1 << i);
            printf("%X ", cur);
            cur = pages[cur].next_page;
        }
        printf("\n");
    }
    printf("Total pages is %X\n", total_pages_count);
    printf("size of page_t: %d\n", sizeof(page_t));
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
            base_addr = kernel_end + ALIGN_MASK_HELPER;

            base_addr = base_addr & ALIGN_MASK; // align for 4k
            end_addr  = end_addr &  ALIGN_MASK; // align for 4k
            length = end_addr - base_addr;
            // [base_addr, end_addr)
            // [0x00F15000, 0x07FE0000)
            // availiable is [0x00F15000, 0x07FDFFFF]
            
            normal_mem.base = base_addr;
            normal_mem.length = length;
            normal_mem.pages_number = (length>>PAGE_SIZE_POWER); 


            uint32_t pages_number = normal_mem.pages_number;

            for (uint32_t i = 0; i < BUDDY_MAXLEVEL; ++i){
                normal_mem.free_lists[i] = pages_number;
            }

            page_t* pages = normal_mem.pages;


            for (uint32_t i = 0; i < MAX_PAGES_NUM; ++i) {
                pages[i].flags = 0 | PAGE_ALLOCATED;
                pages[i].level = 0;
                pages[i].index = i;
                pages[i].next_page = pages_number;
                pages[i].addr = base_addr + i * PAGE_SIZE;
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
                normal_mem.free_lists[block_level] = i;

            }


            print_init_buddy();
            break;
        }
    }    

}


