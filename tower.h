#ifndef TOWER_H
#define TOWER_H
#include "raylib.h"
#include "common.h"
#include "enemy.h"
#include "map.h"

#define TOWER_FRAME_WIDTH 70
#define TOWER_FRAME_HEIGHT 130
#define TOWER_ANIMATION_SPEED 0.1f  
#define TOWER_Y_OFFSET_PIXELS 15.0f 
#define TOWER_DRAW_SCALE 0.6f 
#define ORBIT_RADIUS_FACTOR 1.5f 
#define ORBIT_BUTTON_DRAW_SCALE 2.0f 
#define ORBIT_RADIUS_TILE_FACTOR 1.0f

typedef enum {
    TOWER_TYPE_1,
    TOWER_TYPE_2,
    TOWER_TYPE_3
} TowerType;

typedef struct Tower{
    Vector2 position;
    TowerType type;
    int damage;
    float range;
    float attackSpeed;
    float attackCooldown; 
    bool active;
    Texture2D texture;
    int frameWidth;    
    int frameHeight;   
    int currentFrame;  
    float frameTimer;  
    int row; 
    int col; 
    struct Tower *next; 
    bool purchasedUpgrades[UPGRADE_MASS_SLOW + 1]; 
} Tower;

extern Tower *towersListHead; 
extern Tower *selectedTowerForDeletion; 
extern Vector2 towerSelectionUIPos;   
extern bool isTowerSelectionUIVisible;
extern Vector2 deleteButtonScreenPos;   
extern bool deleteButtonVisible;        
extern float DELETE_BUTTON_DRAW_SCALE; 
extern Texture2D deleteButtonTex;       
extern Texture2D upgradeButtonTex; 

void InitTowerAssets();
void ShutdownTowerAssets();

void PlaceTower(int row, int col, TowerType type);
void UpdateTowerAttacks(EnemyWave *wave, float deltaTime);
void DrawTowers(float globalScale, float offsetX, float offsetY);
void RemoveTower(Tower *towerToRemove); 
void SellTower(Tower *towerToSell);     
Tower* GetTowerAtMapCoord(int row, int col); 
void HideTowerSelectionUI(void); 

void ShowTowerOrbitUI(Tower *tower, float currentTileScale, float mapScreenOffsetX, float mapScreenOffsetY); 
void HideTowerOrbitUI(void); 
bool IsTowerOrbitUIVisible(void); 

Rectangle GetOrbitButtonRect(Vector2 orbitCenter, float orbitRadius, int buttonIndex, int totalButtons, float buttonScale, Texture2D buttonTexture);
bool CheckOrbitButtonClick(Vector2 mousePos, Vector2 orbitCenter, float orbitRadius, int buttonIndex, int totalButtons, float buttonScale, Texture2D buttonTexture);

bool IsTowerSelectionUIVisible(void);
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