#include "kernel/vbe.h"
#include <stdint.h>
#include "kernel/multiboot.h"
#include "kernel/kmm.h" // PAGE_SIZE kmalloc
#include "kernel/vmm.h" // map()

vbe_info_block_t* vbe_info;
mode_info_block_t* vbe_mode_info;
extern page_directory_t pgd_k[1024];

void init_vbe() {
    vbe_info = (vbe_info_block_t*) ((uint32_t)global_multiboot_ptr->vbe_control_info + PAGE_OFFSET);
    vbe_mode_info = (mode_info_block_t*) ((uint32_t)global_multiboot_ptr->vbe_mode_info + PAGE_OFFSET);

    uint32_t pages = (65536 / PAGE_SIZE) * (vbe_info->TotalMemory);
    uint32_t vram = vbe_mode_info->physbase;
    
    for(uint32_t i = 0; i < pages; ++i) {
        map(&pgd_k, vram + i * PAGE_SIZE, vram + i * PAGE_SIZE,  PAGE_PRESENT | PAGE_WRITE);
    }

    for(int i = 0; i<vbe_mode_info->Xres; ++i) {
        for(int j = 0; j<vbe_mode_info->Yres; ++j) {
            put_pixel(vram, i, vbe_mode_info->bpp, j, vbe_mode_info->pitch, 0xffffff);
        }
    }

}

void put_pixel(uint8_t* screen,int x,int bbs,int y,int pitch,uint32_t color)
{
   uint32_t where = (x * bbs/8) + (y * pitch);
   screen[where] = color & 255;
   screen[where + 1] = (color >> 8) & 255;
   screen[where + 2] = (color >> 16) & 255;
   screen[where + 3] = (color >> 24) & 255;
}

void task01() {
    vbe_mode_info = (mode_info_block_t*) ((uint32_t)global_multiboot_ptr->vbe_mode_info + PAGE_OFFSET);
    uint32_t vram = vbe_mode_info->physbase;
    while(1){
        for(int i = 511; i< 800; ++i) {
            for(int j = 300; j<600; ++j) {
                put_pixel(vram, i, vbe_mode_info->bpp, j, vbe_mode_info->pitch, 0xf55142);
            }
        }
        asm volatile ("hlt"); // CPU halt;
    }
}

void task02() {
    vbe_mode_info = (mode_info_block_t*) ((uint32_t)global_multiboot_ptr->vbe_mode_info + PAGE_OFFSET);
    uint32_t vram = vbe_mode_info->physbase;
    while(1){
        for(int i = 511; i< 800; ++i) {
            for(int j = 300; j<600; ++j) {
                put_pixel(vram, i, vbe_mode_info->bpp, j, vbe_mode_info->pitch, 0x24155f);
            }
        }
        asm volatile ("hlt"); // CPU halt;
    }
}