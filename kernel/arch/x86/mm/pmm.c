#include <arch/x86/mm/mm.h>
#include <arch/x86/mm/pmm.h>
#include <lib/kstdio.h>
#include <lib/kstdlib.h>
#include <arch/x86/cpu/multiboot.h>

#define INDEX_BIT(address) (address / 32)
#define OFFSET_BIT(address) (address % 32)

uintptr_t *pmm_bitmap = NULL;
uint32_t used_blocks = 0;
uint32_t total_blocks = 0;
 
void set_bit(uintptr_t address) {
    size_t index  = INDEX_BIT(address);
    size_t offset = OFFSET_BIT(address);
    pmm_bitmap[index] |= (1 << offset);
}

void unset_bit(uintptr_t address) {
    size_t index  = INDEX_BIT(address);
    size_t offset = OFFSET_BIT(address);
    pmm_bitmap[index] &= ~(1 << offset);
}

uintptr_t first_bit() {
    for (uint32_t i = 0; i < total_blocks / 32;  i++) {
        if (pmm_bitmap[i] != 0xFFFFFFFF) {
            for (uint32_t j = 0; j < 32; j++) {
                int mask = 1 << j;
                if (!(pmm_bitmap[j] & mask))
                    return i * 32 + j;
            }
        }
    }
    return -1;
}

uintptr_t more_first_bit(uint32_t count) {
    uint32_t cur_num_block = -1;
    int starting_block_bit = -1;
    int starting_block = -1;
    
    for (uint32_t i = 0; i < total_blocks; i++) {
        if (pmm_bitmap[i] == 0xFFFFFFFF) {
            cur_num_block = 0;
            continue;    
        } 
        for (int j = 0; j < 32; j++) {
            int mask = 1 << j;
            if (!(pmm_bitmap[i] & mask)) {
                starting_block_bit = 0;
                continue;
            }
            if (!cur_num_block) {
                starting_block = i;
                starting_block_bit = j;
            }
            cur_num_block++;
            if (cur_num_block == count) {
                return (32 * starting_block) + starting_block_bit;
            }    
        }
    }
    return -1;
}

void allocate_region(uintptr_t base, size_t size) {
    int align = base / PHYS_BLOCK_SIZE;
    int blocks = size /PHYS_BLOCK_SIZE;
    for (int j = 0; j < blocks; j++) {
        set_bit(align++);
        used_blocks--;
    }
    set_bit(0);
}

void deallocate_region(uintptr_t base, size_t size) {
    int align = base / PHYS_BLOCK_SIZE;
    int blocks = size / PHYS_BLOCK_SIZE;
    for (int j = 0; j < blocks; j++) {
        unset_bit(align++);
        used_blocks++;
    }
    set_bit(0);
}

void *allocate_pmm() {
    if (total_blocks - used_blocks == 0)
        return NULL;
        
    uintptr_t frame = first_bit();  
    if ((intptr_t)frame == -1)
        return NULL;
    set_bit(frame);
    used_blocks++;
    uintptr_t address = frame * PHYS_BLOCK_SIZE;
    return (void*)address;
}

void deallocate_pmm(uintptr_t address) {
    uintptr_t frame = address / PHYS_BLOCK_SIZE;
    unset_bit(frame);
    used_blocks--;
}

void *allocate_more_pmm(size_t count_blocks){
    if (count_blocks == 0)
        return NULL;
    if (total_blocks - used_blocks == 0)
        return NULL;
    uintptr_t frame = first_bit();  
    if ((intptr_t)frame == -1) 
        return NULL;
    for (uint32_t i = 0; i < count_blocks; i++) {
        set_bit(frame);
        used_blocks--;
    }
    uintptr_t address = frame * PHYS_BLOCK_SIZE;
    return (void*)address;
}

void initialize_pmm(multiboot_info_t *multiboot_info) {
    uint32_t phys_mem_size = multiboot_info->mem_lower + multiboot_info->mem_upper + 1024;
    total_blocks = (phys_mem_size * 1024) / PHYS_BLOCK_SIZE;
    used_blocks = total_blocks;
    pmm_bitmap = (uint32_t*)PHYS_END;
    memset(pmm_bitmap,0xFF,total_blocks / PHYS_BLOCKS_PER_BYTE);
    kprintf("Total Blocks : 0x%x\n",total_blocks);
    for (multiboot_memory_map_t * mmap = (multiboot_memory_map_t *) multiboot_info->mmap_addr;
        (unsigned long) mmap < multiboot_info->mmap_addr + multiboot_info->mmap_length;
        mmap = (multiboot_memory_map_t *) ((unsigned long) mmap + mmap->size + sizeof (mmap->size))) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) 
            allocate_region(mmap->base_addr_high + mmap->base_addr_low,mmap->size);
    }
    set_bit(0);
    uintptr_t kernel_phys_start = (uintptr_t)PHYS_START;
    uintptr_t kernel_phys_end = kernel_phys_start + (total_blocks / PHYS_BLOCKS_PER_BYTE);
    kprintf("Physical Memory already installed. Physical Start : 0x%x Physical End : 0x%x\n",kernel_phys_start,kernel_phys_end);
}