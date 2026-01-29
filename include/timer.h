#ifndef TIMER_H
#define TIMER_H
                                    
#include <U8g2lib.h>

#if CONFIG_IDF_TARGET_ESP32
    // Classic ESP32
    extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
                                                                           
#elif CONFIG_IDF_TARGET_ESP32C3
    // ESP32-C3
    extern U8G2_SH1106_128X64_NONAME_F_HW_I2C  u8g2;
#endif

void Timer();
void startTimer(unsigned long timerDuration, int repeats = 0);
void pauseTimer();
void resetTimer();


#endif