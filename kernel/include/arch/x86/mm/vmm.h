#include <lib/types.h>

#define PAGE_P 0x1
#define PAGE_R 0x0
#define PAGE_W 0x2
#define PAGE_S 0x0
#define PAGE_U 0x4

#define PAGE_PS  0x00000080 
#define PAGE_MASK 0xFFFFF000

void initialize_vmm();
void unmap_page(uintptr_t virtaddr);
void map_page(uintptr_t virtaddr,uintptr_t physaddr);
void *get_physaddr(uintptr_t virtaddr,uintptr_t physaddr);