#include <lib/types.h>

void rtc_init();
void rtc_handler();
void rtc_fetch_time();
uint8_t read_cmos_rtc(uint16_t address);
void write_cmos_rtc(uint8_t value,uint16_t address);