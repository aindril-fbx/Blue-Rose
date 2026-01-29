#include <Arduino.h>
#include <Wire.h>   

extern int previousButtonPressed;
extern int selectButtonPressed;
extern int nextButtonPressed;
extern int leftButtonPressed;
extern int rightButtonPressed;

// Select Button
extern const int selectButton;
int selectButtonTap(){
    if(!digitalRead(selectButton) && !selectButtonPressed){
        selectButtonPressed = 1;
        Serial.println("Select Button Pressed");
        return 1;
    }
    return 0;
}

int selectButtonHold(){
    if(!digitalRead(selectButton)){
        return 1;
    }
    return 0;
}

// Up Button
extern const int upButton;
int upButtonTap(){
    if(!digitalRead(upButton) && !previousButtonPressed){
        previousButtonPressed = 1;
        Serial.println("Up Button Pressed");
        return 1;
    }
    return 0;
}

int upButtonHold(){
    if(!digitalRead(upButton)){
        return 1;
    }
    return 0;
}

// Down Button
extern const int downButton;
int downButtonTap(){
    if(!digitalRead(downButton) && !nextButtonPressed){
        nextButtonPressed = 1;
        Serial.println("Down Button Pressed");
        return 1;
    }
    return 0;
}

int downButtonHold(){
    if(!digitalRead(downButton)){
        return 1;
    }
    return 0;
}

// Left Button
extern const int leftButton;
int leftButtonTap(){
    if(!digitalRead(leftButton) && !leftButtonPressed){
        leftButtonPressed = 1;
        Serial.println("Left Button Pressed");
        return 1;
    }
    return 0;
}

int leftButtonHold(){
    if(!digitalRead(leftButton)){
        return 1;
    }
    return 0;
}

// Right Button
extern const int rightButton;
int rightButtonTap(){
    if(!digitalRead(rightButton) && !rightButtonPressed){
        rightButtonPressed = 1;
        Serial.println("Right Button Pressed");
        return 1;
    }
    return 0;
}

int rightButtonHold(){
    if(!digitalRead(rightButton)){
        return 1;
    }
    return 0;
}

void resetButtonStates(){
    if(digitalRead(upButton)){
        previousButtonPressed = 0; // reset the previous button pressed state
    }
    if(digitalRead(downButton)){
        nextButtonPressed = 0; // reset the next button pressed state
    }
    if(digitalRead(selectButton)){
        selectButtonPressed = 0; // reset the select button pressed state
    }
    if(digitalRead(leftButton)){
        leftButtonPressed = 0; // reset the left button pressed state
    }
    if(digitalRead(rightButton)){
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