
#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include <stdbool.h>
#include "map.h" 

#define MAX_ENEMIES_PER_WAVE 20
#define MAX_PATH_POINTS 100
#define WAVE_TIMER_DURATION 5.0f
#define SPAWN_DELAY 0.1f 

typedef struct {
    Texture2D texture;
    int frameCols;     
    int frameRows;     
    int animRow;       
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
    int sheetWidth;
    int sheetHeight;
    int totalCols;
    int totalRows;
    int animRow;
    int animSpeed;
    int frameCount;
    float drawScale;
    const char* texturePath; // Path tekstur untuk dimuat
} EnemyTypeProperties;

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
} EnemyWave;

extern EnemyWave currentWave; 

AnimSprite LoadAnimSprite(const char *filename, int cols, int rows, int rowIndex, int speed, int frameCount);
void UpdateAnimSprite(AnimSprite *sprite);
void DrawAnimSprite(const AnimSprite *sprite, Vector2 position, float scale, Color tint);
void UnloadAnimSprite(AnimSprite *sprite);

void Enemies_InitAssets(void);
void Enemies_ShutdownAssets(void);
void Enemies_Update(EnemyWave *wave, float deltaTime); 
void Enemies_Draw(const EnemyWave *wave, float globalScale, float offsetX, float offsetY);
void Enemies_BuildPath(int startX, int startY); 

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

#endif