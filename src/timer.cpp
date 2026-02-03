#include <U8g2lib.h>
#include <Arduino.h>
#include <clock.h>
#include <buttonBehav.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

bool Trunning = false;
unsigned long startTime_ = 0;
unsigned long duration = 0;
unsigned long remainingTime = 0;
int repeatsLeft = 0;

extern int afkTime;
unsigned long elapsed;

void startTimer(unsigned long timerDuration, int repeats) {
    duration = timerDuration * 1000;
    remainingTime = duration;     
    elapsed = 0;
    startTime_ = millis();         
    repeatsLeft = repeats;
    Trunning = true;
}


void resumeTimer() {
    startTime_ = millis() - (duration - remainingTime);
    Trunning = true;
}

void pauseTimer() {
    remainingTime = duration - (millis() - startTime_);
    Trunning = false;
}


void resetTimer() {
    Trunning = false;
    elapsed = 0;
    remainingTime = duration;
}


void Timer() {
    afkTime = 0;
    if(rightButtonTap()) {
        if(Trunning) {
            pauseTimer();
        } else {
            resumeTimer();
        }
    }
    if(Trunning){
        elapsed = millis() - startTime_;
        if (elapsed >= duration) {
            if (repeatsLeft > 1) {
                startTimer(duration / 1000, repeatsLeft - 1);
            } else {
                Trunning = false;
                elapsed = 0;
                remainingTime = duration;
                clockMode = NONE;
                return;
            }
        }
    }
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    
    String ms = String(((duration - elapsed) % 1000)/10);
    unsigned long totalSeconds = (duration - elapsed) / 1000;
    String hours = String((totalSeconds / 3600) % 100);
    String seconds = String(totalSeconds % 60);
    String minutes = String((totalSeconds / 60) % 60);
    String combinedTime = "";
    if(duration > 3600000){
        combinedTime = (hours.length() < 2 ? "0" + hours : hours) + ":" + (minutes.length() < 2 ? "0" + minutes : minutes) + ":" + (seconds.length() < 2 ? "0" + seconds : seconds);
    }else{
        combinedTime = (minutes.length() < 2 ? "0" + minutes : minutes) + ":" + (seconds.length() < 2 ? "0" + seconds : seconds) + "." + (ms.length() ==1 ? "0" + ms : ms);
    }
    
    int Tprogress = map(elapsed, 0, duration, 0, 128);
    u8g2.drawBox(0, 0, Tprogress, 64);
    
    // Layer 2
    u8g2.setDrawColor(2);
    u8g2.setFont(u8g2_font_profont22_tr);
    u8g2.drawStr(17, 37, combinedTime.c_str());
    
    // Layer 3
    u8g2.setFont(u8g2_font_profont11_tr);
    u8g2.drawStr(17, 55, String(repeatsLeft).c_str());
    u8g2.sendBuffer();
}
