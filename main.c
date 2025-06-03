// main.c
#include "raylib.h"
#include "economy.h"
#include "state.h"
#include "tempEntities.h"
#include "main_menu.h"


// const int screenWidth = 800;
// const int screenHeight = 600;

int main() {
    const int currentScreenWidth = GetMonitorWidth(0); 
    const int currentScreenHeight = GetMonitorHeight(0);

    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(currentScreenWidth, currentScreenHeight, "Tower Defense Game - Fullscreen");
    SetTargetFPS(60);

    // if (!IsWindowFullscreen()) 
    // {
    //     ToggleFullscreen();
    // }

    LoadMainMenuResources();
    InitEconomy();
    InitGameState();
    InitGameEntities();

    // Game Loop 
   while (!WindowShouldClose() && currentGameState != EXITING) { 
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentGameState) {
            case MAIN_MENU:
                DrawMainMenu();
                HandleMainMenuInput();
                break;
            case GAMEPLAY:
                if (!IsGameOver()) {
                    UpdateGameEntities(GetScreenWidth());
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        PlayerBuildTower(GetMousePosition());
                    }
                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        PlayerSellTower(GetMousePosition());
                    }
                }

                DrawLine(0, GetScreenHeight() / 2 + 20, GetScreenWidth(), GetScreenHeight() / 2 + 20, GRAY);
                DrawGameEntities();
                DrawMoney();
                DrawLives();
                break;
            case LEVEL_EDITOR:
                DrawText("LEVEL EDITOR - NOT IMPLEMENTED", GetScreenWidth() / 2 - MeasureText("LEVEL EDITOR - NOT IMPLEMENTED", 20) / 2, GetScreenHeight() / 2 - 10, 20, DARKGRAY);
                if (IsKeyPressed(KEY_ESCAPE)) currentGameState = MAIN_MENU;
                break;
            case SETTINGS:

                DrawText("SETTINGS - NOT IMPLEMENTED", GetScreenWidth() / 2 - MeasureText("SETTINGS - NOT IMPLEMENTED", 20) / 2, GetScreenHeight() / 2 - 10, 20, DARKGRAY);
                if (IsKeyPressed(KEY_ESCAPE)) currentGameState = MAIN_MENU;
                break;
            default:
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}