
#include "raylib.h"
#include "transition.h"
#include "main_menu.h" 
#include <string.h>

void PlayTransitionAnimation(GameState targetState) { 
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    Texture2D backgroundTex = LoadTexture("assets/background.jpg"); 

    const char* transitionText;
    switch (targetState) {
        case MAIN_MENU: transitionText = "MAIN MENU!"; break;
        case GAMEPLAY: transitionText = "STARTING GAME!"; break;
        case LEVEL_EDITOR: transitionText = "LEVEL EDITOR!"; break;
        case SETTINGS: transitionText = "SETTINGS!"; break;
        case LEVEL_COMPLETE: transitionText = "LEVEL COMPLETE!"; break; 
        case GAME_OVER: transitionText = "GAME OVER!"; break;       
        case EXITING: transitionText = "EXITING GAME!"; break;      
        
        default: transitionText = "LOADING..."; break; 
    }
    int fontSize = 60;
    int len = strlen(transitionText);
    int half = len / 2;

    char leftText[32] = {0}, rightText[32] = {0};
    strncpy(leftText, transitionText, half);
    strcpy(rightText, transitionText + half);

    int leftTextWidth = MeasureText(leftText, fontSize);
    int rightTextWidth = MeasureText(rightText, fontSize);
    int totalTextWidth = leftTextWidth + rightTextWidth;

    Vector2 leftTargetPos = {(screenWidth - totalTextWidth) / 2.0f, (screenHeight - fontSize) / 2.0f};
    Vector2 rightTargetPos = {leftTargetPos.x + leftTextWidth, leftTargetPos.y};

    
    Vector2 leftTextPos = {-leftTextWidth * 2.0f, leftTargetPos.y}; 
    Vector2 rightTextPos = {(float)screenWidth + rightTextWidth * 2.0f, rightTargetPos.y}; 

    
    Rectangle leftBackgroundSrc = {0, 0, backgroundTex.width / 2.0f, (float)backgroundTex.height};
    Rectangle rightBackgroundSrc = {backgroundTex.width / 2.0f, 0, backgroundTex.width / 2.0f, (float)backgroundTex.height};
    
    
    Rectangle leftDestRect = {-screenWidth / 2.0f, 0, screenWidth / 2.0f, (float)screenHeight};
    Rectangle rightDestRect = {(float)screenWidth, 0, screenWidth / 2.0f, (float)screenHeight};
    
    float backgroundSpeed = 2000.0f; 
    float textSpeed = 1500.0f;        

    bool backgroundClosed = false;
    bool textArrived = false;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime(); 

        if (!backgroundClosed) {
            leftDestRect.x += backgroundSpeed * deltaTime;
            rightDestRect.x -= backgroundSpeed * deltaTime;

            
            if (leftDestRect.x >= screenWidth / 2.0f - leftDestRect.width && rightDestRect.x <= screenWidth / 2.0f) {
                leftDestRect.x = screenWidth / 2.0f - leftDestRect.width; 
                rightDestRect.x = screenWidth / 2.0f; 
                backgroundClosed = true;
            }
        } else if (!textArrived) {
            leftTextPos.x += textSpeed * deltaTime;
            rightTextPos.x -= textSpeed * deltaTime;

            
            if (leftTextPos.x >= leftTargetPos.x && rightTextPos.x <= rightTargetPos.x) {
                leftTextPos.x = leftTargetPos.x;
                rightTextPos.x = rightTargetPos.x;
                textArrived = true;
                break; 
            }
        }

        BeginDrawing();
        ClearBackground(BLACK); 

        DrawTexturePro(backgroundTex, leftBackgroundSrc, leftDestRect, (Vector2){0, 0}, 0, WHITE);
        DrawTexturePro(backgroundTex, rightBackgroundSrc, rightDestRect, (Vector2){0, 0}, 0, WHITE);
        
        if (backgroundClosed) {
            DrawText(leftText, (int)leftTextPos.x, (int)leftTextPos.y, fontSize, WHITE);
            DrawText(rightText, (int)rightTextPos.x, (int)rightTextPos.y, fontSize, WHITE);
        }
        EndDrawing();
    }

    UnloadTexture(backgroundTex); 
}