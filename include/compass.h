#ifndef COMPASS_H
#define COMPASS_H

#include <U8g2lib.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

extern int autoCompass;
void compass(int offset, int autoRotate);

#endif
