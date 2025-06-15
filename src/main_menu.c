/* File        : main_menu.c 
* Deskripsi   : Implementasi untuk modul Antarmuka Menu Utama (Main Menu). 
*               Berisi semua logika untuk menangani input, memperbarui, dan menggambar 
*               semua layar yang terkait dengan menu utama (menu utama, seleksi game, daftar peta). 
* Dibuat oleh : Mohamad Jibril Fathi Al-Ghifari
* Perubahan terakhir : Senin, 9 Juni 2025
*/

#include "raylib.h"
#include "main_menu.h"
#include "utils.h" 
#include "string.h"
#include "stdio.h"

GameState currentGameState = MAIN_MENU;

Texture2D backgroundTex;
Texture2D menuBgTex;
Texture2D startButtonTex;
Texture2D levelEditorButtonTex;
Texture2D settingsButtonTex;
Texture2D exitButtonTex;
Texture2D backButtonTex; 

CustomMapEntry customMaps[MAX_CUSTOM_MAPS];
int customMapCount = 0;
int selectedCustomMapIndex = -1; 
static int scrollOffset = 0; 

/* I.S. : Aset-aset gambar untuk menu utama (background, tombol, dll.) belum dimuat ke memori.
   F.S. : Semua aset gambar yang diperlukan untuk menu utama telah dimuat ke dalam variabel-variabel Texture2D. */
void LoadMainMenuResources() {
    backgroundTex = LoadTexture("assets/img/mainmenu_imgs/background.jpg"); 
    if (backgroundTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/img/mainmenu_imgs/background.jpg");
    menuBgTex = LoadTexture("assets/img/mainmenu_imgs/logo.png"); 
    if (menuBgTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/img/mainmenu_imgs/logo.png");
    startButtonTex = LoadTexture("assets/img/mainmenu_imgs/start_button.png");
    if (startButtonTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/img/mainmenu_imgs/start_button.png");
    levelEditorButtonTex = LoadTexture("assets/img/mainmenu_imgs/level_editor_button.png");
    if (levelEditorButtonTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/img/mainmenu_imgs/leveleditor_button.png");
    settingsButtonTex = LoadTexture("assets/img/mainmenu_imgs/guide_button.png");
    if (settingsButtonTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/img/mainmenu_imgs/guide_button.png");
    exitButtonTex = LoadTexture("assets/img/mainmenu_imgs/exit_button.png");
    if (exitButtonTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/img/mainmenu_imgs/exit_button.png");
    backButtonTex = LoadTexture("assets/img/level_editor_imgs/back_button.png");
    if (backButtonTex.id == 0) TraceLog(LOG_WARNING, "Failed to load assets/img/leveleditor_imgs/back_button.png");
}

/* I.S. : Aset-aset gambar menu utama sedang berada di dalam memori.
   F.S. : Semua memori GPU yang digunakan oleh tekstur menu utama telah dibebaskan. */
void UnloadMainMenuResources() {
    
    if (backgroundTex.id != 0) UnloadTexture(backgroundTex); 
    if (menuBgTex.id != 0) UnloadTexture(menuBgTex);
    if (startButtonTex.id != 0) UnloadTexture(startButtonTex);
    if (levelEditorButtonTex.id != 0) UnloadTexture(levelEditorButtonTex);
    if (settingsButtonTex.id != 0) UnloadTexture(settingsButtonTex);
    if (exitButtonTex.id != 0) UnloadTexture(exitButtonTex);
    if (backButtonTex.id != 0) UnloadTexture(backButtonTex);
    TraceLog(LOG_INFO, "Main Menu resources unloaded.");
}

/* I.S. : Game berada dalam state MAIN_MENU dan aset telah dimuat.
   F.S. : Tampilan utama dari menu (background, logo, tombol-tombol utama) telah digambar ke layar. */
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

/* I.S. : Game berada dalam state MAIN_MENU, menunggu input dari pemain.
   F.S. : Jika pemain mengklik salah satu tombol utama, currentGameState diubah ke state yang sesuai
          (misalnya, MAIN_MENU_PLAY_SELECTION, LEVEL_EDITOR, dll.). */
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
        TraceLog(LOG_INFO, "Start Game Clicked! Transitioning to play selection menu."); 
        currentGameState = MAIN_MENU_PLAY_SELECTION;
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

/* I.S. : Diberikan parameter untuk posisi, ukuran, teks, dan warna.
   F.S. : Sebuah tombol sederhana berupa persegi panjang dengan teks di tengahnya telah digambar ke layar. */
void DrawButton(Rectangle rect, const char* text, Color bgColor, Color textColor, int fontSize) {
    DrawRectangleRec(rect, bgColor);
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, (float)fontSize, 1);
    DrawText(text, (int)(rect.x + rect.width / 2 - textSize.x / 2), (int)(rect.y + rect.height / 2 - textSize.y / 2), fontSize, textColor);
}

/* Mengirimkan true jika posisi mouse 'mousePos' berada di dalam area persegi 'rect', false jika tidak. */
bool CheckButtonClick(Rectangle rect, Vector2 mousePos) {
    return CheckCollisionPointRec(mousePos, rect);
}

/* I.S. : Daftar peta kustom di dalam program mungkin kosong atau berisi data lama.
   F.S. : Program telah memindai direktori 'maps/', dan daftar peta kustom (array customMaps)
          telah diisi dengan nama-nama file peta yang valid. */
void LoadCustomMapList() {
    customMapCount = 0;
    selectedCustomMapIndex = -1; 
    TraceLog(LOG_INFO, "MAIN_MENU: Loading custom map list from 'maps/' directory.");

    
    FilePathList files = LoadDirectoryFiles("maps/");
    if (files.count == 0) {
        TraceLog(LOG_INFO, "MAIN_MENU: No custom maps found in 'maps/' directory.");
    } else {
        for (unsigned int i = 0; i < files.count; i++) {
            const char* fileName = GetFileName(files.paths[i]);
            
            if (IsFileExtension(fileName, ".txt") && strcmp(fileName, "maps/map.txt") != 0) {
                if (customMapCount < MAX_CUSTOM_MAPS) {
                    StrCopySafe(customMaps[customMapCount].name, fileName, MAP_FILENAME_MAX_LEN);
                    snprintf(customMaps[customMapCount].filePath, MAP_FILENAME_MAX_LEN, "maps/%s", fileName); 
                    TraceLog(LOG_INFO, "MAIN_MENU: Found custom map: %s", customMaps[customMapCount].name);
                    customMapCount++;
                } else {
                    TraceLog(LOG_WARNING, "MAIN_MENU: Max custom maps reached. Skipping: %s", fileName);
                }
            }
        }
    }
    UnloadDirectoryFiles(files);
    TraceLog(LOG_INFO, "MAIN_MENU: Loaded %d custom maps.", customMapCount);
}

/* I.S. : Game berada dalam state MAIN_MENU_PLAY_SELECTION.
   F.S. : Jika pemain mengklik tombol 'Default' atau 'Custom', currentGameState diubah
          ke state berikutnya yang sesuai. */
void UpdatePlaySelectionMenu(void) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition(); 
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        float panelWidth = screenWidth * PANEL_WIDTH_FACTOR;
        float panelHeight = screenHeight * PANEL_HEIGHT_FACTOR;
        float panelX = (screenWidth - panelWidth) / 2.0f;
        float panelY = (screenHeight - panelHeight) / 2.0f;

        float buttonWidth = panelWidth - (2 * PANEL_PADDING);
        float buttonHeight = PANEL_BUTTON_HEIGHT;
        float spacing = PANEL_BUTTON_SPACING;
        
        float totalButtonsHeight = buttonHeight * 3 + spacing * 2;
        float startYInPanel = panelY + (panelHeight - totalButtonsHeight) / 2.0f;

        Rectangle defaultButtonRect = { panelX + PANEL_PADDING, startYInPanel - 200, buttonWidth, buttonHeight + 200};
        Rectangle customButtonRect = { panelX + PANEL_PADDING, startYInPanel + buttonHeight + spacing, buttonWidth, buttonHeight + 200};
        float backButtonWidth = panelWidth * 0.5f; 
        float backButtonHeight = PANEL_BUTTON_HEIGHT;
        Rectangle backButtonRect = { 
            screenWidth / 2.0f - backButtonWidth / 2.0f, 
            panelY + panelHeight - backButtonHeight - PANEL_PADDING, 
            backButtonWidth, 
            backButtonHeight 
        };    
        
        if (CheckButtonClick(defaultButtonRect, mousePos)) { 
            TraceLog(LOG_INFO, "Default Game Selected! Starting GAMEPLAY.");
            currentGameState = GAMEPLAY;
        } else if (CheckButtonClick(customButtonRect, mousePos)) { 
            TraceLog(LOG_INFO, "Custom Game Selected! Loading custom map list.");
            LoadCustomMapList();
            currentGameState = MAIN_MENU_CUSTOM_MAP_LIST;
        } else if (CheckButtonClick(backButtonRect, mousePos)) { 
            TraceLog(LOG_INFO, "Back Button Clicked! Returning to Main Menu.");
            currentGameState = MAIN_MENU;
        }
    }
}

/* I.S. : Game berada dalam state MAIN_MENU_PLAY_SELECTION.
   F.S. : Tampilan untuk memilih mode permainan (Default Game / Custom Game) telah digambar ke layar. */
void DrawPlaySelectionMenu(void) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    DrawTexturePro(backgroundTex, (Rectangle){0, 0, (float)backgroundTex.width, (float)backgroundTex.height},
                   (Rectangle){0, 0, (float)screenWidth, (float)screenHeight}, (Vector2){0,0}, 0.0f, WHITE);
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));

    float panelWidth = screenWidth * PANEL_WIDTH_FACTOR;
    float panelHeight = screenHeight * PANEL_HEIGHT_FACTOR;
    float panelX = (screenWidth - panelWidth) / 2.0f;
    float panelY = (screenHeight - panelHeight) / 2.0f;
    DrawRectangleRec((Rectangle){panelX, panelY, panelWidth, panelHeight}, CLITERAL(Color){30, 30, 30, 200});

    DrawText("Select Game Type", (int)(panelX + panelWidth / 2 - MeasureText("Select Game Type", 25) / 2), (int)(panelY + PANEL_PADDING / 2) + 25, 25, RAYWHITE);

    float buttonWidth = panelWidth - (2 * PANEL_PADDING);
    float buttonHeight = PANEL_BUTTON_HEIGHT;
    float spacing = PANEL_BUTTON_SPACING;
    
    float totalButtonsHeight = buttonHeight * 3 + spacing * 2;
    float startYInPanel = panelY + (panelHeight - totalButtonsHeight) / 2.0f;

    
    Rectangle defaultButtonRect = { panelX + PANEL_PADDING , startYInPanel - 200, buttonWidth, buttonHeight + 200};
    DrawButton(defaultButtonRect, "Default Map", LIGHTGRAY, BLACK, 40);

    Rectangle customButtonRect = { panelX + PANEL_PADDING, startYInPanel + buttonHeight + spacing, buttonWidth, buttonHeight + 200};
    DrawButton(customButtonRect, "Custom Map", LIGHTGRAY, BLACK, 40);

    float backButtonWidth = panelWidth * 0.5f; 
    float backButtonHeight = PANEL_BUTTON_HEIGHT;
    Rectangle backButtonRect = { 
        screenWidth / 2.0f - backButtonWidth / 2.0f, 
        panelY + panelHeight - backButtonHeight - PANEL_PADDING, 
        backButtonWidth, 
        backButtonHeight };        
    DrawButton(backButtonRect, "Back", LIGHTGRAY, BLACK, 20);
}

/* I.S. : Game berada dalam state MAIN_MENU_CUSTOM_MAP_LIST.
   F.S. : Menangani input scroll mouse dan klik pada item daftar peta. Jika sebuah peta dipilih,
          currentGameState diubah ke GAMEPLAY. */
void UpdateCustomMapListMenu(void) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        float panelWidth = screenWidth * PANEL_WIDTH_FACTOR;
        float panelHeight = screenHeight * PANEL_HEIGHT_FACTOR;
        float panelX = (screenWidth - panelWidth) / 2.0f;
        float panelY = (screenHeight - panelHeight) / 2.0f;

        Rectangle listAreaRect = { panelX + 10, panelY + 80, panelWidth - 20, MAP_LIST_VIEW_HEIGHT };

        
        float backButtonWidth = panelWidth * 0.5f; 
        float backButtonHeight = PANEL_BUTTON_HEIGHT;
        Rectangle backButtonRect = { 
            screenWidth / 2.0f - backButtonWidth / 2.0f, 
            panelY + panelHeight - backButtonHeight - PANEL_PADDING, 
            backButtonWidth, 
            backButtonHeight 
        };        
        if (CheckButtonClick(backButtonRect, mousePos)) { 
            TraceLog(LOG_INFO, "Back Button Clicked! Returning to Play Selection Menu.");
            currentGameState = MAIN_MENU_PLAY_SELECTION;
            return;
        }

        
        if (CheckCollisionPointRec(mousePos, listAreaRect)) {
            int clickedY = (int)mousePos.y - (int)listAreaRect.y;
            int itemIndex = (clickedY + scrollOffset) / MAP_LIST_ITEM_HEIGHT;

            if (itemIndex >= 0 && itemIndex < customMapCount) {
                selectedCustomMapIndex = itemIndex;
                TraceLog(LOG_INFO, "MAIN_MENU: Selected custom map: %s", customMaps[selectedCustomMapIndex].name);
                currentGameState = GAMEPLAY;
                return;
            }
        }
    }

    float mouseWheelMove = GetMouseWheelMove();
    if (mouseWheelMove != 0) {
        int maxScrollOffset = (customMapCount * MAP_LIST_ITEM_HEIGHT) - MAP_LIST_VIEW_HEIGHT;
        if (maxScrollOffset < 0) maxScrollOffset = 0;

        scrollOffset -= (int)(mouseWheelMove * MAP_LIST_ITEM_HEIGHT);
        if (scrollOffset < 0) scrollOffset = 0;
        if (scrollOffset > maxScrollOffset) scrollOffset = maxScrollOffset;
    }
}

/* I.S. : Game berada dalam state MAIN_MENU_CUSTOM_MAP_LIST dan daftar peta telah dimuat.
   F.S. : Daftar peta kustom yang dapat di-scroll telah digambar ke layar. */
void DrawCustomMapListMenu(void) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    DrawTexturePro(backgroundTex, (Rectangle){0, 0, (float)backgroundTex.width, (float)backgroundTex.height},
                   (Rectangle){0, 0, (float)screenWidth, (float)screenHeight}, (Vector2){0,0}, 0.0f, WHITE);
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));

    float panelWidth = screenWidth * PANEL_WIDTH_FACTOR;
    float panelHeight = screenHeight * PANEL_HEIGHT_FACTOR;
    float panelX = (screenWidth - panelWidth) / 2.0f;
    float panelY = (screenHeight - panelHeight) / 2.0f;
    DrawRectangleRec((Rectangle){panelX, panelY, panelWidth, panelHeight}, CLITERAL(Color){30, 30, 30, 200});

    DrawText("Select Custom Map", (int)(panelX + panelWidth / 2 - MeasureText("Select Custom Map", 25) / 2), (int)(panelY + PANEL_PADDING / 2) + 25, 25, RAYWHITE);

    Rectangle listAreaRect = { panelX + 10, panelY + 80, panelWidth - 20, MAP_LIST_VIEW_HEIGHT };
    DrawRectangleLinesEx(listAreaRect, 1, WHITE);

    BeginScissorMode((int)listAreaRect.x, (int)listAreaRect.y, (int)listAreaRect.width, (int)listAreaRect.height);
    for (int i = 0; i < customMapCount; i++) {
        Rectangle itemRect = { listAreaRect.x, listAreaRect.y + i * MAP_LIST_ITEM_HEIGHT - scrollOffset, listAreaRect.width, MAP_LIST_ITEM_HEIGHT };
        
        Color bgColor = (i == selectedCustomMapIndex) ? BLUE : (CheckCollisionPointRec(GetMousePosition(), itemRect) ? Fade(BLUE, 0.5f) : (Color){0,0,0,0});
        Color textColor = RAYWHITE;

        DrawRectangleRec(itemRect, bgColor);
        DrawText(customMaps[i].name, (int)itemRect.x + 10, (int)itemRect.y + (MAP_LIST_ITEM_HEIGHT - 20) / 2 - 10, 40, textColor);
    }
    EndScissorMode();

    
    float backButtonWidth = panelWidth * 0.5f; 
    float backButtonHeight = PANEL_BUTTON_HEIGHT;
    Rectangle backButtonRect = { 
        screenWidth / 2.0f - backButtonWidth / 2.0f, 
        panelY + panelHeight - backButtonHeight - PANEL_PADDING, 
        backButtonWidth, 
        backButtonHeight 
    };
    DrawButton(backButtonRect, "Back", LIGHTGRAY, BLACK, 20); 
}