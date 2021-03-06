MEMORY_ALIGN equ 1 << 0
MEMORY_INFO equ 1 << 1
MULTIBOOT_FLAGS equ MEMORY_ALIGN | MEMORY_INFO
MULTIBOOT_MAGIC equ 0x1BADB002
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

section .multiboot
align 4
	dd MULTIBOOT_MAGIC
	dd MULTIBOOT_FLAGS
	dd MULTIBOOT_CHECKSUM

section .bss
align 16
stack_bottom:
resb 16384 
stack_top:

global KERNEL_VIRTUAL_BASE 
KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)

section .data
align 0x1000
global boot_page_directory
boot_page_directory: 
	dd 0x000000083  
	times (KERNEL_PAGE_NUMBER - 1) dd 0
	dd 0x000000083
	times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0


section .text
global loader
loader:
	mov eax, (boot_page_directory - KERNEL_VIRTUAL_BASE)
	mov cr3, eax

	mov eax, cr4
	or eax, 0x00000010
	mov cr4, eax

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	lea ecx, [higher_half_loader]
	jmp ecx

higher_half_loader:
	mov esp , stack_top

	push eax
	add ebx, 0xC0000000
	push ebx
	
	cli
	
	extern kmain
	call kmain

	cli
.hang:	
	hlt
	jmp .hang
.end: