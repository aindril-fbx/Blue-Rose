#ifndef SETTINGS_H
#define SETTINGS_H

#include <U8g2lib.h>
#include <Arduino.h>
#include <buttonBehav.h>
#include <Preferences.h>
#include <NimBLEDevice.h>
#include <BleKeyboard.h>


#if CONFIG_IDF_TARGET_ESP32
    // Classic ESP32
    extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
    
#elif CONFIG_IDF_TARGET_ESP32C3
    // ESP32-C3
    extern U8G2_SH1106_128X64_NONAME_F_HW_I2C  u8g2;
#endif

extern int sleepBarValue;
extern int brightnessBarValue;
void settingsPage();

#endif