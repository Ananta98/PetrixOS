#include <arch/x86/cpu/io.h>
#include <drivers/console.h>

static int posX = 0;
static int posY = 0;
static uint16_t *video_memory =  (uint16_t *)0xB8000;

void console_write(char *str) {
    int index;
    for (index = 0; str[index] != '\0'; index++)
        console_putc(str[index]);
}

void console_move() {
    uint8_t cursor_position = (posY * 80) + posX;
    outb(0x3D4,14);
    outb(0x3D5,cursor_position >> 8);
    outb(0x3D5,15);
    outb(0x3D5,cursor_position);
}

void console_scroll() {
    uint16_t attribute = (0 << 4) | (15 & 0x0F);
    if (posY >= 25) {
        for (int j = 0 * 80; j <= 24 * 80; j++) 
            video_memory[j] = video_memory[j+80];
        for (int j = 24 * 80; j <= 25 * 80; j++)
            video_memory[j] = 0x20 | (attribute << 8);
        posY = 24;
    }
}

void console_clear() {
    uint16_t attribute = (0 << 4) | (15 & 0x0F);
    for (int index = 0; index < 80 * 25; index++) {
        uint16_t blank = 0x20 | (attribute << 8);
        video_memory[index] = blank; 
    }
    posX = posY = 0;
    console_move();
}

void console_putc(char c) {
    volatile uint16_t *where;
    uint16_t attribute = (0 << 4) | (15 & 0x0F);
    if (c >= ' ') {      
        where = video_memory + (posY * 80 + posX);
        *where = c | (attribute << 8);
        posX++;
    } else if (c == '\n') {
        posX = 0;
        posY++;
    } else if (c == '\r') {
        posX = 0;
    } else if (c == 0x09){
        posX = (posX+8) & ~(8-1);
    } else if (c == 0x08 && posX) {
        posX--;
    }
    if (posX >= 80) {
        posX = 0;
        posY++;
    }
    console_scroll();
    console_move();   
}

void find_cursor() {
    uint16_t position = 0;
    outb(0x3D4,0x0F);
    position |= inb(0x3D5);
    outb(0x3D4,0x0E);
    position |= ((uint16_t)inb(0x3D5)) << 8;
    posX = position % 80;
    posY = position / 80;
}

void initialize_console() {
    console_clear();
    console_move();
}