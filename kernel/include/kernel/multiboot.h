#ifndef KERNEL_MULTIBOOT_H_
#define KERNEL_MULTIBOOT_H_

#include <stdint.h>

/* The symbol table for a.out.  */
typedef struct multiboot_aout_symbol_table_t {
  uint32_t tabsize;
  uint32_t strsize;
  uint32_t addr;
  uint32_t reserved;
}__attribute__((packed)) multiboot_aout_symbol_table_t;

/* The section header table for ELF.  */
typedef struct multiboot_elf_section_header_table_t {
  uint32_t num;
  uint32_t size;
  uint32_t addr;
  uint32_t shndx;
}__attribute__((packed)) multiboot_elf_section_header_table_t;


// multiboot information struct
typedef struct multiboot_t {
    uint32_t flags; // multiboot version

    // Available memory from BIOS
    uint32_t mem_lower; // lower memory size
    uint32_t mem_upper; // upper memory size

    uint32_t boot_device; // root partition
    uint32_t cmdline; // kernel command line

    uint32_t mods_count;// bios module list
    uint32_t mods_addr; // bios module address 

    union
    {
        multiboot_aout_symbol_table_t aout_sym;
        multiboot_elf_section_header_table_t elf_sec;
    } u;



    // memory mapping buffer
    uint32_t mmap_length;
    uint32_t mmap_addr;

    uint32_t drives_length; // first drive size
    uint32_t drives_addr; // first drive physical address

    uint32_t config_table; // ROM configuration table
    uint32_t boot_loader_name; // boot loader
    uint32_t apm_table; // APM table

    // video
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    union
    {
      struct
      {
        uint32_t framebuffer_palette_addr;
        uint16_t framebuffer_palette_num_colors;
      };
      struct
      {
        uint8_t framebuffer_red_field_position;
        uint8_t framebuffer_red_mask_size;
        uint8_t framebuffer_green_field_position;
        uint8_t framebuffer_green_mask_size;
        uint8_t framebuffer_blue_field_position;
        uint8_t framebuffer_blue_mask_size;
      };
    };
} __attribute__((packed)) multiboot_t;

// memory entry
typedef struct mmap_entry_t {
    uint32_t size; // size of struct without size varibale itself
    uint32_t base_addr_low;
    uint32_t base_addr_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
}  __attribute__((packed)) mmap_entry_t;


extern multiboot_t* global_multiboot_ptr;

extern multiboot_t* global_multiboot_ptr_tmp;

#endif // MULTIBOOT_H