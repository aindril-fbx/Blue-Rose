#include "GameSystem.h"

#define MAX_GAMES 32

Game* gameList[MAX_GAMES];
int gameCount = 0;

void registerGame(Game* g) {
    if (gameCount < MAX_GAMES) {
        gameList[gameCount++] = g;
    }
}