.section .text

# macro to generate ISR without error code
.macro ISR_NOERRCODE n
.global isr\n
.type isr\n, @function
isr\n:
    cli                  # close interrupt
    push $0               # push 0 which represents the error code in another type ISR
    push $\n              # push interrupt number
    jmp isr_common_stub  # jump to common ISA part
.endm


# macro to generate ISR with error code
.macro ISR_ERRCODE n
.global isr\n
.type isr\n, @function
isr\n:
    cli
    push $\n              # push interrupt number
    jmp isr_common_stub  # jump to common ISA part
.endm


// define ISR by using macro
ISR_NOERRCODE  0
ISR_NOERRCODE  1
ISR_NOERRCODE  2
ISR_NOERRCODE  3
ISR_NOERRCODE  4
ISR_NOERRCODE  5
ISR_NOERRCODE  6
ISR_NOERRCODE  7
ISR_ERRCODE    8
ISR_NOERRCODE  9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19


ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

ISR_NOERRCODE 255



# interrupt service routine program
isr_common_stub:
    pushal
    mov %ds, %ax
    push %eax             # store data segment descriptor

    mov $0x10, %ax       # load kernel data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    push %esp            # pointer of pt_regs, arguments of function interrupt_handler
    call isr_handler     # in idt.c
    add $4, %esp

    pop %ebx
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs
    mov %bx, %ss   
    


    popal                 # Pops edi, esi, ebp, esp, ebx, edx, ecx, eax
    add $8, %esp         # clear error code and ISR
    iret                 # interrupt return

.global idt_flush
.type idt_flush, @function
idt_flush:
    mov 4(%esp), %eax
    lidt (%eax)
    ret