#ifndef LEVEL_EDITOR_H
#define LEVEL_EDITOR_H

#include "raylib.h"
#include "map.h" 

#define EDITOR_BUTTON_WIDTH_FACTOR 1.0f
#define EDITOR_BUTTON_HEIGHT_FACTOR 1.0f
#define EDITOR_BUTTON_SPACING_FACTOR 0.2f
#define EDITOR_PANEL_WIDTH_FACTOR 0.3f
#define EDITOR_PANEL_HEIGHT_FACTOR 0.3f
#define EDITOR_PANEL_PADDING 50.0f
#define EDITOR_WAVE_BUTTON_SIZE 50.0f
#define EDITOR_VIEW_PADDING_TOP_FACTOR 0.1f
#define EDITOR_VIEW_PADDING_BOTTOM_FACTOR 0.2f
#define EDITOR_VIEW_PADDING_SIDE_FACTOR 0.05f

typedef enum
{
    TOOL_NONE,
    TOOL_PATH,
    TOOL_TOWER,
    TOOL_WAVE
} EditorTool;

typedef struct
{
    int map[MAP_ROWS][MAP_COLS];         
    int baseGameMap[MAP_ROWS][MAP_COLS]; 
    EditorTool selectedTool;             
    bool wavePanelActive;                
    int waveCount;                       
    bool isDraggingPath;                     
    Texture2D pathButtonTex;
    Texture2D towerButtonTex;
    Texture2D waveButtonTex;
    Texture2D minusButtonTex;
    Texture2D plusButtonTex;
    Texture2D okButtonTex;
    Texture2D saveButtonTex;
    bool requestSaveAndPlay;   
    char mapFileName[256];     
    char mapDisplayName[64]; 

} LevelEditorState;

extern int customWaveCount; 
extern LevelEditorState editorState;

void InitializeLevelEditor(const int current_game_map[MAP_ROWS][MAP_COLS]);
void UnloadLevelEditor();
void HandleLevelEditorInput(float globalScale, float offsetX, float offsetY);
void DrawLevelEditor(float globalScale, float offsetX, float offsetY);

const char *GetEditorMapDisplayName(void);
void SetEditorMapDisplayName(const char *name);
EditorTool GetEditorSelectedTool();
bool GetEditorRequestSaveAndPlay();
int GetEditorMapTile(int row, int col);
const char *GetEditorMapFileName();
int GetEditorWaveCount();

void SetEditorSelectedTool(EditorTool tool);
void SetEditorWavePanelActive(bool active);
void SetEditorRequestSaveAndPlay(bool value);
void SetEditorMapTile(int row, int col, int value);
void SetEditorMapFileName(const char *fileName);
void SetEditorWaveCount(int count);

bool LoadLevelFromFile(const char *fileName);
void SaveLevelToFile(const char *fileName);

#endif