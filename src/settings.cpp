#include <U8g2lib.h>
#include <Arduino.h>
#include <buttonBehav.h>
#include <Preferences.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

extern Preferences prefs;

extern int maxAfkTime;
extern int brightnessLevel;
extern int soundSetting;

int numOfSettingsOptions = 4;
int currentSettingIndex = 0;

int sleepBarValue = 2;
int brightnessBarValue = 255;
int saveValue = 0;

static const unsigned char image_arrow_bits[] U8X8_PROGMEM = {0x01, 0x03, 0x07, 0x03, 0x01};
static const unsigned char image_save_bits[] U8X8_PROGMEM = {0xfe,0xff,0x7f,0x01,0x00,0x80,0x01,0x00,0x80,0x31,0x97,0x8c,0x89,0x94,0x92,0x91,0x94,0x9e,0xa1,0x94,0x82,0x19,0x67,0x8c,0x01,0x00,0x80,0x01,0x00,0x80,0xfe,0xff,0x7f};
static const unsigned char image_download_bits[] U8X8_PROGMEM = {0x28,0x28,0x7e,0x14,0x3f,0x0a,0x0a};
static const unsigned char image_saveSelected_bits[] U8X8_PROGMEM = {0xfe,0xff,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xcf,0x68,0xf3,0x77,0x6b,0xed,0x6f,0x6b,0xe1,0x5f,0x6b,0xfd,0xe7,0x98,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xff,0x7f};

void changeSoundSetting()
{
    if(rightButtonTap() || leftButtonTap()){
        soundSetting = (soundSetting + 1) % 2;
    }
    soundSetting = constrain(soundSetting, 0, 1);
    return;

}

void changeSleepTimeSetting()
{
    if(rightButtonHold()){
        sleepBarValue += 1;
    }
    if(leftButtonHold()){
        sleepBarValue -= 1;
    }
    sleepBarValue = constrain(sleepBarValue, 2, 61);
    return;
    
}

void changeBrightnessSetting()
{
    if(rightButtonHold()){
        brightnessBarValue += 1;
    }
    if(leftButtonHold()){
        brightnessBarValue -= 1;
    }
    brightnessBarValue = constrain(brightnessBarValue, 2, 61);
    return;
}

static const unsigned char image_saved_bits[] U8X8_PROGMEM = {0xfe,0x3f,0x09,0x55,0x09,0x95,0x09,0x97,0x09,0x90,0xf1,0x8f,0x01,0x80,0x01,0x80,0x01,0x80,0xf9,0x9f,0x09,0x90,0xe9,0x97,0x09,0x90,0xeb,0xd7,0x09,0x90,0xfe,0x7f};

void savedScreen(void) {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // Layer 6
    u8g2.setFont(u8g2_font_profont11_tr);
    u8g2.drawStr(50, 48, "SAVED");

    // download
    u8g2.drawXBMP(57, 18, 16, 16, image_saved_bits);

    u8g2.sendBuffer();
    delay(1000);
}


void changeSaveSetting()
{
    if(rightButtonTap()){
        prefs.begin("settings", false);
        maxAfkTime = sleepBarValue * sleepBarValue * sleepBarValue;
        brightnessLevel = (255 * brightnessBarValue) / 61;
        prefs.putInt("Brightness", brightnessLevel);
        prefs.putInt("SleepTime", sleepBarValue * 100);
        prefs.putInt("Sound", soundSetting);
        prefs.end();
        savedScreen();
    }
    return;
}

void nextSetting()
{
    currentSettingIndex = (currentSettingIndex + 1) % numOfSettingsOptions;
}

void previousSetting()
{
    currentSettingIndex = (currentSettingIndex - 1 + numOfSettingsOptions) % numOfSettingsOptions;
}


void settingsPage(void) {
    u8g2.clearBuffer();
    u8g2.setContrast((255 * brightnessBarValue) / 61);
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // Layer 1
    u8g2.setFont(u8g2_font_4x6_tr);
    u8g2.drawStr(13, 9, "SETTINGS:");

    // Layer 2
    u8g2.drawStr(4, 20, "Sound Haptics");

    // download
    u8g2.drawXBMP(4, 3, 7, 7, image_download_bits);

    // Layer 6
    u8g2.drawStr(4, 32, "Sleep Time");

    // Layer 7
    u8g2.drawFrame(50, 26, 65, 8);

    // Layer 8
    u8g2.drawBox(52, 28, sleepBarValue, 4);

    // Layer 10
    u8g2.drawStr(4, 45, "Brigthness");

    // Layer 7 copy
    u8g2.drawFrame(50, 39, 65, 8);

    // Layer 8 copy
    u8g2.drawBox(52, 41, brightnessBarValue, 4);

    // download
    if(currentSettingIndex == 3){
        u8g2.drawXBMP(91, 50, 24, 11, image_saveSelected_bits);
    } else {
        u8g2.drawXBMP(91, 50, 24, 11, image_save_bits);
    }

    // Layer 13
    u8g2.setFont(u8g2_font_profont10_tr);
    if(soundSetting == 0)
        u8g2.drawStr(99, 20, "OFF");
    else{
        u8g2.drawStr(103, 20, "ON");
    }

    if(downButtonTap()){
        nextSetting();
    }

    if(upButtonTap()){
        previousSetting();
    }

    switch (currentSettingIndex)
    {
    case 0:
        changeSoundSetting();
        u8g2.drawXBMP(120, 15, 3, 5, image_arrow_bits);
        break;
    case 1:  
        changeSleepTimeSetting();
        u8g2.drawXBMP(120, 28, 3, 5, image_arrow_bits);
        break;
    case 2:
        changeBrightnessSetting();
        u8g2.drawXBMP(120, 41, 3, 5, image_arrow_bits);
        break;
    case 3:
        changeSaveSetting();
        u8g2.drawXBMP(120, 53, 3, 5, image_arrow_bits);
        break;
    default:
        changeSoundSetting();
        break;
    }
    u8g2.setDrawColor(2);
    if(soundSetting){
        u8g2.drawBox(94, 12, 21, 10);
    }else{
        u8g2.drawFrame(94, 12, 21, 10);
    }

    u8g2.sendBuffer();
}



