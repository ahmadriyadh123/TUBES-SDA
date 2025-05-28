/*
 * File: map.h
 * Description:
 *   Header file utama untuk game tower defense 2D ini. 
 *   Berisi definisi konstanta, tipe data, dan deklarasi fungsi serta variabel global 
 *   yang digunakan dalam manajemen peta dan objek menara.
 *
 *   Fitur utama:
 *     - Konstanta ukuran peta dan parameter skala ubin/menara.
 *     - Enum `TowerType` untuk mengidentifikasi jenis menara.
 *     - Struktur `TowerAnimData` untuk menyimpan data animasi menara.
 *     - Struktur `TowerNode` untuk linked list menara yang dipasang pada peta.
 *     - Deklarasi fungsi-fungsi utama seperti penggambaran dan input.
*/

#ifndef MAP_H
#define MAP_H

#include "raylib.h"

#define TIMER_OVERALL_SIZE_FACTOR 1.0f
#define TIMER_IMAGE_DISPLAY_FACTOR 1.5f
#define ROWS 10
#define COLS 15
#define TILE_SIZE 32
#define TILE_SCALE 3.0f
#define TOWER_SCALE 2.0f
#define NUM_FRAMES 6
#define FRAME_SPEED 3
#define DECORATION_DRAW_SCALE 0.5f
#define MAX_DECORATIONS 20 

extern int map[ROWS][COLS];
extern Texture2D sheetTiles;
extern Texture2D timerTex;

typedef enum { 
    DECORATION_TREE1, 
    DECORATION_TREE2,
    DECORATION_BUSH, 
} DecorationType;

typedef enum
{
    TOWER1 = 4,
    TOWER2 = 5,
    TOWER3 = 6
} TowerType;

typedef struct
{
    Texture2D texture;
    int frameWidth, frameHeight;
    Rectangle frameRec;
    int currentFrame;
    int frameCounter;
} TowerAnimData;
extern TowerAnimData towers[3];
extern const char *towerFiles[3];

typedef struct TowerNode
{
    TowerType type;
    int row, col;
    struct TowerNode *next;
} TowerNode;
extern TowerNode *towerList;

typedef struct {
    DecorationType type;
    int row, col;
} DecorationData;

typedef struct Decoration {
    DecorationType type;
    int row, col;
    void* associatedObject; 
} Decoration;
extern Decoration *decorationsListHead; 

extern bool timerFastForwardActive;
extern float waveTimerCurrentTime;
extern float waveTimerDuration;
extern bool waveTimerVisible;

// Timer Position on Map Grid
extern int timerMapRow;
extern int timerMapCol;
extern DecorationData DecorationsArray[MAX_DECORATIONS];
extern int DecorationsCount; 

void InitResources(void);
void UnloadResources(void);
Rectangle GetTileSourceRect(int index);
void DrawTileWithCircle(int tileIndex, int x, int y);
void AddDecoration(DecorationType type, int row, int col);
void AddTowerToList(TowerType type, int row, int col);
void DrawMap(void);
void DrawTowers(void);
void DrawTower(TowerType type, int row, int col);
void handleInput();
void DrawGameTimer(float globalScale, float offsetX, float offsetY, int timerRow, int timerCol);
void DrawDecorationElement(DecorationType type, int row, int col, float globalScale, float offsetX, float offsetY);
void DrawDecorations(float globalScale, float offsetX, float offsetY);

#endif