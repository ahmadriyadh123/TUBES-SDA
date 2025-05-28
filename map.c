/*
 * File: map.c
 * Description:
 *   File ini berfungsi untuk mengelola dan merender peta permainan, termasuk memuat
 *   dan membebaskan resource tekstur, menggambar ubin (tiles), serta menangani
 *   penempatan dan animasi menara (tower).
 *
 *   Fitur utama:
 *     - Penyimpanan data peta dalam array 2D `map`.
 *     - Pemrosesan dan penggambaran ubin menggunakan texture atlas.
 *     - Penanganan input mouse untuk menempatkan menara di ubin tertentu.
 *     - Struktur data linked list untuk menyimpan menara yang telah ditempatkan.
 *     - Penggambaran animasi menara secara frame-by-frame.
 */

#include <stdio.h>
#include <stdlib.h>
#include "map.h"

int map[ROWS][COLS] = {
    {0, 8, 37, 53, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3},
    {0, 8, 37, 53, 0, 0, 0, 0, 0, 0, 0, 8, 37, 37, 37},
    {0, 8, 37, 53, 0, 0, 0, 0, 0, 0, 0, 8, 37, 53, 34},
    {0, 8, 37, 53, 0, 0, 0, 0, 0, 0, 0, 8, 37, 53, 0},
    {0, 8, 37, 25, 3, 3, 3, 3, 3, 4, 3, 8, 37, 4, 0},
    {0, 4, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 53, 0},
    {0, 0, 34, 34, 34, 4, 34, 34, 34, 34, 34, 34, 34, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};
Texture2D sheetTiles;
Texture2D circle;
Texture2D timerTex;
Texture2D tree1Tex;
Texture2D tree2Tex;
Texture2D bushTex;

const char *towerFiles[3] = {
    "assets/tower1.png",
    "assets/tower2.png",
    "assets/tower3.png"};
TowerAnimData towers[3];

static bool texturesLoaded = false;
bool timerFastForwardActive = false;
float waveTimerCurrentTime = 0.0f;
float waveTimerDuration = 20.0f;
bool waveTimerVisible = true;

int timerMapRow = 0;
int timerMapCol = 2;

DecorationData DecorationsArray[MAX_DECORATIONS];
int DecorationsCount = 0;

void InitResources(void)
{

    sheetTiles = LoadTexture("assets/tilesheet.png");
    circle = LoadTexture("assets/tanahkosong.png");
    timerTex = LoadTexture("assets/timer.png");
    tree1Tex = LoadTexture("assets/tree1.png");
    tree2Tex = LoadTexture("assets/tree2.png");
    bushTex = LoadTexture("bush.png");

    for (int i = 0; i < 3; i++)
    {
        towers[i].texture = LoadTexture(towerFiles[i]);
        towers[i].frameWidth = towers[i].texture.width / NUM_FRAMES;
        towers[i].frameHeight = towers[i].texture.height;
        towers[i].frameRec = (Rectangle){0, 0,
                                         (float)towers[i].frameWidth,
                                         (float)towers[i].frameHeight};
        towers[i].currentFrame = 0;
        towers[i].frameCounter = 0;
    }
    texturesLoaded = true;

    AddDecoration(DECORATION_TREE1, 2, 0);
    AddDecoration(DECORATION_TREE1, 7, 12);
    AddDecoration(DECORATION_TREE1, 8, 3);
    AddDecoration(DECORATION_TREE1, 9, 0);
    AddDecoration(DECORATION_TREE1, 9, 7);
    AddDecoration(DECORATION_TREE1, 4, 5);
    AddDecoration(DECORATION_TREE2, 0, 6);
    AddDecoration(DECORATION_TREE2, 2, 4);
    AddDecoration(DECORATION_TREE2, 4, 0);
    AddDecoration(DECORATION_TREE2, 3, 8);
    AddDecoration(DECORATION_TREE2, 9, 13);
    AddDecoration(DECORATION_BUSH, 0, 4);
}
void UnloadResources(void)
{
    UnloadTexture(sheetTiles);
    UnloadTexture(timerTex);
    UnloadTexture(tree1Tex);
    UnloadTexture(tree2Tex);
    UnloadTexture(bushTex);
    
    if (texturesLoaded)
    {
        for (int i = 0; i < 3; i++)
            UnloadTexture(towers[i].texture);
    }
}
void DrawTileWithCircle(int tileIndex, int x, int y)
{
    float scale = 1.0f;
    Vector2 tilePos = {x * 32, y * 32};

    DrawTexturePro(
        sheetTiles,
        GetTileSourceRect(tileIndex),
        (Rectangle){tilePos.x, tilePos.y, 32 * scale, 32 * scale},
        (Vector2){0, 0},
        0.0f,
        WHITE);

    float offsetX = (63 - 32) / 2.0f;
    float offsetY = (64 - 32) / 2.0f;
    DrawTextureEx(
        circle,
        (Vector2){tilePos.x - offsetX, tilePos.y - offsetY},
        0.0f,
        1.0f,
        WHITE);
}

Rectangle GetTileSourceRect(int index)
{
    int cols = 8;
    int tileWidth = 32;
    int tileHeight = 32;
    int x = (index % cols) * tileWidth;
    int y = (index / cols) * tileHeight;
    return (Rectangle){x, y, tileWidth, tileHeight};
}
TowerNode *towerList = NULL;

void AddDecoration(DecorationType type, int row, int col) {
    if (DecorationsCount < MAX_DECORATIONS) {
        DecorationsArray[DecorationsCount].type = type;
        DecorationsArray[DecorationsCount].row = row;
        DecorationsArray[DecorationsCount].col = col;
        DecorationsCount++;
    } else {
        TraceLog(LOG_WARNING, "Max decorations reached. Cannot add more.");
    }
}

void AddTowerToList(TowerType type, int row, int col)
{
    TowerNode *node = malloc(sizeof(*node));
    node->type = type;
    node->row = row;
    node->col = col;
    node->next = towerList;
    towerList = node;
}
void DrawTower(TowerType type, int row, int col)
{
    TowerAnimData *t = &towers[type - TOWER1];

    t->frameCounter++;
    if (t->frameCounter >= (60 / FRAME_SPEED))
    {
        t->frameCounter = 0;
        t->currentFrame = (t->currentFrame + 1) % NUM_FRAMES;
        t->frameRec.x = t->currentFrame * t->frameWidth;
    }
    float tileScreenSize = TILE_SIZE * TILE_SCALE;
    float towerW = t->frameWidth * TOWER_SCALE;
    float towerH = t->frameHeight * TOWER_SCALE;
    float px = col * tileScreenSize + (tileScreenSize - towerW) / 2;
    float tileCenterY = row * tileScreenSize + tileScreenSize / 2.0f;
    float offsetY = 20.0f;
    float py = tileCenterY - towerH + offsetY;
    DrawTexturePro(
        t->texture,
        t->frameRec,
        (Rectangle){px, py, towerW, towerH},
        (Vector2){0, 0},
        0.0f, WHITE);
}
void DrawTowers()
{
    TowerNode *current = towerList;
    while (current)
    {
        DrawTower(current->type, current->row, current->col);
        current = current->next;
    }
}

void DrawGameTimer(float globalScale, float offsetX, float offsetY, int timerRow, int timerCol)
{
    float tileScreenSize = TILE_SIZE * globalScale;
    float timerCenterX = offsetX + timerCol * tileScreenSize + tileScreenSize / 2.0f;
    float timerCenterY = offsetY + timerRow * tileScreenSize + tileScreenSize / 2.0f;
    
    Vector2 position = { timerCenterX, timerCenterY };
    float timerRadius = (TILE_SIZE / 2.0f) * globalScale * TIMER_OVERALL_SIZE_FACTOR;

    if (waveTimerCurrentTime < waveTimerDuration)
    {
        waveTimerCurrentTime += GetFrameTime();
    }
    else if (waveTimerVisible) {
        timerFastForwardActive = true;
        waveTimerVisible = false;
    }

    float progress = waveTimerCurrentTime / waveTimerDuration;
    if (progress > 1.0f) progress = 1.0f; 
    float angle = 360.0f * progress;

    if (waveTimerVisible) {
        DrawCircleSector(position, timerRadius, -90, -90 + angle, 100, RED);
        DrawCircle(position.x, position.y, timerRadius * 0.9f, LIGHTGRAY);

        Rectangle destination = { position.x, position.y, timerRadius * TIMER_IMAGE_DISPLAY_FACTOR, timerRadius * TIMER_IMAGE_DISPLAY_FACTOR };
        Vector2 origin = { destination.width / 2.0f, destination.height / 2.0f };
        
        Rectangle source = { 0, 0, (float)timerTex.width, (float)timerTex.height }; 
        DrawTexturePro(timerTex, source, destination, origin, 0.0f, WHITE);
    }
}

void DrawDecorationElement(DecorationType type, int row, int col, float globalScale, float offsetX, float offsetY) {
    Texture2D decorationTexture;
    Rectangle sourceRect;
    float decorationScale = DECORATION_DRAW_SCALE;
    float decorationOffsetY = 0.0f; 

    if (type == DECORATION_TREE1) {
        decorationTexture = tree1Tex;
        sourceRect = (Rectangle){ 0, 0, (float)decorationTexture.width, (float)decorationTexture.height };
        decorationOffsetY = -10.0f * globalScale;
        
        float tileScreenSize = TILE_SIZE * globalScale;
        float decorationWidth = sourceRect.width * decorationScale * globalScale;
        float decorationHeight = sourceRect.height * decorationScale * globalScale;
        float px = offsetX + col * tileScreenSize + (tileScreenSize - decorationWidth) / 2.0f;
        float tileCenterY = offsetY + row * tileScreenSize + tileScreenSize / 2.0f;
        float py = tileCenterY - decorationHeight + decorationOffsetY;

        DrawTexturePro(decorationTexture, sourceRect, (Rectangle){ px, py, decorationWidth, decorationHeight }, (Vector2){ 0, 0 }, 0.0f, WHITE);

    } else if (type == DECORATION_TREE2) { 
        decorationTexture = tree2Tex;
        sourceRect = (Rectangle){ 0, 0, (float)decorationTexture.width, (float)decorationTexture.height };
        decorationOffsetY = -10.0f * globalScale;

        float tileScreenSize = TILE_SIZE * globalScale;
        float decorationWidth = sourceRect.width * decorationScale * globalScale;
        float decorationHeight = sourceRect.height * decorationScale * globalScale;
        float px = offsetX + col * tileScreenSize + (tileScreenSize - decorationWidth) / 2.0f;
        float tileCenterY = offsetY + row * tileScreenSize + tileScreenSize / 2.0f;
        float py = tileCenterY - decorationHeight + decorationOffsetY;
        
        DrawTexturePro(decorationTexture, sourceRect, (Rectangle){ px, py, decorationWidth, decorationHeight }, (Vector2){ 0, 0 }, 0.0f, WHITE);

    } else if (type == DECORATION_BUSH) { 
        decorationTexture = bushTex;
        sourceRect = (Rectangle){ 0, 0, (float)decorationTexture.width, (float)decorationTexture.height };
        decorationOffsetY = 0.0f * globalScale;

        float tileScreenSize = TILE_SIZE * globalScale;
        float decorationWidth = sourceRect.width * decorationScale * globalScale;
        float decorationHeight = sourceRect.height * decorationScale * globalScale;
        float px = offsetX + col * tileScreenSize + (tileScreenSize - decorationWidth) / 2.0f;
        float tileCenterY = offsetY + row * tileScreenSize + tileScreenSize / 2.0f;
        float py = tileCenterY - decorationHeight + decorationOffsetY;
        
        DrawTexturePro(decorationTexture, sourceRect, (Rectangle){ px, py, decorationWidth, decorationHeight }, (Vector2){ 0, 0 }, 0.0f, WHITE);
    }
}
void DrawDecorations(float globalScale, float offsetX, float offsetY) {
    for (int i = 0; i < DecorationsCount; i++) {
        DrawDecorationElement(DecorationsArray[i].type, DecorationsArray[i].row, DecorationsArray[i].col, globalScale, offsetX, offsetY);
    }
}

void handleInput()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 m = GetMousePosition();
        int col = m.x / (TILE_SIZE * TILE_SCALE);
        int row = m.y / (TILE_SIZE * TILE_SCALE);

        if (map[row][col] == 4)
        {
            map[row][col] = TOWER1;
            AddTowerToList(TOWER1, row, col);
        }
    }
}
void DrawMap(void)
{
    for (int y = 0; y < ROWS; y++)
    {
        for (int x = 0; x < COLS; x++)
        {
            int tileIndex = map[y][x];

            Rectangle src = {
                (tileIndex % 8) * 32,
                (tileIndex / 8) * 32,
                32, 32};
            Rectangle dst = {
                x * TILE_SIZE * TILE_SCALE,
                y * TILE_SIZE * TILE_SCALE,
                TILE_SIZE * TILE_SCALE,
                TILE_SIZE * TILE_SCALE};
            DrawTexturePro(sheetTiles, src, dst, (Vector2){0, 0}, 0.0f, WHITE);

            if (tileIndex == 4)
            {
                Rectangle circleSrc = {0, 0, 63, 64};
                Rectangle circleDst = dst;
                DrawTexturePro(circle, circleSrc, circleDst, (Vector2){0, 0}, 0.0f, WHITE);
            }
        }
    }
}
