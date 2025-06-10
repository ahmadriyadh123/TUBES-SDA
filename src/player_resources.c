#include "player_resources.h"
#include "raylib.h"

static int money = 0;
static int life = 10;

void InitEconomy() {
    money = 0;
    TraceLog(LOG_INFO, "Economy initialized.");
}

void AddMoney(int amount) {
    money += amount;
    if (money < 0) money = 0;
}

int GetMoney() {
    return money;
}

void SetMoney(int amount) {
    money = amount >= 0 ? amount : 0;
    TraceLog(LOG_INFO, "Money set to: $%d.", money);
}

void InitGameState() {
    life = 10;
    TraceLog(LOG_INFO, "Game state initialized with %d lives.", life);
}

void LoseLife() {
    life--;
    if (life < 0) life = 0;
    TraceLog(LOG_INFO, "Lost a life. Remaining lives: %d.", life);
}

bool IsGameOver() {
    bool gameOver = life <= 0;
    if (gameOver) {
        TraceLog(LOG_INFO, "Game over: No lives remaining.");
    }
    return gameOver;
}

int GetLife() {
    return life;
}

void SetLife(int amount) {
    life = amount >= 0 ? amount : 0;
    TraceLog(LOG_INFO, "Lives set to: %d.", life);
}

void DrawLife() {
    char text[32];
    snprintf(text, sizeof(text), "Life: %d", life);
    DrawText(text, 10, 40, 20, BLACK);
}

void DecreaseLife(int amount) {
    life -= amount;
    if (life <= 0) {
        TraceLog(LOG_INFO, "Game over: Life reached zero.");
    }
}