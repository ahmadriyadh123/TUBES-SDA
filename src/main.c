#include "common.h" 
#include "gameplay.h"
#include "level_editor.h"
#include "main_menu.h"
#include "upgrade_tree.h"
#include "research_menu.h"

#include "transition.h"
#include "audio.h"

int main() {
    InitWindow(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, "Tower Defense");
    SetTargetFPS(60); 
    SetTraceLogLevel(LOG_INFO); 
    
    LoadMainMenuResources();  
    InitGameplay();
    LoadProgress(&playerProgress); // <-- 2. TAMBAHKAN PEMUATAN PROGRES DI SINI
    InitUpgradeTree(&tower1UpgradeTree, TOWER_TYPE_1); 
    InitSettingsMenu();
    InitResearchMenu();
    InitGameAudio(); 

    while (!WindowShouldClose() && currentGameState != EXITING) {
        float deltaTime = GetFrameTime(); 
        mousePos = GetMousePosition();

        float screenWidth = (float)VIRTUAL_WIDTH;
        float screenHeight = (float)VIRTUAL_HEIGHT;
        float baseMapWidth = MAP_COLS * TILE_SIZE;
        float baseMapHeight = MAP_ROWS * TILE_SIZE;
        currentTileScale = fmin((float)VIRTUAL_WIDTH / baseMapWidth, (float)VIRTUAL_HEIGHT / baseMapHeight);
        mapScreenOffsetX = (screenWidth - baseMapWidth * currentTileScale) / 2.0f;
        mapScreenOffsetY = (screenHeight - baseMapHeight * currentTileScale) / 2.0f;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        UpdateGameAudio();
        switch (currentGameState){
        case MAIN_MENU:
            PlayRegularMusic();
            selectedCustomMapIndex = -1; 
            SetEditorStartRow(-1);
            SetEditorStartCol(-1);
            SetEditorMapFileName("maps/map.txt"); 
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
        case RESEARCH_MENU:
            UpdateResearchMenu();
            DrawResearchMenu();
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
    FreeUpgradeTree(&tower1UpgradeTree);
    UnloadGameAudio();
    TraceLog(LOG_INFO, "All game modules unloaded.");
    CloseWindow();
    return 0; 
}