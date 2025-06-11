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
    float buttonsGroupWidth = (buttonWidth * 3 + buttonSpacing * 2);
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
        okButtonHeight};

    Rectangle minusButtonRect = {panelX + EDITOR_PANEL_PADDING, panelY + panelHeight / 2 - EDITOR_WAVE_BUTTON_SIZE / 2, EDITOR_WAVE_BUTTON_SIZE, EDITOR_WAVE_BUTTON_SIZE};
    Rectangle plusButtonRect = {panelX + panelWidth - EDITOR_PANEL_PADDING - EDITOR_WAVE_BUTTON_SIZE, panelY + panelHeight / 2 - EDITOR_WAVE_BUTTON_SIZE / 2, EDITOR_WAVE_BUTTON_SIZE, EDITOR_WAVE_BUTTON_SIZE};

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
        saveButtonDrawHeight};

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {

        if (CheckCollisionPointRec(mousePos, saveButtonRect))
        {
            TraceLog(LOG_INFO, "Editor: 'Save and Play' button clicked (stub).");
            editorState.requestSaveAndPlay = true;
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
            if (CheckCollisionPointRec(mousePos, minusButtonRect))
            {
                if (editorState.waveCount > 1)
                    editorState.waveCount--;
                TraceLog(LOG_INFO, "Editor: Wave count decreased to %d.", editorState.waveCount);
                return;
            }
            else if (CheckCollisionPointRec(mousePos, plusButtonRect))
            {
                editorState.waveCount++;
                TraceLog(LOG_INFO, "Editor: Wave count increased to %d.", editorState.waveCount);
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

        if (!editorState.wavePanelActive)
        {
            if (CheckCollisionPointRec(mousePos, pathButtonRect))
            {
                SetEditorSelectedTool(TOOL_PATH);
                SetEditorWavePanelActive(false);
                editorState.isDraggingPath = false;
                TraceLog(LOG_INFO, "Editor: Selected PATH tool.");
                return;
            }
            else if (CheckCollisionPointRec(mousePos, towerButtonRect))
            {
                SetEditorSelectedTool(TOOL_TOWER);
                SetEditorWavePanelActive(false);
                editorState.isDraggingPath = false;
                TraceLog(LOG_INFO, "Editor: Selected TOWER tool.");
                return;
            }
            else if (CheckCollisionPointRec(mousePos, waveButtonRect))
            {
                SetEditorSelectedTool(TOOL_WAVE);
                SetEditorWavePanelActive(true);
                editorState.isDraggingPath = false;
                TraceLog(LOG_INFO, "Editor: Selected WAVE tool. Opening wave panel.");
                return;
            }
        }

        if (!editorState.wavePanelActive)
        {
            int col = (int)((mousePos.x - editorMapOffsetX) / tileScreenSize);
            int row = (int)((mousePos.y - editorMapOffsetY) / tileScreenSize);

            if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
            {
                int currentTileValue = GetEditorMapTile(row, col);

                if (editorState.selectedTool == TOOL_PATH)
                {
                    SetEditorMapTile(row, col, (currentTileValue == 37) ? 0 : 37);
                    if (GetEditorMapTile(row, col) == 37)
                    {
                        editorState.isDraggingPath = true;
                        TraceLog(LOG_INFO, "Editor: Path tile (%d,%d) toggled to 37. Starting drag.", row, col);
                    }
                    else
                    {
                        editorState.isDraggingPath = false;
                        TraceLog(LOG_INFO, "Editor: Path tile (%d,%d) toggled to 0. Dragging disabled.", row, col);
                    }
                    return;
                }
                else if (editorState.selectedTool == TOOL_TOWER)
                {
                    SetEditorMapTile(row, col, (currentTileValue == 4) ? 0 : 4);
                    TraceLog(LOG_INFO, "Editor: Tower tile at (%d, %d) toggled to %d.", row, col, GetEditorMapTile(row, col));
                    return;
                }
            }
        }
    }

    if (!editorState.wavePanelActive && editorState.selectedTool == TOOL_PATH && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && editorState.isDraggingPath)
    {
        int col = (int)((mousePos.x - editorMapOffsetX) / tileScreenSize);
        int row = (int)((mousePos.y - editorMapOffsetY) / tileScreenSize);
        if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
        {
            int targetTileValue = GetEditorMapTile(row, col);
            if (targetTileValue != 4 && targetTileValue != 5 && targetTileValue != 6)
            {
                SetEditorMapTile(row, col, 37);
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
                tileScreenSize};
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

            DrawRectangleLinesEx(destRect, 1, Fade(LIGHTGRAY, 0.5f));
        }
    }

    float buttonWidth = TILE_SIZE * editorMapScale * EDITOR_BUTTON_WIDTH_FACTOR;
    float buttonHeight = TILE_SIZE * editorMapScale * EDITOR_BUTTON_HEIGHT_FACTOR;
    float buttonSpacing = TILE_SIZE * editorMapScale * EDITOR_BUTTON_SPACING_FACTOR;
    float buttonsGroupWidth = (buttonWidth * 3 + buttonSpacing * 2);
    float buttonsStartX = screenWidth / 2.0f - buttonsGroupWidth / 2.0f;
    float buttonsStartY = editorMapOffsetY + editorMapDisplayHeight + 20.0f * editorMapScale;

    Rectangle pathButtonDrawRect = {buttonsStartX, buttonsStartY, buttonWidth, buttonHeight};
    Rectangle towerButtonDrawRect = {buttonsStartX + buttonWidth + buttonSpacing, buttonsStartY, buttonWidth, buttonHeight};
    Rectangle waveButtonDrawRect = {buttonsStartX + 2 * (buttonWidth + buttonSpacing), buttonsStartY, buttonWidth, buttonHeight};

    DrawTexturePro(editorState.pathButtonTex, (Rectangle){0, 0, (float)editorState.pathButtonTex.width, (float)editorState.pathButtonTex.height},
                   pathButtonDrawRect, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTexturePro(editorState.towerButtonTex, (Rectangle){0, 0, (float)editorState.towerButtonTex.width, (float)editorState.towerButtonTex.height},
                   towerButtonDrawRect, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTexturePro(editorState.waveButtonTex, (Rectangle){0, 0, (float)editorState.waveButtonTex.width, (float)editorState.waveButtonTex.height},
                   waveButtonDrawRect, (Vector2){0, 0}, 0.0f, WHITE);

    if (editorState.selectedTool == TOOL_PATH)
    {
        DrawRectangleLinesEx(pathButtonDrawRect, 3, BLUE);
    }
    else if (editorState.selectedTool == TOOL_TOWER)
    {
        DrawRectangleLinesEx(towerButtonDrawRect, 3, BLUE);
    }
    else if (editorState.selectedTool == TOOL_WAVE)
    {
        DrawRectangleLinesEx(waveButtonDrawRect, 3, BLUE);
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
        saveButtonDrawHeight};
    DrawTexturePro(editorState.saveButtonTex, (Rectangle){0, 0, saveButtonTexOriginalWidth, saveButtonTexOriginalHeight},
                   saveButtonDrawRect, (Vector2){0, 0}, 0.0f, WHITE);

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
        float buttonsGroupWidth = (buttonWidth * 3 + buttonSpacing * 2);
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
            okButtonHeight};

        Rectangle minusButtonRect = {panelX + EDITOR_PANEL_PADDING, panelY + panelHeight / 2 - EDITOR_WAVE_BUTTON_SIZE / 2, EDITOR_WAVE_BUTTON_SIZE, EDITOR_WAVE_BUTTON_SIZE};
        Rectangle plusButtonRect = {panelX + panelWidth - EDITOR_PANEL_PADDING - EDITOR_WAVE_BUTTON_SIZE, panelY + panelHeight / 2 - EDITOR_WAVE_BUTTON_SIZE / 2, EDITOR_WAVE_BUTTON_SIZE, EDITOR_WAVE_BUTTON_SIZE};

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
            saveButtonDrawHeight};

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {

            if (CheckCollisionPointRec(mousePos, saveButtonRect))
            {
                TraceLog(LOG_INFO, "Editor: 'Save and Play' button clicked (stub).");
                editorState.requestSaveAndPlay = true;
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
                if (CheckCollisionPointRec(mousePos, minusButtonRect))
                {
                    if (editorState.waveCount > 1)
                        editorState.waveCount--;
                    TraceLog(LOG_INFO, "Editor: Wave count decreased to %d.", editorState.waveCount);
                    return;
                }
                else if (CheckCollisionPointRec(mousePos, plusButtonRect))
                {
                    editorState.waveCount++;
                    TraceLog(LOG_INFO, "Editor: Wave count increased to %d.", editorState.waveCount);
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

            if (!editorState.wavePanelActive)
            {
                if (CheckCollisionPointRec(mousePos, pathButtonRect))
                {
                    SetEditorSelectedTool(TOOL_PATH);
                    SetEditorWavePanelActive(false);
                    editorState.isDraggingPath = false;
                    TraceLog(LOG_INFO, "Editor: Selected PATH tool.");
                    return;
                }
                else if (CheckCollisionPointRec(mousePos, towerButtonRect))
                {
                    SetEditorSelectedTool(TOOL_TOWER);
                    SetEditorWavePanelActive(false);
                    editorState.isDraggingPath = false;
                    TraceLog(LOG_INFO, "Editor: Selected TOWER tool.");
                    return;
                }
                else if (CheckCollisionPointRec(mousePos, waveButtonRect))
                {
                    SetEditorSelectedTool(TOOL_WAVE);
                    SetEditorWavePanelActive(true);
                    editorState.isDraggingPath = false;
                    TraceLog(LOG_INFO, "Editor: Selected WAVE tool. Opening wave panel.");
                    return;
                }
            }

            if (!editorState.wavePanelActive)
            {
                int col = (int)((mousePos.x - editorMapOffsetX) / tileScreenSize);
                int row = (int)((mousePos.y - editorMapOffsetY) / tileScreenSize);

                if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
                {
                    int currentTileValue = GetEditorMapTile(row, col);

                    if (editorState.selectedTool == TOOL_PATH)
                    {
                        SetEditorMapTile(row, col, (currentTileValue == 37) ? 0 : 37);
                        if (GetEditorMapTile(row, col) == 37)
                        {
                            editorState.isDraggingPath = true;
                            TraceLog(LOG_INFO, "Editor: Path tile (%d,%d) toggled to 37. Starting drag.", row, col);
                        }
                        else
                        {
                            editorState.isDraggingPath = false;
                            TraceLog(LOG_INFO, "Editor: Path tile (%d,%d) toggled to 0. Dragging disabled.", row, col);
                        }
                        return;
                    }
                    else if (editorState.selectedTool == TOOL_TOWER)
                    {
                        SetEditorMapTile(row, col, (currentTileValue == 4) ? 0 : 4);
                        TraceLog(LOG_INFO, "Editor: Tower tile at (%d, %d) toggled to %d.", row, col, GetEditorMapTile(row, col));
                        return;
                    }
                }
            }
        }

        if (!editorState.wavePanelActive && editorState.selectedTool == TOOL_PATH && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && editorState.isDraggingPath)
        {
            int col = (int)((mousePos.x - editorMapOffsetX) / tileScreenSize);
            int row = (int)((mousePos.y - editorMapOffsetY) / tileScreenSize);
            if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
            {
                int targetTileValue = GetEditorMapTile(row, col);
                if (targetTileValue != 4 && targetTileValue != 5 && targetTileValue != 6)
                {
                    SetEditorMapTile(row, col, 37);
                }
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            editorState.isDraggingPath = false;
        }
    }
}