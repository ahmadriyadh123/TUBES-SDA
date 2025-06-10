#ifndef PLAYER_RESOURCES_H
#define PLAYER_RESOURCES_H
#include "raylib.h"
#include "stdio.h"

void InitEconomy();
void AddMoney(int amount);
int GetMoney();
void SetMoney(int amount);
void InitGameState();
void LoseLife();
bool IsGameOver();
int GetLife();
void SetLife(int amount);
void DrawLife();
void DecreaseLife(int amount);

#endif