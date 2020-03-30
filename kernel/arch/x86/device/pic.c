#include <arch/x86/cpu/io.h>
#include <arch/x86/device/pic.h>

#define PIC1 0x20
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC1_OFFSET 0x20

#define PIC2 0xA0
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)
#define PIC2_OFFSET 0x28

#define PIC_EOI 0x20

#define ICW1_ICW4 0x01
#define ICW1_INIT 0x10

void initialize_pic() {
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, PIC1_OFFSET);
    outb(PIC2_DATA, PIC2_OFFSET);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
}

void pic_ack(uint8_t irq) {
    if (irq >= 8)
        outb(PIC2_COMMAND,PIC_EOI);
    outb(PIC1_COMMAND,PIC_EOI);
}

void disable_pic(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) | (1 << irq);
    outb(port,value);
}

void enable_pic(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port,value);
}