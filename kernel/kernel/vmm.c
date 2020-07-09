#include "kernel/vmm.h"
#include "kernel/pmm.h"
#include "kernel/idt.h"
#include <string.h>

__attribute__((aligned(4096))) page_directory_t pgd_k[1024];
__attribute__((aligned(4096))) static page_t tables[KERNEL_NORMAL_ZONE_DIRECTORY_SIZE][1024];
extern uint32_t* pt0;
void init_vmm(void) {

    // map 0xC0000000 to 0x00000000 (virtual -> physic)
    uint32_t page_of_set_dir_index = PGD_INDEX(PAGE_OFFSET);

    // clear page directory entry
    for(uint32_t i = 0; i < page_of_set_dir_index; ++i) {
        pgd_k[i] = 0;
    }

    for(uint32_t i = 0; i < KERNEL_NORMAL_ZONE_DIRECTORY_SIZE; ++i) {
        pgd_k[i+page_of_set_dir_index] 
        = (page_t*)(((uint32_t)&tables[i] - PAGE_OFFSET) | PAGE_PRESENT | PAGE_WRITE);
    }

    // use the first linear paging directory to return callee's next instruction
    pgd_k[0] = (page_t*)(((uint32_t)pt0) | PAGE_PRESENT | PAGE_WRITE);

    for(uint32_t i = 0; i < KERNEL_NORMAL_ZONE_DIRECTORY_SIZE; ++i) {
        for(uint32_t j = 0; j < 1024; ++j) {
            uint32_t k = i * 1024 + j;
            tables[i][j] = (k << 12) | PAGE_PRESENT | PAGE_WRITE;
        }
    }
    uint32_t pgd_k_phy_addr = ((uint32_t)&pgd_k) - PAGE_OFFSET;
    switch_pgd(pgd_k_phy_addr);
}

void switch_pgd(uint32_t pd) {
    asm volatile ("mov %0, %%cr3" : : "r" (pd));
}


void map(page_directory_t* pd, uint32_t va, uint32_t pa, uint32_t flags) {
    // page directory entry index
    uint32_t pgd_index = PGD_INDEX(va);
    // page table entry index
    uint32_t pte_index = PTE_INDEX(va);

    page_t* table = (page_t*) ((uint32_t) pd[pgd_index] & PAGE_MASK);
    if(!table) {
        // if such entry doesn't exist
        table = (page_t*) physic_alloc(sizeof(PAGE_SIZE));
        pd[pgd_index] = (uint32_t) table | flags;
        table = (page_t*) ((uint32_t) table + PAGE_OFFSET);
        memset(table, 0, PAGE_SIZE);
    }
    else {
        table = (page_t*) ((uint32_t) table + PAGE_OFFSET);
    }

    table[pte_index] = (pa & PAGE_MASK) | flags;

    // Force CPU refesh paging buffer
    asm volatile ("invlpg (%0)" : : "a" (va));
}

void unmap(page_directory_t* pd, uint32_t va) {
    uint32_t pgd_index = PGD_INDEX(va);
    uint32_t pte_index = PTE_INDEX(va);
    
    page_t* table = (page_t*) ((uint32_t) pd[pgd_index] & PAGE_MASK);
    if(!table)
        return;

    table = (page_t*) ((uint32_t) table + PAGE_OFFSET);
    table[pte_index] = 0;

    asm volatile ("invlpg (%0)" : : "a" (va));
}

uint32_t get_mapping(page_directory_t* pd, uint32_t va, uint32_t* pa) {
    uint32_t pgd_index = PGD_INDEX(va);
    uint32_t pte_index = PTE_INDEX(va);
    
    page_t* table = (page_t*) ((uint32_t) pd[pgd_index] & PAGE_MASK);
    if(!table)
        return 0;
    
    table = (page_t*) ((uint32_t) table + PAGE_OFFSET);

    if(table[pte_index] && pa) {
        *pa = table[pte_index] & PAGE_MASK;
        return 1;
    }

    return 0;
}


