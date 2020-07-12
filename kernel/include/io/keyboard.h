#ifndef IO_KEABOARD_H_
#define IO_KEABOARD_H_

#include <stdint.h>

#define KBD_BUF_PORT 0x60 
#define KEY_PRESSED 1
#define KEY_RELEASED 0

#define SCANCODE_MASK 0x00FF

// partial control charactor
#define esc '\003'
#define backspace '\b'
#define tab '\t'
#define enter '\r'
#define delete '\177'

// invinsible character
#define CHAR_INVINSIBLE 0
#define ctrl_l_char CHAR_INVINSIBLE
#define ctrl_r_char CHAR_INVINSIBLE
#define shift_l_char CHAR_INVINSIBLE
#define shift_r_char CHAR_INVINSIBLE
#define alt_l_char CHAR_INVINSIBLE
#define alt_r_char CHAR_INVINSIBLE
#define caps_lock_char CHAR_INVINSIBLE

#define UNSET_KEY_MAP CHAR_INVINSIBLE

// partial control key scan code
#define L_SHIFT_PRESSED 0x2A
#define L_SHIFT_RELEASED 0xAA
#define R_SHIFT_PRESSED 0x36
#define R_SHIFT_RELEASED 0xB6
#define L_ALT_PRESSED 0x38
#define L_ALT_RELEASED 0xB8
#define R_ALT_PRESSED 0xE038
#define R_ALT_RELEASED 0xE0B8
#define L_CTRL_PRESSED 0x1D
#define L_CTRL_RELEASED 0x9D
#define R_CTRL_PRESSED 0xE01D
#define R_CTRL_RELEASED 0xE09D
#define CAPS_LOCK_PRESSED 0x3A 

#define EXTEND_CODE_PREFIX 0xE0
#define EXTEND_CODE_FLUSH  0


void init_keyboard();




static uint8_t crtl_status, shift_status, alt_status, caps_lock_status, ext_scancode;

#endif // IO_KEABOARD_H_