#include "common.h" 
#include "gameplay.h"
#include "level_editor.h"
#include "main_menu.h"
#include "audio.h"
#include "transition.h"

int main() {
    
    InitWindow(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, "Tower Defense");

    SetTargetFPS(60); 

    SetTraceLogLevel(LOG_INFO); 
    
    LoadMainMenuResources();  
    InitGameplay();
    InitSettingsMenu();
    InitGameAudio(); 
    PlayRegularMusic();

    while (!WindowShouldClose() && currentGameState != EXITING) {
        float deltaTime = GetFrameTime(); 
        mousePos = GetMousePosition();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        UpdateGameAudio();
        switch (currentGameState){
        case MAIN_MENU:
            HandleMainMenuInput();
            DrawMainMenu();
            break;
        case MAIN_MENU_PLAY_SELECTION:
            UpdatePlaySelectionMenu();
            DrawPlaySelectionMenu();
            break;
        case MAIN_MENU_CUSTOM_MAP_LIST:
            UpdateCustomMapListMenu();        
            DrawCustomMapListMenu();
            break;
        case GAMEPLAY:
            UpdateGameplay(deltaTime);
            HandleGameplayInput(mousePos);
            DrawGameplay();
            break;
        case GAME_PAUSED:
            UpdatePauseMenu();
            DrawGameplay();
            DrawPauseMenu();
            break;
        case LEVEL_EDITOR:
            UpdateEditor();
            DrawLevelEditor(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
            break;
        case LEVEL_EDITOR_SAVE:
            UpdateSaveMapEditor();
            DrawSaveMapEditor();
            break;
        case SETTINGS:
            UpdateSettingsMenu();
            DrawSettingsMenu();
            break;
        case LEVEL_COMPLETE:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ENTER))
            {  
                PlayTransitionAnimation(MAIN_MENU);
                currentGameState = MAIN_MENU;
                UnloadGameplay();
            }
            DrawGameplay(); 
            DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Fade(BLACK, 0.6f));
            
            DrawText("LEVEL COMPLETE!", VIRTUAL_WIDTH / 2 - MeasureText("LEVEL COMPLETE!", 60) / 2, VIRTUAL_HEIGHT / 2 - 40, 60, GOLD);
            DrawText("Click or Press Enter to return to Main Menu", VIRTUAL_WIDTH / 2 - MeasureText("Click or Press Enter to return to Main Menu", 20) / 2, VIRTUAL_HEIGHT / 2 + 40, 20, LIGHTGRAY);
            break;
            break;
        case GAME_OVER:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ENTER))
            {  
                PlayTransitionAnimation(MAIN_MENU);
                currentGameState = MAIN_MENU;
                UnloadGameplay();
            }
            DrawGameplay(); 
            DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Fade(BLACK, 0.7f));
            DrawText("GAME OVER", VIRTUAL_WIDTH / 2 - MeasureText("GAME OVER", 80) / 2, VIRTUAL_HEIGHT / 2 - 60, 80, RED);
            DrawText("Click or Press Enter to return to Main Menu", VIRTUAL_WIDTH / 2 - MeasureText("Click or Press Enter to return to Main Menu", 20) / 2, VIRTUAL_HEIGHT / 2 + 40, 20, LIGHTGRAY);
            break;
        default: 
        break;      
        }

        EndDrawing(); 
    }

    UnloadGameplay();
    UnloadMainMenuResources();
    UnloadLevelEditor();
    UnloadGameAudio();
    TraceLog(LOG_INFO, "All game modules unloaded.");
    CloseWindow();
    return 0; 
}