#include <lib/kstdio.h>
#include <lib/kstdlib.h>
#include <arch/memory.h>
#include <arch/pmm.h>
#include <arch/vmm.h>
#include <arch/interrupt.h>

extern void flush_tlb();
extern void paging_enable();
extern void switch_page_directory(uintptr_t);

uintptr_t kpage_dir[1024];

void paging_invalidate_page(uintptr_t virt) {
	asm volatile ("invlpg (%0)" :: "b"(virt) : "memory");
}

void map_page(uintptr_t virtaddr,uintptr_t physaddr) {
    uintptr_t pdindex = virtaddr >> 22;
    uintptr_t ptindex = virtaddr >> 12 & 0x03ff;
    uint32_t flags = PAGE_P | PAGE_W;

    if ((uint32_t)virtaddr < VIRT_START)
        flags |= PAGE_U;
    
    uintptr_t *pd = (uintptr_t*)0xFFFFF000;
    if (!(pd[pdindex] & PAGE_P)) {
        uintptr_t tab_phys = (uintptr_t)allocate_pmm();
        if (!tab_phys) {
            kprintf("Memory not enough\n");
            return;
        }
        memset((void*)tab_phys,0,4096);
        pd[pdindex] = tab_phys | flags;
    }

    uintptr_t *pt = (uintptr_t*)0xFFC00000 + (0x1000 * pdindex);
    if (!(pt[ptindex] & PAGE_P)) {
        if ((intptr_t)physaddr == -1) {
            physaddr = (uintptr_t)allocate_pmm();
            if (!physaddr) {
                kprintf("Memory not enough\n");
                return;
            }
           memset((void*)physaddr,0,4096);
        }
    }
    pt[ptindex] = ((unsigned long)physaddr) | flags;
    flush_tlb();
}

void unmap_page(uintptr_t virtaddr) {
    uintptr_t pdindex = virtaddr >> 22;
    uintptr_t ptindex = virtaddr >> 12 & 0x03ff;

    uintptr_t *pd = (uintptr_t*)0xFFFFF000;
    uintptr_t *pt = (uintptr_t*)0xFFC00000 + (0x1000 * pdindex);

    if ((pd[pdindex] & PAGE_P)) {
        if ((pt[ptindex] & PAGE_P)) {
            uintptr_t page_phys = (pt[ptindex] & PAGE_MASK);
            pt[ptindex] = 0;
            paging_invalidate_page(page_phys);
            deallocate_pmm((uintptr_t)page_phys);
        }

        for (uint32_t i = 0; i < 1024; i++) {
            if (pt[i] & PAGE_P)
                break;
            if (i == 1023) {
                uintptr_t tab_phys = (pd[pdindex] & PAGE_MASK);
                pd[pdindex] = 0;
                paging_invalidate_page(tab_phys);
                deallocate_pmm(tab_phys);
            }
        }
    }
    flush_tlb();
}


void page_fault(register_t *r) {
    uint32_t cr2;
    asm volatile ("mov %%cr2, %0" : "=r"(cr2));
    kprintf("Page fault at 0x%x, faulting address 0x%x\n",r->eip,cr2);
    kprintf("Error code : %x\n", r->err_code);
    for(;;);
}

void initialize_vmm() { 
    isr_install_handler(14, page_fault);

    uintptr_t phys = (uintptr_t)allocate_pmm();
    kpage_dir[1023] = (uintptr_t)kpage_dir | PAGE_P | PAGE_W;
    kpage_dir[0] = phys | PAGE_W | PAGE_P;
    flush_tlb();

    // Paging bug in this line of code
    // uintptr_t *tab = (uintptr_t*)0xFFC00000;
    // for (unsigned i = 0; i < 1024; i++){
    //     tab[i] = (i * PAGE_SIZE) | PAGE_W | PAGE_P;
    // }    
    
    kpage_dir[768] = kpage_dir[0];
    kpage_dir[0] = 0;
    flush_tlb();

    paging_enable();
}