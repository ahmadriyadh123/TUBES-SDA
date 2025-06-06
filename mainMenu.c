#include "mainMenu.h"
#include "raylib.h" // Pastikan raylib.h di-include jika belum dari main_menu.h secara langsung

// GameState currentGameState = MAIN_MENU; // Ini sudah ada di file .c Anda atau di main.c

Texture2D backgroundTex;
Texture2D menuBgTex; // Untuk logo utama "TOWER DEFENSE"
GameState currentGameState = MAIN_MENU; 

// Hapus definisi konstanta skala tombol jika tidak relevan lagi
// #define MENU_BG_DRAW_SCALE 2.0f // Ganti nama atau sesuaikan untuk LOGO
// #define MENU_BUTTON_DRAW_SCALE 1.5f
// #define BUTTON_SPACING_PX 5.0f

// Definisikan properti untuk logo dan tombol baru
#define LOGO_SCALE 0.7f // Sesuaikan skala logo Anda di sini
#define BUTTON_WIDTH_RATIO 0.35f // Lebar tombol sebagai rasio dari lebar layar
#define BUTTON_HEIGHT_RATIO 0.08f // Tinggi tombol sebagai rasio dari tinggi layar
#define BUTTON_SPACING_RATIO 0.03f // Jarak antar tombol sebagai rasio dari tinggi layar
#define BUTTON_FONT_SIZE_RATIO 0.04f // Ukuran font tombol sebagai rasio dari tinggi layar
#define EXIT_TEXT_FONT_SIZE_RATIO 0.06f // Ukuran font untuk teks "EXIT"

// Global atau static untuk menyimpan Rectangle tombol agar bisa diakses HandleMainMenuInput
static Rectangle startButtonRect;
static Rectangle levelEditorButtonRect;
static Rectangle guideButtonRect; // Dulu settingsButtonRect
static Rectangle exitClickRect;


void LoadMainMenuResources() {
    backgroundTex = LoadTexture("assets/background.jpg");
    if (backgroundTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/background.jpg");

    menuBgTex = LoadTexture("assets/logo.png"); // Ini adalah logo "TOWER DEFENSE" dengan perisai
    if (menuBgTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/logo.png");

    // Tidak perlu load tekstur tombol individual lagi
    // startButtonTex = LoadTexture("assets/start_button.png");
    // levelEditorButtonTex = LoadTexture("assets/level_editor_button.png");
    // settingsButtonTex = LoadTexture("assets/guide_button.png");
    // exitButtonTex = LoadTexture("assets/exit_button.png");

    TraceLog(LOG_INFO, "Main Menu resources loaded.");
}

void UnloadMainMenuResources() {
    if (backgroundTex.id != 0) UnloadTexture(backgroundTex);
    if (menuBgTex.id != 0) UnloadTexture(menuBgTex);

    // Tidak perlu unload tekstur tombol individual lagi
    // if (startButtonTex.id != 0) UnloadTexture(startButtonTex);
    // if (levelEditorButtonTex.id != 0) UnloadTexture(levelEditorButtonTex);
    // if (settingsButtonTex.id != 0) UnloadTexture(settingsButtonTex);
    // if (exitButtonTex.id != 0) UnloadTexture(exitButtonTex);

    TraceLog(LOG_INFO, "Main Menu resources unloaded.");
}

void DrawMainMenu() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // 1. Gambar Latar Belakang
    DrawTexturePro(backgroundTex,
                   (Rectangle){0, 0, (float)backgroundTex.width, (float)backgroundTex.height},
                   (Rectangle){0, 0, (float)screenWidth, (float)screenHeight},
                   (Vector2){0,0}, 0.0f, WHITE);

    // 2. Gambar Logo Utama ("TOWER DEFENSE" dengan perisai)
    float logoOriginalWidth = (float)menuBgTex.width;
    float logoOriginalHeight = (float)menuBgTex.height;
    float logoScaledWidth = logoOriginalWidth * LOGO_SCALE;
    float logoScaledHeight = logoOriginalHeight * LOGO_SCALE;

    // Atur posisi logo sedikit lebih ke atas
    Rectangle logoDestRect = {
        screenWidth / 2.0f - logoScaledWidth / 2.0f,
        screenHeight * 0.12f, // Misalnya 12% dari atas layar, sesuaikan!
        logoScaledWidth,
        logoScaledHeight
    };
    DrawTexturePro(menuBgTex,
                   (Rectangle){0, 0, logoOriginalWidth, logoOriginalHeight},
                   logoDestRect, (Vector2){0,0}, 0.0f, WHITE);

    // 3. Gambar Tombol-Tombol (START, LEVEL EDITOR, GUIDE) secara prosedural
    float buttonWidth = screenWidth * BUTTON_WIDTH_RATIO;
    float buttonHeight = screenHeight * BUTTON_HEIGHT_RATIO;
    float buttonSpacing = screenHeight * BUTTON_SPACING_RATIO;
    int buttonFontSize = (int)(screenHeight * BUTTON_FONT_SIZE_RATIO);
    float buttonBorderThickness = 2.5f; // Ketebalan border tombol

    Color buttonBgColor = BLACK;
    Color buttonBorderColor = WHITE;
    Color buttonTextColor = YELLOW;

    // Posisi Y awal untuk tombol pertama, di bawah logo
    float currentButtonY = logoDestRect.y + logoDestRect.height + screenHeight * 0.05f; // Jarak dari logo

    // Tombol START
    startButtonRect = (Rectangle){
        screenWidth / 2.0f - buttonWidth / 2.0f,
        currentButtonY,
        buttonWidth,
        buttonHeight
    };
    DrawRectangleRec(startButtonRect, buttonBgColor);
    DrawRectangleLinesEx(startButtonRect, buttonBorderThickness, buttonBorderColor);
    const char* startText = "START";
    int startTextWidth = MeasureText(startText, buttonFontSize);
    DrawText(startText, (int)(startButtonRect.x + (startButtonRect.width - startTextWidth) / 2),
             (int)(startButtonRect.y + (startButtonRect.height - buttonFontSize) / 2),
             buttonFontSize, buttonTextColor);

    // Tombol LEVEL EDITOR
    currentButtonY += buttonHeight + buttonSpacing;
    levelEditorButtonRect = (Rectangle){
        screenWidth / 2.0f - buttonWidth / 2.0f,
        currentButtonY,
        buttonWidth,
        buttonHeight
    };
    DrawRectangleRec(levelEditorButtonRect, buttonBgColor);
    DrawRectangleLinesEx(levelEditorButtonRect, buttonBorderThickness, buttonBorderColor);
    const char* editorText = "LEVEL EDITOR";
    int editorTextWidth = MeasureText(editorText, buttonFontSize);
    DrawText(editorText, (int)(levelEditorButtonRect.x + (levelEditorButtonRect.width - editorTextWidth) / 2),
             (int)(levelEditorButtonRect.y + (levelEditorButtonRect.height - buttonFontSize) / 2),
             buttonFontSize, buttonTextColor);

    // Tombol GUIDE (menggunakan state SETTINGS)
    currentButtonY += buttonHeight + buttonSpacing;
    guideButtonRect = (Rectangle){
        screenWidth / 2.0f - buttonWidth / 2.0f,
        currentButtonY,
        buttonWidth,
        buttonHeight
    };
    DrawRectangleRec(guideButtonRect, buttonBgColor);
    DrawRectangleLinesEx(guideButtonRect, buttonBorderThickness, buttonBorderColor);
    const char* guideText = "GUIDE";
    int guideTextWidth = MeasureText(guideText, buttonFontSize);
    DrawText(guideText, (int)(guideButtonRect.x + (guideButtonRect.width - guideTextWidth) / 2),
             (int)(guideButtonRect.y + (guideButtonRect.height - buttonFontSize) / 2),
             buttonFontSize, buttonTextColor);

    // 4. Gambar Teks "EXIT" di bagian bawah
    const char* exitText = "EXIT";
    int exitFontSize = (int)(screenHeight * EXIT_TEXT_FONT_SIZE_RATIO);
    int exitTextWidth = MeasureText(exitText, exitFontSize);
    float exitTextY = screenHeight * 0.88f; // Misalnya 88% dari atas layar, sesuaikan!
                                          // Pastikan tidak terlalu rendah hingga tertutup taskbar jika tidak fullscreen.

    DrawText(exitText, (int)(screenWidth / 2.0f - exitTextWidth / 2.0f),
             (int)exitTextY, exitFontSize, YELLOW); // Sesuai gambar, teks EXIT berwarna kuning

    // Definisikan area klik untuk teks EXIT (digunakan di HandleMainMenuInput)
    // Berikan sedikit padding vertikal agar lebih mudah diklik
    float exitClickPaddingY = exitFontSize * 0.2f;
    exitClickRect = (Rectangle){
        screenWidth / 2.0f - exitTextWidth / 2.0f,
        exitTextY - exitClickPaddingY, // Mulai sedikit di atas teks
        (float)exitTextWidth,
        (float)exitFontSize + (2 * exitClickPaddingY) // Tinggi area klik = tinggi font + padding atas & bawah
    };
    // Untuk debug, bisa gambar area klik exit:
    // DrawRectangleLinesEx(exitClickRect, 1.0f, GREEN);
}

void HandleMainMenuInput() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();

        if (CheckCollisionPointRec(mousePos, startButtonRect)) {
            TraceLog(LOG_INFO, "Start Game Clicked! Initiating transition to GAMEPLAY.");
            currentGameState = GAMEPLAY;
        } else if (CheckCollisionPointRec(mousePos, levelEditorButtonRect)) {
            TraceLog(LOG_INFO, "Level Editor Clicked! Initiating transition to LEVEL_EDITOR.");
            currentGameState = LEVEL_EDITOR;
        } else if (CheckCollisionPointRec(mousePos, guideButtonRect)) { // Dulu settingsButtonRect
            TraceLog(LOG_INFO, "Guide (Settings) Clicked! Initiating transition to SETTINGS.");
            currentGameState = SETTINGS; // Tombol "GUIDE" mengarah ke state SETTINGS
        } else if (CheckCollisionPointRec(mousePos, exitClickRect)) { // Cek klik pada area teks EXIT
            TraceLog(LOG_INFO, "Exit Text Clicked! Changing state to EXITING.");
            currentGameState = EXITING;
        }
    }
}