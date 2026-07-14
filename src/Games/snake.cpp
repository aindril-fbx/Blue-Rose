#include <Arduino.h>
#include <U8g2lib.h>
#include <buttonBehav.h>
#include <esp_random.h>
#include <GameMacro.h>
#include <games.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

#pragma region "Sprites"
static const unsigned char image_Body_bits[] U8X8_PROGMEM = {0x3c, 0x3c, 0xc3, 0xc3, 0xc3, 0xc3, 0x3c, 0x3c};
static const unsigned char image_Food_bits[] U8X8_PROGMEM = {0x0f, 0x0f, 0x0f, 0x0f};
static const unsigned char image_Head_bits[] U8X8_PROGMEM = {0x3c, 0x3c, 0xff, 0xff, 0xff, 0xff, 0x3c, 0x3c};
// ' SankeIcon', 16x16px
static const unsigned char image_SnakeIcon_bits[] U8X8_PROGMEM = {0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x1f, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x3f, 0xc0, 0x17, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xc0, 0x15, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xc1, 0x13, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xc0, 0x11, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x3f, 0xc0, 0x10, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0xc0, 0x1f, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0xfc, 0xff, 0xff, 0x01, 0x00, 0x00, 0x3e, 0x00, 0xfe, 0xff, 0xff, 0x03, 0x00, 0x00, 0x3e, 0x00, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x3e, 0x00, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x3e, 0x00, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x3e, 0x00, 0x3f, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x3e, 0x00, 0x1f, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x3e, 0x00, 0x1f, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x3e, 0x00, 0x1f, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x7e, 0x80, 0x1f, 0x00, 0xc0, 0x07, 0x00, 0x00, 0xfe, 0xff, 0x1f, 0x00, 0xc0, 0x07, 0x00, 0x00, 0xfe, 0xff, 0x1f, 0x00, 0xc0, 0x07, 0x00, 0x00, 0xfe, 0xff, 0x1f, 0x00, 0xc0, 0x07, 0x00, 0x00, 0xfc, 0xff, 0x0f, 0x00, 0xc0, 0x07, 0x00, 0x00, 0xf8, 0xff, 0x07, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x04, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00};

static const unsigned char image_arrow_down_bits[] U8X8_PROGMEM = {0x04,0x04,0x04,0x04,0x15,0x0e,0x04};
static const unsigned char image_arrow_up_bits[] U8X8_PROGMEM = {0x04,0x0e,0x15,0x04,0x04,0x04,0x04};
#pragma endregion

const int maxLength = 128;
int snake[maxLength][2] = {{2, 3}, {2, 2}};
int currentLength = 2;
int gameStarted = 0;
int direction[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
int foodPos[20][2];
int foodIndex = 0;
int directionIndex = 1;

bool initGrid = 0;
void snakeCleanup();

extern void backtoMenu(void);
void Quit(void) {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // string 4
    u8g2.setFont(u8g2_font_6x13O_tr);
    u8g2.drawStr(5, 16, "# PAUSED");
    // arrow_up
    u8g2.drawXBMP(77, 43, 5, 7, image_arrow_up_bits);
    // arrow_down
    u8g2.drawXBMP(51, 27, 5, 7, image_arrow_down_bits);
    // string 7
    u8g2.setFont(u8g2_font_haxrcorp4089_tr);
    u8g2.drawStr(6, 34, "RESUME");
    // string 7 copy 1
    u8g2.drawStr(6, 51, "BACK TO MENU");
    playBuzzer(false);
    u8g2.sendBuffer();
    while(true){
        if(upButtonHold()){
            u8g2.clearBuffer();
            gamesMenu(1);
            delay(200);
            break;
        }else if(downButtonHold()){
            delay(200);
            break;
        }
        delay(100);
    }
}



bool isOpposite(int newDir, int currDir)
{
    return (
        direction[newDir][0] + direction[currDir][0] == 0 &&
        direction[newDir][1] + direction[currDir][1] == 0);
}

bool snakeStarted = true;

extern int afkTime;
bool won = false;
void endScreen()
{
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    // string 1
    u8g2.setFont(u8g2_font_6x13O_tr);

    if (won)
    {
        u8g2.drawStr(40, 33, "YOU WIN!");
    }
    else
    {
        u8g2.drawStr(40, 33, "YOU LOSE!");
    }

    // string 2
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(34, 53, "> to RESTART");

    if (rightButtonHold() && !snakeStarted)
    {
        snakeCleanup();
    }

    u8g2.sendBuffer();
}

void snakeGame()
{
    if(selectButtonTap()){
        Quit();
    }
    afkTime = 0;
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
        directionIndex = changeDir;
    }

    static int posX = 0, posY = 0;
    static unsigned long lastFrame = 0;
    const unsigned long frameInterval = 400;
    if (!snakeStarted)
        endScreen();
    if (millis() - lastFrame >= frameInterval && snakeStarted)
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

        int foodSpawnX = esp_random() % 16;
        int foodSpawnY = esp_random() % 8;

        bool foodInside = true;
        int maxFood = 5;
        while (foodInside && (maxLength - currentLength > maxFood))
        {
            foodInside = false;
            for (int i = 0; i < currentLength; i++)
            {
                if (foodSpawnX == snake[i][0] && foodSpawnY == snake[i][1])
                {
                    foodInside = true;
                    break;
                }
            }
            for (int i = 0; i < foodIndex; i++)
            {
                if (foodSpawnX == foodPos[i][0] && foodSpawnY == foodPos[i][1])
                {
                    foodInside = true;
                    break;
                }
            }
            if (!foodInside)
                break;
            foodSpawnX = (foodSpawnX + 1) % 16;
            if (foodSpawnX == 15)
            {
                foodSpawnY = (foodSpawnY + 1) % 8;
            }
            Serial.print(foodSpawnX);
            Serial.print(" , ");
            Serial.println(foodSpawnY);
        }

        for (int i = 1; i < currentLength; i++)
        {
            if (snake[0][0] == snake[i][0] && snake[0][1] == snake[i][1])
            {
                snakeStarted = false;
                delay(500);
            }
        }
        int tempMaxfood = (maxLength - currentLength > maxFood) ? maxFood : 2;
        Serial.println(tempMaxfood);
        if (foodIndex < tempMaxfood)
        {
            foodPos[foodIndex][0] = foodSpawnX;
            foodPos[foodIndex][1] = foodSpawnY;
            foodIndex++;
        }

        for (int i = 0; i < foodIndex; i++)
        {
            if (snake[0][0] == foodPos[i][0] && snake[0][1] == foodPos[i][1])
            {
                for (int j = i; j < foodIndex; j++)
                {
                    foodPos[j][0] = foodPos[j + 1][0];
                    foodPos[j][1] = foodPos[j + 1][1];
                }

                currentLength++;
                snake[currentLength - 1][0] = snake[currentLength - 2][0];
                snake[currentLength - 1][1] = snake[currentLength - 2][1];
                foodIndex--;
                break;
            }
        }
    }
    if (currentLength == (maxLength - 1) && snakeStarted)
    {
        Serial.println("Won!");
        won = true;
        snakeStarted = false;
    }
    if (snakeStarted)
    {
        for (int i = 0; i < foodIndex; i++)
        {
            u8g2.drawXBMP(foodPos[i][0] * 8 + 2, foodPos[i][1] * 8 + 2, 4, 4, image_Food_bits);
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
        // u8g2.drawFrame(0, 0, 128, 64);
        u8g2.sendBuffer();
    }
}

void snakeCleanup()
{
    currentLength = 2;
    snakeStarted = true;
    won = false;
    foodPos;
    directionIndex = 1;
    initGrid = 0;
    foodIndex = 0;
    for (int i = 0; i < currentLength; i++)
    {
        snake[i][0] = 2;
        snake[i][1] = currentLength - i;
    }
}

Game snakeG = {
    "SNAKE",
    "GAME",
    image_SnakeIcon_bits,
    snakeGame,
    snakeGame,
    snakeCleanup,
};

REGISTER_GAME(snakeG);