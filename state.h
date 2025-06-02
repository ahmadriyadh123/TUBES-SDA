#ifndef STATE_H
#define STATE_H

#include <stdbool.h>

void InitGameState();
void DecreaseLife();
int GetLives();
bool IsGameOver();
void DrawLives();

#endif