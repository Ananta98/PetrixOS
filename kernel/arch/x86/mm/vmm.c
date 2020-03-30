#include <lib/kstdio.h>
#include <lib/kstdlib.h>
#include <arch/x86/mm/mm.h>
#include <arch/x86/mm/pmm.h>
#include <arch/x86/mm/vmm.h>

extern void paging_enable();
extern void paging_disable();
extern void flush_tlb(uintptr_t);
extern void switch_page_directory(uintptr_t);

typedef struct {
    uintptr_t page_table_entries[PAGE_PER_TABLE];
} page_table;

typedef struct {
    uintptr_t page_directory_entries[PAGE_PER_DIR];
} page_directory;

page_directory *current_page_directory = NULL;

uintptr_t PTE_pfn(uintptr_t entry) {
    return entry & PTE_FRAME;
}

void PTE_add_attribute(uintptr_t *entry,uintptr_t attribute) {
    *entry |= attribute;
}

void PTE_delete_attribute(uintptr_t *entry,uintptr_t attribute) {
    *entry &= ~attribute;
}

bool PTE_is_present(uintptr_t entry) {
    return entry & PTE_PRESENT;
}

void PTE_set_frame(uintptr_t *entry, uintptr_t address) {
    *entry = (*entry & ~PTE_FRAME) | address;
}

void PDE_add_attribute(uintptr_t *entry, uintptr_t attribute) {
    *entry |= attribute;
}

void PDE_delete_attribute(uintptr_t *entry, uintptr_t attribute) {
    *entry &= ~attribute;
}

void PDE_set_frame(uintptr_t *entry, uintptr_t address) {
    *entry = (*entry & ~PTE_FRAME) | address;
}

bool PDE_is_present(uintptr_t entry) {
    return entry & PDE_PRESENT;
}

bool PDE_is_user(uintptr_t entry) {
    return entry & PDE_USER;
}

bool PDE_is_writable(uintptr_t entry) {
    return entry & PDE_WRITABLE;
}

bool PDE_is_4MB(uintptr_t entry) {
    return entry & PDE_4MB;
}

uintptr_t PDE_pfn(uintptr_t entry) {
    return entry & PDE_FRAME;
}

void allocate_page(uintptr_t *entry) {
    void *physical_address = allocate_pmm();
    if (physical_address == NULL) {
        kprintf("Cann't allocate page memory\n");
        return;
    }
    PTE_set_frame(entry,PTE_PRESENT);
    PTE_add_attribute(entry,(uintptr_t)physical_address);
}

void free_page(uintptr_t *entry) {
    void *physical_address = (void*)PTE_pfn(*entry);
    if (physical_address == NULL)
        deallocate_pmm((uintptr_t)physical_address);
    PTE_delete_attribute(entry,PTE_PRESENT);
}

uintptr_t *page_table_lookup_entry(page_table *entry, uintptr_t virtual_address) {
    if (entry != NULL)
        return &entry->page_table_entries[PAGE_TABLE_INDEX(virtual_address)];
    return 0;
}

uintptr_t *page_directory_lookup_entry(page_directory *entry, uintptr_t virtual_address) {
    if (entry != NULL)
        return &entry->page_directory_entries[PAGE_TABLE_INDEX(virtual_address)];
    return 0;
}

void map_page(void *physical_address, void *virtual_address) {
    page_directory *page_directory = current_page_directory;
    uintptr_t *entry = &page_directory->page_directory_entries[PAGE_DIRECTORY_INDEX((uintptr_t)virtual_address)];
    if ((*entry & PTE_PRESENT) != PTE_PRESENT) {
        page_table *current_table = (page_table*)allocate_pmm();
        if (current_table == NULL) 
            return;
        memset(current_table,0,sizeof(page_table));
        uintptr_t *entry = &page_directory->page_directory_entries[PAGE_DIRECTORY_INDEX((uintptr_t)virtual_address)];
        PDE_add_attribute(entry,PDE_PRESENT);
        PDE_add_attribute(entry,PDE_WRITABLE);
        PDE_set_frame(entry,(uintptr_t)current_table);
    }
    page_table *table = (page_table*)PAGE_GET_PHYSICAL_ADDRESS(entry);
    uintptr_t  *page  = &table->page_table_entries[PAGE_TABLE_INDEX((uintptr_t)virtual_address)];
    PTE_set_frame(page,(uintptr_t)physical_address);
    PTE_add_attribute(page,PTE_PRESENT);
}


void initialize_vmm() {
    page_table *first_table = (page_table*)allocate_pmm();
    if (first_table == NULL) {
        kprintf("PMM out of memory !!! Cann't allocate for first table\n");
        return;
    }

    page_table *second_table = (page_table*)allocate_pmm();
    if (second_table == NULL) {
        kprintf("PMM out of memory !!! Cann't allocate for second table\n");
        return;
    }

    memset(first_table,0,sizeof(page_table));
    memset(second_table,0,sizeof(page_table));

    for (int i = 0, frame = 0x100000, virtual = 0x00000000; i < 1024; i++, frame += 4096, virtual += 4096) {
        uintptr_t page = 0;
        PTE_add_attribute(&page, PTE_PRESENT);
        PTE_set_frame(&page, frame);
        first_table->page_table_entries[PAGE_TABLE_INDEX(virtual)] = page;
    }

    for (int i = 0, frame = 0x0, virtual = 0xC0000000; i < 1024; i++, frame += 4096, virtual += 4096) {
        uintptr_t page = 0;
        PTE_add_attribute(&page, PTE_PRESENT);
        PTE_set_frame(&page, frame);
        second_table->page_table_entries[PAGE_TABLE_INDEX(virtual)] = page;
    }

    page_directory *page_dir = (page_directory*)allocate_more_pmm(3);
    memset(page_dir,0, sizeof(page_directory));

    uintptr_t *first_page_directory_entry = &page_dir->page_directory_entries[PAGE_DIRECTORY_INDEX(0xC0000000)];
    PDE_add_attribute(first_page_directory_entry, PDE_PRESENT);
    PDE_add_attribute(first_page_directory_entry,PDE_WRITABLE);
    PDE_set_frame(first_page_directory_entry,(uintptr_t)first_table);

    uintptr_t *second_page_directory_entry = &page_dir->page_directory_entries[PAGE_DIRECTORY_INDEX(0x00000000)];
    PDE_add_attribute(second_page_directory_entry, PDE_PRESENT);
    PDE_add_attribute(second_page_directory_entry,PDE_WRITABLE);
    PDE_set_frame(second_page_directory_entry,(uintptr_t)second_table);

    current_page_directory = page_dir;

    switch_page_directory((uintptr_t)&page_dir->page_directory_entries);
    paging_enable();

    kprintf("Virtual Memory Initialized\n");
}