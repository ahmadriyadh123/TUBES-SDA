#include "economy.h"
#include "raylib.h"

static int money = 0;

void InitEconomy() {
    money = 100;
}

void AddMoney(int amount) {
    money += amount;
}

void SpendMoney(int amount) {
    if (money >= amount) money -= amount;
}

int GetMoney() {
    return money;
}

void DrawMoney() {
    DrawText(TextFormat("Money: $%d", money), 10, 10, 20, DARKGREEN);
}
