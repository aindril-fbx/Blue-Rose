#ifndef TIMER_H
#define TIMER_H
                                    
#include <U8g2lib.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

void Timer();
void startTimer(unsigned long timerDuration, int repeats = 0);
void pauseTimer();
void resetTimer();


#endif