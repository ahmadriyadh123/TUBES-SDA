// #ifndef ENEMY_H
// #define ENEMY_H

// #include "raylib.h"
// #include <stdbool.h>
// // #include "map.h"

// #define MAX_PATH_POINTS 60
// #define MAX_WAVES 5
// #define SPAWN_DELAY 1.0f

// typedef struct AnimSprite
// {
//     Texture2D texture;
//     Rectangle frameRec;
//     int frameCols, frameRows;
//     int frameWidth, frameHeight;
//     int frameCount;
//     int animRow;
//     int currentFrame;
//     int frameCounter;
//     int frameSpeed;
// } AnimSprite;

// typedef struct Enemy
// {
//     // Vector2 position;
//     float t;
//     float speed;
//     int segment;
//     bool active;
//     bool spawned;
//     int spriteType;
// } Enemy;

// typedef struct EnemyWave
// {
//     Enemy *allEnemies;
//     int total;
//     // int spawnedCount;
//     // int activeCount;
//     int nextSpawnIndex;
//     float spawnTimer;
// } EnemyWave;

// extern AnimSprite enemy1_sprite;
// extern AnimSprite enemy2_sprite;
// extern Vector2 path[MAX_PATH_POINTS];
// extern int pathCount;

// AnimSprite LoadAnimSprite(const char *filename, int cols, int rows, int rowIndex, int speed, int frameCount);
// void UpdateAnimSprite(AnimSprite *sprite);
// void DrawAnimSprite(AnimSprite *sprite, Vector2 position, float scale, Color tint);
// void UnloadAnimSprite(AnimSprite *sprite);

// void Enemies_InitAssets(void);

// void Enemies_BuildPath(int startX, int startY);
// EnemyWave CreateWave(int count, int waveNum);
// void FreeWave(EnemyWave *wave);
// bool AllEnemiesInWaveFinished(EnemyWave wave);
// void Enemies_Update(EnemyWave *wave, float delta);
// void Enemies_Draw(const EnemyWave *wave, float globalScale, float mapScreenOffsetX, float mapScreenOffsetY);
// void Enemies_ShutdownAssets(void);

// #endif
