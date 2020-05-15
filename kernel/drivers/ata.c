#include <lib/kstdio.h>
#include <drivers/ata.h>
#include <arch/io.h>
#include <arch/pic.h>
#include <arch/interrupt.h>

void identify(bool master,uint16_t port_base) {
    outb(port_base + DEVICE_PORT,master ? 0xA0 : 0xB0);
    outb(port_base + CONTROL_PORT, 0);
    outb(port_base + DEVICE_PORT, 0xA0);
    uint8_t status = inb(port_base + COMMAND_PORT);
    if (status == 0xFF)
        return;
    outb(port_base + DEVICE_PORT,master ? 0xA0 : 0xB0);
    outb(port_base + SECTOR_COUNT,0);
    outb(LBA_LOW,0);
    outb(LBA_MID,0);
    outb(LBA_HIGH,0);
    outb(COMMAND_PORT,0xEC);
    ata_poll(port_base);
}

void delay_400ns(uint16_t port_base) {
    inb(port_base + ATA_REG_ALTSTATUS);
    inb(port_base + ATA_REG_ALTSTATUS);
    inb(port_base + ATA_REG_ALTSTATUS);
    inb(port_base + ATA_REG_ALTSTATUS);
}

void ata_poll(uint16_t port_base) {
    delay_400ns(port_base);
    uint8_t status = inb(port_base + COMMAND_PORT);
    if (status == 0x00)
        return;
    while (status & BSY)
        status = inb(port_base + COMMAND_PORT);
    if (status & ERR)
        kprintf("ERROR\n");
    while (status & DRQ)
        status = inb(port_base + COMMAND_PORT);
}

void read_ata(uint16_t port_base,uint32_t sector_num, uint8_t *buffer, bool master) {
    outb(port_base + DEVICE_PORT,master ? 0xE0 : 0xF0  | ((sector_num & 0x0F000000) >> 24));
    outb(port_base + ERROR_PORT,0);
    outb(SECTOR_COUNT,1);
    outb(LBA_LOW,(uint8_t)(sector_num & 0x000000FF));
    outb(LBA_MID,(uint8_t)(sector_num & 0x0000FF00) >> 8);
    outb(LBA_HIGH,(uint8_t)(sector_num & 0x00FF0000) >> 16);
    ata_poll(port_base);
    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(port_base);
        *(uint16_t*)(buffer + i * 2) = data;
    }
    delay_400ns(port_base);
}


void write_ata(uint16_t port_base,uint32_t sector_num, uint8_t *buffer, bool master) {
    outb(port_base + DEVICE_PORT,master ? 0xE0 : 0xF0  | ((sector_num & 0x0F000000) >> 24));
    outb(SECTOR_COUNT,1);
    outb(LBA_LOW,(uint8_t)sector_num & 0x000000FF );
    outb(LBA_MID,(uint8_t)(sector_num & 0x0000FF00) >> 8);
    outb(LBA_HIGH,(uint8_t)(sector_num & 0x00FF0000) >> 16);
    outb(port_base + COMMAND_PORT, 0x30);
    ata_poll(port_base);
    for (int i = 0; i < 256; i++) {
        uint16_t data = buffer[i];
        outb(port_base,data);
    }
    delay_400ns(port_base);
    flush(port_base,master);
}

void flush(uint16_t port_base,bool master) {
    outb(port_base + DEVICE_PORT,master ? 0xE0 : 0xF0);
    outb(port_base + COMMAND_PORT,0xE7);
    ata_poll(port_base);
}

void handler_primary() {
    pic_ack(IRQ_PRIMARY);
}

void handler_secondary() {
    pic_ack(IRQ_SECONDARY);
}

void init_primary_ata() {
    identify(true,PORT_BASE_PRIMARY);
    identify(false,PORT_BASE_PRIMARY);
}

void init_secondary_ata() {
    identify(true,PORT_BASE_SECONDARY);
    identify(false,PORT_BASE_SECONDARY);
}

void ata_init() {
    init_primary_ata();
    init_secondary_ata();
    irq_install_handler(IRQ_PRIMARY,handler_primary);
    irq_install_handler(IRQ_SECONDARY,handler_secondary);
}