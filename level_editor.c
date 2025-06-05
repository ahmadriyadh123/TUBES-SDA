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

extern int customWaveCount; 

void InitializeLevelEditor(const int current_game_map[MAP_ROWS][MAP_COLS])
{
    
    memcpy(editorState.map, current_game_map, sizeof(int) * MAP_ROWS * MAP_COLS); 
    memcpy(editorState.baseGameMap, current_game_map, sizeof(int) * MAP_ROWS * MAP_COLS); 

    
    editorState.pathButtonTex = LoadTextureSafe("assets/path_button.png");
    editorState.towerButtonTex = LoadTextureSafe("assets/tower_button.png");
    editorState.waveButtonTex = LoadTextureSafe("assets/waves_button.png");  
    editorState.minusButtonTex = LoadTextureSafe("assets/minus_button.png");
    editorState.plusButtonTex = LoadTextureSafe("assets/plus_button.png");
    editorState.okButtonTex = LoadTextureSafe("assets/ok_button.png");
    editorState.saveButtonTex = LoadTextureSafe("assets/save_button.png");

    editorState.selectedTool = TOOL_PATH;
    editorState.wavePanelActive = false;
    editorState.waveCount = customWaveCount > 0 ? customWaveCount : 1; 
    editorState.isDraggingPath = false;
    editorState.requestSaveAndPlay = false;
    StrCopySafe(editorState.mapFileName, "maps/custom_map.csv", sizeof(editorState.mapFileName)); 
    TraceLog(LOG_INFO, "Level editor initialized.");
}

void UnloadLevelEditor(void)
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

void HandleLevelEditorInput(float globalScale, float offsetX, float offsetY)
{
    Vector2 mousePos = GetMousePosition();
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    float availableWidth = screenWidth * (1.0f - 2 * EDITOR_VIEW_PADDING_SIDE_FACTOR);
    float availableHeight = screenHeight * (1.0f - EDITOR_VIEW_PADDING_TOP_FACTOR - EDITOR_VIEW_PADDING_BOTTOM_FACTOR);
    float baseMapWidth = MAP_COLS * TILE_SIZE;
    float baseMapHeight = MAP_ROWS * TILE_SIZE;
    float editorMapScale = fmin(availableWidth / baseMapWidth, availableHeight / baseMapHeight);
    float editorMapDisplayWidth = baseMapWidth * editorMapScale;
    float editorMapDisplayHeight = baseMapHeight * editorMapScale;
    float editorMapOffsetX = screenWidth * EDITOR_VIEW_PADDING_SIDE_FACTOR + (availableWidth - editorMapDisplayWidth) / 2.0f;
    float editorMapOffsetY = screenHeight * EDITOR_VIEW_PADDING_TOP_FACTOR + (availableHeight - editorMapDisplayHeight) / 2.0f;
    float tileScreenSize = TILE_SIZE * editorMapScale;

    
    float buttonWidth = TILE_SIZE * editorMapScale * EDITOR_BUTTON_WIDTH_FACTOR;
    float buttonHeight = TILE_SIZE * editorMapScale * EDITOR_BUTTON_HEIGHT_FACTOR;
    float buttonSpacing = TILE_SIZE * editorMapScale * EDITOR_BUTTON_SPACING_FACTOR;
    float buttonsGroupWidth = (buttonWidth * 5 + buttonSpacing * 4); 
    float buttonsStartX = screenWidth / 2.0f - buttonsGroupWidth / 2.0f;
    float buttonsStartY = editorMapOffsetY + editorMapDisplayHeight + 20.0f * editorMapScale; 

    
    Rectangle pathButtonRect = {buttonsStartX, buttonsStartY, buttonWidth, buttonHeight};
    Rectangle towerButtonRect = {buttonsStartX + buttonWidth + buttonSpacing, buttonsStartY, buttonWidth, buttonHeight};
    Rectangle waveButtonRect = {buttonsStartX + 2 * (buttonWidth + buttonSpacing), buttonsStartY, buttonWidth, buttonHeight};

    
    float panelWidth = screenWidth * EDITOR_PANEL_WIDTH_FACTOR;
    float panelHeight = screenHeight * EDITOR_PANEL_HEIGHT_FACTOR;
    float panelX = (screenWidth - panelWidth) / 2.0f;
    float panelY = (screenHeight - panelHeight) / 2.0f;
    Rectangle panelRect = {panelX, panelY, panelWidth, panelHeight};

    
    float okButtonWidth = editorState.okButtonTex.width * (EDITOR_WAVE_BUTTON_SIZE / editorState.okButtonTex.height);
    float okButtonHeight = EDITOR_WAVE_BUTTON_SIZE;
    Rectangle okButtonRect = {
        panelX + panelWidth / 2 - okButtonWidth / 2,
        panelY + panelHeight - EDITOR_PANEL_PADDING - okButtonHeight,
        okButtonWidth,
        okButtonHeight
    };

    
    float saveButtonScale = editorMapScale * 0.25f;
    float saveButtonTexOriginalWidth = (float)editorState.saveButtonTex.width;
    float saveButtonTexOriginalHeight = (float)editorState.saveButtonTex.height;
    float saveButtonDrawWidth = saveButtonTexOriginalWidth * saveButtonScale;
    float saveButtonDrawHeight = saveButtonTexOriginalHeight * saveButtonScale;
    float saveButtonMargin = 20.0f;
    Rectangle saveButtonRect = {
        screenWidth - saveButtonDrawWidth - saveButtonMargin,
        screenHeight - saveButtonDrawHeight - saveButtonMargin,
        saveButtonDrawWidth,
        saveButtonDrawHeight
    };

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mousePos, saveButtonRect))
        {
            SaveLevelToFile(editorState.mapFileName); 
            editorState.requestSaveAndPlay = true;
            TraceLog(LOG_INFO, "Editor: 'Save and Play' button clicked. Requesting state change.");
            return;
        }

        if (editorState.wavePanelActive) 
        {
            if (CheckCollisionPointRec(mousePos, okButtonRect))
            {
                customWaveCount = editorState.waveCount;
                editorState.wavePanelActive = false;
                TraceLog(LOG_INFO, "Editor: Wave count set to %d. Panel closed.", customWaveCount);
                return;
            }
            Rectangle minusButtonRect = {panelX + EDITOR_PANEL_PADDING, panelY + panelHeight - EDITOR_PANEL_PADDING - EDITOR_WAVE_BUTTON_SIZE, EDITOR_WAVE_BUTTON_SIZE, EDITOR_WAVE_BUTTON_SIZE};
            Rectangle plusButtonRect = {panelX + panelWidth - EDITOR_PANEL_PADDING - EDITOR_WAVE_BUTTON_SIZE, panelY + panelHeight - EDITOR_PANEL_PADDING - EDITOR_WAVE_BUTTON_SIZE, EDITOR_WAVE_BUTTON_SIZE, EDITOR_WAVE_BUTTON_SIZE};
            if (CheckCollisionPointRec(mousePos, minusButtonRect))
            {
                if (editorState.waveCount > 1) 
                    editorState.waveCount--;
                return;
            }
            else if (CheckCollisionPointRec(mousePos, plusButtonRect))
            {
                editorState.waveCount++;
                return;
            }
            if (CheckCollisionPointRec(mousePos, panelRect))
            {
                return; 
            }
            else
            {
                editorState.wavePanelActive = false; 
                TraceLog(LOG_INFO, "Clicked outside wave panel. Closing panel.");
                return;
            }
        }

        
        if (CheckCollisionPointRec(mousePos, pathButtonRect))
        {
            editorState.selectedTool = TOOL_PATH;
            editorState.wavePanelActive = false;
            TraceLog(LOG_INFO, "Editor: Selected PATH tool.");
            return;
        }
        else if (CheckCollisionPointRec(mousePos, towerButtonRect))
        {
            editorState.selectedTool = TOOL_TOWER;
            editorState.wavePanelActive = false;
            TraceLog(LOG_INFO, "Editor: Selected TOWER tool.");
            return;
        }
        else if (CheckCollisionPointRec(mousePos, waveButtonRect))
        {
            editorState.selectedTool = TOOL_WAVE;
            editorState.wavePanelActive = true;
            TraceLog(LOG_INFO, "Editor: Selected WAVE tool. Panel opened.");
            return;
        }

        else 
        {
            int col = (int)((mousePos.x - editorMapOffsetX) / tileScreenSize);
            int row = (int)((mousePos.y - editorMapOffsetY) / tileScreenSize);

            if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
            {
                int currentTileValue = editorState.map[row][col];
                
                if (editorState.selectedTool == TOOL_PATH)
                {
                    
                    editorState.map[row][col] = (currentTileValue == 37) ? 0 : 37;
                    editorState.isDraggingPath = true; 
                    TraceLog(LOG_INFO, "Editor: Path tile at (%d, %d) set to %d.", row, col, editorState.map[row][col]);
                }
                else if (editorState.selectedTool == TOOL_TOWER)
                {
                    
                    editorState.map[row][col] = (currentTileValue == 4) ? 0 : 4;
                    TraceLog(LOG_INFO, "Editor: Tower tile at (%d, %d) set to %d.", row, col, editorState.map[row][col]);
                }
                return; 
            }
        }
    }
    
    
    if (editorState.selectedTool == TOOL_PATH && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (editorState.isDraggingPath)
        {
            int col = (int)((mousePos.x - editorMapOffsetX) / tileScreenSize);
            int row = (int)((mousePos.y - editorMapOffsetY) / tileScreenSize);
            if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
            {
                if (editorState.map[row][col] != 4) 
                {
                    editorState.map[row][col] = 37;
                }
            }
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        editorState.isDraggingPath = false; 
    }
}

void DrawLevelEditor(float globalScale, float offsetX, float offsetY)
{
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    float availableWidth = screenWidth * (1.0f - 2 * EDITOR_VIEW_PADDING_SIDE_FACTOR);
    float availableHeight = screenHeight * (1.0f - EDITOR_VIEW_PADDING_TOP_FACTOR - EDITOR_VIEW_PADDING_BOTTOM_FACTOR);
    float baseMapWidth = MAP_COLS * TILE_SIZE;
    float baseMapHeight = MAP_ROWS * TILE_SIZE;
    float editorMapScale = fmin(availableWidth / baseMapWidth, availableHeight / baseMapHeight);
    float editorMapDisplayWidth = baseMapWidth * editorMapScale;
    float editorMapDisplayHeight = baseMapHeight * editorMapScale;
    float editorMapOffsetX = screenWidth * EDITOR_VIEW_PADDING_SIDE_FACTOR + (availableWidth - editorMapDisplayWidth) / 2.0f;
    float editorMapOffsetY = screenHeight * EDITOR_VIEW_PADDING_TOP_FACTOR + (availableHeight - editorMapDisplayHeight) / 2.0f;
    float tileScreenSize = TILE_SIZE * editorMapScale;

    
    for (int r = 0; r < MAP_ROWS; r++)
    {
        for (int c = 0; c < MAP_COLS; c++)
        {
            int tileIndex = editorState.map[r][c];
            Rectangle sourceRect = GetTileSourceRect(tileIndex); 
            Rectangle destRect = {
                editorMapOffsetX + c * tileScreenSize, 
                editorMapOffsetY + r * tileScreenSize, 
                tileScreenSize,                        
                tileScreenSize
            }; 
            Color drawColor = WHITE;

            if (tileIndex == 0) 
            {
                sourceRect = GetTileSourceRect(0); 
                drawColor = Fade(WHITE, 0.3f);
            }
            
            DrawTexturePro(tileSheetTex, sourceRect, destRect, (Vector2){0, 0}, 0.0f, drawColor); 

            
            if (tileIndex == 4)
            {
                Rectangle circleSource = {0, 0, (float)emptyCircleTex.width, (float)emptyCircleTex.height}; 
                DrawTexturePro(emptyCircleTex, circleSource, destRect, (Vector2){0, 0}, 0.0f, drawColor); 
            }
            
            
            
            
        }
    }
    
    
    float buttonWidth = TILE_SIZE * editorMapScale * EDITOR_BUTTON_WIDTH_FACTOR;
    float buttonHeight = TILE_SIZE * editorMapScale * EDITOR_BUTTON_HEIGHT_FACTOR;
    float buttonSpacing = TILE_SIZE * editorMapScale * EDITOR_BUTTON_SPACING_FACTOR;
    float buttonsGroupWidth = (buttonWidth * 5 + buttonSpacing * 4); 
    float buttonsStartX = screenWidth / 2.0f - buttonsGroupWidth / 2.0f;
    float buttonsStartY = editorMapOffsetY + editorMapDisplayHeight + 20.0f * editorMapScale;

    DrawTexturePro(editorState.pathButtonTex, (Rectangle){0, 0, (float)editorState.pathButtonTex.width, (float)editorState.pathButtonTex.height},
                   (Rectangle){buttonsStartX, buttonsStartY, buttonWidth, buttonHeight}, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTexturePro(editorState.towerButtonTex, (Rectangle){0, 0, (float)editorState.towerButtonTex.width, (float)editorState.towerButtonTex.height},
                   (Rectangle){buttonsStartX + buttonWidth + buttonSpacing, buttonsStartY, buttonWidth, buttonHeight}, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTexturePro(editorState.waveButtonTex, (Rectangle){0, 0, (float)editorState.waveButtonTex.width, (float)editorState.waveButtonTex.height},
                   (Rectangle){buttonsStartX + 2 * (buttonWidth + buttonSpacing), buttonsStartY, buttonWidth, buttonHeight}, (Vector2){0, 0}, 0.0f, WHITE);

    
    if (editorState.selectedTool == TOOL_PATH)
    {
        DrawRectangleLinesEx((Rectangle){buttonsStartX, buttonsStartY, buttonWidth, buttonHeight}, 3, BLUE);
    }
    else if (editorState.selectedTool == TOOL_TOWER)
    {
        DrawRectangleLinesEx((Rectangle){buttonsStartX + buttonWidth + buttonSpacing, buttonsStartY, buttonWidth, buttonHeight}, 3, BLUE);
    }
    else if (editorState.selectedTool == TOOL_WAVE)
    {
        DrawRectangleLinesEx((Rectangle){buttonsStartX + 2 * (buttonWidth + buttonSpacing), buttonsStartY, buttonWidth, buttonHeight}, 3, BLUE);
    }
    
    if (editorState.wavePanelActive)
    {
        float panelWidth = screenWidth * EDITOR_PANEL_WIDTH_FACTOR;
        float panelHeight = screenHeight * EDITOR_PANEL_HEIGHT_FACTOR;
        float panelX = (screenWidth - panelWidth) / 2.0f;
        float panelY = (screenHeight - panelHeight) / 2.0f;
        DrawRectangleRec((Rectangle){panelX, panelY, panelWidth, panelHeight}, CLITERAL(Color){30, 30, 30, 200}); 
        DrawText("Wave Settings", (int)(panelX + panelWidth / 2 - MeasureText("Wave Settings", 25) / 2), (int)(panelY + EDITOR_PANEL_PADDING), 25, RAYWHITE);
        char waveCountText[16];
        sprintf(waveCountText, "Waves: %d", editorState.waveCount);
        DrawText(waveCountText, (int)(panelX + panelWidth / 2 - MeasureText(waveCountText, 30) / 2), (int)(panelY + panelHeight / 2 - 15), 30, RAYWHITE);

        
        Rectangle minusButtonRect = {panelX + EDITOR_PANEL_PADDING, panelY + panelHeight - EDITOR_PANEL_PADDING - EDITOR_WAVE_BUTTON_SIZE, EDITOR_WAVE_BUTTON_SIZE, EDITOR_WAVE_BUTTON_SIZE};
        DrawTexturePro(editorState.minusButtonTex, (Rectangle){0, 0, (float)editorState.minusButtonTex.width, (float)editorState.minusButtonTex.height},
                       minusButtonRect, (Vector2){0, 0}, 0.0f, GRAY);
        
        Rectangle plusButtonRect = {panelX + panelWidth - EDITOR_PANEL_PADDING - EDITOR_WAVE_BUTTON_SIZE, panelY + panelHeight - EDITOR_PANEL_PADDING - EDITOR_WAVE_BUTTON_SIZE, EDITOR_WAVE_BUTTON_SIZE, EDITOR_WAVE_BUTTON_SIZE};
        DrawTexturePro(editorState.plusButtonTex, (Rectangle){0, 0, (float)editorState.plusButtonTex.width, (float)editorState.plusButtonTex.height},
                       plusButtonRect, (Vector2){0, 0}, 0.0f, WHITE);
        
        float okButtonWidth = editorState.okButtonTex.width * (EDITOR_WAVE_BUTTON_SIZE / editorState.okButtonTex.height);
        float okButtonHeight = EDITOR_WAVE_BUTTON_SIZE;
        Rectangle okButtonDrawRect = {
            panelX + panelWidth / 2 - okButtonWidth / 2,
            panelY + panelHeight - EDITOR_PANEL_PADDING - okButtonHeight,
            okButtonWidth,
            okButtonHeight};
        DrawTexturePro(editorState.okButtonTex, (Rectangle){0, 0, (float)editorState.okButtonTex.width, (float)editorState.okButtonTex.height},
                       okButtonDrawRect, (Vector2){0, 0}, 0.0f, WHITE);
    }
    
    
    float saveButtonScale = editorMapScale * 0.25f; 
    float saveButtonTexOriginalWidth = (float)editorState.saveButtonTex.width;
    float saveButtonTexOriginalHeight = (float)editorState.saveButtonTex.height;
    float saveButtonDrawWidth = saveButtonTexOriginalWidth * saveButtonScale;
    float saveButtonDrawHeight = saveButtonTexOriginalHeight * saveButtonScale;
    float saveButtonMargin = 20.0f; 
    Rectangle saveButtonDrawRect = {
        screenWidth - saveButtonDrawWidth - saveButtonMargin,
        screenHeight - saveButtonDrawHeight - saveButtonMargin,
        saveButtonDrawWidth,
        saveButtonDrawHeight
    };
    DrawTexturePro(editorState.saveButtonTex, (Rectangle){0, 0, saveButtonTexOriginalWidth, saveButtonTexOriginalHeight},
                   saveButtonDrawRect, (Vector2){0, 0}, 0.0f, WHITE);
}




EditorTool GetEditorSelectedTool(void) { return editorState.selectedTool; }
bool GetEditorWavePanelActive(void) { return editorState.wavePanelActive; }
bool GetEditorRequestSaveAndPlay(void) { return editorState.requestSaveAndPlay; }
int GetEditorMapTile(int row, int col) {
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS) {
        return editorState.map[row][col];
    }
    return 0;
}
const char *GetEditorMapFileName(void) { return editorState.mapFileName; }
int GetEditorWaveCount(void) { return editorState.waveCount; }

void SetEditorSelectedTool(EditorTool tool) { editorState.selectedTool = tool; }
void SetEditorWavePanelActive(bool active) { editorState.wavePanelActive = active; }
void SetEditorRequestSaveAndPlay(bool value) { editorState.requestSaveAndPlay = value; }
void SetEditorMapTile(int row, int col, int value) {
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS) {
        editorState.map[row][col] = value;
    }
}
void SetEditorMapFileName(const char *fileName) {
    
    
    strncpy(editorState.mapFileName, fileName, sizeof(editorState.mapFileName) - 1);
    editorState.mapFileName[sizeof(editorState.mapFileName) - 1] = '\0'; 
}
void SetEditorWaveCount(int count) { editorState.waveCount = count >= 1 ? count : 1; }


bool LoadLevelFromFile(const char *fileName) {
    if (!FileExistsSafe(fileName)) { 
        TraceLog(LOG_WARNING, "Failed to load map: File %s does not exist.", fileName);
        return false;
    }

    FILE *file = fopen(fileName, "r");
    if (!file) {
        TraceLog(LOG_WARNING, "Failed to open map file %s for reading.", fileName);
        return false;
    }

    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            int value;
            if (c == MAP_COLS - 1) {
                fscanf(file, "%d", &value);
            } else {
                fscanf(file, "%d,", &value);
            }
            editorState.map[r][c] = value; 
        }
        fscanf(file, "\n"); 
    }

    int waveCount;
    if (fscanf(file, "waveCount,%d", &waveCount) == 1) {
        editorState.waveCount = waveCount;
    } else {
        TraceLog(LOG_WARNING, "Failed to read waveCount from file %s. Defaulting to 1.", fileName);
        editorState.waveCount = 1;
    }

    fclose(file);
    TraceLog(LOG_INFO, "Level loaded from %s with wave count %d.", fileName, editorState.waveCount);
    return true;
}

void SaveLevelToFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        TraceLog(LOG_WARNING, "Failed to open file %s for writing.", filename);
        return;
    }

    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            fprintf(file, "%d", editorState.map[r][c]); 
            if (c < MAP_COLS - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
    fprintf(file, "waveCount,%d\n", editorState.waveCount); 
    fclose(file);
    TraceLog(LOG_INFO, "Level saved to %s with wave count %d.", filename, editorState.waveCount);
}