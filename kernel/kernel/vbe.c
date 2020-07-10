#include "kernel/vbe.h"
#include <stdint.h>
#include <string.h> // memcpy
#include "kernel/multiboot.h"
#include "kernel/kmm.h" // PAGE_SIZE kmalloc
#include "kernel/vmm.h" // map()
#include "kernel/math.h" // power of 2
#include "interrupt/timer.h" // register timer

vbe_info_block_t* vbe_info;
mode_info_block_t* vbe_mode_info;
extern page_directory_t pgd_k[1024];
uint32_t vram = 0;
uint32_t vram_size = 0;
uint8_t* vbe_buffer = 0;

void swap_buffer();

void init_vbe() {
    vbe_info = (vbe_info_block_t*) ((uint32_t)global_multiboot_ptr->vbe_control_info + PAGE_OFFSET);
    vbe_mode_info = (mode_info_block_t*) ((uint32_t)global_multiboot_ptr->vbe_mode_info + PAGE_OFFSET);
    uint32_t Xres = vbe_mode_info->Xres;
    uint32_t Yres = vbe_mode_info->Yres;
    uint32_t bpp  = vbe_mode_info->bpp;
    bpp = next_power_of_2(bpp);

    // uint32_t pages = (65536 / PAGE_SIZE) * (vbe_info->TotalMemory);
    uint32_t pages = Xres * Yres * (bpp / 8) / PAGE_SIZE;
    vram = vbe_mode_info->physbase;
    
    vbe_buffer = (uint8_t*) kmalloc(pages * PAGE_SIZE);
    vram_size = pages * PAGE_SIZE;

    for(uint32_t i = 0; i < pages; ++i) {
        map(&pgd_k, vram + i * PAGE_SIZE, vram + i * PAGE_SIZE,  PAGE_PRESENT | PAGE_WRITE);
    }

    for(int i = 0; i<vbe_mode_info->Xres; ++i) {
        for(int j = 0; j<vbe_mode_info->Yres; ++j) {
            put_pixel((uint8_t*) vram, i, vbe_mode_info->bpp, j, vbe_mode_info->pitch, 0xffffff);
            put_pixel((uint8_t*) vbe_buffer, i, vbe_mode_info->bpp, j, vbe_mode_info->pitch, 0xffffff);
        }
    }

    register_timer(swap_buffer, 60);
    
}

void put_pixel(uint8_t* screen,int x,int bbs,int y,int pitch,uint32_t color) {
   uint32_t where = (x * bbs/8) + (y * pitch);
   screen[where] = color & 255;
   screen[where + 1] = (color >> 8) & 255;
   screen[where + 2] = (color >> 16) & 255;
   screen[where + 3] = (color >> 24) & 255;
}

void swap_buffer() {
    static int i = 0;
    vbe_buffer[i + 1] = 0xab;
    vbe_buffer[i+2] = 0xc1;
    vbe_buffer[i++ + 3] = 0x23;
    memcpy((void*)vram, (void*)vbe_buffer, vram_size);
}
