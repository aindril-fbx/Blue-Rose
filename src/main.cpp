#pragma region "Header Files"
#include <Arduino.h>
#include <U8g2lib.h>
#include <esp_sleep.h>
#include <Preferences.h>
#include "driver/gpio.h"

#include <compass.h> //TODO: find a way to integrate spotify into this
#include <snake.h>
#include <menuPage.h>
#include <buttonTest.h>
#include <LaptopControl.h>
#include <buttonBehav.h>
#include <settings.h>
#include <pomodoro.h>
#include <clock.h>
#include <games.h>
#include <stats.h>
#include <faces.h>
#include <versionInfo.h>
#include <textScreen.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include <BleKeyboard.h>
#pragma endregion

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

BleKeyboard bleKeyboard;
Preferences prefs;

#pragma region "Button Variables"
extern const int selectButton = 33;
extern const int upButton = 18;
extern const int downButton = 19;
extern const int leftButton = 4;
extern const int rightButton = 32;
extern const int buzzerPin = 27;
int previousButtonPressed = 0;
int selectButtonPressed = 0;
int nextButtonPressed = 0;
int leftButtonPressed = 0;
int rightButtonPressed = 0;
#pragma endregion

RTC_DATA_ATTR int currentItemIndex = 0; // current item index in the menu
int previousItemIndex;                  // previous item index in the menu
int nextItemIndex;                      // next item index in the menu

RTC_DATA_ATTR int currentScene = 0;      // current scene index, -1 = face, 0 = mainMenu, 1 = Compass, 2 = Game Menu
RTC_DATA_ATTR int wokefromSleep = 0;     // flag to indicate if the device woke up from sleep
RTC_DATA_ATTR int brightnessLevel = 255; // brightness level (0-255)
RTC_DATA_ATTR int buzzAlarm = 0;

time_t timeUntilAlarm = -1; // In seconds

#pragma region "Icons"
static const unsigned char coolBar[] U8X8_PROGMEM = {0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00};

static const unsigned char sleepBits[] U8X8_PROGMEM = {0x20, 0x00, 0x38, 0x70, 0x14, 0x40, 0x12, 0x20, 0x8a, 0x77, 0x09, 0x02, 0x09, 0x01, 0x89, 0x07, 0x11, 0x00, 0x11, 0x60, 0x62, 0x38, 0x82, 0x27, 0x04, 0x10, 0x18, 0x0c, 0xe0, 0x03, 0x00, 0x00};

#pragma endregion

void wokeFromSleepScreen(void)
{
    u8g2.setBitmapMode(1);

    for (int i = -101; i <= 148; i += 16)
    {
        u8g2.clearBuffer();
        u8g2.drawXBMP(i, 0, 81, 64, coolBar);
        u8g2.sendBuffer();
    }
    // Layer 7

    u8g2.sendBuffer();
}

RTC_DATA_ATTR int maxAfkTime = 2000; // maximum AFK time in milliseconds
int afkTime = 0;                     // current AFK time in milliseconds
int soundSetting = 1;

bool alarmSound = false;

void updateAlarmBeepBeep()
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

    if (!alarmSound)
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

void mainAlarmScreen(void)
{
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
    u8g2.drawStr(11, 31, "BEEP BEEP");
    u8g2.sendBuffer();
}

int alarmEn = 0;
void setup(void)
{
    Serial.begin(115200);
    u8g2.begin();       // start the u8g2 library
    u8g2.clearBuffer(); // clear the internal memory

    if (wokefromSleep == 1)
    {
        wokeFromSleepScreen();
        wokefromSleep = 0;
    }

    pinMode(A0, INPUT);
    pinMode(upButton, INPUT_PULLUP);
    pinMode(selectButton, INPUT_PULLUP);
    pinMode(downButton, INPUT_PULLUP);
    pinMode(leftButton, INPUT_PULLUP);
    pinMode(rightButton, INPUT_PULLUP);
    pinMode(buzzerPin, OUTPUT);

    innitIndices();

    Serial.println("Starting BLE work!");
    delay(100);
    bleKeyboard.begin();

    esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();

    prefs.begin("settings", false);                    //* Global settings of the device
    maxAfkTime = prefs.getInt("SleepTime", 2000);      // Sleep timer
    brightnessLevel = prefs.getInt("Brightness", 255); // Brightness
    soundSetting = prefs.getInt("Sound", 0);           // Sound haptics toggle
    brightnessBarValue = (brightnessLevel * 61) / 255;
    sleepBarValue = maxAfkTime / 100;
    prefs.end();

    prefs.begin("clock", false); //* Alarm Enabled setting
    alarmEn = prefs.getInt("aEnabled", 0);
    prefs.end();

    u8g2.setContrast(brightnessLevel);
    delay(100);
    syncTimeAsync(); // Sync time when device wakes from sleep

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_TIMER)
    {
        currentScene = 5;
        clockMode = REALTIME;
        // Start alarm if the device woke up from sleep at alarm time.
        if (buzzAlarm)
        {
            alarmSound = true;
            mainAlarmScreen();
            while (true)
            {
                updateAlarmBeepBeep();
                if (rightButtonHold())
                {
                    buzzAlarm = false;
                    break;
                }
            }
            alarmSound = false;
            syncTimeAsync();
        }
    }
}

void sleepScreen(void)
{
    u8g2.setPowerSave(0);
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    u8g2.drawXBMP(57, 24, 15, 16, sleepBits);
    u8g2.setDrawColor(2);
    u8g2.sendBuffer();
    delay(1000);
}

int backDelay = 100;
int backTime = 0;

bool buzzerOn = false;
bool lastHeld = false;
unsigned long buzzerTimer = 0;

const unsigned long BEEP_TIME = 30;
const unsigned long scrollDelay = 180;
const unsigned long BEEP_DELAY = scrollDelay - BEEP_TIME;

void updateBuzzer()
{
    bool held = anyButtonHold();

    // Detect button press
    if (held && !lastHeld && !buzzerOn)
    {
        playBuzzer(true);
        buzzerOn = true;
        buzzerTimer = millis();
    }

    if (buzzerOn)
    {
        // Finish current beep
        if (millis() - buzzerTimer >= BEEP_TIME)
        {
            playBuzzer(false);
            buzzerOn = false;
            buzzerTimer = millis();
        }
    }
    else if (held)
    {
        // Start another beep after the delay
        if (millis() - buzzerTimer >= BEEP_DELAY)
        {
            playBuzzer(true);
            buzzerOn = true;
            buzzerTimer = millis();
        }
    }

    lastHeld = held;
}

//* Function can be called from inside any app to return to main menu
void backtoMenu(void)
{
    currentScene = 0;
}

void loop(void)
{
    updateAlarmBeepBeep();
    if (!alarmSound)
    {
        updateBuzzer();
    }
    resetButtonStates();

    if (anyButtonHold())
    { //* AFK condition, reset timer when any button is pressed
        afkTime = 0;
    }else{
        afkTime += 1;
    }

    static unsigned long lastStep = millis();
    if (millis() - lastStep > 1000 && alarmEn)
    {
        timeUntilAlarm -= 1;
        if (timeUntilAlarm == 0)
        { //* Alarm condition if the device is awake during alarm time
            alarmSound = true;
            mainAlarmScreen();
            while (true)
            {
                updateAlarmBeepBeep();
                if (rightButtonHold())
                {
                    buzzAlarm = false;
                    break;
                }
            }
            alarmSound = false;
            syncTimeAsync();
        }
        lastStep = millis();
    }

    if (afkTime >= maxAfkTime)
    { //* Sleep condition
        sleepScreen();
        btStop();
        if (alarmEn)
        {
            if (timeUntilAlarm > 3000)
            {
                esp_sleep_enable_timer_wakeup(3000 * 1000000ULL);
                buzzAlarm = 0;
            }
            else if (timeUntilAlarm > 1000)
            {
                esp_sleep_enable_timer_wakeup(1000 * 1000000ULL);
                buzzAlarm = 0;
            }
            else if (timeUntilAlarm > 300)
            {
                esp_sleep_enable_timer_wakeup(300 * 1000000ULL);
                buzzAlarm = 0;
            }
            else if (timeUntilAlarm > 0)
            {
                esp_sleep_enable_timer_wakeup(timeUntilAlarm * 1000000ULL);
                buzzAlarm = 1;
            }
        }
        esp_sleep_enable_ext0_wakeup((gpio_num_t)33, 0); // SELECT button is used for waking up the device
        u8g2.setPowerSave(1); // Turn screen off before shutting down
        delay(50);
        while (digitalRead(leftButton) == LOW) delay(10);// Make sure the wake pin is not grounded before going going to sleep
        wokefromSleep = 1;
        esp_deep_sleep_start();
        return;
    }

    switch (currentScene) //* Swtich case of each app
    {
    case -1: //* FACES
        faces();
        if (selectButtonTap())
        {
            currentScene = 0;
        }
        break;
    case 0: //* MAIN MENU
        static long lastScroll = millis();
        mainMenu(); // display the main menu
        if (upButtonHold() && millis() - lastScroll > scrollDelay)
        {
            previousItem();
            lastScroll = millis();
        }
        if (selectButtonTap())
        {
            if (currentItemIndex == 0)
            {
                currentScene = -1;
                return;
            }
            currentScene = currentItemIndex; // set the current scene to the selected item index
        }
        if (downButtonHold() && millis() - lastScroll > scrollDelay)
        {
            nextItem(); // if the next button is pressed, go to the next item
            lastScroll = millis();
        }
        break;

    case 1: // TODO: REMOVE THE COMPASS AND IMPLEMENT MEDIA CONTROL
        //* COMPASS
        compass(!leftButtonHold() && !rightButtonHold() ? 0 : (!leftButtonHold() && rightButtonHold() ? -1 : 1), downButtonTap()); // display the compass

        if (selectButtonTap())
        {
            currentScene = 0;
        }
        break;

    case 2: //* BUTTON TEST
        //? Should I remove the bluetooth controller capability of the botton test?
        if (rightButtonHold())
        {
            backTime += 1;
            if (backTime > backDelay)
            {

                backTime = 0;
                currentScene = 0;
                return;
            }
        }
        else
        {
            backTime = 0;
        }
        ButtonTest(backTime, backDelay);
        break;

    case 3: //* PC CONTROL
        PC_Control();
        if (selectButtonTap())
        {
            currentScene = 0;
        }
        break;

    case 4: //* GLOBAL SETTINGS
        settingsPage();
        if (selectButtonTap())
        {
            currentScene = 0;
        }
        break;

    case 5: //* CLOCK APPS
        clockFunc(0);
        if (selectButtonTap())
        {
            if (clockMode != NONE)
            {
                clockMode = NONE;
                return;
            }
            clockFunc(1);
            gotInfo = 0;
            currentScene = 0;
            syncTimeAsync();
        }
        break;

    case 6: //* GAMES
        gamesMenu(0);
        break;

    case 7: //* VERSION INFO + GITHUB QR
        version();
        if (selectButtonTap())
        {
            currentScene = 0;
        }
        break;
    case 8: //* RANDOM TEXT SCREEN FOR LARPING
        drawTextScreen();
        if (selectButtonTap())
        {
            currentScene = 0;
        }
        break;
    }
}