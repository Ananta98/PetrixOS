#include <lib/types.h>

typedef struct registers {
    uint32_t fs,es,gs,ds;
    uint32_t edi,esi,ebp,esp,ebx,edx,ecx,eax;
    uint32_t int_no,err_code;
    uint32_t eip,cs,eflags,useresp,ss;
} register_t;

typedef void (*interrupt_handler)(register_t *r);
typedef void (*exception_handler)(register_t *r);

void enable_interrupt();
void disable_interrupt();

void initialize_idt();
void initialize_exception();
void initialize_interrupt();
void idt_set_gate(uint8_t num,uint32_t base,uint16_t sel,uint8_t flags);

void irq_handler(register_t *r);
void irq_uninstall_handler(int irq_number);
void irq_install_handler(int irq_number,interrupt_handler handler);

void isr_handler(register_t *r);
void isr_uninstall_handler(int isr_number);
void isr_install_handler(int isr_number, exception_handler handler);