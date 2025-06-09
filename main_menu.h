
#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "raylib.h"
#include "common.h"

#define MENU_BG_DRAW_SCALE 2.0f      
#define MENU_BUTTON_DRAW_SCALE 1.5f  
#define BUTTON_SPACING_PX 5.0f
#define MAP_LIST_ITEM_HEIGHT 60
#define MAP_LIST_VIEW_HEIGHT 200 // Tinggi area daftar yang terlihat
#define PANEL_WIDTH_FACTOR 0.4f
#define PANEL_HEIGHT_FACTOR 0.5f
#define PANEL_PADDING 20.0f
#define PANEL_BUTTON_HEIGHT 50.0f // Tinggi tombol di dalam panel
#define PANEL_BUTTON_SPACING 10.0f

extern Texture2D backgroundTex; 
extern Texture2D menuBgTex;
extern Texture2D startButtonTex;
extern Texture2D levelEditorButtonTex;
extern Texture2D settingsButtonTex;
extern Texture2D exitButtonTex;

// --- BARU: Untuk daftar custom map ---
#define MAX_CUSTOM_MAPS 10
#define MAP_FILENAME_MAX_LEN 256
typedef struct {
    char name[MAP_FILENAME_MAX_LEN];
    char filePath[MAP_FILENAME_MAX_LEN];  // Path lengkap file CSV
    int startOffset;                      // Offset byte di file untuk peta ini
    int endOffset;                        // Offset byte di file setelah peta ini
} CustomMapEntry;

extern CustomMapEntry customMaps[MAX_CUSTOM_MAPS];
extern int customMapCount;
extern int selectedCustomMapIndex; // Indeks map yang dipilih dari daftar
// --- AKHIR BARU ---

extern GameState currentGameState; 

void LoadMainMenuResources();
void UnloadMainMenuResources();
void DrawMainMenu();
// Fungsi gambar tombol saja (tidak mengecek klik)
void DrawButton(Rectangle rect, const char* text, Color bgColor, Color textColor, int fontSize);
// Fungsi cek klik tombol saja (tidak menggambar)
bool CheckButtonClick(Rectangle rect, Vector2 mousePos);
void HandleMainMenuInput();
void UpdatePlaySelectionMenu();
void DrawPlaySelectionMenu();
void UpdateCustomMapListMenu();
void DrawCustomMapListMenu();
void LoadCustomMapList();

#endif