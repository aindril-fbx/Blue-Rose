#ifndef BUTTONBEHAV
#define BUTTONBEHAV

#include <Arduino.h>

int selectButtonTap();
int selectButtonHold();

int upButtonTap();
int upButtonHold();

int downButtonTap();
int downButtonHold();

int leftButtonTap();
int leftButtonHold();

int rightButtonTap();
int rightButtonHold();

void resetButtonStates();

int anyButtonHold();

#endif