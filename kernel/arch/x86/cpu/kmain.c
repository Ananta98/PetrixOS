#include <drivers/console.h>
#include <arch/x86/cpu/gdt.h>
#include <arch/x86/cpu/interrupt.h>
#include <arch/x86/device/timer.h>
#include <arch/x86/mm/pmm.h>
#include <arch/x86/mm/vmm.h>

#ifndef CONFIG_ARCH_X86_32
    #define CONFIG_ARCH_X86_32
#endif

void kmain(multiboot_info_t *multiboot_info) {
    initialize_console();
    initialize_gdt();
    initialize_idt();
    initialize_interrupt();
    initialize_exception();
    disable_interrupt();
    initialize_timer(50);
    enable_interrupt();
    initialize_pmm(multiboot_info);
    initialize_vmm();
}