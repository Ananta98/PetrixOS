#include <lib/types.h>

#define PTE_PRESENT       1
#define PTE_WRITABLE      2
#define PTE_USER          4
#define PTE_WRITETHOUGH   8
#define PTE_NOT_CACHEABLE 0x10
#define PTE_ACCESSED      0x20
#define PTE_DIRTY         0x40
#define PTE_PAT           0x80
#define PTE_CPU_GLOBAL    0x100
#define PTE_LV4_GLOBAL    0x200
#define PTE_FRAME         0x7FFFF000

#define PDE_PRESENT    1
#define PDE_WRITABLE   2
#define PDE_USER       4
#define PDE_PWT        8
#define PDE_PCD        0x10
#define PDE_ACCESSED   0x20
#define PDE_DIRTY      0x40
#define PDE_4MB        0x80
#define PDE_CPU_GLOBAL 0x100
#define PDE_LV4_GLOBAL 0x200
#define PDE_FRAME      0x7FFFF000

#define PAGE_DIRECTORY_INDEX(x)      (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x)          (((x) >> 12) & 0x3FF)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xFFF)

#define PAGE_SIZE                         4096
#define PAGE_PER_TABLE                    1024
#define PAGE_PER_DIR                      1024
#define PAGE_TABLE_ADDRESS_SPACE_SIZE     0x400000
#define PAGE_DIRECTORY_ADDRESS_SPACE_SIZE 0x10000000

void initialize_vmm();