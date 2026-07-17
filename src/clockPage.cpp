#include <Arduino.h>
#include <U8g2lib.h>
#include <pomodoro.h>
#include <stopWatch.h>
#include <timer.h>
#include <buttonBehav.h>
#include <Preferences.h>
#include <stats.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
extern Preferences prefs;

static const unsigned char image_Arrow_bits[] U8X8_PROGMEM = {0x01, 0x03, 0x07, 0x03, 0x01};
static const unsigned char image_download_bits[] U8X8_PROGMEM = {0x9e, 0x3c, 0xcd, 0x59, 0xb7, 0x76, 0x0b, 0x68, 0x05, 0x50, 0x82, 0x20, 0x82, 0x20, 0x81, 0x40, 0x83, 0x60, 0x41, 0x40, 0x22, 0x20, 0x12, 0x20, 0x04, 0x10, 0x08, 0x08, 0xb4, 0x16, 0xc2, 0x21};
static const unsigned char image_Layer_3_bits[] U8X8_PROGMEM = {0x01, 0x02, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x1c};

static const unsigned char image_download_1_bits[] U8X8_PROGMEM = {0x04, 0x06, 0x07, 0x06, 0x04};

enum ClockMode
{
    TIMER,
    TIMER_SETTING,
    STOPWATCH,
    POMODORO,
    POMODORO_SETTING,
    REALTIME,
    NONE,
    ALARMSETTING
};

ClockMode clockMode = NONE;

int totalStates = 4;
RTC_DATA_ATTR int currentState = 0;
int arrowBlink = 0;
int arrowBlinkDelay = 16;

int pSettingState = 0;
int pFperiod = 25; // In Minutes
int pBperiod = 5;
int pCycles = 3;

int tsettingState = 0;
int ttotalStates = 5;

int asettingState = 0;
int atotalStates = 4;

int thours = 0;
int tminutes = 0;
int tseconds = 0;
int trepeats = 0;
unsigned long tduration = 0;

void clockMenu()
{
    if (downButtonTap())
    {
        currentState = (currentState + 1) % totalStates;
    }
    if (upButtonTap())
    {
        currentState = (currentState - 1 + totalStates) % totalStates;
    }
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // Layer 1
    if (arrowBlink > arrowBlinkDelay / 2)
    {
        u8g2.drawXBMP(103, 10, 3, 5, image_download_1_bits);
    }
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(3, 10, "# CLOCK");

    // Layer 2
    u8g2.drawLine(5, 13, 5, 52);

    switch (currentState)
    {
    case 0:
        if (rightButtonTap())
        {
            delay(100);
            clockMode = TIMER_SETTING;
            break;
        }
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawXBMP(49, 19, 3, 5, image_Arrow_bits);
        break;
    case 1:
        if (rightButtonTap())
        {
            clockMode = STOPWATCH;
        }
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawXBMP(78, 30, 3, 5, image_Arrow_bits);
        break;
    case 2:
        if (rightButtonTap())
        {
            delay(100);
            clockMode = POMODORO_SETTING;

            prefs.begin("clock", false);
            pFperiod = prefs.getInt("FP", 45); // default to 45 mins if not set
            pBperiod = prefs.getInt("BP", 15); // default to 15 mins if not set
            pCycles = prefs.getInt("CY", 3);   // default to 3 if not set
            prefs.end();
        }
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawXBMP(73, 41, 3, 5, image_Arrow_bits);
        break;
    case 3:
        if (rightButtonTap())
        {
            delay(100);
            clockMode = ALARMSETTING;
        }
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawXBMP(53, 52, 3, 5, image_Arrow_bits);
        break;
    default:
        if (rightButtonTap())
        {
            delay(100);
            clockMode = TIMER_SETTING;
            break;
        }
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawXBMP(49, 19, 3, 5, image_Arrow_bits);
        break;
    }

    // Layer 3
    u8g2.drawXBMP(6, 19, 5, 36, image_Layer_3_bits);

    // Layer 4
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.drawStr(14, 25, "TIMER");
    // Layer 4 copy
    u8g2.drawStr(14, 36, "STOPWATCH");
    // Layer 4 copy
    u8g2.drawStr(14, 47, "POMO DORO");

    u8g2.drawStr(14, 58, "ALARM");
    // download
    u8g2.drawXBMP(109, 4, 15, 16, image_download_bits);

    u8g2.sendBuffer();
}

#pragma region
static const unsigned char image_BeginSelect_bits[] U8X8_PROGMEM = {0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x42, 0x00, 0x01, 0x00, 0x00, 0x00, 0xc2, 0x00, 0x01, 0x00, 0x00, 0x00, 0xc2, 0x01, 0x01, 0x00, 0x00, 0x00, 0xc2, 0x00, 0x01, 0x00, 0x00, 0x00, 0x42, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00};
static const unsigned char image_ValueChange_bits[] U8X8_PROGMEM = {0x04, 0x00, 0x00, 0x00, 0x00, 0x02, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x06, 0x00, 0x00, 0x00, 0x00, 0x06, 0x04, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0x3f, 0x00};
#pragma endregion

void countDown()
{
    playBuzzerBypass(false);
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // Layer 1
    u8g2.setFont(u8g2_font_6x13O_tr);

    u8g2.drawStr(63, 35, "3");
    u8g2.sendBuffer();
    delay(600);
    u8g2.clearBuffer();
    u8g2.drawStr(63, 35, "2");
    u8g2.sendBuffer();
    delay(600);
    u8g2.clearBuffer();
    u8g2.drawStr(63, 35, "1");
    u8g2.sendBuffer();
    delay(600);
    u8g2.clearBuffer();
    u8g2.drawStr(60, 35, "GO");
    u8g2.sendBuffer();
    delay(1000);
}
void pomodoroSettings()
{

    if (downButtonTap())
    {
        pSettingState = (pSettingState + 1) % 4;
    }
    if (upButtonTap())
    {
        pSettingState = (pSettingState - 1 + 4) % 4;
    }
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);

    // Layer 2
    u8g2.setFont(u8g2_font_5x8_tr);
    u8g2.drawStr(3, 13, "CYCLES:");
    u8g2.drawStr(3, 27, "FOCUS PERIOD:");
    // Layer 2 copy
    u8g2.drawStr(3, 43, "BREAK PERIOD:");

    String FPStr = String(pFperiod) + " M";
    String BPStr = String(pBperiod) + " M";
    String CStr = String(pCycles);
    // Layer 6
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(84, 44, BPStr.c_str());
    // Layer 6 copy
    u8g2.drawStr(84, 28, FPStr.c_str());
    // Layer 6 copy
    u8g2.drawStr(84, 14, CStr.c_str());

    switch (pSettingState)
    {
    case 0:
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            u8g2.drawXBMP(77, 7, 44, 9, image_ValueChange_bits);
        }
        if (rightButtonTap())
        {
            pCycles = min(pCycles + 1, 1000);
        }
        if (leftButtonTap())
        {
            pCycles = max(pCycles - 1, 1);
        }
        break;
    case 1:
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            u8g2.drawXBMP(77, 21, 44, 9, image_ValueChange_bits);
        }
        if (rightButtonTap())
        {
            pFperiod = min(pFperiod + 5, 1000);
        }
        if (leftButtonTap())
        {
            pFperiod = max(pFperiod - 5, 5);
        }
        break;
    case 2:
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            u8g2.drawXBMP(77, 38, 44, 9, image_ValueChange_bits);
        }
        if (rightButtonTap())
        {
            pBperiod = min(pBperiod + 5, 1000);
        }
        if (leftButtonTap())
        {
            pBperiod = max(pBperiod - 5, 5);
        }
        break;
    case 3:
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            u8g2.drawXBMP(3, 50, 41, 11, image_BeginSelect_bits);
        }
        if (rightButtonTap())
        {
            prefs.begin("clock", false);
            prefs.putInt("FP", pFperiod);
            prefs.putInt("BP", pBperiod);
            prefs.putInt("CY", pCycles);
            prefs.end();
            countDown();
            clockMode = POMODORO;
            resetPomodoro();
            pSettingState = 0;
        }
        break;
    default:
        break;
    }

    // Layer 9
    u8g2.drawStr(5, 59, "BEGIN");

    u8g2.sendBuffer();
}

#pragma region
static const unsigned char image_Begin_Select_bits[] U8X8_PROGMEM = {0xff, 0xff, 0xff, 0x7f, 0x00, 0x01, 0x00, 0x00, 0x40, 0x00, 0x01, 0x00, 0x00, 0x40, 0x00, 0x01, 0x00, 0x00, 0x40, 0x02, 0x01, 0x00, 0x00, 0x40, 0x06, 0x01, 0x00, 0x00, 0x40, 0x0e, 0x01, 0x00, 0x00, 0x40, 0x06, 0x01, 0x00, 0x00, 0x40, 0x02, 0x01, 0x00, 0x00, 0x40, 0x00, 0x01, 0x00, 0x00, 0x40, 0x00, 0x01, 0x00, 0x00, 0x40, 0x00, 0xff, 0xff, 0xff, 0x7f, 0x00};
static const unsigned char image_TimeSelect_bits[] U8X8_PROGMEM = {0x00, 0x20, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x03, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x02, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00};
#pragma endregion

void timerSettings(void)
{

    if (tsettingState < 4)
    {
        if (rightButtonTap())
        {
            tsettingState = (tsettingState + 1) % ttotalStates;
        }
        if (leftButtonTap())
        {
            tsettingState = (tsettingState - 1 + ttotalStates) % ttotalStates;
        }
    }
    else
    {
        if (leftButtonTap())
        {
            tsettingState = (tsettingState - 1 + ttotalStates) % ttotalStates;
        }
    }
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // Layer 2
    u8g2.setDrawColor(2);

    // Layer 6
    u8g2.setFont(u8g2_font_profont10_tr);
    u8g2.drawStr(89, 58, "BEGIN");

    static long lastStep = millis();
    long lastStepDelay = 100;
    switch (tsettingState)
    {
    case 0:
        if (upButtonHold() && (millis() - lastStep > lastStepDelay))
        {
            thours = min(thours + 1, 99);
            lastStep = millis();
        }
        if (downButtonHold() && (millis() - lastStep > lastStepDelay))
        {
            thours = max(thours - 1, 0);
            lastStep = millis();
        }
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawXBMP(15, 3, 26, 30, image_TimeSelect_bits);
        break;
    case 1:
        if (upButtonHold() && (millis() - lastStep > lastStepDelay))
        {
            tminutes = min(tminutes + 1, 59);
            lastStep = millis();
        }
        if (downButtonHold() && (millis() - lastStep > lastStepDelay))
        {
            tminutes = max(tminutes - 1, 0);
            lastStep = millis();
        }
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawXBMP(51, 3, 26, 30, image_TimeSelect_bits);
        break;
    case 2:
        if (upButtonHold() && (millis() - lastStep > lastStepDelay))
        {
            tseconds = min(tseconds + 1, 59);
            lastStep = millis();
        }
        if (downButtonHold() && (millis() - lastStep > lastStepDelay))
        {
            tseconds = max(tseconds - 1, 0);
            lastStep = millis();
        }
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawXBMP(87, 3, 26, 30, image_TimeSelect_bits);
        break;
    case 3:
        if (upButtonHold() && (millis() - lastStep > lastStepDelay / 2))
        {
            trepeats = min(trepeats + 1, 10000);
            lastStep = millis();
        }
        if (downButtonHold() && (millis() - lastStep > lastStepDelay / 2))
        {
            trepeats = max(trepeats - 1, 0);
            lastStep = millis();
        }
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawLine(65, 46, 72, 46);
        break;
    case 4:
        if (rightButtonTap())
        {
            clockMode = TIMER;
            startTimer(tduration, trepeats);
            resetTimer();
            break;
        }
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawXBMP(86, 49, 36, 12, image_Begin_Select_bits);
        break;
    default:
        break;
    }
    String hoursStr = (thours < 10) ? "0" + String(thours) : String(thours);
    String minutesStr = (tminutes < 10) ? "0" + String(tminutes) : String(tminutes);
    String secondsStr = (tseconds < 10) ? "0" + String(tseconds) : String(tseconds);
    String timeString = hoursStr + ":" + minutesStr + ":" + secondsStr;

    String repeatsString = String(trepeats);

    tduration = (thours * 3600UL + tminutes * 60UL + tseconds);

    u8g2.setFont(u8g2_font_profont22_tr);
    u8g2.drawStr(17, 25, timeString.c_str());

    // Layer 3
    u8g2.setFont(u8g2_font_profont11_tr);
    u8g2.drawStr(66, 44, repeatsString.c_str());

    // Layer 3
    u8g2.setDrawColor(1);
    u8g2.drawStr(15, 44, "Repeats:");

    u8g2.sendBuffer();
}

void alarmSettings(void)
{

    if (asettingState < 3)
    {
        if (rightButtonTap())
        {
            asettingState = (asettingState + 1) % atotalStates;
        }
        if (leftButtonTap())
        {
            asettingState = (asettingState - 1 + atotalStates) % atotalStates;
        }
    }
    else
    {
        if (upButtonTap())
        {
            asettingState = (asettingState - 1 + atotalStates) % atotalStates;
        }
        if (downButtonTap())
        {
            asettingState = (asettingState + 1) % atotalStates;
        }
    }
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // Layer 2
    u8g2.setDrawColor(2);
    u8g2.setFont(u8g2_font_profont22_tr);
    u8g2.drawStr(17, 27, "00:00|PM");
    // rect 4
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 40, 128, 16);
    // string 5
    u8g2.setDrawColor(2);
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.drawStr(5, 52, "ALARM STATUS =");
    // string 6
    u8g2.setFont(u8g2_font_profont12_tr);
    u8g2.drawStr(94, 52, "ON");
    // Begin Select
    static long lastStep = millis();
    long lastStepDelay = 100;
    // TimeSelect
    u8g2.setDrawColor(1);
    switch (asettingState)
    {
    case 0:
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawXBMP(15, 5, 26, 30, image_TimeSelect_bits);
        break;
    case 1:
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawXBMP(51, 5, 26, 30, image_TimeSelect_bits);
        break;
    case 2:
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawXBMP(87, 5, 26, 30, image_TimeSelect_bits);
        break;
    case 3:
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.setDrawColor(2);
        u8g2.drawXBMP(92, 42, 36, 12, image_Begin_Select_bits);
        break;
    default:
        if (arrowBlink < arrowBlinkDelay / 2)
        {
            break;
        }
        u8g2.drawXBMP(15, 5, 26, 30, image_TimeSelect_bits);
        break;
    }

    u8g2.sendBuffer();
}

void clockFunc(int resetState = 0)
{
    arrowBlink = (arrowBlink + 1) % arrowBlinkDelay;
    if (resetState == 1)
    {
        clockMode = NONE;
        return;
    }

    switch (clockMode)
    {
    case TIMER:
        Timer();
        return;
        break;
    case TIMER_SETTING:
        timerSettings();
        return;
        break;
    case STOPWATCH:
        Stopwatch();
        return;
        break;
    case POMODORO:
        Pomodoro(pFperiod, pBperiod, pCycles);
        return;
        break;
    case POMODORO_SETTING:
        pomodoroSettings();
        return;
        break;
    case ALARMSETTING:
        alarmSettings();
        return;
        break;
    case REALTIME:
        showStats();
        if (leftButtonTap())
        {
            clockMode = NONE;
        }
        return;
        break;
    default:
        clockMenu();
        if (leftButtonTap())
        {
            clockMode = REALTIME;
        }
        break;
    }
}
