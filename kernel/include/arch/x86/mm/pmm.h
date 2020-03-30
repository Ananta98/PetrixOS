#include <lib/types.h>
#include <arch/x86/cpu/multiboot.h>

void *allocate_pmm();
void deallocate_pmm(uintptr_t address);
void *allocate_more_pmm(size_t count_blocks);
void initialize_pmm(multiboot_info_t *multiboot_info);
