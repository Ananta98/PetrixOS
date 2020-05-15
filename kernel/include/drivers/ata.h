#include <lib/types.h>

#define IRQ_PRIMARY 14
#define IRQ_SECONDARY 15

#define PORT_BASE_PRIMARY 0x1F0
#define PORT_BASE_SECONDARY 0x170

#define SECTOR_COUNT 0x1F2
#define LBA_LOW 0x1F3
#define LBA_MID 0x1F4
#define LBA_HIGH 0x1F5

#define DEVICE_PORT 0x6
#define COMMAND_PORT 0x7
#define ERROR_PORT 0x01
#define CONTROL_PORT 0x206
#define ATA_REG_ALTSTATUS  0x0C

#define ERR 0x01
#define DRQ 0x08
#define BSY 0x80

void ata_init();
void ata_poll(uint16_t port_base);
void delay_400ns(uint16_t port_base);
void flush(uint16_t port_base,bool master);
void identify(bool master,uint16_t port_base);
void read_ata(uint16_t port_base,uint32_t sector_num, uint8_t *buffer, bool master);
void write_ata(uint16_t port_base,uint32_t sector_num, uint8_t *buffer, bool master);