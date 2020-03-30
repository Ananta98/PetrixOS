#include <lib/kstdlib.h>
#include <arch/x86/cpu/interrupt.h>

#define MAX_IDT 256

typedef struct {
    uint16_t base_low;
    uint16_t selector;
    uint8_t always_zero;
    uint8_t flags;
    uint16_t base_high;    
} __attribute__((packed)) idt_entry;

typedef struct  {
    uint16_t limit;
    uintptr_t base;
} __attribute__((packed)) idt_ptr;


idt_entry idt_entries[MAX_IDT];
idt_ptr idt_ptr_t;

extern void load_idt(uintptr_t);

void enable_interrupt() {
    asm("sti");
}

void disable_interrupt() {
    asm("cli");
}

void initialize_idt() {
    idt_ptr_t.limit = (sizeof(idt_entry) * MAX_IDT) - 1;
    idt_ptr_t.base =(uintptr_t)&idt_entries;
    memset(&idt_entries,0,sizeof(idt_entry) * MAX_IDT);
    asm volatile("lidt %0" :: "m"(idt_ptr_t));
}

void idt_set_gate(uint8_t num,uint32_t base,uint16_t sel,uint8_t flags) {
    idt_entries[num].base_low = (base & 0xFFFF);
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;
    idt_entries[num].selector = sel;
    idt_entries[num].always_zero = 0;
    idt_entries[num].flags = flags | 0x60;
}