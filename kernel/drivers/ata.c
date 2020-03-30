#include <lib/kstdio.h>
#include <drivers/ata.h>
#include <arch/x86/cpu/io.h>
#include <arch/x86/device/pic.h>
#include <arch/x86/cpu/interrupt.h>

static uint8_t read_status(uint16_t port_base) {
    inb(port_base + COMMAND_STATUS);
    inb(port_base + COMMAND_STATUS);
    inb(port_base + COMMAND_STATUS);
    inb(port_base + COMMAND_STATUS);
    return inb(port_base + COMMAND_STATUS);
}

static void poll(uint16_t port_base) {
    uint8_t status;
    do
        status = read_status(port_base);
    while ((status & BSY) != 0);
}

void check_drive(uint16_t port_base, uint8_t master_or_slave) {
    outb(port_base + SECTOR_COUNT, 0);
    outb(port_base + LBA_LOW,      0);
    outb(port_base + LBA_MID,      0);
    outb(port_base + LBA_HIGH,     0);
    
    outb(port_base + COMMAND_STATUS, IDENTIFY);
    uint8_t status = read_status(port_base);

    if (status == 0) {
        kprintf("ATA not found\n");
        return;
    }

    while ((status & BSY) != 0)
        status = read_status(port_base);
    
    while((status & DRQ) == 0 && (status & BSY) == 0)
        status = read_status(port_base);
    
    if ((status & ERR) != 0) {
        kprintf("Something wrong with ATA drive\n");
        return;
    }
}

void ata_read_sector(uint16_t port_base, uint8_t master_or_slave, uint32_t lba, uint8_t sector_count, uint16_t buffer[]) {
    outb(port_base + DRIVE_SELECT, master_or_slave | ((lba >> 24) & 0xF0000000));
    outb(port_base + SECTOR_COUNT, (uint8_t)sector_count);
    outb(port_base + LBA_LOW, (uint8_t)lba & 0xFF);
    outb(port_base + LBA_MID, (uint8_t)(lba >> 8) & 0xFF);
    outb(port_base + LBA_HIGH, (uint8_t)(lba >> 16) & 0xFF);
    outb(port_base + COMMAND_STATUS, READ_SECTORS);
    for (size_t i = 0; i < sector_count; i++) {
        poll(port_base);
        uint16_t data = inw(port_base);
        *(uint16_t*)(buffer + i * 2) = data;
    }
}


void primary_ata_handler() {
    pic_ack(SECONDARY_ATA_IRQ);
}

void secondary_ata_handler() {
    pic_ack(PRIMARY_ATA_IRQ);
}

void initialize_ata() {
    uint8_t primary_status = inb(BASE_PRIMARY_ATA + COMMAND_STATUS);
    uint8_t secondary_status = inb(BASE_SECONDARY_ATA + COMMAND_STATUS);

    if ((primary_status == 0xFF) && (secondary_status == 0xFF)) {
        kprintf("Error ATA What's Going on\n");
        return;
    }

    check_drive(BASE_PRIMARY_ATA,  SEL_MASTER);
    check_drive(BASE_PRIMARY_ATA,   SEL_SLAVE);
    check_drive(BASE_SECONDARY_ATA, SEL_MASTER);
    check_drive(BASE_SECONDARY_ATA, SEL_SLAVE);

    irq_install_handler(PRIMARY_ATA_IRQ,primary_ata_handler);
    irq_install_handler(SECONDARY_ATA_IRQ,secondary_ata_handler);

}