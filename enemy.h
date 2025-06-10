
#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include <stdbool.h>
#include "map.h" 

#define TIMER_OVERALL_SIZE_FACTOR 0.8f
#define TIMER_IMAGE_DISPLAY_FACTOR 0.8f
#define MAX_ENEMIES_PER_WAVE 20
#define MAX_PATH_POINTS 100
#define WAVE_TIMER_DURATION 10.0f
#define SPAWN_DELAY 1.5f 
#define INTER_WAVE_DELAY 3.0f

typedef struct {
    Texture2D texture;
    int frameCols;        
    int frameSpeed;    
    int frameCount;    
    int currentFrame;  
    float frameCounter; 
    int frameWidth;    
    int frameHeight;   
    Rectangle frameRec; 
} AnimSprite;

typedef struct {    
    Vector2 position;
    int hp;
    float speed;
    bool active;
    bool spawned;
    int pathIndex;
    int segment;        
    float t;  
    AnimSprite animData; 
    int spriteType;     
    float drawScale;     
} Enemy;

typedef struct {
    Enemy *allEnemies; 
    Vector2 path[MAX_PATH_POINTS];
    int pathCount;
    int total;
    int activeCount;
    int spawnedCount;      
    int nextSpawnIndex;    
    float spawnTimer;      
    int waveNum;
    float timerCurrentTime;
    float timerDuration;
    bool timerVisible;
    bool active;
    int timerMapRow;
    int timerMapCol;
    Texture2D timerTexture; 
    float interWaveTimer;
    float waitingForNextWave;
    float lastWaveSpawnTime; 
    float previousWaveSpawnTimeReference;
    // <--- BARU: Waktu (dari GetTime()) saat gelombang sebelumnya mulai spawn penuh
} EnemyWave;

extern EnemyWave *currentWave; 
extern int currentWaveNum;

AnimSprite LoadAnimSprite(const char *filename, int cols, int speed, int frameCount);
void UpdateAnimSprite(AnimSprite *sprite);
void DrawAnimSprite(const AnimSprite *sprite, Vector2 position, float scale, Color tint);
void UnloadAnimSprite(AnimSprite *sprite);

void Enemies_InitAssets();
void Enemies_ShutdownAssets();
void Enemies_Update(EnemyWave *wave, float deltaTime); 
void Enemies_Draw(const EnemyWave *wave, float globalScale, float offsetX, float offsetY);
void Enemies_BuildPath(int startX, int startY, EnemyWave* waveToBuild); // <--- UBAH PROTOTIPE

Vector2 GetEnemyPosition(const Enemy *enemy);
int GetEnemyHP(const Enemy *enemy);
float GetEnemySpeed(const Enemy *enemy);
bool GetEnemyActive(const Enemy *enemy);
bool GetEnemySpawned(const Enemy *enemy);
int GetEnemyPathIndex(const Enemy *enemy); 
Texture2D GetEnemyTexture(int index); 

void SetEnemyPosition(Enemy *enemy, Vector2 position);
void SetEnemyHP(Enemy *enemy, int hp);
void SetEnemySpeed(Enemy *enemy, float speed);
void SetEnemyActive(Enemy *enemy, bool active);
void SetEnemySpawned(Enemy *enemy, bool spawned);
void SetEnemyPathIndex(Enemy *enemy, int index); 

void InitWaveAssets();
void ShutdownWaveAssets();
EnemyWave* CreateWave(int startRow, int startCol); // <--- UBAH PROTOTIPE
void FreeWave(EnemyWave **wave);                  // <--- UBAH PROTOTIPE
void UpdateWaveTimer(EnemyWave *wave, float deltaTime); 
bool AllEnemiesInWaveFinished(const EnemyWave *wave);
void DrawGameTimer(const EnemyWave *wave, float globalScale, float offsetX, float offsetY, int mapRow, int mapCol);

int GetWaveTotal(const EnemyWave *wave);
int GetWaveActiveCount(const EnemyWave *wave);
int GetWaveNum(const EnemyWave *wave);
float GetWaveTimerCurrentTime(const EnemyWave *wave);
float GetWaveTimerDuration(const EnemyWave *wave);
bool GetWaveTimerVisible(const EnemyWave *wave);
bool GetWaveActive(const EnemyWave *wave);
int GetTimerMapRow(const EnemyWave *wave);
int GetTimerMapCol(const EnemyWave *wave);

void SetWaveTotal(EnemyWave *wave, int total);
void SetWaveActiveCount(EnemyWave *wave, int count);
void SetWaveNum(EnemyWave *wave, int num);
void SetWaveTimerCurrentTime(EnemyWave *wave, float time);
void SetWaveTimerDuration(EnemyWave *wave, float duration);
void SetWaveTimerVisible(EnemyWave *wave, bool visible);
void SetWaveActive(EnemyWave *wave, bool active);
void SetTimerMapRow(EnemyWave *wave, int row);
void SetTimerMapCol(EnemyWave *wave, int col);

#endif