#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <clock.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

static const unsigned char image_clock_bits[] U8X8_PROGMEM = {0x9e,0x3c,0xcd,0x59,0xb7,0x76,0x0b,0x68,0x05,0x50,0x82,0x20,0x82,0x20,0x81,0x40,0x83,0x60,0x41,0x40,0x22,0x20,0x12,0x20,0x04,0x10,0x08,0x08,0xb4,0x16,0xc2,0x21};

int progress = 0;

enum PomodoroState {
    WORK,
    BREAK
};
PomodoroState pomoState = BREAK;
extern int afkTime;

unsigned long stateStartTime = 0;
unsigned long stateDuration = 0;

int currentCycle = 0;
void changeState(PomodoroState pomoState, unsigned int pomodoroWORKdelay, unsigned int pomodoroBREAKdelay){
    stateStartTime = millis();
    if(pomoState == WORK){
        stateDuration = pomodoroWORKdelay * 60 * 1000;
    } else {
        stateDuration = pomodoroBREAKdelay * 60 * 1000;
    }
    if(pomoState == BREAK){
        currentCycle++;
    }
}

void resetPomodoro(){
    pomoState = BREAK;
    stateStartTime = 0;
    stateDuration = 0;
    progress = 0;
    currentCycle = 0;
}

int blinkDelayTime = 0;
int blinkDelay = 32;

void Pomodoro(int pomodoroWORKdelay, int pomodoroBREAKdelay, int cycles = 0) {

    if(pomoState == WORK && (millis() - stateStartTime >= stateDuration) && (cycles == 0 || currentCycle < cycles)){
        pomoState = BREAK;
        changeState(pomoState, pomodoroWORKdelay, pomodoroBREAKdelay);
    } else if (pomoState == BREAK && (millis() - stateStartTime >= stateDuration) && (cycles == 0 || currentCycle < cycles)){
        pomoState = WORK;
        changeState(pomoState, pomodoroWORKdelay, pomodoroBREAKdelay);
    }
    if(currentCycle >= cycles && cycles != 0){
        clockMode = NONE;
        return;
    }

    progress = (millis() - stateStartTime) * 100 / stateDuration;

    int barValue = map(progress, 0, 100, 0, 128);
    afkTime = 0; // reset afk time whenever in pomodoro mode

    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // Layer 2
    u8g2.drawBox(0, 0, barValue, 64);

    String stateStr = (pomoState == WORK) ? "FOCUS" : "BREAK";
    // Layer 3
    u8g2.setDrawColor(2);
    u8g2.setFont(u8g2_font_profont29_tr);
    u8g2.drawStr(3, 23, stateStr.c_str());

    String WPStr = "FP " + String(pomodoroWORKdelay) + " MINS";
    String BPStr = "BP " + String(pomodoroBREAKdelay) + " MINS";
    // Layer 3
    u8g2.setFont(u8g2_font_4x6_tr);
    u8g2.drawStr(3, 34, WPStr.c_str());

    // Layer 3 copy
    u8g2.drawStr(3, 43, BPStr.c_str());

    // Layer 4
    u8g2.setFont(u8g2_font_timR18_tr);
    String progressStr = String(progress) + "%";
    u8g2.drawStr(83, 60, progressStr.c_str());

    blinkDelayTime = (blinkDelayTime + 1) % blinkDelay;
    if(blinkDelayTime < blinkDelay/2){
        u8g2.drawXBMP(108, 6, 15, 16, image_clock_bits);
    }
    u8g2.sendBuffer();
}
