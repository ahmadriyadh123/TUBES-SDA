#ifndef ECONOMY_H
#define ECONOMY_H
#include "raylib.h"

#define TOWER_COST 30
#define SELL_REFUND_PERCENT 0.7f

void InitEconomy();
void AddMoney(int amount);
void SpendMoney(int amount);
int GetMoney();
void DrawMoney();
void PlayerBuildTower(Vector2 pos);
void PlayerSellTower(Vector2 mouse);

#endif