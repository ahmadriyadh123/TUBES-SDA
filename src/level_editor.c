/* File        : level_editor.c
* Deskripsi   : Implementasi untuk modul Level Editor.
*               Berisi semua logika untuk menangani input pengguna, memperbarui state peta, 
*               dan menggambar keseluruhan antarmuka pengguna (UI) dari Level Editor. 
* Dibuat oleh  : Mohamad Jibril Fathi Al-ghifari
* Perubahan terakhir : Rabu, 11 Juni 2025
*/

#include "common.h"
#include "level_editor.h"
#include "map.h"
#include "gameplay.h"
#include "utils.h"
#include "raylib.h"
#include "enemy.h"
#include "main_menu.h"
#include "transition.h"
#include "audio.h"

int customWaveCount = 0; 
bool editorInitialized = false;
LevelEditorState editorState = {.startRow = -1, .startCol = -1, .waveCount = 1}; 

/* I.S. : State level editor belum terinisialisasi.
   F.S. : Seluruh state editor, termasuk salinan peta, tool yang dipilih, dan aset UI,
          telah diinisialisasi dan siap digunakan. */
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

    editorState.pathButtonTex = LoadTextureSafe("assets/img/level_editor_imgs/path_tool.png");
    editorState.towerButtonTex = LoadTextureSafe("assets/img/level_editor_imgs/tower_tool.png");
    editorState.waveButtonTex = LoadTextureSafe("assets/img/level_editor_imgs/waves_tool.png");
    editorState.startPointButtonTex = LoadTextureSafe("assets/img/level_editor_imgs/start_tool.png"); 
    editorState.minusButtonTex = LoadTextureSafe("assets/img/level_editor_imgs/minus_button.png");
    editorState.plusButtonTex = LoadTextureSafe("assets/img/level_editor_imgs/plus_button.png");
    editorState.okButtonTex = LoadTextureSafe("assets/img/level_editor_imgs/ok_button.png");
    editorState.saveButtonTex = LoadTextureSafe("assets/img/level_editor_imgs/save_button.png");

    editorState.selectedTool = TOOL_PATH;
    editorState.wavePanelActive = false;
    editorState.startRow = -1; 
    editorState.startCol = -1; 
    editorState.waveCount = 1;
    editorState.isDraggingPath = false;
    editorState.requestSaveAndPlay = false;
    StrCopySafe(editorState.mapFileName, "maps/map.txt", sizeof(editorState.mapFileName));
    editorState.textInputBuffer[0] = '\0';
    editorState.letterCount = 0;
    TraceLog(LOG_INFO, "Level editor initialized.");
}

/* I.S. : Aset-aset untuk level editor (seperti tekstur tombol) sedang dimuat di memori.
   F.S. : Semua aset yang dialokasikan untuk level editor telah dibebaskan dari memori. */
void UnloadLevelEditor()
{

    UnloadTextureSafe(&editorState.pathButtonTex);
    UnloadTextureSafe(&editorState.towerButtonTex);
    UnloadTextureSafe(&editorState.waveButtonTex);
    UnloadTextureSafe(&editorState.startPointButtonTex); 
    UnloadTextureSafe(&editorState.minusButtonTex);
    UnloadTextureSafe(&editorState.plusButtonTex);
    UnloadTextureSafe(&editorState.okButtonTex);
    UnloadTextureSafe(&editorState.saveButtonTex);
    TraceLog(LOG_INFO, "Level editor unloaded.");
}

/* I.S. : Level editor dalam keadaan menunggu input dari pengguna (misalnya, klik mouse).
   F.S. : Input dari pengguna telah diproses. State dari peta editor (misalnya, penambahan
          tile jalur atau tower) atau state editor (misalnya, tool yang dipilih) telah diperbarui. */
void HandleLevelEditorInput(float globalScale, float offsetX, float offsetY)
{
    Vector2 mousePos = GetMousePosition();
    float screenWidth = (float)VIRTUAL_WIDTH;
    float screenHeight = (float)VIRTUAL_HEIGHT;
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

    
    float buttonsGroupWidth = (buttonWidth * 4) + (buttonSpacing * 3);
    float buttonsStartX = screenWidth / 2.0f - buttonsGroupWidth / 2.0f;
    float buttonsStartY = editorMapOffsetY + editorMapDisplayHeight + 20.0f * editorMapScale;

    
    Rectangle startPointButtonRect = {buttonsStartX, buttonsStartY, buttonWidth, buttonHeight};
    Rectangle pathButtonRect = {buttonsStartX + (buttonWidth + buttonSpacing), buttonsStartY, buttonWidth, buttonHeight};
    Rectangle towerButtonRect = {buttonsStartX + 2 * (buttonWidth + buttonSpacing), buttonsStartY, buttonWidth, buttonHeight};
    Rectangle waveButtonRect = {buttonsStartX + 3 * (buttonWidth + buttonSpacing), buttonsStartY, buttonWidth, buttonHeight};
    

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
            currentGameState = LEVEL_EDITOR_SAVE; 
            TraceLog(LOG_INFO, "Editor: 'Save and Play' button clicked. Opening save dialog.");
            return; 
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
            TraceLog(LOG_INFO, "Editor: Selected WAVE tool.");
            return;
        }
        else if (CheckCollisionPointRec(mousePos, startPointButtonRect))
        { 
            SetEditorSelectedTool(TOOL_START_POINT);
            SetEditorWavePanelActive(false);
            editorState.isDraggingPath = false;
            TraceLog(LOG_INFO, "Editor: Selected START POINT tool.");
            return;
        }

        int col = (int)((mousePos.x - editorMapOffsetX) / tileScreenSize);
        int row = (int)((mousePos.y - editorMapOffsetY) / tileScreenSize);

        if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
        {
            int currentTileValue = GetEditorMapTile(row, col);

            if (editorState.selectedTool == TOOL_PATH)
            {
                SetEditorMapTile(row, col, (currentTileValue == 1) ? 0 : 1);

                if (GetEditorMapTile(row, col) == 1)
                {
                    editorState.isDraggingPath = true;
                    TraceLog(LOG_INFO, "Editor: Path tile (%d,%d) toggled to 1. Starting drag.", row, col);
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
            else if (editorState.selectedTool == TOOL_START_POINT)
            { 
                if (row == 0 || row == MAP_ROWS - 1 || col == 0 || col == MAP_COLS - 1)
                {
                    SetEditorMapTile(row, col, 1);
                    editorState.startRow = row;
                    editorState.startCol = col;
                    TraceLog(LOG_INFO, "Editor: Start point set at (%d, %d).", row, col);
                }
                else
                {
                    TraceLog(LOG_WARNING, "Editor: Start point can only be placed on the edges of the map.");
                }
                return;
            }
        }
    }

    if (editorState.selectedTool == TOOL_PATH && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && editorState.isDraggingPath)
    {
        int col = (int)((mousePos.x - editorMapOffsetX) / tileScreenSize);
        int row = (int)((mousePos.y - editorMapOffsetY) / tileScreenSize);
        if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
        {
            int targetTileValue = GetEditorMapTile(row, col);
            if (targetTileValue != 4 && targetTileValue != 5 && targetTileValue != 6)
            {
                SetEditorMapTile(row, col, 1);
            }
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        editorState.isDraggingPath = false;
    }
}

/* I.S. : State level editor telah terdefinisi (peta, tool, dll.).
   F.S. : Seluruh antarmuka pengguna (UI) untuk level editor, termasuk grid peta dan
          tombol-tombol alat, telah digambar ke layar. */
void DrawLevelEditor(float globalScale, float offsetX, float offsetY)
{
    float screenWidth = (float)VIRTUAL_WIDTH;
    float screenHeight = (float)VIRTUAL_HEIGHT;
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
        }
    }

    if (editorState.startRow != -1 && editorState.startCol != -1)
    { 
        Rectangle startMarkerRect = {editorMapOffsetX + editorState.startCol * tileScreenSize, editorMapOffsetY + editorState.startRow * tileScreenSize, tileScreenSize, tileScreenSize};
        DrawRectangleLinesEx(startMarkerRect, 4, GREEN);
    }

    
    
    float buttonWidth = TILE_SIZE * editorMapScale * EDITOR_BUTTON_WIDTH_FACTOR;
    float buttonHeight = TILE_SIZE * editorMapScale * EDITOR_BUTTON_HEIGHT_FACTOR;
    float buttonSpacing = TILE_SIZE * editorMapScale * EDITOR_BUTTON_SPACING_FACTOR;
    float buttonsGroupWidth = (buttonWidth * 4) + (buttonSpacing * 3);
    float buttonsStartX = screenWidth / 2.0f - buttonsGroupWidth / 2.0f;
    float buttonsStartY = editorMapOffsetY + editorMapDisplayHeight + 20.0f * editorMapScale;

    
    Rectangle startPointButtonRect = {buttonsStartX, buttonsStartY, buttonWidth, buttonHeight};
    Rectangle pathButtonRect = {buttonsStartX + (buttonWidth + buttonSpacing), buttonsStartY, buttonWidth, buttonHeight};
    Rectangle towerButtonRect = {buttonsStartX + 2 * (buttonWidth + buttonSpacing), buttonsStartY, buttonWidth, buttonHeight};
    Rectangle waveButtonRect = {buttonsStartX + 3 * (buttonWidth + buttonSpacing), buttonsStartY, buttonWidth, buttonHeight};

    
    DrawTexturePro(editorState.startPointButtonTex, (Rectangle){0, 0, (float)editorState.startPointButtonTex.width, (float)editorState.startPointButtonTex.height},
                   startPointButtonRect, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTexturePro(editorState.pathButtonTex, (Rectangle){0, 0, (float)editorState.pathButtonTex.width, (float)editorState.pathButtonTex.height},
                   pathButtonRect, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTexturePro(editorState.towerButtonTex, (Rectangle){0, 0, (float)editorState.towerButtonTex.width, (float)editorState.towerButtonTex.height},
                   towerButtonRect, (Vector2){0, 0}, 0.0f, WHITE);
    DrawTexturePro(editorState.waveButtonTex, (Rectangle){0, 0, (float)editorState.waveButtonTex.width, (float)editorState.waveButtonTex.height},
                   waveButtonRect, (Vector2){0, 0}, 0.0f, WHITE);

    
    if (editorState.selectedTool == TOOL_START_POINT)
    {
        DrawRectangleLinesEx(startPointButtonRect, 3, BLUE);
    }
    else if (editorState.selectedTool == TOOL_PATH)
    {
        DrawRectangleLinesEx(pathButtonRect, 3, BLUE);
    }
    else if (editorState.selectedTool == TOOL_TOWER)
    {
        DrawRectangleLinesEx(towerButtonRect, 3, BLUE);
    }
    else if (editorState.selectedTool == TOOL_WAVE)
    {
        DrawRectangleLinesEx(waveButtonRect, 3, BLUE);
    }
    

    float margin = 25.0f;
    float backButtonScale = 2.0f; 
    if (backButtonTex.id != 0)
    {
        DrawTextureEx(backButtonTex, (Vector2){margin, margin}, 0.0f, backButtonScale, WHITE);
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
        saveButtonDrawHeight};
    DrawTexturePro(editorState.saveButtonTex, (Rectangle){0, 0, saveButtonTexOriginalWidth, saveButtonTexOriginalHeight},
                   saveButtonDrawRect, (Vector2){0, 0}, 0.0f, WHITE);
}

void UpdateEditor(void) {
    PlayRegularMusic();
    StopAllMusicExcept(regularBacksound);
    if (!editorInitialized)
    {
        PlayTransitionAnimation(LEVEL_EDITOR);
        InitializeLevelEditor(gameMap);
        editorInitialized = true;
        TraceLog(LOG_INFO, "Level editor initialized after transition.");
    }
    float margin = 25.0f;
    float backButtonScale = 2.0f; 
    Rectangle backButtonRect = { margin, margin, backButtonTex.width * backButtonScale, backButtonTex.height * backButtonScale };

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, backButtonRect))
    {
        PlayTransitionAnimation(MAIN_MENU);
        currentGameState = MAIN_MENU;
        editorInitialized = false; 
        UnloadLevelEditor();       
        TraceLog(LOG_INFO, "Editor: Returning to MAIN_MENU via back button.");
        return; 
    }
    HandleLevelEditorInput(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
    if (GetEditorRequestSaveAndPlay())
    {   
        RestartGameplay();
        SetEditorRequestSaveAndPlay(false);
        editorInitialized = false;
        UnloadLevelEditor();

    }
}

void UpdateSaveMapEditor(){
    int key = GetCharPressed();
    while (key > 0)
    {
        if ((key >= 32) && (key <= 125) && (editorState.letterCount < 63))
        {
            editorState.textInputBuffer[editorState.letterCount] = (char)key;
            editorState.textInputBuffer[editorState.letterCount+1] = '\0';
            editorState.letterCount++;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        if (editorState.letterCount > 0)
        {
            editorState.letterCount--;
            editorState.textInputBuffer[editorState.letterCount] = '\0';
        }
    }

    float panelWidth = VIRTUAL_WIDTH * 0.4f;
    float panelHeight = VIRTUAL_HEIGHT * 0.25f;
    float panelX = (VIRTUAL_WIDTH - panelWidth) / 2.0f;
    float panelY = (VIRTUAL_HEIGHT - panelHeight) / 2.0f;
    float buttonWidth = 120;
    float buttonHeight = 40;
    Rectangle saveBtnRect = { panelX + panelWidth/2 - buttonWidth - 10, panelY + panelHeight - buttonHeight - 20, buttonWidth, buttonHeight };
    Rectangle cancelBtnRect = { panelX + panelWidth/2 + 10, panelY + panelHeight - buttonHeight - 20, buttonWidth, buttonHeight };

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePos, saveBtnRect)) {
            if (editorState.letterCount > 0) {
                char finalPath[256];
                snprintf(finalPath, sizeof(finalPath), "maps/%s.txt", editorState.textInputBuffer);

                SaveLevelToFile(finalPath);
                SetEditorMapFileName(finalPath);
                SetEditorRequestSaveAndPlay(true);
                currentGameState = LEVEL_EDITOR; 
            }
        } else if (CheckCollisionPointRec(mousePos, cancelBtnRect)) {
            currentGameState = LEVEL_EDITOR; 
        }
    }
}

void DrawSaveMapEditor(){
    DrawLevelEditor(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
    DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Fade(BLACK, 0.7f));
    
    float panelWidth = VIRTUAL_WIDTH * 0.4f;
    float panelHeight = VIRTUAL_HEIGHT * 0.25f;
    float panelX = (VIRTUAL_WIDTH - panelWidth) / 2.0f;
    float panelY = (VIRTUAL_HEIGHT - panelHeight) / 2.0f;
    DrawRectangleRec((Rectangle){panelX, panelY, panelWidth, panelHeight}, RAYWHITE);
    DrawRectangleLinesEx((Rectangle){panelX, panelY, panelWidth, panelHeight}, 2, BLACK);
    DrawText("Save Map As...", panelX + 20, panelY + 20, 20, BLACK);

    Rectangle textBoxRect = { panelX + 20, panelY + 60, panelWidth - 40, 40 };
    DrawRectangleRec(textBoxRect, LIGHTGRAY);
    DrawRectangleLinesEx(textBoxRect, 1, DARKGRAY);
    DrawText(editorState.textInputBuffer, textBoxRect.x + 10, textBoxRect.y + 10, 20, BLACK);
    
    if (editorState.letterCount < 63 && ((int)(GetTime()*2.0f) % 2 == 0) ) {
        DrawText("|", textBoxRect.x + 10 + MeasureText(editorState.textInputBuffer, 20), textBoxRect.y + 10, 20, BLACK);
    }
    
    float buttonWidth = 120;
    float buttonHeight = 40;
    Rectangle saveBtnRect = { panelX + panelWidth/2 - buttonWidth - 10, panelY + panelHeight - buttonHeight - 20, buttonWidth, buttonHeight };
    Rectangle cancelBtnRect = { panelX + panelWidth/2 + 10, panelY + panelHeight - buttonHeight - 20, buttonWidth, buttonHeight };
    DrawRectangleRec(saveBtnRect, DARKGREEN);
    DrawText("Save", saveBtnRect.x + 40, saveBtnRect.y + 10, 20, WHITE);
    DrawRectangleRec(cancelBtnRect, MAROON);
    DrawText("Cancel", cancelBtnRect.x + 30, cancelBtnRect.y + 10, 20, WHITE);
}
/* Mengirimkan nilai enum dari tool yang sedang aktif dipilih oleh pemain. */
EditorTool GetEditorSelectedTool() { return editorState.selectedTool; }

/* Mengirimkan status boolean dari flag 'requestSaveAndPlay'. */
bool GetEditorRequestSaveAndPlay() { return editorState.requestSaveAndPlay; }

/* Mengirimkan ID tile pada posisi (row, col) dari peta yang ada di editor. */
int GetEditorMapTile(int row, int col)
{
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
    {
        return editorState.map[row][col];
    }
    return 0;
}

/* Mengirimkan path file dari peta yang sedang dikerjakan. */
const char *GetEditorMapFileName() { return editorState.mapFileName; }

/* Mengirimkan jumlah gelombang yang telah diatur di editor. */
int GetEditorWaveCount() { return editorState.waveCount; }

/* Mengirimkan posisi baris titik awal musuh yang telah diatur. */
int GetEditorStartRow() { return editorState.startRow; } 

/* Mengirimkan posisi kolom titik awal musuh yang telah diatur. */
int GetEditorStartCol() { return editorState.startCol; } 

/* I.S. : Tool yang aktif adalah tool sebelumnya.
   F.S. : State tool yang aktif diubah menjadi 'tool'. */
void SetEditorSelectedTool(EditorTool tool) { editorState.selectedTool = tool; }

/* I.S. : Panel pengaturan gelombang memiliki status visibilitas lama.
   F.S. : Status visibilitas panel pengaturan gelombang diatur menjadi 'active'. */
void SetEditorWavePanelActive(bool active) { editorState.wavePanelActive = active; }

/* I.S. : Flag 'requestSaveAndPlay' memiliki nilai lama.
   F.S. : Nilai flag 'requestSaveAndPlay' diatur menjadi 'value'. */
void SetEditorRequestSaveAndPlay(bool value) { editorState.requestSaveAndPlay = value; }

/* I.S. : Tile di posisi (row, col) pada peta editor memiliki nilai lama.
   F.S. : Nilai tile di posisi (row, col) diubah menjadi 'value'. */
void SetEditorMapTile(int row, int col, int value)
{
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
    {
        editorState.map[row][col] = value;
    }
}

/* I.S. : State editor menyimpan path file yang lama.
   F.S. : Path file kerja di dalam state editor diubah menjadi 'fileName'. */
void SetEditorMapFileName(const char *fileName)
{

    strncpy(editorState.mapFileName, fileName, sizeof(editorState.mapFileName) - 1);
    editorState.mapFileName[sizeof(editorState.mapFileName) - 1] = '\0';
}

/* I.S. : Jumlah gelombang memiliki nilai lama.
   F.S. : Jumlah gelombang di state editor diatur menjadi 'count' (dengan validasi minimal 1). */
void SetEditorWaveCount(int count) { editorState.waveCount = count >= 1 ? count : 1; }

/* I.S. : Posisi baris titik awal memiliki nilai lama.
   F.S. : Posisi baris titik awal di state editor diatur menjadi 'row'. */
void SetEditorStartRow(int row) { editorState.startRow = row; } 

/* I.S. : Posisi kolom titik awal memiliki nilai lama.
   F.S. : Posisi kolom titik awal di state editor diatur menjadi 'col'. */
void SetEditorStartCol(int col) { editorState.startCol = col; } 

/* I.S. : State editor berisi data peta lama atau kosong.
   F.S. : State editor (peta, wave count, start point) telah diisi dengan data yang dibaca
          dari 'fileName'. Mengembalikan true jika berhasil, false jika gagal. */
bool LoadLevelFromFile(const char *fileName)
{
    if (!FileExistsSafe(fileName)) 
    {
        TraceLog(LOG_WARNING, "Failed to load map: File %s does not exist.", fileName); 
        return false;
    }

    FILE *file = fopen(fileName, "r");
    if (!file)
    {
        TraceLog(LOG_WARNING, "Failed to open map file %s for reading.", fileName); 
        return false;
    }

    
    for (int r = 0; r < MAP_ROWS; r++) 
    {
        for (int c = 0; c < MAP_COLS; c++) 
        {
            int value;
            if (fscanf(file, "%d", &value) != 1) { 
                TraceLog(LOG_ERROR, "LoadLevelFromFile: Failed to read map tile at (%d, %d) from %s. File format error.", r, c, fileName); 
                fclose(file);
                return false; 
            }
            SetEditorMapTile(r, c, value); 
        }   
    }
    
    
    SetEditorWaveCount(1); 
    SetEditorStartRow(-1); 
    SetEditorStartCol(-1); 

    char buffer[256];
    int temp_waveCount, temp_startCol, temp_startRow;

    
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        
        if (sscanf(buffer, "waveCount %d", &temp_waveCount) == 1) {
            
            SetEditorWaveCount(temp_waveCount); 
        } 
        
        else if (sscanf(buffer, "startPoint %d %d", &temp_startCol, &temp_startRow) == 2) {
            editorState.startCol = temp_startCol; 
            editorState.startRow = temp_startRow; 
        }
    }

    fclose(file);
    TraceLog(LOG_INFO, "Level loaded from %s with wave count %d and start point (%d, %d).", fileName, editorState.waveCount, editorState.startCol, editorState.startRow); 
    return true;
}

/* I.S. : State editor berisi data peta yang akan disimpan.
   F.S. : Data peta dari state editor telah ditulis dan disimpan ke dalam file 'fileName'. */
void SaveLevelToFile(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        TraceLog(LOG_WARNING, "Failed to open file %s for writing.", filename); 
        return;
    }

    
    for (int r = 0; r < MAP_ROWS; r++) 
    {
        for (int c = 0; c < MAP_COLS; c++) 
        {
            fprintf(file, "%d", GetEditorMapTile(r, c)); 
            if (c < MAP_COLS - 1)
                fprintf(file, " "); 
        }
        fprintf(file, "\n"); 
    }
    
    
    fprintf(file, "waveCount %d\n", GetEditorWaveCount()); 
    
    
    if (editorState.startRow != -1 && editorState.startCol != -1) { 
        fprintf(file, "startPoint %d %d\n", editorState.startCol, editorState.startRow); 
    }
    
    fclose(file);
    TraceLog(LOG_INFO, "Level saved to %s with wave count %d and start point (%d, %d).", filename, GetEditorWaveCount(), editorState.startCol, editorState.startRow); 
}