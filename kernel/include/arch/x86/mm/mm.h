#include <lib/types.h>

extern uintptr_t phys_start;
extern uintptr_t phys_end;

extern uintptr_t virt_start;
extern uintptr_t virt_end;

#define PHYS_START ((uintptr_t)&phys_start)
#define PHYS_END   ((uintptr_t)&phys_end)
#define VIRT_START ((uintptr_t)&virt_start)
#define VIRT_END   ((uintptr_t)&virt_end)

#define PHYS_SIZE PHYS_END - PHYS_START
#define VIRT_SIZE VIRT_END - VIRT_START

#define PHYS_BLOCKS_PER_BYTE 8
#define PHYS_BLOCK_SIZE      4096

#define KMALLOC_START  0x100000
#define KMALLOC_LENGTH 0x100000

