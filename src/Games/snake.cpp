#include <Arduino.h>
#include <U8g2lib.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

int snakeLength = 2;
int *snakeX = new int[snakeLength];
int *snakeY = new int[snakeLength];
int gameStarted = 0;
int direction[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
int directionIndex = 0;

void snakeGame(int up, int down, int left, int right)
{
    if (snakeX[0] > 128 || snakeX[0] < 0 || snakeY[0] > 64 || snakeY[0] < 0)
    {
        snakeY[0] = (snakeY[0] + 64) % 64;
    }
    u8g2.clearBuffer();
    u8g2.setDrawColor(1);
    if (!gameStarted)
    {
        gameStarted = 1;
        snakeX[0] = 64;
        snakeY[0] = 32;
        snakeX[1] = 64;
        snakeY[1] = 33;
    }
    if (gameStarted)
    {
        for (int i = 0; i < snakeLength; i++)
        {
            u8g2.drawBox(snakeX[i], snakeY[i], 2, 2);
        }
        for (int i = 0; i < snakeLength; i++)
        {
            int nextX = snakeX[i] + direction[directionIndex][0];
            int nextY = snakeY[i] + direction[directionIndex][1];
            int *snakeXtemp = new int[snakeLength];
            int *snakeYtemp = new int[snakeLength];
            for (int j = 0; j < snakeLength; j++)
            {
                snakeXtemp[j] = snakeX[j];
            }
            for (int j = 0; j < snakeLength; j++)
            {
                snakeYtemp[j] = snakeY[j];
            }
            free(snakeX); // free the old snake positions
            free(snakeY);
            snakeX = (int *)malloc(snakeLength * sizeof(int));
            snakeY = (int *)malloc(snakeLength * sizeof(int));
            if (i == 0)
            {
                snakeX[i] = nextX;
                snakeY[i] = nextY;
            }
            else
            {
                snakeX[i] = snakeXtemp[i - 1];
                snakeY[i] = snakeYtemp[i - 1];
            }
            free(snakeXtemp);
            free(snakeYtemp);
        }

        if (down)
        {
            directionIndex = 0;
        }
        else if (right)
        {
            directionIndex = 1;
        }
        else if (up)
        {
            directionIndex = 2;
        }
        else if (left)
        {
            directionIndex = 3;
        }
    }

    u8g2.sendBuffer();
}