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

void startTimer(unsigned long timerDuration, int repeats)
{
    duration = timerDuration * 1000;
    remainingTime = duration;
    elapsed = 0;
    startTime_ = millis();
    repeatsLeft = repeats;
    Trunning = true;
}

void resumeTimer()
{
    startTime_ = millis() - (duration - remainingTime);
    Trunning = true;
}

void pauseTimer()
{
    remainingTime = duration - (millis() - startTime_);
    Trunning = false;
}

void resetTimer()
{
    Trunning = false;
    elapsed = 0;
    remainingTime = duration;
}

bool alarmEnabled = false;

void updateAlarm()
{
    static const uint16_t pattern[] = {
        40,  // ON
        40,  // OFF
        80,  // ON
        80,  // OFF
        160, // ON
        800  // OFF
    };
    static uint8_t step = 0;
    static unsigned long lastChange = 0;

    if (!alarmEnabled)
    {
        playBuzzerBypass(false);
        step = 0;
        lastChange = millis();
        return;
    }

    if (millis() - lastChange >= pattern[step])
    {
        lastChange = millis();
        step = (step + 1) % 6;
        playBuzzerBypass((step % 2) == 0);
    }
}

bool alarmEnabled2 = true;

void updateAlarm2()
{
    static const uint16_t pattern[] = {120, 80, 120, 500};
    static uint8_t step = 0;
    static unsigned long lastChange = 0;

    if (!alarmEnabled2)
    {
        playBuzzerBypass(false);
        step = 0;
        lastChange = millis();
        return;
    }

    if (millis() - lastChange >= pattern[step])
    {
        lastChange = millis();
        step = (step + 1) % 4;

        playBuzzerBypass(step == 0 || step == 2);
    }
}

void alarmScreen(void) {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // rect 3
    u8g2.drawBox(0, 12, 128, 23);
    // string 1 copy 1
    u8g2.setFont(u8g2_font_6x12_tr);
    u8g2.drawStr(36, 51, "> to STOP");
    // string 1
    u8g2.setDrawColor(2);
    u8g2.setFont(u8g2_font_profont22_tr);
    u8g2.drawStr(11, 31, "TIME'S UP");
    u8g2.sendBuffer();
}

void Timer()
{
    static unsigned long beepNow = millis();
    // updateAlarm();
    afkTime = 0;
    if (rightButtonTap())
    {
        if (Trunning)
        {
            pauseTimer();
        }
        else
        {
            resumeTimer();
        }
    }
    if (Trunning)
    {
        elapsed = millis() - startTime_;
        if (elapsed >= duration)
        {
            if (repeatsLeft > 1)
            {
                startTimer(duration / 1000, repeatsLeft - 1);
                alarmEnabled2 = true;
                beepNow = millis();
            }
            else
            {
                Trunning = false;
                elapsed = 0;
                remainingTime = duration;
                clockMode = NONE;
                alarmEnabled2 = false;
                alarmEnabled = true;
                alarmScreen();
                while (true)
                {
                    updateAlarm();
                    if (rightButtonHold())
                    {
                        break;
                    }
                }
                alarmEnabled = false;
                return;
            }
        }
    }
    
    unsigned long beepTime = 1000;
    updateAlarm2();

    if(millis() - beepNow > beepTime && alarmEnabled2){
        alarmEnabled2 = false;
    }

    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);

    String ms = String(((duration - elapsed) % 1000) / 10);
    unsigned long totalSeconds = (duration - elapsed) / 1000;
    String hours = String((totalSeconds / 3600) % 100);
    String seconds = String(totalSeconds % 60);
    String minutes = String((totalSeconds / 60) % 60);
    String combinedTime = "";
    if (duration > 3600000)
    {
        combinedTime = (hours.length() < 2 ? "0" + hours : hours) + ":" + (minutes.length() < 2 ? "0" + minutes : minutes) + ":" + (seconds.length() < 2 ? "0" + seconds : seconds);
    }
    else
    {
        combinedTime = (minutes.length() < 2 ? "0" + minutes : minutes) + ":" + (seconds.length() < 2 ? "0" + seconds : seconds) + "." + (ms.length() == 1 ? "0" + ms : ms);
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
