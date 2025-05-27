#ifndef OBJECT_H
#define OBJECT_H

#include "raylib.h"
#include <stdbool.h>

typedef struct {
    float t;
    float speed;
    int segment;
    bool active;
    bool spawned;
    int spriteType;
} Enemy;

typedef struct {
    Enemy* allEnemies;
    int total;
    int nextSpawnIndex;
    float spawnTimer;
} EnemyWave;

typedef struct {
    Texture2D texture;
    Rectangle frameRec;
    int frameCols, frameRows;
    int frameWidth, frameHeight;
    int frameCount;       // ← jumlah frame AKTIF pada baris
    int animRow;
    int currentFrame;
    int frameCounter;
    int frameSpeed;
}AnimSprite;

AnimSprite LoadAnimSprite(const char *filename, int cols, int rows, int rowIndex, int speed, int frameCount);
void UpdateAnimSprite(AnimSprite* sprite);
void DrawAnimSprite(AnimSprite *sprite, Vector2 position, float scale);
void UnloadAnimSprite(AnimSprite* sprite);
void walkSprite(AnimSprite* sprite);

#endif