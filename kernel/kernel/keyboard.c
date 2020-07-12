#include "io/keyboard.h"
#include "io/ioport.h"
#include "interrupt/interrupt.h"
#include "kernel/kmm.h"
#include <stdio.h>
#include <stdint.h>


uint8_t extend_flag = 0;
uint32_t keyboard_output_buffer_length = 512;
uint8_t* keyboard_output_buffer;
uint32_t k_index = 0;

void keyboard_input_handler(); 


// mapping from scancode to ASCII
// 0x00 - 0x3A 
static char keymap[][2] = {
/* 0x00 */ {0, 0}, 
/* 0x01 */ {esc, esc}, 
/* 0x02 */ {'1', '!'}, 
/* 0x03 */ {'2', '@'},
/* 0x04 */ {'3', '#'},
/* 0x05 */ {'4', '$'},
/* 0x06 */ {'5', '%'},
/* 0x07 */ {'6', '^'},
/* 0x08 */ {'7', '&'},
/* 0x09 */ {'8', '*'}, 
/* 0x0A */ {'9', '('},
/* 0x0B */ {'0', ')'}, 
/* 0x0C */ {'-', '_'},
/* 0x0D */ {'=', '+'},
/* 0x0E */ {backspace, backspace},
/* 0x0F */ {tab, tab},
/* 0x10 */ {'q', 'Q'},
/* 0x11 */ {'w', 'W'},
/* 0x12 */ {'e', 'E'},
/* 0x13 */ {'r', 'R'},
/* 0x14 */ {'t', 'T'},
/* 0x15 */ {'y', 'Y'},
/* 0x16 */ {'u', 'U'},
/* 0x17 */ {'i', 'I'},
/* 0x18 */ {'o', 'O'},
/* 0x19 */ {'p', 'P'},
/* 0x1A */ {'[', '{'},
/* 0x1B */ {']', '}'},
/* 0x1C */ {enter, enter},
/* 0x1D */ {ctrl_l_char, ctrl_l_char},
/* 0x1E */ {'a', 'A'},
/* 0x1F */ {'s', 'S'},
/* 0x20 */ {'d', 'D'},
/* 0x21 */ {'f', 'F'},
/* 0x22 */ {'g', 'G'},
/* 0x23 */ {'h', 'H'},
/* 0x24 */ {'j', 'J'},
/* 0x25 */ {'k', 'K'},
/* 0x26 */ {'l', 'L'},
/* 0x27 */ {';', ':'},
/* 0x28 */ {'\'', '"'},
/* 0x29 */ {'`', '~'},
/* 0x2A */ {shift_l_char, shift_l_char},
/* 0x2B */ {'\\', '|'},
/* 0x2C */ {'z', 'Z'},
/* 0x2D */ {'x', 'X'},
/* 0x2E */ {'c', 'C'},
/* 0x2F */ {'v', 'V'},
/* 0x30 */ {'b', 'B'},
/* 0x31 */ {'n', 'N'},
/* 0x32 */ {'m', 'M'},
/* 0x33 */ {',', '<'},
/* 0x34 */ {'.', '>'},
/* 0x35 */ {'/', '?'},
/* 0x36 */ {shift_r_char, shift_r_char},
/* 0x37 */ {'*', '*'},
/* 0x38 */ {alt_l_char, alt_l_char},
/* 0x39 */ {' ', ' '},
/* 0x3A */ {caps_lock_char, caps_lock_char}
};



void init_keyboard() {
    // regisrer keyboard interrupt
    crtl_status = 0; 
    shift_status = 0;
    alt_status = 0;
    caps_lock_status = 0;
    ext_scancode = 0;
    keyboard_output_buffer = (uint8_t*) kmalloc(keyboard_output_buffer_length);
    register_interrupt_handler(0x21, keyboard_input_handler);
    //release register buffer
    inb(KBD_BUF_PORT);
}

void keyboard_input_handler(pt_regs *regs) {
    uint8_t scancode = inb(KBD_BUF_PORT);
    // printf("%X\n", scancode);
    if(extend_flag == EXTEND_CODE_PREFIX) {
        // if scancode following 0xE0
        extend_flag = EXTEND_CODE_FLUSH;
        switch(scancode) {
            case R_ALT_PRESSED & SCANCODE_MASK:
                alt_status = KEY_PRESSED;
                break;
            case R_ALT_RELEASED & SCANCODE_MASK:
                alt_status = KEY_RELEASED;
                break;
            case R_CTRL_PRESSED & SCANCODE_MASK:
                crtl_status = KEY_PRESSED;
                break;
            case R_CTRL_RELEASED & SCANCODE_MASK:
                crtl_status = KEY_RELEASED;
                break;
        }
    }
    else {
        switch(scancode) {
            case EXTEND_CODE_PREFIX:
                // extended scancode
                extend_flag = EXTEND_CODE_PREFIX;
                break;
            case L_SHIFT_PRESSED:
                shift_status = KEY_PRESSED;
                break;
            case L_SHIFT_RELEASED:
                shift_status = KEY_RELEASED;
                break;
            case R_SHIFT_PRESSED:
                shift_status = KEY_PRESSED;
                break;
            case R_SHIFT_RELEASED:
                shift_status = KEY_RELEASED;
                break;
            case L_ALT_PRESSED:
                alt_status = KEY_PRESSED;
                break;
            case L_ALT_RELEASED:
                alt_status = KEY_RELEASED;
                break;
            case L_CTRL_PRESSED:
                crtl_status = KEY_PRESSED;
                break;
            case L_CTRL_RELEASED:
                crtl_status = KEY_RELEASED;
                break;
            case CAPS_LOCK_PRESSED:
                caps_lock_status = (caps_lock_status == KEY_PRESSED)?KEY_RELEASED:KEY_PRESSED;
                break;
            default:
            {
                if(scancode > 0x3A)
                    return;
                char c = 0; // scanning character
                if(keymap[scancode][0] >= 'a' && keymap[scancode][1] <= 'z') {
                    uint32_t offset = (shift_status == KEY_PRESSED) ^ (caps_lock_status == KEY_PRESSED);
                    c = keymap[scancode][offset];
                }
                else if(keymap[scancode][0] == CHAR_INVINSIBLE) {
                    break;
                }
                else {
                    uint32_t offset = (shift_status == KEY_PRESSED);
                    c = keymap[scancode][offset]; 
                }
                keyboard_output_buffer[k_index++] = c;
            }

        }
    }
    

}


