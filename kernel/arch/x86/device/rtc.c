#include <arch/x86/cpu/io.h>
#include <arch/x86/device/rtc.h>
#include <arch/x86/cpu/interrupt.h>

struct rtc_time {
    uint8_t seconds;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} __attribute__((packed));

#define bcd_to_binary(bcd) ((bcd / 16) * 10) + (bcd & 0x0F)

#define RTC_BASE 0x80
#define RTC_SECOND RTC_BASE + 0
#define RTC_MINUTE RTC_BASE + 2
#define RTC_HOUR RTC_BASE + 4
#define RTC_DAY RTC_BASE + 7
#define RTC_MONTH RTC_BASE + 8
#define RTC_YEAR RTC_BASE + 9

static struct rtc_time rtc_time_cached;

uint8_t read_cmos_rtc(uint16_t address) {
    outb(0x70,address);
    return inb(0x71);
}

void write_cmos_rtc(uint8_t value,uint16_t address) {
    outb(0x70,address);
    outb(0x71,value);
}

void rtc_fetch_time() {
    struct rtc_time t;
    bool addpm = false;
    do {
        t.seconds = read_cmos_rtc(RTC_SECOND);
        t.minute = read_cmos_rtc(RTC_MINUTE);
        t.hour = read_cmos_rtc(RTC_HOUR);
        t.day = read_cmos_rtc(RTC_DAY);
        t.month = read_cmos_rtc(RTC_MONTH);
        t.year = read_cmos_rtc(RTC_YEAR);
    } while (t.seconds != read_cmos_rtc(0x00));

    if (t.hour & 0x80) {
        t.hour &= 0x7F;
        addpm = true;
    }

    t.seconds = bcd_to_binary(t.seconds);
    t.minute = bcd_to_binary(t.minute);
    t.hour = bcd_to_binary(t.hour);
    if (addpm)
        t.hour += 12;
    t.day = bcd_to_binary(t.day);
    t.month = bcd_to_binary(t.month);
    t.year = bcd_to_binary(t.year);
    
    if (t.year >= 70)
        t.year += 1900;
    else
        t.year += 2000;
        
    rtc_time_cached = t;
}

void rtc_handler() {
    outb(0x70,0x0C);
    rtc_fetch_time();
    // printk("Calendar : %d/%d/%d\n",rtc_time_cached.day,rtc_time_cached.month,rtc_time_cached.year);
    // printk("Time : %d:%d:%d\n",rtc_time_cached.hour,rtc_time_cached.minute,rtc_time_cached.seconds);
}

void rtc_init() {
    outb(0x70,0x8B);
    uint8_t status = inb(0x71);
    outb(0x70,0x8B);
    outb(0x71,status | 0x40);
    irq_install_handler(8,rtc_handler);
    printk("Initialize RTC\n");
}