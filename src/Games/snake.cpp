#include <Arduino.h>
#include <U8g2lib.h>
#include <buttonBehav.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

#pragma region "Sprites"
static const unsigned char image_Body_bits[] U8X8_PROGMEM = {0x3c, 0x3c, 0xc3, 0xc3, 0xc3, 0xc3, 0x3c, 0x3c};
static const unsigned char image_Food_bits[] U8X8_PROGMEM = {0x0f, 0x0f, 0x0f, 0x0f};
static const unsigned char image_Head_bits[] U8X8_PROGMEM = {0x3c, 0x3c, 0xff, 0xff, 0xff, 0xff, 0x3c, 0x3c};
#pragma endregion

const int maxLength = 128;
int snake[maxLength][2] = {{2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6}};
int currentLength = 6;
int gameStarted = 0;
int direction[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
int directionIndex = 2;

bool initGrid = 0;

bool isOpposite(int newDir, int currDir)
{
    return (
        direction[newDir][0] + direction[currDir][0] == 0 &&
        direction[newDir][1] + direction[currDir][1] == 0);
}

void snakeGame()
{
    u8g2.clearBuffer();
    u8g2.setDrawColor(1);

    int changeDir = directionIndex;

    if (upButtonTap())
         changeDir = 0;
    else if (rightButtonTap())
         changeDir = 1;
    else if (downButtonTap())
         changeDir = 2;
    else if (leftButtonTap())
         changeDir = 3;

    if (!isOpposite(changeDir, directionIndex))
    {
        directionIndex =  changeDir;
    }

    static int posX = 0, posY = 0;
    static unsigned long lastFrame = 0;
    const unsigned long frameInterval = 200;
    if (millis() - lastFrame >= frameInterval)
    {
        lastFrame = millis();
        for (int i = currentLength - 1; i > 0; i--)
        {
            snake[i][0] = snake[i - 1][0];
            snake[i][1] = snake[i - 1][1];
        }

        snake[0][0] += direction[directionIndex][0];
        snake[0][1] += direction[directionIndex][1];

        if (snake[0][0] < 0)
            snake[0][0] = 15;
        if (snake[0][0] > 15)
            snake[0][0] = 0;
        if (snake[0][1] < 0)
            snake[0][1] = 7;
        if (snake[0][1] > 7)
            snake[0][1] = 0;
    }
    for (int i = 0; i < currentLength; i++)
    {
        if (i == 0)
        {
            u8g2.drawXBMP(snake[i][0] * 8, snake[i][1] * 8, 8, 8, image_Head_bits);
        }
        else
        {
            u8g2.drawXBMP(snake[i][0] * 8, snake[i][1] * 8, 8, 8, image_Body_bits);
        }
    }

    u8g2.sendBuffer();
}