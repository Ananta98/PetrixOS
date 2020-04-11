#include <lib/kstdio.h>
#include <lib/kstdlib.h>
#include <arch/x86/mm/mm.h>
#include <arch/x86/mm/pmm.h>
#include <arch/x86/mm/vmm.h>

extern void flush_tlb(uintptr_t address);

struct page_table_entry {
   bool present : 1;
   bool rw : 1;
   bool user : 1;
   bool accessed : 1;
   bool dirty : 1;
   uint32_t unused : 7;
   uint32_t frame : 20;
} __attribute__((packed));

struct page_table {
    struct page_table_entry entries[PAGE_PER_TABLE];
}__attribute__((packed));

struct page_directory_entry {
    bool present : 1;
    bool rw : 1;
    bool user : 1;
    bool accessed : 1;
    bool dirty : 1;
    uint32_t unused : 7;
    uint32_t frame : 20;
}__attribute__((packed));

struct page_directory {
    struct page_directory_entry entries[PAGE_PER_TABLE];
}__attribute__((packed));

void map_page(uintptr_t physaddr, uintptr_t virtualaddr, bool writable, bool kernel) {
    unsigned long ptindex = (unsigned long)virtualaddr >> 22;
    unsigned long pdindex = (unsigned long)(virtualaddr >> 12) & 0x3ff;
    struct page_directory *page_directory = (struct page_directory*)(0xFFFFF000);
    if (!(page_directory->entries[pdindex].present)) {
        struct page_table *table = (struct page_table*)allocate_pmm();
        memset(&page_directory->entries[pdindex],0,sizeof(struct page_directory));
        if (table == NULL) return;
        page_directory->entries[pdindex].present = true;
        page_directory->entries[pdindex].rw = writable ? 1 : 0;
        page_directory->entries[pdindex].user = kernel ? 0 : 1;
        page_directory->entries[pdindex].frame = (uintptr_t)table >> 12;
    }
    struct page_table_entry *page_table = ((struct page_table_entry *)0xFFC00000) + (0x1000 * pdindex);
    if (page_table[ptindex].present) {
        kprintf("Mapped page\n");
        return;
    }
    page_table[ptindex].present = true;
    page_table[ptindex].rw = writable ? 1 : 0;
    page_table[ptindex].user = kernel ? 0 : 1;
    page_table[ptindex].frame = (uint32_t)physaddr / PAGE_SIZE;
    flush_tlb(virtualaddr);
}

void unmap_page(uintptr_t physaddr, uintptr_t virtualaddr) {

}

// void *get_physaddr(uintptr_t virtualaddr) {
//     unsigned long ptindex = (unsigned long)virtualaddr >> 22;
//     unsigned long pdindex = (unsigned long)(virtualaddr >> 12) & 0x3ff;
// }

void initialize_vmm() {
    kprintf("Initialize Paging\n");

}