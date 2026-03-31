#ifndef GAMES_H
#define GAMES_H

#include <U8g2lib.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

void gamesMenu(int reset = 0);
void cleanUpGames();

#endif