#include <lib/types.h>

#define PRIMARY_ATA_IRQ   14
#define SECONDARY_ATA_IRQ 15

#define BASE_PRIMARY_ATA   0x1F0 
#define BASE_SECONDARY_ATA 0x170

#define DATA             0
#define ERROR            1
#define SECTOR_COUNT     2
#define LBA_LOW          3
#define LBA_MID          4
#define LBA_HIGH         5
#define DRIVE_SELECT     6
#define COMMAND_STATUS   7

#define IDENTIFY     0xEC
#define SEL_MASTER   0xA0
#define SEL_SLAVE    0xB0
#define READ_SECTORS 0x20

#define ERR 0x01
#define DRQ 0x08
#define BSY 0x80

void initialize_ata();
void check_drive(uint16_t port_base, uint8_t master_or_slave);