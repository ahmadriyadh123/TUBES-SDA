#ifndef MAP_H
#define MAP_H
#include "raylib.h"

#define MAP_ROWS 10
#define MAP_COLS 15
#define TILE_SIZE 32

extern int gameMap[MAP_ROWS][MAP_COLS];
extern Texture2D tileSheetTex;
extern Texture2D emptyCircleTex;

void InitMapAssets(void);
void ShutdownMapAssets(void);
void DrawMap(float globalScale, float offsetX, float offsetY);
Rectangle GetTileSourceRect(int index);

int GetMapTile(int row, int col);
void SetMapTile(int row, int col, int value);
Texture2D GetTileSheetTexture(void);
int GetPathCount(void);
Vector2 GetPathPoint(int index);

#endif