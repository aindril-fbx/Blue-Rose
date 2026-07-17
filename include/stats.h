#ifndef STATS_H
#define STATS_H

#include <U8g2lib.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

extern bool gotInfo;

void showStats();
void syncTimeAsync();
extern TaskHandle_t wifiTaskHandle;

#endif