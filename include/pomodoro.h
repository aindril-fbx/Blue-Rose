#ifndef POMODORO_H
#define POMODORO_H

#include <U8g2lib.h>
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

extern int progress;
void Pomodoro(int pomodoroWORKdelay, int pomodoroBREAKdelay, int cycles);
void resetPomodoro();

#endif