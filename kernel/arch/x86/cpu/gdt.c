#include <arch/x86/cpu/gdt.h>

struct GDT_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
}__attribute__((packed));

struct GDT_ptr {
    uint16_t limit;
    uintptr_t base;
}__attribute__((packed));

struct GDT_entry gdt_entries[MAX_GDT];
struct GDT_ptr gdt_ptr;

extern void load_gdt(uintptr_t address);

uint32_t number = 0;


void initialize_gdt() {
    uint8_t gran_cs = GDT_FLAG_32_BIT | GDT_FLAG_4K_GRAN;
    uint8_t gran_ds = gran_cs;
    uint32_t limit = 0xFFFFFFFF;

    #ifdef CONFIG_ARCH_X86_64
	    gran_cs = GDT_FLAG_64_BIT;
	    gran_ds = 0;
	    limit = 0;
    #endif

    gdt_ptr.limit = (sizeof(struct GDT_entry) * MAX_GDT) - 1;
    gdt_ptr.base = (uintptr_t)&gdt_entries;
    setup_gdt_gate(number++,0,0,0,0);
    setup_gdt_gate(number++,0,limit,GDT_FLAG_RING0 | GDT_FLAG_SEGMENT | GDT_FLAG_CODESEG | GDT_FLAG_PRESENT, gran_cs);
    setup_gdt_gate(number++,0,limit,GDT_FLAG_RING0 | GDT_FLAG_SEGMENT | GDT_FLAG_CODESEG | GDT_FLAG_PRESENT, gran_ds);
    setup_gdt_gate(number++,0,limit,GDT_FLAG_RING3 | GDT_FLAG_SEGMENT | GDT_FLAG_CODESEG | GDT_FLAG_PRESENT, GDT_FLAG_32_BIT | GDT_FLAG_4K_GRAN);
    setup_gdt_gate(number++,0,limit,GDT_FLAG_RING0 | GDT_FLAG_SEGMENT | GDT_FLAG_CODESEG | GDT_FLAG_PRESENT, gran_ds);
    asm volatile("lgdt %0" :: "m"(gdt_ptr));
}

void setup_gdt_gate(uint8_t number,uint64_t base, uint64_t limit,uint8_t access,uint8_t granularity) {
    gdt_entries[number].base_low = (base & 0xFFFF);
    gdt_entries[number].base_middle = (base >> 16) & 0xFF;
    gdt_entries[number].base_high = (base >> 24) & 0xFF;

    gdt_entries[number].limit_low = (limit & 0xFFFF);
    gdt_entries[number].granularity = (limit >> 16) & 0x0F;

    gdt_entries[number].granularity |= (granularity & 0xF0);
    gdt_entries[number].access = access;
}