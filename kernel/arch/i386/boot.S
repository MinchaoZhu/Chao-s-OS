# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM


# Reserve a stack for temporary stack
.section .init.data
.align 16
stack_bottom:
.skip 1024 # 1 KiB
stack_top:

# The kernel entry point
.section .init.text

# multiboot information pointer before paging
.global global_multiboot_ptr_tmp
global_multiboot_ptr_tmp:
.skip 4

.global _start
.type _start, @function
_start:
	# init current stack
	movl $stack_top, %esp
	# set multiboot ptr, copy from ebx
	mov %ebx, global_multiboot_ptr_tmp

	# Transfer control to the main kernel.
	call kernel_main_init

	# Hang if kernel unexpectedly returns.
	cli
1:	hlt
	jmp 1b


.section .text
# multiboot information pointer after paging
.global global_multiboot_ptr
global_multiboot_ptr:
.skip 4

