#ifndef GUI_TERMINAL_H_
#define GUI_TERMINAL_H_

#include "gui/psf.h"
#include <stdint.h>
#include "kernel/vbe.h"



extern PSF_font* font;
extern uint8_t* glyphs;




typedef struct terminal {
    uint16_t width; // terminal width in pixel
    uint16_t height;// terminal height in pixel
    uint16_t posX;  // terminal left-upper corner in pixel - X
    uint16_t posY;  // terminal left-upper corner in pixel - Y
    uint16_t charX; // current cursor position in char - X
    uint16_t charY; // current cursor position in char - Y
    uint16_t rows;  // terminal height in char
    uint16_t cols;  // terminal width in char
    uint32_t bc;    // background color
    uint32_t fc;    // character color
} __attribute__((packed)) terminal_t;




void init_main_terminal();

void main_terminal_write(char* str, uint32_t size);


#endif // GUI_TERMINAL_H_