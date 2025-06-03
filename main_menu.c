#include "raylib.h"
#include "main_menu.h"




GameState currentGameState = MAIN_MENU;

Texture2D backgroundTex;
Texture2D menuBgTex;
Texture2D startButtonTex;
Texture2D levelEditorButtonTex;
Texture2D settingsButtonTex;
Texture2D exitButtonTex;

#define MENU_BG_DRAW_SCALE 2.0f      
#define MENU_BUTTON_DRAW_SCALE 1.5f  
#define BUTTON_SPACING_PX 5.0f       

void LoadMainMenuResources() {
    backgroundTex = LoadTexture("assets/background.jpg"); 
    if (backgroundTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/background.jpg");
    menuBgTex = LoadTexture("assets/logo.png"); 
    if (menuBgTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/logo.png");
    startButtonTex = LoadTexture("assets/start_button.png");
    if (startButtonTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/start_button.png");
    levelEditorButtonTex = LoadTexture("assets/level_editor_button.png");
    if (levelEditorButtonTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/leveleditor_button.png");
    settingsButtonTex = LoadTexture("assets/guide_button.png");
    if (settingsButtonTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/guide_button.png");
    exitButtonTex = LoadTexture("assets/exit_button.png");
    if (exitButtonTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/exit_button.png");
    TraceLog(LOG_INFO, "Main Menu resources loaded.");
}

void UnloadMainMenuResources() {
    
    if (backgroundTex.id != 0) UnloadTexture(backgroundTex); 
    if (menuBgTex.id != 0) UnloadTexture(menuBgTex);
    if (startButtonTex.id != 0) UnloadTexture(startButtonTex);
    if (levelEditorButtonTex.id != 0) UnloadTexture(levelEditorButtonTex);
    if (settingsButtonTex.id != 0) UnloadTexture(settingsButtonTex);
    if (exitButtonTex.id != 0) UnloadTexture(exitButtonTex);
    TraceLog(LOG_INFO, "Main Menu resources unloaded.");
}

void DrawMainMenu() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    DrawTexturePro(backgroundTex, (Rectangle){0, 0, (float)backgroundTex.width, (float)backgroundTex.height},
                   (Rectangle){0, 0, (float)screenWidth, (float)screenHeight}, (Vector2){0,0}, 0.0f, WHITE);

    float bgTexScaledWidth = menuBgTex.width * MENU_BG_DRAW_SCALE;
    float bgTexScaledHeight = menuBgTex.height * MENU_BG_DRAW_SCALE;
    Rectangle bgTexDestRect = {
        screenWidth / 2.0f - bgTexScaledWidth / 2.0f, 
        screenHeight * 0.05f, 
        bgTexScaledWidth,
        bgTexScaledHeight
    };
    DrawTexturePro(menuBgTex, (Rectangle){0, 0, (float)menuBgTex.width, (float)menuBgTex.height}, bgTexDestRect, (Vector2){0,0}, 0.0f, WHITE);

    float originalButtonWidth = (float)startButtonTex.width;
    float originalButtonHeight = (float)startButtonTex.height;
    float scaledButtonWidth = originalButtonWidth * MENU_BUTTON_DRAW_SCALE;
    float scaledButtonHeight = originalButtonHeight * MENU_BUTTON_DRAW_SCALE;
    float actualButtonSpacing = BUTTON_SPACING_PX * MENU_BUTTON_DRAW_SCALE;

    float buttonsGroupStartY = screenHeight * 0.5f; 

    Rectangle startButtonRect = { screenWidth / 2.0f - scaledButtonWidth / 2.0f, buttonsGroupStartY, scaledButtonWidth, scaledButtonHeight };
    DrawTexturePro(startButtonTex, (Rectangle){0,0,originalButtonWidth,originalButtonHeight}, startButtonRect, (Vector2){0,0}, 0, WHITE);

    Rectangle editorButtonRect = { screenWidth / 2.0f - scaledButtonWidth / 2.0f, buttonsGroupStartY + scaledButtonHeight + actualButtonSpacing, scaledButtonWidth, scaledButtonHeight };
    DrawTexturePro(levelEditorButtonTex, (Rectangle){0,0,originalButtonWidth,originalButtonHeight}, editorButtonRect, (Vector2){0,0}, 0, WHITE);

    Rectangle settingsButtonRect = { screenWidth / 2.0f - scaledButtonWidth / 2.0f, buttonsGroupStartY + (scaledButtonHeight + actualButtonSpacing) * 2, scaledButtonWidth, scaledButtonHeight };
    DrawTexturePro(settingsButtonTex, (Rectangle){0,0,originalButtonWidth,originalButtonHeight}, settingsButtonRect, (Vector2){0,0}, 0, WHITE);

    Rectangle exitButtonRect = { screenWidth / 2.0f - scaledButtonWidth / 2.0f, buttonsGroupStartY + (scaledButtonHeight + actualButtonSpacing) * 3, scaledButtonWidth, scaledButtonHeight };
    DrawTexturePro(exitButtonTex, (Rectangle){0,0,originalButtonWidth,originalButtonHeight}, exitButtonRect, (Vector2){0,0}, 0, WHITE);
}

void HandleMainMenuInput() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { 
        Vector2 mousePos = GetMousePosition();
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        float originalButtonWidth = (float)startButtonTex.width;
        float originalButtonHeight = (float)startButtonTex.height;
        float scaledButtonWidth = originalButtonWidth * MENU_BUTTON_DRAW_SCALE;
        float scaledButtonHeight = originalButtonHeight * MENU_BUTTON_DRAW_SCALE;
        float actualButtonSpacing = BUTTON_SPACING_PX * MENU_BUTTON_DRAW_SCALE;
        
        float buttonsGroupStartY = screenHeight * 0.5f;
        Rectangle startButtonRect = { screenWidth / 2.0f - scaledButtonWidth / 2.0f, buttonsGroupStartY, scaledButtonWidth, scaledButtonHeight };
        Rectangle editorButtonRect = { screenWidth / 2.0f - scaledButtonWidth / 2.0f, buttonsGroupStartY + scaledButtonHeight + actualButtonSpacing, scaledButtonWidth, scaledButtonHeight };
        Rectangle settingsButtonRect = { screenWidth / 2.0f - scaledButtonWidth / 2.0f, buttonsGroupStartY + (scaledButtonHeight + actualButtonSpacing) * 2, scaledButtonWidth, scaledButtonHeight };
        Rectangle exitButtonRect = { screenWidth / 2.0f - scaledButtonWidth / 2.0f, buttonsGroupStartY + (scaledButtonHeight + actualButtonSpacing) * 3, scaledButtonWidth, scaledButtonHeight };

    if (CheckCollisionPointRec(mousePos, startButtonRect)) {
        TraceLog(LOG_INFO, "Start Game Clicked! Initiating transition to GAMEPLAY.");
        
        currentGameState = GAMEPLAY; 
    } else if (CheckCollisionPointRec(mousePos, editorButtonRect)) {
        TraceLog(LOG_INFO, "Level Editor Clicked! Initiating transition to LEVEL_EDITOR.");
        currentGameState = LEVEL_EDITOR;
    } else if (CheckCollisionPointRec(mousePos, settingsButtonRect)) {
        TraceLog(LOG_INFO, "Settings Clicked! Initiating transition to SETTINGS.");
        currentGameState = SETTINGS;
    } else if (CheckCollisionPointRec(mousePos, exitButtonRect)) {
        TraceLog(LOG_INFO, "Exit Clicked! Changing state to EXITING.");
        currentGameState = EXITING;
    }
    }
}