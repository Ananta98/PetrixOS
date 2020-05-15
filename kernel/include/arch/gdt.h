#include <lib/types.h>

#define MAX_GDT 6

#define GDT_FLAG_DATASEG	0x02
#define GDT_FLAG_CODESEG	0x0a
#define GDT_FLAG_TSS		0x09
#define GDT_FLAG_TSS_BUSY	0x02

#define GDT_FLAG_SEGMENT	0x10
#define GDT_FLAG_RING0		0x00
#define GDT_FLAG_RING1		0x20
#define GDT_FLAG_RING2		0x40
#define GDT_FLAG_RING3		0x60
#define GDT_FLAG_PRESENT    0x80
#define GDT_FLAG_ACCESSED   0x01

#define GDT_FLAG_4K_GRAN	0x80

#define GDT_FLAG_16_BIT		0x00
#define GDT_FLAG_32_BIT		0x40
#define GDT_FLAG_64_BIT		0x20

void initialize_gdt();
void setup_gdt_gate(uint8_t  number,uint64_t base, uint64_t limit,uint8_t access,uint8_t granularity);