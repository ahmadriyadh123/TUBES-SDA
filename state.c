#include "state.h"
#include "raylib.h"

#define STARTING_LIVES 10

static int lives = 0;

void InitGameState() {
    lives = STARTING_LIVES;
}

void DecreaseLife() {
    if (lives > 0) {
        lives--;
    }
}

int GetLives() {
    return lives;
}

bool IsGameOver() {
    return lives <= 0;
}

void DrawLives() {
    DrawText(TextFormat("Lives: %d", lives), 10, 10, 20, MAROON); // Posisi kiri atas
    if (IsGameOver()) { // Menampilkan pesan Game Over jika nyawa habis
        DrawText("GAME OVER!", GetScreenWidth()/2 - MeasureText("GAME OVER!", 40)/2, GetScreenHeight()/2 - 20, 40, RED);
    }
}