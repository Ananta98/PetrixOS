#include <lib/types.h>

void initialize_pic();
void pic_ack(uint8_t irq);
void enable_pic(uint8_t irq);
void disable_pic(uint8_t irq);