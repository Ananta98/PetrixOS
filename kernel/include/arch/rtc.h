#include <lib/types.h>

void rtc_handler();
void rtc_fetch_time();
void initialize_RTC();
uint8_t read_cmos_rtc(uint16_t address);
void write_cmos_rtc(uint8_t value,uint16_t address);