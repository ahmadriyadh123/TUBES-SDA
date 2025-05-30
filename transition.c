#include "raylib.h"
#include "transition.h"
#include <string.h>

// Transition function
void PlayTransitionAnimation(GameState targetState) {
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    // Load background texture
    Texture2D backgroundTex = LoadTexture("assets/background.jpg");

    const char* transitionText;
    switch (targetState) {
        case MAIN_MENU: transitionText = "MAIN MENU!"; break;
        case GAMEPLAY: transitionText = "STARTING GAME!"; break;
        case LEVEL_EDITOR: transitionText = "LEVEL EDITOR!"; break;
        case SETTINGS: transitionText = "SETTINGS!"; break;
        default: transitionText = "TRANSITION!"; break;
    }

    int fontSize = 60;
    int len = strlen(transitionText);
    int half = len / 2;

    // Split text
    char leftText[32] = {0}, rightText[32] = {0};
    strncpy(leftText, transitionText, half);
    strcpy(rightText, transitionText + half);

    // Measure text widths
    int leftTextWidth = MeasureText(leftText, fontSize);
    int rightTextWidth = MeasureText(rightText, fontSize);
    int totalTextWidth = leftTextWidth + rightTextWidth;

    // Target positions for text
    Vector2 leftTargetPos = {(screenWidth - totalTextWidth) / 2.0f, (screenHeight - fontSize) / 2.0f};
    Vector2 rightTargetPos = {leftTargetPos.x + leftTextWidth, leftTargetPos.y};

    // Start positions for text off-screen
    Vector2 leftTextPos = {-leftTextWidth, leftTargetPos.y};
    Vector2 rightTextPos = {(float)screenWidth, rightTargetPos.y};

    // Background setup (split into two parts)
    Rectangle leftBackground = {0, 0, backgroundTex.width / 2.0f, (float)backgroundTex.height};
    Rectangle rightBackground = {backgroundTex.width / 2.0f, 0, backgroundTex.width / 2.0f, (float)backgroundTex.height};
    Rectangle leftDest = {-screenWidth / 2.0f, 0, screenWidth / 2.0f, (float)screenHeight};
    Rectangle rightDest = {(float)screenWidth, 0, screenWidth / 2.0f, (float)screenHeight};

    float speed = 30.0f;
    bool backgroundClosed = false;
    bool textArrived = false;

    while (!WindowShouldClose()) {
        // Update background positions
        if (!backgroundClosed) {
            if (leftDest.x < 0) leftDest.x += speed;
            if (rightDest.x > screenWidth / 2.0f) rightDest.x -= speed;
            if (leftDest.x >= 0 && rightDest.x <= screenWidth / 2.0f) {
                backgroundClosed = true;
            }
        }
        // After background closes, move text
        else if (!textArrived) {
            if (leftTextPos.x < leftTargetPos.x) leftTextPos.x += speed;
            else leftTextPos.x = leftTargetPos.x;

            if (rightTextPos.x > rightTargetPos.x) rightTextPos.x -= speed;
            else rightTextPos.x = rightTargetPos.x;

            if (leftTextPos.x == leftTargetPos.x && rightTextPos.x == rightTargetPos.x) {
                textArrived = true;
                break; // Exit transition after text arrives
            }
        }

        // Draw everything
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw background halves
        DrawTexturePro(backgroundTex, leftBackground, leftDest, (Vector2){0, 0}, 0, WHITE);
        DrawTexturePro(backgroundTex, rightBackground, rightDest, (Vector2){0, 0}, 0, WHITE);

        // Draw text after background closes
        if (backgroundClosed) {
            DrawText(leftText, (int)leftTextPos.x, (int)leftTextPos.y, fontSize, WHITE);
            DrawText(rightText, (int)rightTextPos.x, (int)rightTextPos.y, fontSize, WHITE);
        }

        EndDrawing();
    }

    // Unload texture
    UnloadTexture(backgroundTex);
}