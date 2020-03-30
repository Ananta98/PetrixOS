#include <lib/types.h>
#include <drivers/console.h>

void kprintf_unsigned(uint32_t value) {
    int multiple_of_ten, decimal_number;
    multiple_of_ten = 1;
    while ((value / multiple_of_ten) >= 10)
        multiple_of_ten *= 10;
    while (value != 0) {
        decimal_number = value / multiple_of_ten;
        console_putc('0' + decimal_number);
        value = value - (decimal_number * multiple_of_ten);
        multiple_of_ten /= 10;
    }
}

void kprintf_int(int32_t value) {
    if (value == 0) {
        console_putc('0');
        return;
    } else if (value < 0) {
        console_putc('-');
        value -= value;
    }
    kprintf_unsigned(value);
}

void kprintf_hex(int32_t value) {
    for (int j = 28; j >= 0; j = j - 4) {
        uint8_t current = (value >> j) & 0x0F;
        if (current < 10) 
            console_putc('0' + current);
        else
            console_putc('a' + current - 10);
    }
}

void kprintf(const char *format, ...) {
    va_list arguments;
    va_start(arguments,format);
    while (*format != '\0') {
        if (*format != '%') {
            console_putc(*format);
        } else {
            format++;
            if (*format != '\0') {
                if (*format == 'u') {
                    uint32_t unsigned_number = va_arg(arguments,uint32_t);
                    kprintf_unsigned(unsigned_number);
                } else if (*format == 'd') {
                    int32_t integer_number = va_arg(arguments,uint32_t);
                    kprintf_int(integer_number);
                } else if (*format == 'c') {
                    console_putc(*format);
                } else if (*format == 'x') {
                    uint32_t unsigned_number = va_arg(arguments,uint32_t);
                    kprintf_hex(unsigned_number);
                } else if (*format == 's') {
                    char *string = va_arg(arguments,char *);
                    console_write(string);
                } else {
                    console_putc(*format);
                }
            } else {
                return;
            }
        }
        format++;
    }
    va_end(arguments);
}