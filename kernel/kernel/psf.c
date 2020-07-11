#include "gui/psf.h"
#include <stdint.h>
#include <stdio.h>

extern uint8_t _binary_arch_i386_font_psf_start;
extern uint8_t _binary_arch_i386_font_psf_end;

//psf font header
PSF_font* font = 0;
//font bitmap
uint8_t* glyphs = 0;

void init_psf() {
    font = (PSF_font*) &_binary_arch_i386_font_psf_start;
    glyphs = (uint8_t*) (((uint32_t)font) + font -> headersize); 
}


// ((uint8_t*) font) + font->header_size +'1' * font->bytesperglyph
// 00000000
// 00000000
// 00011000
// 00111000
// 01111000
// 00011000
// 00011000
// 00011000
// 00011000
// 00011000
// 00011000
// 01111110
// 00000000
// 00000000
// 00000000
// 00000000