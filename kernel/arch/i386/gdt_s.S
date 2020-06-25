.section .text
.global gdt_flush
.type gdt_flush, @function
gdt_flush:
    mov 4(%esp), %eax   # argument gdt_ptr
    lgdt (%eax)         # load gdt

    mov $0x10, %ax       # 0x10 is our data selector
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    jmp $0x8, $.flush

.flush:
    ret