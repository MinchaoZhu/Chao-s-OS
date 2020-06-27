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

# irq0 - irq15
# isr32- isr47
ISR_NOERRCODE 32 # irq0    system timer (cannot be changed)
ISR_NOERRCODE 33 # irq1    keyboard controller (cannot be changed)
ISR_NOERRCODE 34 # irq2    cascaded signals from IRQs 8–15 (any devices configured to use IRQ 2 will actually be using IRQ 9)
ISR_NOERRCODE 35 # irq3    serial port controller for serial port 2 (shared with serial port 4, if present)
ISR_NOERRCODE 36 # irq4    serial port controller for serial port 1 (shared with serial port 3, if present)
ISR_NOERRCODE 37 # irq5    parallel port 2 and 3  or  sound card
ISR_NOERRCODE 38 # irq6    floppy disk controller
ISR_NOERRCODE 39 # irq7    parallel port 1.
ISR_NOERRCODE 40 # irq8    real-time clock (RTC)
ISR_NOERRCODE 41 # irq9    Advanced Configuration and Power Interface (ACPI) system control interrupt on Intel chipsets.
ISR_NOERRCODE 42 # irq10   The Interrupt is left open for the use of peripherals (open interrupt/available, SCSI or NIC)
ISR_NOERRCODE 43 # irq11   The Interrupt is left open for the use of peripherals (open interrupt/available, SCSI or NIC)
ISR_NOERRCODE 44 # irq12   mouse on PS/2 connector
ISR_NOERRCODE 45 # irq13   CPU co-processor  or  integrated floating point unit  or  inter-processor interrupt (use depends on OS)
ISR_NOERRCODE 46 # irq14   primary ATA channel (ATA interface usually serves hard disk drives and CD drives)
ISR_NOERRCODE 47 # irq15   secondary ATA channel


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
    iret                 # interrupt return: pop CS, EIP, EFLAGS, SS, ESP

.global idt_flush
.type idt_flush, @function
idt_flush:
    mov 4(%esp), %eax
    lidt (%eax)
    ret




