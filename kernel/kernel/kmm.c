#include "kernel/kmm.h"

kmalloc_manager_t kmms;
extern normal_mem_t normal_mem;
const uint16_t slice_sizes[PAGE_SLICE_LEVEL] = {16, 32, 64, 128, 256, 512, 1024, 2048};


static inline void init_linked_page(page_slice_allocator_t* allocator, uint16_t slice_size);
static void* kmalloc_helper(page_slice_allocator_manager_t* manager, uint32_t slice_index, uint32_t mode);
static uint32_t get_slice_addr(page_slice_allocator_t* allocator, uint16_t slice_size, uint32_t* addr, uint32_t mode);
static inline void* alloc_one_page_mem_virtual(void);
static inline page_frame_t* alloc_one_page_virtual(void);
static inline void init_allocator_manager(page_slice_allocator_manager_t* m);
static inline uint32_t is_linked_mode(uint16_t slice_size);






// get a page from physic memory
static inline void* alloc_one_page_mem_virtual() {
    return (void*)(((uint32_t) physic_alloc(PAGE_FRAME_SIZE)) + PAGE_OFFSET);
}

// get a page from physic memory
static inline page_frame_t* alloc_one_page_virtual(void) {
    return (page_frame_t*)(((uint32_t) frame_alloc(1)) + PAGE_OFFSET);
}

void init_kmm() {
    for(uint32_t i = 0; i < PAGE_SLICE_LEVEL; ++i) {
        page_slice_allocator_manager_t* manager = (page_slice_allocator_manager_t*) alloc_one_page_mem_virtual();
        manager -> slice_size = (uint16_t) slice_sizes[i]; 
        manager -> next = (page_slice_allocator_manager_t*) 0; // currently only one manager
        manager -> p_full = (uint16_t) END_FLAG; 
        manager -> full = (uint16_t) END_FLAG; 
        manager -> free = (uint16_t) 0;

        init_allocator_manager(manager);
        kmms.alloc_ms[i] = manager;
    }
}

static inline void init_allocator_manager(page_slice_allocator_manager_t* m) {
    page_slice_allocator_t* allocs = m->allocs;
    allocs[0].first_avai = (uint8_t) 0;
    allocs[0].next_allocator = (uint16_t) 1;
    allocs[0].prev_allocator = (uint16_t) END_FLAG;
    allocs[0].target_page = (page_frame_t*) 0;
    allocs[0].avai_num = 0;
    for(uint32_t i = 1; i < NUM_OF_ALLOC_PER_MANAGER - 1; ++i) {
        allocs[i].first_avai = (uint8_t) 0;
        allocs[i].next_allocator = (uint16_t) (i + 1);
        allocs[i].prev_allocator = (uint16_t) (i - 1);
        allocs[i].target_page = (page_frame_t*) 0;
        allocs[i].avai_num = 0;
    }
    allocs[NUM_OF_ALLOC_PER_MANAGER - 1].first_avai = (uint8_t) 0;
    allocs[NUM_OF_ALLOC_PER_MANAGER - 1].next_allocator = (uint16_t) END_FLAG;
    allocs[NUM_OF_ALLOC_PER_MANAGER - 1].prev_allocator = (uint16_t) (NUM_OF_ALLOC_PER_MANAGER - 2);
    allocs[NUM_OF_ALLOC_PER_MANAGER - 1].target_page = (page_frame_t*) 0;
    allocs[NUM_OF_ALLOC_PER_MANAGER - 1].avai_num = 0;
}

void* kmalloc(uint32_t byte_size) {
    if(byte_size > MAX_ALLOC)
        return 0;

    for(uint32_t i = 0; i < PAGE_SLICE_LEVEL; ++i) {
        if(byte_size <= slice_sizes[i]) {   
            if(i <= 4) // if byte size is less than or equal to 256
                return kmalloc_helper(kmms.alloc_ms[i], i, KMA_MODE_LINKED);
            else
                return kmalloc_helper(kmms.alloc_ms[i], i, KMA_MODE_BITMAP);
        }
    }

    //if byte size if bigger than 2048, return several whole pages
    return (void*)(((uint32_t) physic_alloc(byte_size)) + PAGE_OFFSET);
}



// mode 0: linked block, mode 1: bitmap
static void* kmalloc_helper(page_slice_allocator_manager_t* manager, uint32_t slice_index, uint32_t mode) {
    if(manager == 0) {
        manager = (page_slice_allocator_manager_t*) alloc_one_page_mem_virtual();
        manager -> slice_size = slice_sizes[slice_index]; 
        manager -> next = kmms.alloc_ms[slice_index]; 
        manager -> p_full = END_FLAG; 
        manager -> full = END_FLAG; 
        manager -> free = 0;
        kmms.alloc_ms[slice_index] = manager;
        init_allocator_manager(manager);
    }

    uint16_t slice_size = manager -> slice_size;
    page_slice_allocator_t* allocator = 0;
    if(manager->p_full != END_FLAG) {
        // if there is a partially full allocator
        uint16_t allocator_index = manager->p_full;
        allocator = &(manager->allocs[allocator_index]);
        uint32_t addr = 0;
        if(get_slice_addr(allocator, slice_size, &addr, mode)) {
            // move allocator to full list
            uint16_t pfull_next_index = allocator->next_allocator;
            if(pfull_next_index != END_FLAG)
                manager->allocs[pfull_next_index].prev_allocator = allocator->prev_allocator;
            manager->p_full = pfull_next_index;

            uint16_t full_first_index = manager->full;
            allocator->next_allocator = full_first_index;
            if(full_first_index != END_FLAG) {
                allocator->prev_allocator = manager->allocs[full_first_index].prev_allocator; 
                manager->allocs[full_first_index].prev_allocator = allocator_index;
            }
            manager->full = allocator_index;
        }
        allocator->avai_num -= 1;
        return (void*) addr;
    }
    else {
        // if there is no partially full allocator
        if(manager->free != END_FLAG) {
            // if there is an free allocator
            uint16_t allocator_index = manager->free;
            allocator = &(manager->allocs[allocator_index]);
            if(allocator -> target_page == 0) {
                // if not page was assigned for this allocator
                page_frame_t* new_page = alloc_one_page_virtual();
                new_page->allocator = (uint32_t)allocator;
                allocator->target_page = new_page;
                allocator->avai_num = PAGE_FRAME_SIZE / slice_size;
                if(mode == KMA_MODE_LINKED){
                    allocator->avai_num = (PAGE_FRAME_SIZE - sizeof(page_slice_allocator_t**)) / (slice_size + sizeof(uint8_t));
                    init_linked_page(allocator, slice_size);
                }
            }
            
            uint32_t addr = ((uint32_t)allocator->target_page->physic_addr) + PAGE_OFFSET;
            if(mode == KMA_MODE_LINKED) {
                addr += sizeof(page_slice_allocator_t**);
                uint8_t next_avail = *((uint8_t*) addr);
                *((uint8_t*) addr) = 0xFF;
                addr += sizeof(uint8_t);
                allocator -> first_avai = next_avail;
            }
            else{
                allocator->first_avai = 0x80;
            }
                        // move the list to part full list
            uint16_t free_next_index = allocator->next_allocator;
            if(free_next_index != END_FLAG)
                manager->allocs[free_next_index].prev_allocator = allocator->prev_allocator;
            manager->free = free_next_index;

            uint16_t pfull_first_index = manager->p_full;
            allocator->next_allocator = pfull_first_index;
            if(pfull_first_index != END_FLAG) {
                allocator->prev_allocator = manager->allocs[pfull_first_index].prev_allocator; 
                manager->allocs[pfull_first_index].prev_allocator = allocator_index;
            }
            manager->p_full = allocator_index;
            allocator->avai_num -= 1;
            return (void*)addr;

        }
        else {
            // if all allocators are full, create a new manager
            return kmalloc_helper(manager->next, slice_index, mode);
        }

    }
}


static uint32_t get_slice_addr(page_slice_allocator_t* allocator, uint16_t slice_size, uint32_t* addr, uint32_t mode) {
    if(mode == KMA_MODE_LINKED) {
        // linked blcok
        uint32_t real_slice_size = slice_size + sizeof(uint8_t);
        uint8_t first_avai = allocator->first_avai;
        uint32_t ret_addr = ((uint32_t)allocator->target_page->physic_addr) + PAGE_OFFSET;
        ret_addr += sizeof(page_slice_allocator_t**) + first_avai * real_slice_size;
        *addr =  (void*) ret_addr;
        uint8_t next_avail = *((uint8_t*) ret_addr);
        *((uint8_t*) *addr) = 0xFF;
        *addr += sizeof(uint8_t);
        allocator -> first_avai = next_avail;
        return next_avail == 0xFF;
    }
    else{
        uint8_t bitmap = allocator->first_avai;
        uint32_t i = 0;
        uint32_t bits = PAGE_FRAME_SIZE / slice_size;
        for(; i < bits; ++i) {
            if(((bitmap >> (7-i)) & 1) == 0)
                break;
        }
        *addr = ((uint32_t) allocator -> target_page->physic_addr) + PAGE_OFFSET;
        *addr += (uint32_t)i * (uint32_t)slice_size;
        bitmap = bitmap | (1 << (7-i));
        allocator->first_avai = bitmap;
        return (bitmap>>(8 - bits)) == (0xFF>>(8 - bits));
    }

}

static void  init_linked_page(page_slice_allocator_t* allocator, uint16_t slice_size) {
    uint32_t page_addr = ((uint32_t)(allocator->target_page->physic_addr)) + PAGE_OFFSET;
    page_slice_allocator_t** header_allocator = (page_slice_allocator_t**) page_addr; 
    *header_allocator = allocator;
 
    uint32_t real_slice_size = slice_size + sizeof(uint8_t);
    uint32_t addr = page_addr;
    addr += sizeof(page_slice_allocator_t**);
 
    uint8_t* next_avail;
    for(uint8_t i = 0; addr+real_slice_size <= page_addr+PAGE_FRAME_SIZE; addr += real_slice_size, ++i) {
        next_avail = (uint8_t*) addr;
        *next_avail = i+1;
    }
    *next_avail = 0xFF;

}



void kfree(void* addr) {
    if(addr == 0) return;
    uint32_t page_physic_addr = (((uint32_t)addr) & PAGE_MASK) - PAGE_OFFSET;
    uint32_t page_frame_index = (page_physic_addr - normal_mem.base) / PAGE_FRAME_SIZE;
    page_frame_t* pages = normal_mem.frames;
    pages = (page_frame_t*) ((uint32_t) pages + PAGE_OFFSET); // virtual addr of pages
    // its allocator
    page_slice_allocator_t* allocator = (page_slice_allocator_t*)pages[page_frame_index].allocator;
    // allocator's manager
    page_slice_allocator_manager_t* manager = (page_slice_allocator_manager_t*) (((uint32_t) allocator) & PAGE_MASK);
    allocator->avai_num += 1;
    
    uint16_t allocator_index = ((uint32_t)allocator - (uint32_t)(&manager->allocs))/sizeof(page_slice_allocator_t);
    uint16_t slice_size = manager->slice_size;
    uint8_t sum = 0;
    uint16_t index = 0;
    if(is_linked_mode(slice_size)) {
        // if memory slice is allocated in linked block mode
        index = (uint32_t)addr - (((uint32_t)addr) & PAGE_MASK) - sizeof(page_slice_allocator_t**);
        index /= (slice_size + sizeof(uint8_t));
        sum = (PAGE_FRAME_SIZE - sizeof(page_slice_allocator_t**)) / (slice_size + sizeof(uint8_t));
        *((uint8_t*) addr) = allocator->first_avai;
        allocator->first_avai = index;
    }
    else {
        // if memory slice is allocated in bitmap mode
        index = (uint32_t)addr - (((uint32_t)addr) & PAGE_MASK);
        index /= slice_size;
        sum = PAGE_FRAME_SIZE / slice_size;
        uint8_t bitmap = allocator->first_avai;
        bitmap = bitmap ^ (1<<(7-index));
        allocator->first_avai = bitmap;
    }

    if(sum == allocator->avai_num) {
        // if allocator is free
        // move allocator from p_full to free
        pages[page_frame_index].allocator = 0;
        allocator->target_page = 0;
        physic_free(page_physic_addr);
        allocator->avai_num = 0;

        uint16_t prev = allocator->prev_allocator;
        uint16_t next = allocator->next_allocator;
        if(next != END_FLAG)
            manager->allocs[next].prev_allocator = prev;
        if(prev != END_FLAG)
            manager->allocs[prev].next_allocator = next;
        else
            manager->p_full = next;

        allocator->next_allocator = manager->free;
        allocator->prev_allocator = END_FLAG;
        if(manager->free != END_FLAG) 
            manager->allocs[manager->free].prev_allocator = allocator_index;
        manager->free = allocator_index;
    }
    else if(allocator->avai_num == 1){
        // if allocator is from full list
        // move allocator from full to p_full
        uint16_t prev = allocator->prev_allocator;
        uint16_t next = allocator->next_allocator;
        if(next != END_FLAG)
            manager->allocs[next].prev_allocator = prev;
        if(prev != END_FLAG)
            manager->allocs[prev].next_allocator = next;
        else
            manager->full = next;

        allocator->next_allocator = manager->p_full;
        allocator->prev_allocator = END_FLAG;
        if(manager->p_full != END_FLAG) 
            manager->allocs[manager->p_full].prev_allocator = allocator_index;
        manager->p_full = allocator_index;
    }

}


static inline uint32_t is_linked_mode(uint16_t slice_size) {
    return slice_size <= 256;
}