#include "gui/terminal.h"
#include <stdint.h>
#include "kernel/vbe.h"
#include "gui/psf.h"
#include <string.h>

extern uint8_t* vbe_buffer;
extern mode_info_block_t* vbe_mode_info;
extern uint32_t vram_size;

static inline void main_terminal_putchar(char ch);
static inline void main_terminal_newline();
static inline void main_terminal_roll();

terminal_t main_terminal;


void init_main_terminal() {
    main_terminal.posX = 0;
    main_terminal.posY = 0;
    main_terminal.width = vbe_mode_info->Xres;
    main_terminal.height = vbe_mode_info->Yres;
    main_terminal.charX = 0; 
    main_terminal.charY = 0; 

    uint32_t char_height = font->height;
    uint32_t char_width = font->width;

    main_terminal.rows = main_terminal.height / char_height;
    main_terminal.cols = main_terminal.width / char_width;
    main_terminal.fc   = 0x00000000;
    main_terminal.bc   = 0x00ffffff;

}


static inline void main_terminal_putchar(char ch) {
    if(ch == '\n'){
        main_terminal_newline();
        return;
    }

    uint8_t* glyph = glyphs + ch * font->bytesperglyph; // character's bitmap
    
    uint32_t char_height = font->height;
    uint32_t char_width = font->width;

    uint32_t pixelX = main_terminal.charX * char_width;
    uint32_t pixelY = main_terminal.charY * char_height;

    for(int i = 0; i < char_height; ++i) {
        for(int j = 0; j < char_width; ++j) {
            uint32_t index = (j + i * char_width) / 8;
            uint32_t bit_off   = (j + i * char_width) % 8;
            uint32_t c = (glyph[index] & (1 << (7 - bit_off))) ? main_terminal.fc : main_terminal.bc;
            put_pixel(vbe_buffer, pixelX + j, vbe_mode_info->bpp, pixelY + i, vbe_mode_info->pitch, c);
        }
    }
    if(++(main_terminal.charX) == main_terminal.cols) {
        main_terminal_newline();
    }

}

void main_terminal_write(char* str, uint32_t size) {
    for(uint32_t i = 0; i<size; ++i) {
        main_terminal_putchar(str[i]);
    }
}

static inline void main_terminal_newline() {
    if(++(main_terminal.charY) == main_terminal.rows) {
        main_terminal_roll();
        main_terminal.charY = main_terminal.rows - 1;
    }
    main_terminal.charX = 0;
}

static inline void main_terminal_roll() {
    uint32_t Xres = vbe_mode_info->Xres;
    uint32_t bpp = vbe_mode_info->bpp;
    memcpy((void*)vbe_buffer, (void*)(vbe_buffer + font->height * Xres * 4), vram_size - font->height * Xres * 4);
    for(uint32_t y = 0; y<font->height; ++y) {
        uint32_t ytmp = y + main_terminal.height - font -> height;
        for(uint32_t x = 0; x<main_terminal.width; ++x) {
            put_pixel(vbe_buffer, x, vbe_mode_info->bpp, ytmp, vbe_mode_info->pitch, main_terminal.bc);
        }
    }
}