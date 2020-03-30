section .text

[GLOBAL switch_page_directory]
switch_page_directory:
    mov eax, [esp + 4]
    mov cr3, eax

[GLOBAL paging_enable]
paging_enable:
	mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

[GLOBAL paging_disable]
paging_disable:
    mov eax, cr0
    or eax, 0x7fffffff
    mov cr0, eax

[GLOBAL flush_tlb]
flush_tlb:
	cli
    mov eax, [esp + 4]
    invlpg [eax]
	sti