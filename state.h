#ifndef STATE
#define STATE

#include <stdbool.h>
#include "raylib.h"

void InitGameState();
void DecreaseLife();
int GetLives();
bool IsGameOver();
void DrawLives();

#endif
