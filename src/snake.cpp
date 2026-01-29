#include <Arduino.h>
#include <U8g2lib.h> // u8g2 library is used to draw graphics on the OLED display
#include <Wire.h>    // library required for IIC communication

#if CONFIG_IDF_TARGET_ESP32
    // Classic ESP32
    extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
    
#elif CONFIG_IDF_TARGET_ESP32C3
    // ESP32-C3
    extern U8G2_SH1106_128X64_NONAME_F_HW_I2C  u8g2;
#endif

int snakeLength = 2;
int *snakeX = new int[snakeLength];
int *snakeY = new int[snakeLength];
int gameStarted = 0; // flag to check if the snake game has started
int direction[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}}; // directions: up, right, down, left
int directionIndex = 0; // current direction index, 0 = up, 1 = right, 2 = down, 3 = left

void snakeGame(int up, int down,int left, int right){
    if(snakeX[0] > 128 || snakeX[0] < 0 || snakeY[0] > 64 || snakeY[0] < 0){ // if the snake goes out of bounds, wrap around
        snakeX[0] = (snakeX[0] + 128) % 128; // wrap around the x position
        snakeY[0] = (snakeY[0] + 64) % 64; // wrap around the y position
    }
    u8g2.clearBuffer(); // clear the internal memory
    u8g2.setDrawColor(1); // set the drawing color to white
    if(!gameStarted){
        gameStarted = 1;
        snakeX[0] = 64; // set the initial position of the snake
        snakeY[0] = 32; // set the initial position of the snake
        snakeX[1] = 64; // set the initial position of the snake
        snakeY[1] = 33; // set the initial position of the snake
    }
    if(gameStarted){
        for(int i = 0; i < snakeLength; i++){
            u8g2.drawBox(snakeX[i], snakeY[i], 2, 2); // draw the snake
        }
        for(int i = 0; i < snakeLength; i++){
            int nextX = snakeX[i] + direction[directionIndex][0]; // calculate the next x position
            int nextY = snakeY[i] + direction[directionIndex][1];
            int *snakeXtemp = new int[snakeLength]; // temporary array to store the snake positions
            int *snakeYtemp = new int[snakeLength];
            for(int j = 0; j < snakeLength; j++){
                snakeXtemp[j] = snakeX[j];
            }
            for(int j = 0; j < snakeLength; j++){
                snakeYtemp[j] = snakeY[j];
            }
            free(snakeX); // free the old snake positions
            free(snakeY);
            snakeX = (int*)malloc(snakeLength * sizeof(int)); // allocate memory for the new snake positions
            snakeY = (int*)malloc(snakeLength * sizeof(int)); // allocate memory for the new snake positions
            if(i == 0){ // if this is the head of the snake, update the position
                snakeX[i] = nextX; // update the head position
                snakeY[i] = nextY; // update the head position
            }else{
                snakeX[i] = snakeXtemp[i-1]; // update the body positions
                snakeY[i] = snakeYtemp[i-1]; // update the body positions
            }
            free(snakeXtemp); // free the temporary array
            free(snakeYtemp); // free the temporary array
        }
        
        if(down){ // if the right button is pressed, change the direction to the right
            directionIndex = 0;
        }else if(right){ // if the left button is pressed, change the direction to the left
            directionIndex = 1;
        }else if(up){ // if the down button is pressed, change the direction to down
            directionIndex = 2;
        }else if(left){ // if the up button is pressed, change the direction to up
            directionIndex = 3;
        }
    }

    u8g2.sendBuffer(); // transfer internal memory to the display
}