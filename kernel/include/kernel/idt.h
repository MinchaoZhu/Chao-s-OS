#ifndef INCLUDE_IDT_H
#define INCLUDE_IDT_H

#include <stdint.h>

// Interrupt descriptor
typedef
struct idt_entry_t {
    uint16_t base_lo; // interrupt service routine address bits[0:15]
    uint16_t sel;     // selector
    uint8_t always0;  // always 0
    uint8_t flags;    // some flags
    uint16_t base_hi; // interrupt service routine address bits[16:31]
}__attribute__((packed)) idt_entry_t;

// IDTR
typedef
struct idt_ptr_t {
    uint16_t limit;   // length limit
    uint32_t base;   // base address
} __attribute__((packed)) idt_ptr_t;


typedef
struct pt_regs_t {
    uint32_t ds;        // data segment descriptor
    uint32_t edi;       // from edi to eax, value will be pushed in stack
    uint32_t esi; 
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t int_no;    // interrupt number
    uint32_t err_code;      // error code, if error exists it will be pushed by CPU automatically 
    uint32_t eip;       // blow values will be pushed by CPU automatically
    uint32_t cs;        
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
} pt_regs;




// declare isr from 0 to 19 which are exception interrupt belongs to CPU
// ISR:(interrupt service routine)
void isr0();        // 0 #DE divisor is 0 
void isr1();        // 1 #DB debug exception 
void isr2();        // 2 NMI 
void isr3();        // 3 BP break point abnormal exception 
void isr4();        // 4 #OF overflow exception 
void isr5();        // 5 #BR index out of bound for array 
void isr6();        // 6 #UD undefined operator code
void isr7();        // 7 #NM  
void isr8();        // 8 #DF                                   (ERROR CODE) 
void isr9();        // 9  
void isr10();       // 10 #TS invalid TSS                      (ERROR CODE) 
void isr11();       // 11 #NP no such segment                  (ERROR CODE) 
void isr12();       // 12 #SS stack error                      (ERROR CODE) 
void isr13();       // 13 #GP protect                          (ERROR CODE) 
void isr14();       // 14 #PF page fault                       (ERROR CODE) 
void isr15();       // 15 CPU reserved 
void isr16();       // 16 #MF float uint errot 
void isr17();       // 17 #AC align check 
void isr18();       // 18 #MC machine check
void isr19();       // 19 #XM SIMD float abnormal

// 20 ~ 31 Intel reserve
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();


// 32 ~ 255 user define
void isr255();

//initialize idt
void init_idt();

#endif