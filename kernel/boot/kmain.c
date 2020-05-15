#include <drivers/console.h>
#include <arch/gdt.h>
#include <arch/interrupt.h>

#include <arch/memory.h>
#include <arch/pmm.h>
#include <arch/vmm.h>
#include <arch/rtc.h>
#include <arch/timer.h>
// #include <mm/kmalloc.h>
#include <lib/kstdio.h>


void kmain(multiboot_info_t *multiboot_info) {
    initialize_console();
    initialize_gdt();
    initialize_idt();
    initialize_interrupt();
    initialize_exception();
    disable_interrupt();
    initialize_timer(50);
    initialize_RTC();
    enable_interrupt();
    initialize_pmm(multiboot_info);    
    initialize_vmm();
    // initialize_kmalloc(VIRT_START,4 * 1024 * 1024);
}