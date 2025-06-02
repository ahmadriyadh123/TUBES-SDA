// main.c
#include "raylib.h"
#include "economy.h"
#include "state.h"
#include "tempEntities.h"

// const int screenWidth = 800;
// const int screenHeight = 600;

int main() {
    const int currentScreenWidth = GetMonitorWidth(0); 
    const int currentScreenHeight = GetMonitorHeight(0);

    InitWindow(currentScreenWidth, currentScreenHeight, "Tower Defense Game - Fullscreen");
    SetTargetFPS(60);

    // if (!IsWindowFullscreen()) 
    // {
    //     ToggleFullscreen();
    // }

    InitEconomy();
    InitGameState();
    InitGameEntities();

    // Game Loop 
    while (!WindowShouldClose()) {
        if (!IsGameOver()) {
            UpdateGameEntities(GetScreenWidth());
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                PlayerBuildTower(GetMousePosition());
            }
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                PlayerSellTower(GetMousePosition());
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawLine(0, GetScreenHeight() / 2 + 20, GetScreenWidth(), GetScreenHeight() / 2 + 20, GRAY); 
        DrawGameEntities();
        DrawMoney();       
        DrawLives();       
        EndDrawing();
    }

    CloseWindow();
    return 0;
}