
#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "raylib.h"
#include "common.h"

#define MENU_BG_DRAW_SCALE 2.0f      
#define MENU_BUTTON_DRAW_SCALE 1.5f  
#define BUTTON_SPACING_PX 5.0f
#define MAP_LIST_ITEM_HEIGHT 60
#define MAP_LIST_VIEW_HEIGHT 200 
#define PANEL_WIDTH_FACTOR 0.4f
#define PANEL_HEIGHT_FACTOR 0.5f
#define PANEL_PADDING 20.0f
#define PANEL_BUTTON_HEIGHT 50.0f 
#define PANEL_BUTTON_SPACING 10.0f

extern Texture2D backgroundTex; 
extern Texture2D menuBgTex;
extern Texture2D startButtonTex;
extern Texture2D levelEditorButtonTex;
extern Texture2D settingsButtonTex;
extern Texture2D exitButtonTex;

#define MAX_CUSTOM_MAPS 10
#define MAP_FILENAME_MAX_LEN 256
typedef struct {
    char name[MAP_FILENAME_MAX_LEN];
    char filePath[MAP_FILENAME_MAX_LEN];  
    int startOffset;                      
    int endOffset;                        
} CustomMapEntry;

extern CustomMapEntry customMaps[MAX_CUSTOM_MAPS];
extern int customMapCount;
extern int selectedCustomMapIndex; 

extern GameState currentGameState; 

void LoadMainMenuResources();
void UnloadMainMenuResources();
void DrawMainMenu();

void DrawButton(Rectangle rect, const char* text, Color bgColor, Color textColor, int fontSize);

bool CheckButtonClick(Rectangle rect, Vector2 mousePos);
void HandleMainMenuInput();
void UpdatePlaySelectionMenu();
void DrawPlaySelectionMenu();
void UpdateCustomMapListMenu();
void DrawCustomMapListMenu();
void LoadCustomMapList();

#endif