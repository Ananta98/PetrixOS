#include <lib/kstdio.h>
#include <arch/x86/cpu/io.h>
#include <arch/x86/device/pic.h>
#include <arch/x86/cpu/interrupt.h>

uint32_t timer_ticks = 0;

void timer_callback() {
    timer_ticks++;
}

void initialize_timer(uint8_t frequency) {
    irq_install_handler(0,timer_callback);
    uint32_t divisor = 1193180 / frequency;
    outb(0x43,0x36);
    outb(0x40,divisor & 0xFF);
    outb(0x40,divisor >> 8);
}
