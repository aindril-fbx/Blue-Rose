#ifndef CLOCK_H
#define CLOCK_H

#include <U8g2lib.h>
#include <Preferences.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

enum ClockMode
{
    TIMER,
    TIMER_SETTING,
    STOPWATCH,
    POMODORO,
    POMODORO_SETTING,
    NONE
};

extern ClockMode clockMode;
void clockFunc(int resetState = 0);

#endif