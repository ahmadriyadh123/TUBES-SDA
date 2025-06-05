#include "economy.h"
#include "raylib.h"
#include "raymath.h"  
#include "tempEntities.h"

static int money = 0;

void InitEconomy() {
    money = 100;
}

void AddMoney(int amount) {
    money += amount;
}

void SpendMoney(int amount) {
    if (money >= amount) {
        money -= amount;
    }
}

int GetMoney() {
    return money;
}

void DrawMoney() {
    DrawText(TextFormat("Money: %d G", money), GetScreenWidth() - MeasureText(TextFormat("Money: %d G", money), 20) - 10, 10, 20, GOLD);
}

void PlayerBuildTower(Vector2 pos) {
    if (money < TOWER_COST) return;

    for (int i = 0; i < MAX_TOWERS; i++) {
        if (!towers[i].active) {
            towers[i] = (Tower){ .position = pos, .damage = 10, .range = 100, .active = true };
            money -= TOWER_COST;
            break;
        }
    }
}

void PlayerSellTower(Vector2 mouse) {
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (towers[i].active && CheckCollisionPointCircle(mouse, towers[i].position, 10)) {
            towers[i].active = false;
            money += (int)(TOWER_COST * SELL_REFUND_PERCENT);
            break;
        }
    }
}