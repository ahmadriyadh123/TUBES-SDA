#ifndef TOWER_H
#define TOWER_H
#include "raylib.h"
#include "enemy.h"
#include "map.h"

#define TOWER_FRAME_WIDTH 70
#define TOWER_FRAME_HEIGHT 130
#define TOWER_ANIMATION_SPEED 0.1f  
#define TOWER_Y_OFFSET_PIXELS 15.0f 
#define TOWER_DRAW_SCALE 0.7f 

typedef enum {
    TOWER_TYPE_1,
    TOWER_TYPE_2,
    TOWER_TYPE_3
} TowerType;

typedef struct {
    Vector2 position;
    TowerType type;
    int frameWidth;    
    int frameHeight;   
    int currentFrame;  
    float frameTimer;  
    int damage;
    float range;
    float attackSpeed;
    float attackCooldown; 
    bool active;
    Texture2D texture;
    int row; 
    int col; 
    struct Tower *next; 
} Tower;

extern Tower *towersListHead; 
extern Tower *selectedTowerForDeletion; 
extern Vector2 deleteButtonScreenPos;   
extern bool deleteButtonVisible;        
extern Texture2D deleteButtonTex;       
extern float DELETE_BUTTON_DRAW_SCALE; 

void InitTowerAssets(void);
void ShutdownTowerAssets(void);

void PlaceTower(int row, int col, TowerType type);
void UpdateTowerAttacks(EnemyWave *wave, float deltaTime);
void DrawTowers(float globalScale, float offsetX, float offsetY);
void RemoveTower(Tower *towerToRemove); 
void SellTower(Tower *towerToSell);     
Tower* GetTowerAtMapCoord(int row, int col); 
void HideDeleteButton();


Vector2 GetTowerPosition(const Tower *tower);
TowerType GetTowerType(const Tower *tower);
int GetTowerDamage(const Tower *tower);
float GetTowerRange(const Tower *tower);
float GetTowerAttackSpeed(const Tower *tower);
float GetTowerAttackCooldown(const Tower *tower);
bool GetTowerActive(const Tower *tower);

void SetTowerPosition(Tower *tower, Vector2 position);
void SetTowerType(Tower *tower, TowerType type);
void SetTowerDamage(Tower *tower, int damage);
void SetTowerRange(Tower *tower, float range);
void SetTowerAttackSpeed(Tower *tower, float attackSpeed);
void SetTowerAttackCooldown(Tower *tower, float cooldown);
void SetTowerActive(Tower *tower, bool active);

#endif