#include "map.h"
#include "utils.h"
#include <stdio.h>

#define MAX_PATH_POINTS 100

int gameMap[MAP_ROWS][MAP_COLS] = {
    {0, 8, 37, 53, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3},
    {0, 8, 37, 53, 0, 0, 0, 0, 0, 0, 0, 8, 37, 37, 37},
    {0, 8, 37, 53, 0, 0, 0, 0, 0, 0, 0, 8, 37, 53, 34},
    {0, 8, 37, 53, 0, 0, 0, 0, 0, 0, 0, 8, 37, 53, 0},
    {0, 4, 37, 25, 3, 3, 3, 3, 3, 4, 3, 8, 37, 4, 0},
    {0, 8, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 53, 0},
    {0, 0, 34, 34, 34, 4, 34, 34, 34, 34, 34, 34, 34, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

Texture2D tileSheetTex = {0};
Texture2D emptyCircleTex = {0};
static Vector2 path[MAX_PATH_POINTS] = {0};
static int pathCount = 0;

void InitMapAssets(void) {
    tileSheetTex = LoadTextureSafe("assets/tilesheet.png");
    emptyCircleTex = LoadTextureSafe("assets/kosong2.png");
    TraceLog(LOG_INFO, "Map assets initialized.");
}

void ShutdownMapAssets(void) {
    UnloadTextureSafe(&tileSheetTex);
    UnloadTextureSafe(&emptyCircleTex);
    TraceLog(LOG_INFO, "Map assets unloaded.");
}

void DrawMap(float globalScale, float offsetX, float offsetY) {
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            int tileIndex = gameMap[r][c]; 
            Rectangle sourceRect = GetTileSourceRect(tileIndex); 

            Rectangle destRect = {
                offsetX + c * TILE_SIZE * globalScale,
                offsetY + r * TILE_SIZE * globalScale,
                TILE_SIZE * globalScale,
                TILE_SIZE * globalScale
            };

            DrawTexturePro(tileSheetTex, sourceRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);

            if (tileIndex == 4) {
                
                Rectangle circleSource = {0, 0, (float)emptyCircleTex.width, (float)emptyCircleTex.height};
                DrawTexturePro(emptyCircleTex, circleSource, destRect, (Vector2){0, 0}, 0.0f, WHITE);
            }
            
        }
    }
}

Rectangle GetTileSourceRect(int tileIndex) {
    switch (tileIndex) {
        case 0: return (Rectangle){0, 0, TILE_SIZE, TILE_SIZE};
        case 3: return (Rectangle){2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
        case 8: return (Rectangle){0, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        case 25: return (Rectangle){5 * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        case 34: return (Rectangle){2 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        case 37: return (Rectangle){5 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        case 53: return (Rectangle){4 * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        default: return (Rectangle){0, 0, TILE_SIZE, TILE_SIZE};
    }
}

int GetMapTile(int row, int col) {
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS) {
        return gameMap[row][col];
    }
    return 0;
}

void SetMapTile(int row, int col, int value) {
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS) {
        gameMap[row][col] = value;
    }
}

Texture2D GetTileSheetTexture(void) {
    return tileSheetTex;
}

int GetPathCount(void) {
    return pathCount;
}

Vector2 GetPathPoint(int index) {
    if (index >= 0 && index < pathCount) {
        return path[index];
    }
    return (Vector2){0, 0};
}