#include "level_editor.h"
#include "map.h" 
#include "utils.h" 
#include "raylib.h"
#include "enemy.h" 
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>

LevelEditorState editorState = {0};

void InitializeLevelEditor(const int current_game_map[MAP_ROWS][MAP_COLS])
{
    memcpy(editorState.map, current_game_map, sizeof(int) * MAP_ROWS * MAP_COLS);
    memcpy(editorState.baseGameMap, current_game_map, sizeof(int) * MAP_ROWS * MAP_COLS);
    
    for (int row = 0; row < MAP_ROWS; row++)
    {
        for (int col = 0; col < MAP_COLS; col++)
        {
            editorState.map[row][col] = 0;
        }
    }
    
    editorState.pathButtonTex = LoadTextureSafe("assets/path_button.png");
    editorState.towerButtonTex = LoadTextureSafe("assets/tower_button.png");
    editorState.waveButtonTex = LoadTextureSafe("assets/waves_button.png");
    editorState.minusButtonTex = LoadTextureSafe("assets/minus_button.png");
    editorState.plusButtonTex = LoadTextureSafe("assets/plus_button.png");
    editorState.okButtonTex = LoadTextureSafe("assets/ok_button.png");
    editorState.saveButtonTex = LoadTextureSafe("assets/save_button.png");

    editorState.selectedTool = TOOL_PATH; 
    editorState.wavePanelActive = false;
    editorState.waveCount = (customWaveCount > 0) ? customWaveCount : 1; 
    editorState.isDraggingPath = false;
    editorState.requestSaveAndPlay = false;
    StrCopySafe(editorState.mapFileName, "maps/map.txt", sizeof(editorState.mapFileName)); 
    StrCopySafe(editorState.mapDisplayName, "New Map", sizeof(editorState.mapDisplayName)); 

    TraceLog(LOG_INFO, "Level editor initialized.");
}

void UnloadLevelEditor()
{
    UnloadTextureSafe(&editorState.pathButtonTex);
    UnloadTextureSafe(&editorState.towerButtonTex);
    UnloadTextureSafe(&editorState.waveButtonTex);
    UnloadTextureSafe(&editorState.minusButtonTex);
    UnloadTextureSafe(&editorState.plusButtonTex);
    UnloadTextureSafe(&editorState.okButtonTex);
    UnloadTextureSafe(&editorState.saveButtonTex);
    TraceLog(LOG_INFO, "Level editor unloaded.");
}

const char *GetEditorMapDisplayName(void) { return editorState.mapDisplayName; }
void SetEditorMapDisplayName(const char *name) { StrCopySafe(editorState.mapDisplayName, name, sizeof(editorState.mapDisplayName)); }
EditorTool GetEditorSelectedTool() { return editorState.selectedTool; }
bool GetEditorRequestSaveAndPlay() { return editorState.requestSaveAndPlay; }
int GetEditorMapTile(int row, int col)
{
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
    {
        return editorState.map[row][col];
    }
    return 0; 
}
const char *GetEditorMapFileName() { return editorState.mapFileName; }
int GetEditorWaveCount() { return editorState.waveCount; }

void SetEditorSelectedTool(EditorTool tool) { editorState.selectedTool = tool; }
void SetEditorWavePanelActive(bool active) { editorState.wavePanelActive = active; }
void SetEditorRequestSaveAndPlay(bool value) { editorState.requestSaveAndPlay = value; }
void SetEditorMapTile(int row, int col, int value)
{
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
    {
        editorState.map[row][col] = value;
    }
}
void SetEditorMapFileName(const char *fileName)
{
    StrCopySafe(editorState.mapFileName, fileName, sizeof(editorState.mapFileName));
}
void SetEditorWaveCount(int count) { editorState.waveCount = count >= 1 ? count : 1; } 

bool LoadLevelFromFile(const char *fileName)
{
    TraceLog(LOG_INFO, "LoadLevelFromFile stub called for: %s", fileName);
    return false; 
}
void SaveLevelToFile(const char *fileName)
{
    TraceLog(LOG_INFO, "SaveLevelToFile stub called for: %s", fileName);
    
}

void HandleLevelEditorInput(float globalScale, float offsetX, float offsetY)
{
    // ntaran bang
}
void DrawLevelEditor(float globalScale, float offsetX, float offsetY)
{
    // ntaran bang
}