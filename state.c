#include "state.h"
#include "raylib.h"

#define STARTING_LIVES 10

static int lives = 0;

void InitGameState() {
    lives = STARTING_LIVES;
}

void DecreaseLife() {
    if (lives > 0) lives--;
}

int GetLives() {
    return lives;
}

bool IsGameOver() {
    return lives <= 0;
}

void DrawLives() {
    DrawText(TextFormat("Lives: %d", lives), 10, 40, 20, MAROON);
    if (lives <= 0) {
        DrawText("GAME OVER!", 300, 250, 40, RED);
    }
}
