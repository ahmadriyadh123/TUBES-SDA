#include "raylib.h"
#include "map.h"
#include "enemy.h"
#include "tower.h"
#include "level_editor.h"
#include "state.h"
#include "economy.h"
#include "main_menu.h"
#include "transition.h"
#include <stdio.h>
#include <math.h>
extern EnemyWave currentWave;
int customWaveCount = 5;
extern Tower *towersListHead;
extern Tower *selectedTowerForDeletion;
extern Vector2 deleteButtonScreenPos;
extern bool deleteButtonVisible;
extern Texture2D deleteButtonTex;
extern float DELETE_BUTTON_DRAW_SCALE;
static int currentWaveNum = 1;
static float currentTileScale = 1.0f;
static float mapScreenOffsetX = 0.0f;
static float mapScreenOffsetY = 0.0f;
static bool gameplayInitialized = false;
static bool editorInitialized = false;
Music regularBacksound;
Music battleBacksound;

int main(void)
{
    InitWindow(2560, 1575, "Tower Defense");
    InitAudioDevice();
    SetTargetFPS(60);
    LoadMainMenuResources();
    InitGameState();
    InitMapAssets();
    InitTowerAssets();
    Enemies_InitAssets();
    InitWaveAssets();
    SetMoney(100);
    Vector2 mousePos = {0};

    regularBacksound = LoadMusicStream("assets/audio/regular.wav");
    if (regularBacksound.stream.buffer == NULL)
    {
        TraceLog(LOG_WARNING, "Failed to load regular.wav");
    }
    else
    {
        SetMusicVolume(regularBacksound, 2.0f);
    }
    battleBacksound = LoadMusicStream("assets/audio/battle.wav");
    if (battleBacksound.stream.buffer == NULL)
    {
        TraceLog(LOG_WARNING, "Failed to load battle.wav");
    }
    else
    {
        SetMusicVolume(battleBacksound, 2.0f);
    }

    while (!WindowShouldClose() && currentGameState != EXITING)
    {
        float deltaTime = GetFrameTime();
        mousePos = GetMousePosition();
        if (regularBacksound.stream.buffer != NULL)
        {
            UpdateMusicStream(regularBacksound);
        }
        if (battleBacksound.stream.buffer != NULL)
        {
            UpdateMusicStream(battleBacksound);
        }

        float screenWidth = (float)GetScreenWidth();
        float screenHeight = (float)GetScreenHeight();
        float availableWidth = screenWidth * 1.0f;
        float availableHeight = screenHeight * 1.0f;
        float baseMapWidth = MAP_COLS * TILE_SIZE;
        float baseMapHeight = MAP_ROWS * TILE_SIZE;
        currentTileScale = fmin(availableWidth / baseMapWidth, availableHeight / baseMapHeight);
        mapScreenOffsetX = (screenWidth - baseMapWidth * currentTileScale) / 2.0f;
        mapScreenOffsetY = (screenHeight - baseMapHeight * currentTileScale) / 2.0f;
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentGameState)
        {
        case MAIN_MENU:
            if (regularBacksound.stream.buffer != NULL && !IsMusicStreamPlaying(regularBacksound))
            {
                PlayMusicStream(regularBacksound);
            }
            if (battleBacksound.stream.buffer != NULL && IsMusicStreamPlaying(battleBacksound))
            {
                StopMusicStream(battleBacksound);
            }
            DrawMainMenu();
            HandleMainMenuInput();
            break;
        case GAMEPLAY:
            if (!gameplayInitialized)
            {
                PlayTransitionAnimation(GAMEPLAY);
                Enemies_BuildPath(2, 0);
                if (GetPathCount() == 0)
                {
                    TraceLog(LOG_ERROR, "No path found after building! Enemies will not move.");
                }

                currentWaveNum = 1;
                currentWave = CreateWave(0, 2);
                SetWaveTimerCurrentTime(&currentWave, 0.0f);
                SetWaveTimerDuration(&currentWave, WAVE_TIMER_DURATION);
                SetWaveTimerVisible(&currentWave, true);
                SetWaveActive(&currentWave, false);
                SetTimerMapRow(&currentWave, 0);
                SetTimerMapCol(&currentWave, 2);
                SetMoney(100);
                SetLife(10);
                gameplayInitialized = true;
                TraceLog(LOG_INFO, "Gameplay initialized with custom map.");
            }

            UpdateWaveTimer(&currentWave, deltaTime);
            bool isWaveActive = GetWaveActive(&currentWave);
            if (isWaveActive)
            {
                if (battleBacksound.stream.buffer != NULL && !IsMusicStreamPlaying(battleBacksound))
                {
                    PlayMusicStream(battleBacksound);
                }
                if (regularBacksound.stream.buffer != NULL && IsMusicStreamPlaying(regularBacksound))
                {
                    StopMusicStream(regularBacksound);
                }
            }
            else
            {
                if (regularBacksound.stream.buffer != NULL && !IsMusicStreamPlaying(regularBacksound))
                {
                    PlayMusicStream(regularBacksound);
                }
                if (battleBacksound.stream.buffer != NULL && IsMusicStreamPlaying(battleBacksound))
                {
                    StopMusicStream(regularBacksound);
                }
            }

            if (GetWaveActive(&currentWave))
            {
                Enemies_Update(&currentWave, deltaTime);
                UpdateTowerAttacks(&currentWave, deltaTime);
            }
            if (AllEnemiesInWaveFinished(&currentWave))
            {
                TraceLog(LOG_INFO, "Wave %d completed. Checking for next wave...", currentWaveNum);
                FreeWave(&currentWave);
                currentWaveNum++;
                if (currentWaveNum <= customWaveCount)
                {
                    currentWave = CreateWave(0, 2);
                    SetWaveTimerCurrentTime(&currentWave, 0.0f);
                    SetWaveTimerDuration(&currentWave, WAVE_TIMER_DURATION);
                    SetWaveTimerVisible(&currentWave, true);
                    SetWaveActive(&currentWave, false);
                    TraceLog(LOG_INFO, "Starting next wave: Wave %d of %d.", currentWaveNum, customWaveCount);
                }
                else
                {
                    TraceLog(LOG_INFO, "All %d waves completed! Level complete!", customWaveCount);
                    PlayTransitionAnimation(LEVEL_COMPLETE);
                    currentGameState = LEVEL_COMPLETE;
                }
            }
            if (IsGameOver())
            {
                PlayTransitionAnimation(GAME_OVER);
                currentGameState = GAME_OVER;
                FreeWave(&currentWave);
                gameplayInitialized = false;
                TraceLog(LOG_INFO, "Game over. Switched to GAME_OVER after transition.");
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                int col = (int)((mousePos.x - mapScreenOffsetX) / (TILE_SIZE * currentTileScale));
                int row = (int)((mousePos.y - mapScreenOffsetY) / (TILE_SIZE * currentTileScale));
                bool handledClick = false;

                if (deleteButtonVisible && selectedTowerForDeletion != NULL)
                {
                    float buttonDrawWidth = deleteButtonTex.width * DELETE_BUTTON_DRAW_SCALE;
                    float buttonDrawHeight = deleteButtonTex.height * DELETE_BUTTON_DRAW_SCALE;
                    Rectangle deleteButtonRect = {
                        deleteButtonScreenPos.x - buttonDrawWidth / 2.0f,
                        deleteButtonScreenPos.y - buttonDrawHeight / 2.0f,
                        buttonDrawWidth, buttonDrawHeight};

                    if (CheckCollisionPointRec(mousePos, deleteButtonRect))
                    {
                        SellTower(selectedTowerForDeletion);
                        TraceLog(LOG_INFO, "Delete button clicked. Tower sold.");
                        handledClick = true;
                    }
                }
                if (!handledClick)
                {
                    int timerMapRow = GetTimerMapRow(&currentWave);
                    int timerMapCol = GetTimerMapCol(&currentWave);
                    float tileScreenSize = TILE_SIZE * currentTileScale;
                    Rectangle timerAreaRect = {
                        mapScreenOffsetX + timerMapCol * tileScreenSize,
                        mapScreenOffsetY + timerMapRow * tileScreenSize,
                        tileScreenSize,
                        tileScreenSize};
                    if (CheckCollisionPointRec(mousePos, timerAreaRect) && GetWaveTimerVisible(&currentWave))
                    {
                        SetWaveTimerCurrentTime(&currentWave, GetWaveTimerDuration(&currentWave));
                        TraceLog(LOG_INFO, "Wave timer accelerated by click.");
                        handledClick = true;
                    }
                }

                Tower *clickedTower = NULL;
                if (!handledClick)
                {
                    Tower *currentTowerNode = towersListHead;
                    while (currentTowerNode != NULL)
                    {
                        float towerDrawWidth = currentTowerNode->frameWidth * TOWER_DRAW_SCALE * currentTileScale;
                        float towerDrawHeight = currentTowerNode->frameHeight * TOWER_DRAW_SCALE * currentTileScale;
                        Rectangle towerClickRect = {
                            mapScreenOffsetX + (currentTowerNode->position.x * currentTileScale) - (towerDrawWidth / 2.0f),
                            mapScreenOffsetY + (currentTowerNode->position.y * currentTileScale) - towerDrawHeight + (TOWER_Y_OFFSET_PIXELS * currentTileScale),
                            towerDrawWidth,
                            towerDrawHeight};
                        if (CheckCollisionPointRec(mousePos, towerClickRect))
                        {
                            clickedTower = currentTowerNode;
                            handledClick = true;
                            break;
                        }
                        currentTowerNode = (Tower *)currentTowerNode->next;
                    }
                }

                if (handledClick)
                {
                    if (clickedTower != NULL)
                    {

                        if (selectedTowerForDeletion == clickedTower && deleteButtonVisible)
                        {
                            HideDeleteButton();
                            TraceLog(LOG_INFO, "Clicked on same tower. Hiding delete button.");
                        }
                        else
                        {

                            selectedTowerForDeletion = clickedTower;
                            Vector2 towerCenterPos = {
                                mapScreenOffsetX + (clickedTower->position.x * currentTileScale),
                                mapScreenOffsetY + (clickedTower->position.y * currentTileScale)};
                            deleteButtonScreenPos = towerCenterPos;
                            deleteButtonVisible = true;
                            TraceLog(LOG_INFO, "Clicked on tower at (%d, %d). Displaying delete button.", clickedTower->row, clickedTower->col);
                        }
                    }
                    else
                    {
                    }
                }
                else
                {
                    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS && GetMapTile(row, col) == 4 && GetTowerAtMapCoord(row, col) == NULL)
                    {
                        PlaceTower(row, col, TOWER_TYPE_1);
                        TraceLog(LOG_INFO, "Tower placed at (%d, %d).", row, col);
                    }
                    else
                    {

                        HideDeleteButton();
                        TraceLog(LOG_INFO, "Clicked outside any relevant area. Clearing UI.");
                    }
                }
            }

            DrawMap(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
            if (GetWaveTotal(&currentWave) > 0)
            {
                Enemies_Draw(&currentWave, currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
            }
            DrawTowers(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
            DrawGameTimer(&currentWave, currentTileScale, mapScreenOffsetX, mapScreenOffsetY, GetTimerMapRow(&currentWave), GetTimerMapCol(&currentWave));
            DrawLife();
            char moneyText[32];
            snprintf(moneyText, sizeof(moneyText), "Money: $%d", GetMoney());
            DrawText(moneyText, 10, 10, 20, BLACK);
            break;
        case LEVEL_EDITOR:
            if (regularBacksound.stream.buffer != NULL && !IsMusicStreamPlaying(regularBacksound))
            {
                PlayMusicStream(regularBacksound);
            }
            if (battleBacksound.stream.buffer != NULL && IsMusicStreamPlaying(battleBacksound))
            {
                StopMusicStream(battleBacksound);
            }
            if (!editorInitialized)
            {
                PlayTransitionAnimation(LEVEL_EDITOR);
                InitializeLevelEditor(gameMap);
                editorInitialized = true;
                TraceLog(LOG_INFO, "Level editor initialized after transition.");
            }
            HandleLevelEditorInput(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
            DrawLevelEditor(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
            if (GetEditorRequestSaveAndPlay())
            {
                SetEditorRequestSaveAndPlay(false);
                editorInitialized = false;
                UnloadLevelEditor();

                if (LoadLevelFromFile(GetEditorMapFileName()))
                {

                    for (int r = 0; r < MAP_ROWS; r++)
                    {
                        for (int c = 0; c < MAP_COLS; c++)
                        {
                            SetMapTile(r, c, GetEditorMapTile(r, c));
                        }
                    }
                    customWaveCount = GetEditorWaveCount();

                    currentWaveNum = 1;
                    currentWave = CreateWave(0, 2);
                    SetWaveTimerCurrentTime(&currentWave, 0.0f);
                    SetWaveTimerVisible(&currentWave, true);
                    SetWaveActive(&currentWave, false);
                    gameplayInitialized = false;
                    PlayTransitionAnimation(GAMEPLAY);
                    currentGameState = GAMEPLAY;
                    TraceLog(LOG_INFO, "Editor: Map loaded and switched to GAMEPLAY after transition.");
                }
                else
                {
                    TraceLog(LOG_WARNING, "Editor: Failed to load map. Returning to MAIN_MENU after transition.");
                    PlayTransitionAnimation(MAIN_MENU);
                    currentGameState = MAIN_MENU;
                }
            }
            break;
        case SETTINGS:
            if (regularBacksound.stream.buffer != NULL && !IsMusicStreamPlaying(regularBacksound))
            {
                PlayMusicStream(regularBacksound);
            }
            if (battleBacksound.stream.buffer != NULL && IsMusicStreamPlaying(battleBacksound))
            {
                StopMusicStream(battleBacksound);
            }
            DrawText("SETTINGS SCREEN", screenWidth / 2 - MeasureText("SETTINGS SCREEN", 40) / 2, screenHeight / 2 - 20, 40, BLUE);
            Rectangle backButtonSettings = {screenWidth / 2 - 100, screenHeight / 2 + 30, 200, 40};
            DrawRectangleRec(backButtonSettings, LIGHTGRAY);
            DrawText("Back to Main Menu", backButtonSettings.x + 20, backButtonSettings.y + 10, 20, BLACK);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, backButtonSettings))
            {
                PlayTransitionAnimation(MAIN_MENU);
                currentGameState = MAIN_MENU;
                TraceLog(LOG_INFO, "Settings: Returning to MAIN_MENU after transition.");
            }
            break;
        case LEVEL_COMPLETE:
        {
            DrawText("Level Complete!", screenWidth / 2 - MeasureText("Level Complete!", 40) / 2, screenHeight / 2 - 20, 40, GREEN);
            Rectangle menuButtonLevelComplete = {screenWidth / 2 - 100, screenHeight / 2 + 30, 200, 40};
            DrawRectangleRec(menuButtonLevelComplete, LIGHTGRAY);
            DrawText("Main Menu", menuButtonLevelComplete.x + 50, menuButtonLevelComplete.y + 10, 20, BLACK);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, menuButtonLevelComplete))
            {
                currentGameState = MAIN_MENU;
                gameplayInitialized = false;
                TraceLog(LOG_INFO, "Level Complete: Returning to MAIN_MENU after transition.");
            }
            break;
        }
        case GAME_OVER:
        {
            DrawText("Game Over!", screenWidth / 2 - MeasureText("Game Over!", 40) / 2, screenHeight / 2 - 20, 40, RED);
            Rectangle menuButtonGameOver = {screenWidth / 2 - 100, screenHeight / 2 + 30, 200, 40};
            DrawRectangleRec(menuButtonGameOver, LIGHTGRAY);
            DrawText("Main Menu", menuButtonGameOver.x + 50, menuButtonGameOver.y + 10, 20, BLACK);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, menuButtonGameOver))
            {
                currentGameState = MAIN_MENU;
                gameplayInitialized = false;
                TraceLog(LOG_INFO, "Game Over: Returning to MAIN_MENU after transition.");
            }
            break;
        }
        case EXITING:

            break;
        }
        EndDrawing();
    }

    UnloadMainMenuResources();
    FreeWave(&currentWave);
    UnloadLevelEditor();
    ShutdownWaveAssets();
    Enemies_ShutdownAssets();
    ShutdownTowerAssets();
    ShutdownMapAssets();
    if (regularBacksound.stream.buffer != NULL)
    {
        UnloadMusicStream(regularBacksound);
    }
    if (battleBacksound.stream.buffer != NULL)
    {
        UnloadMusicStream(battleBacksound);
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}