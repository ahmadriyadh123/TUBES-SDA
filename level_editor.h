#ifndef LEVEL_EDITOR_H
#define LEVEL_EDITOR_H

#include "raylib.h"
#include "map.h"

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
} LevelEditorState;

extern int customWaveCount;

extern LevelEditorState editorState;

void InitializeLevelEditor(const int current_game_map[MAP_ROWS][MAP_COLS]);
void UnloadLevelEditor(void);
void HandleLevelEditorInput(float globalScale, float offsetX, float offsetY);
void DrawLevelEditor(float globalScale, float offsetX, float offsetY);

bool LoadLevelFromFile(const char *fileName);
void SaveLevelToFile(const char *fileName);

EditorTool GetEditorSelectedTool(void);
bool GetEditorRequestSaveAndPlay(void);
int GetEditorMapTile(int row, int col);
const char *GetEditorMapFileName(void);
int GetEditorWaveCount(void);

void SetEditorSelectedTool(EditorTool tool);
void SetEditorWavePanelActive(bool active);
void SetEditorRequestSaveAndPlay(bool value);
void SetEditorMapTile(int row, int col, int value);
void SetEditorMapFileName(const char *fileName);
void SetEditorWaveCount(int count);

#endif