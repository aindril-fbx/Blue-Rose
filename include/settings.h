#ifndef SETTINGS_H
#define SETTINGS_H

#include <U8g2lib.h>
#include <Arduino.h>
#include <buttonBehav.h>
#include <Preferences.h>
#include <NimBLEDevice.h>
#include <BleKeyboard.h>


extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

extern int sleepBarValue;
extern int brightnessBarValue;
void settingsPage();

#endif