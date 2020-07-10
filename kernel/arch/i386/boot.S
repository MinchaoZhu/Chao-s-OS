# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set MODE_TYPE,1<<2             # video mode type
.set FLAGS,    ALIGN | MEMINFO | MODE_TYPE # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
# https://www.gnu.org/software/grub/manual/multiboot/html_node/Header-layout.html#Header-layout
# Offset	Type	Field Name	Note
# 0	u32	magic	required
# 4	u32	flags	required
# 8	u32	checksum	required
# 12	u32	header_addr	if flags[16] is set
# 16	u32	load_addr	if flags[16] is set
# 20	u32	load_end_addr	if flags[16] is set
# 24	u32	bss_end_addr	if flags[16] is set
# 28	u32	entry_addr	if flags[16] is set
# 32	u32	mode_type	if flags[2] is set
# 36	u32	width	if flags[2] is set
# 40	u32	height	if flags[2] is set
# 44	u32	depth	if flags[2] is set
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0, 0, 0, 0, 0
.long 0 #set graphics mode; VESA: https://en.wikipedia.org/wiki/VESA_BIOS_Extensions
.long 1024 #width
.long 768  # height
.long 32   # depth


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

