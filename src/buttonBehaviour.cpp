#include <Arduino.h>
#include <stats.h>

extern int previousButtonPressed;
extern int selectButtonPressed;
extern int nextButtonPressed;
extern int leftButtonPressed;
extern int rightButtonPressed;

bool selectButtonOverride = false;
bool previousButtonOverride = false;
bool nextButtonOverride = false;
bool leftButtonOverride = false;
bool rightButtonOverride = false;

extern const int buzzerPin;
void playBuzzer(bool yes){
    if(yes){
        digitalWrite(buzzerPin, HIGH);
    }else{
        digitalWrite(buzzerPin, LOW);
    }
}

// Select Button
extern const int selectButton;
int selectButtonTap(){
    if(selectButtonOverride && !selectButtonPressed){
        selectButtonPressed = 1;
        return 1;
    }
    if(!digitalRead(selectButton) && !selectButtonPressed){
        selectButtonPressed = 1;
        Serial.println("Select Button Pressed");
        return 1;
    }
    return 0;
}

int selectButtonHold(){
    if(selectButtonOverride){
        return 1;
    }
    if(!digitalRead(selectButton)){
        return 1;
    }
    return 0;
}

// Up Button
extern const int upButton;
int upButtonTap(){
    if(previousButtonOverride && !previousButtonPressed){
        previousButtonPressed = 1;
        return 1;
    }
    if(!digitalRead(upButton) && !previousButtonPressed){
        previousButtonPressed = 1;
        Serial.println("Up Button Pressed");
        return 1;
    }
    return 0;
}

int upButtonHold(){
    if(previousButtonOverride){
        return 1;
    }
    if(!digitalRead(upButton)){
        return 1;
    }
    return 0;
}

// Down Button
extern const int downButton;
int downButtonTap(){
    if(nextButtonOverride && !nextButtonPressed){
        nextButtonPressed = 1;
        return 1;
    }
    if(!digitalRead(downButton) && !nextButtonPressed){
        nextButtonPressed = 1;
        Serial.println("Down Button Pressed");
        return 1;
    }
    return 0;
}

int downButtonHold(){
    if(nextButtonOverride){
        return 1;
    }
    if(!digitalRead(downButton)){
        return 1;
    }
    return 0;
}

// Left Button
extern const int leftButton;
int leftButtonTap(){
    if(leftButtonOverride && !leftButtonPressed){
        leftButtonPressed = 1;
        return 1;
    }
    if(!digitalRead(leftButton) && !leftButtonPressed){
        leftButtonPressed = 1;
        Serial.println("Left Button Pressed");
        return 1;
    }
    return 0;
}

int leftButtonHold(){
    if(leftButtonOverride){
        return 1;
    }
    if(!digitalRead(leftButton)){
        return 1;
    }
    return 0;
}

// Right Button
extern const int rightButton;
int rightButtonTap(){
    if(rightButtonOverride && !rightButtonPressed){
        rightButtonPressed = 1;
        return 1;
    }
    if(!digitalRead(rightButton) && !rightButtonPressed){
        rightButtonPressed = 1;
        Serial.println("Right Button Pressed");
        return 1;
    }
    return 0;
}

int rightButtonHold(){
    if(rightButtonOverride){
        return 1;
    }
    if(!digitalRead(rightButton)){
        return 1;
    }
    return 0;
}

void resetButtonStates(){
    if(digitalRead(upButton) && !previousButtonOverride){
        previousButtonPressed = 0; // reset the previous button pressed state
    }
    if(digitalRead(downButton) && !nextButtonOverride){
        nextButtonPressed = 0; // reset the next button pressed state
    }
    if(digitalRead(selectButton) && !selectButtonOverride){
        selectButtonPressed = 0; // reset the select button pressed state
    }
    if(digitalRead(leftButton) && !leftButtonOverride){
        leftButtonPressed = 0; // reset the left button pressed state
    }
    if(digitalRead(rightButton) && !rightButtonOverride){
        rightButtonPressed = 0; // reset the right button pressed state
    }
}

int anyButtonHold(){
    if(downButtonHold() || upButtonHold() || selectButtonHold() || leftButtonHold() || rightButtonHold()){
        // At least one button is being held down
        return 1;
    }
    return 0;
}

void demoMovements(void *param){
    Serial.println("demo start");
    //vTaskSuspend(wifiTaskHandle);
    
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Select Button
    selectButtonOverride = true;
    vTaskDelay(pdMS_TO_TICKS(100));
    selectButtonOverride = false;

    vTaskDelay(pdMS_TO_TICKS(5000));

    // Select Button
    selectButtonOverride = true;
    vTaskDelay(pdMS_TO_TICKS(100));
    selectButtonOverride = false;

    vTaskDelay(pdMS_TO_TICKS(1000));

    for (int i = 0; i < 3; i++)
    {
        // Up Button
        previousButtonOverride = true;
        vTaskDelay(pdMS_TO_TICKS(100));
        previousButtonOverride = false;
        vTaskDelay(pdMS_TO_TICKS(400));
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    // Select Button
    selectButtonOverride = true;
    vTaskDelay(pdMS_TO_TICKS(100));
    selectButtonOverride = false;

    vTaskDelay(pdMS_TO_TICKS(1000));

    // Left Button
    leftButtonOverride = true;
    vTaskDelay(pdMS_TO_TICKS(100));
    leftButtonOverride = false;

    vTaskDelay(pdMS_TO_TICKS(5000));

    // Select Button
    selectButtonOverride = true;
    vTaskDelay(pdMS_TO_TICKS(100));
    selectButtonOverride = false;

    vTaskDelay(pdMS_TO_TICKS(1000));

    for (int i = 0; i < 3; i++)
    {
        // Right Button
        rightButtonOverride = true;
        vTaskDelay(pdMS_TO_TICKS(100));
        rightButtonOverride = false;
        vTaskDelay(pdMS_TO_TICKS(400));
    }

    vTaskDelay(pdMS_TO_TICKS(500));

    for (int i = 0; i < 5; i++)
    {
        // Up Button
        previousButtonOverride = true;
        vTaskDelay(pdMS_TO_TICKS(100));
        previousButtonOverride = false;
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    for (int i = 0; i < 4; i++)
    {
        // Right Button
        rightButtonOverride = true;
        vTaskDelay(pdMS_TO_TICKS(100));
        rightButtonOverride = false;
        vTaskDelay(pdMS_TO_TICKS(400));
    }

    vTaskDelay(pdMS_TO_TICKS(6000));

    // Down Button
    nextButtonOverride = true;
    vTaskDelay(pdMS_TO_TICKS(100));
    nextButtonOverride = false;
    vTaskDelay(pdMS_TO_TICKS(100));

    vTaskDelay(pdMS_TO_TICKS(1000));

    for (int i = 0; i < 2; i++)
    {
        // Right Button
        rightButtonOverride = true;
        vTaskDelay(pdMS_TO_TICKS(100));
        rightButtonOverride = false;
        vTaskDelay(pdMS_TO_TICKS(400));
    }

    Serial.println("demo end");
    //vTaskResume(wifiTaskHandle);
    vTaskDelete(NULL);
}

void demoMode(){
    xTaskCreate(
        demoMovements,
        "DEMO MODE",
        4096,
        NULL,
        1,
        NULL
    );
}