#include "raylib.h"
#include "tower.h"
#include "utils.h"
#include "map.h"
#include "raymath.h"
#include "enemy.h"
#include "upgrade_tree.h"
#include "player_resources.h"
#include <stddef.h>
#include <stdlib.h>

Tower *towersListHead = NULL;
Tower *selectedTowerForDeletion = NULL;
Vector2 deleteButtonScreenPos = {0, 0};
Vector2 towerSelectionUIPos = {0, 0}; 
bool isTowerSelectionUIVisible = false;
bool deleteButtonVisible = false;
Texture2D deleteButtonTex = {0};
Texture2D upgradeButtonTex = {0};
float DELETE_BUTTON_DRAW_SCALE = ORBIT_BUTTON_DRAW_SCALE; 
float UPGRADE_BUTTON_DRAW_SCALE = ORBIT_BUTTON_DRAW_SCALE;

static Texture2D towerTexture = {0};

void InitTowerAssets()
{
    towerTexture = LoadTextureSafe("assets/tower1.png");
    deleteButtonTex = LoadTextureSafe("assets/delete_button.png");
    upgradeButtonTex = LoadTextureSafe("assets/upgrade_button.png");
    TraceLog(LOG_INFO, "Tower assets initialized.");
}

void ShutdownTowerAssets()
{
    UnloadTextureSafe(&towerTexture);
    UnloadTextureSafe(&deleteButtonTex);
    UnloadTextureSafe(&upgradeButtonTex);

    Tower *current = towersListHead;
    while (current != NULL)
    {
        Tower *next = (Tower *)current->next;
        free(current);
        current = next;
    }
    towersListHead = NULL;
    HideTowerOrbitUI();
    TraceLog(LOG_INFO, "Tower assets shutdown.");
}   

void ShowTowerOrbitUI(Tower *tower, float currentTileScale, float mapScreenOffsetX, float mapScreenOffsetY)
{
    selectedTowerForDeletion = tower; 

    
    Vector2 towerCenterPos = {
        mapScreenOffsetX + (tower->position.x * currentTileScale),
        mapScreenOffsetY + (tower->position.y * currentTileScale)};
    towerSelectionUIPos = towerCenterPos;
    isTowerSelectionUIVisible = true;
    deleteButtonVisible = true; 

    ResetUpgradeOrbit(); 

    TraceLog(LOG_INFO, "Displaying tower orbit UI for tower at (%d, %d).", tower->row, tower->col);
}

void HideTowerOrbitUI(void)
{
    selectedTowerForDeletion = NULL;
    isTowerSelectionUIVisible = false;
    deleteButtonVisible = false; 
    ResetUpgradeOrbit(); 
    TraceLog(LOG_INFO, "Tower orbit UI hidden.");
}
bool IsTowerOrbitUIVisible(void)
{
    return isTowerSelectionUIVisible;
}

void HideTowerSelectionUI(void) { 
    selectedTowerForDeletion = NULL;
    deleteButtonVisible = false;
    isTowerSelectionUIVisible = false; 
    TraceLog(LOG_INFO, "Tower selection UI hidden.");
}

bool IsTowerSelectionUIVisible(void) {
    return deleteButtonVisible;
}

Rectangle GetOrbitButtonRect(Vector2 orbitCenter, float orbitRadius, int buttonIndex, int totalButtons, float buttonScale, Texture2D buttonTexture) {
    
    
    
    float angleStep = 360.0f / totalButtons;
    float currentAngle = (float)buttonIndex * angleStep - 45.0f; 

    Vector2 buttonPosInOrbit = {
        orbitCenter.x + orbitRadius * cosf(DEG2RAD * currentAngle),
        orbitCenter.y + orbitRadius * sinf(DEG2RAD * currentAngle)
    };

    float visualButtonSize = 57.0f; 
    float btnDrawSize = visualButtonSize * buttonScale; 

    

    return (Rectangle){
        buttonPosInOrbit.x - btnDrawSize / 2.0f,
        buttonPosInOrbit.y - btnDrawSize / 2.0f,
        btnDrawSize,
        btnDrawSize
    };
}

bool CheckOrbitButtonClick(Vector2 mousePos, Vector2 orbitCenter, float orbitRadius, int buttonIndex, int totalButtons, float buttonScale, Texture2D buttonTexture) {
    Rectangle rect = GetOrbitButtonRect(orbitCenter, orbitRadius, buttonIndex, totalButtons, buttonScale, buttonTexture);
    return CheckCollisionPointRec(mousePos, rect);
}

void PlaceTower(int row, int col, TowerType type)
{

    if (GetTowerAtMapCoord(row, col) != NULL)
    {
        TraceLog(LOG_WARNING, "Cannot place tower: Already a tower at (%d, %d).", row, col);
        return;
    }
    if (GetMoney() < 50)
    {
        TraceLog(LOG_WARNING, "Cannot place tower: Insufficient money.");
        return;
    }
    if (GetMapTile(row, col) != 4)
    {
        TraceLog(LOG_WARNING, "Cannot place tower: Invalid tile type %d at (%d, %d).", GetMapTile(row, col), row, col);
        return;
    }

    Tower *newTower = (Tower *)malloc(sizeof(Tower));
    if (newTower == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for new tower.");
        return;
    }

    *newTower = (Tower){0};
    SetTowerPosition(newTower, (Vector2){col * TILE_SIZE + TILE_SIZE / 2.0f, row * TILE_SIZE + TILE_SIZE / 2.0f});
    SetTowerType(newTower, type);
    SetTowerDamage(newTower, 30);
    SetTowerRange(newTower, 100.0f);
    SetTowerAttackSpeed(newTower, 0.8f);
    SetTowerAttackCooldown(newTower, 0.0f);
    SetTowerActive(newTower, true);
    newTower->texture = towerTexture;
    newTower->frameWidth = TOWER_FRAME_WIDTH;
    newTower->frameHeight = TOWER_FRAME_HEIGHT;
    newTower->currentFrame = 0;
    newTower->frameTimer = 0.0f;
    newTower->row = row;
    newTower->col = col;
    newTower->next = NULL;

    newTower->next = (struct Tower *)towersListHead;
    towersListHead = newTower;

    AddMoney(-50);
    SetMapTile(row, col, 7);
    TraceLog(LOG_INFO, "Tower placed at (%d, %d). Money: $%d.", row, col, GetMoney());
    HideTowerOrbitUI();
}

void RemoveTower(Tower *towerToRemove)
{
    if (towerToRemove == NULL)
        return;

    Tower *current = towersListHead;
    Tower *prev = NULL;

    while (current != NULL && current != towerToRemove)
    {
        prev = current;
        current = (Tower *)current->next;
    }

    if (current == NULL)
    {
        TraceLog(LOG_WARNING, "Attempted to remove a tower not found in the list.");
        return;
    }

    if (prev == NULL)
    {
        towersListHead = (Tower *)current->next;
    }
    else
    {
        prev->next = (struct Tower *)current->next;
    }

    SetMapTile(current->row, current->col, 4);

    free(current);
    TraceLog(LOG_INFO, "Tower removed from map at (%d, %d).", towerToRemove->row, towerToRemove->col);
    HideTowerOrbitUI();
}

void SellTower(Tower *towerToSell)
{
    if (towerToSell == NULL)
        return;

    AddMoney(25);
    TraceLog(LOG_INFO, "Tower at (%d, %d) sold. Money: $%d.", towerToSell->row, towerToSell->col, GetMoney());

    RemoveTower(towerToSell);
}

void UpdateTowerAttacks(EnemyWave *wave, float deltaTime)
{
    if (wave == NULL || wave->allEnemies == NULL || GetWaveActiveCount(wave) <= 0)
        return;

    Tower *current = towersListHead;
    while (current != NULL)
    {
        if (!GetTowerActive(current))
        {
            current = (Tower *)current->next;
            continue;
        }

        current->frameTimer += deltaTime;
        if (current->frameTimer >= TOWER_ANIMATION_SPEED)
        {
            current->currentFrame++;
            if (current->currentFrame >= 6)
            {
                current->currentFrame = 0;
            }
            current->frameTimer = 0.0f;
        }

        float attackCooldown = GetTowerAttackCooldown(current);
        if (attackCooldown > 0)
        {
            SetTowerAttackCooldown(current, attackCooldown - deltaTime);
            current = (Tower *)current->next;
            continue;
        }

        for (int j = 0; j < GetWaveTotal(wave); j++)
        {
            Enemy *enemy = &wave->allEnemies[j];
            if (!GetEnemyActive(enemy) || !GetEnemySpawned(enemy))
                continue;

            Vector2 enemyPos = GetEnemyPosition(enemy);
            float distance = Vector2Distance(GetTowerPosition(current), enemyPos);
            if (distance <= GetTowerRange(current))
            {
                int damage = GetTowerDamage(current);
                SetEnemyHP(enemy, GetEnemyHP(enemy) - damage);
                SpawnVisualShot(GetTowerPosition(current), enemyPos, RAYWHITE, 3.0f, 0.05f); 
                SetTowerAttackCooldown(current, GetTowerAttackSpeed(current));
                if (GetEnemyHP(enemy) <= 0)
                {
                    SetEnemyActive(enemy, false);   
                    SetWaveActiveCount(wave, GetWaveActiveCount(wave) - 1);
                    AddMoney(10 + (wave->waveNum * 2));
                }
                break;
            }
        }
        current = (Tower *)current->next;
    }
}

void DrawTowers(float globalScale, float offsetX, float offsetY)
{
    if (currentGameState == GAMEPLAY && IsTowerOrbitUIVisible() && selectedTowerForDeletion != NULL) { 
        Vector2 orbitCenter = towerSelectionUIPos;
        float orbitRadius = TILE_SIZE * globalScale * ORBIT_RADIUS_TILE_FACTOR;
        
        DrawCircleLines((int)orbitCenter.x, (int)orbitCenter.y, orbitRadius, RAYWHITE);
        
        
        Rectangle deleteBtnRect = GetOrbitButtonRect(orbitCenter, orbitRadius, 0, 2, ORBIT_BUTTON_DRAW_SCALE, deleteButtonTex);
        DrawTexturePro(deleteButtonTex, (Rectangle){0,0,(float)deleteButtonTex.width,(float)deleteButtonTex.height}, deleteBtnRect, (Vector2){0,0}, 0.0f, WHITE);
        
        
        Rectangle upgradeBtnRect = GetOrbitButtonRect(orbitCenter, orbitRadius, 1, 2, ORBIT_BUTTON_DRAW_SCALE, upgradeButtonTex);
        DrawTexturePro(upgradeButtonTex, (Rectangle){0,0,(float)upgradeButtonTex.width,(float)upgradeButtonTex.height}, upgradeBtnRect, (Vector2){0,0}, 0.0f, WHITE);
    }
    Tower *current = towersListHead;
    while (current != NULL)
    {
        if (!GetTowerActive(current))
        {
            current = (Tower *)current->next;
            continue;
        }

        Rectangle sourceRect = {
            (float)current->currentFrame * current->frameWidth,
            0.0f,
            (float)current->frameWidth,
            (float)current->frameHeight};

        Vector2 pos = GetTowerPosition(current);

        float finalDrawWidth = current->frameWidth * TOWER_DRAW_SCALE * globalScale;
        float finalDrawHeight = current->frameHeight * TOWER_DRAW_SCALE * globalScale;

        Rectangle destRect = {
            offsetX + (pos.x * globalScale) - (finalDrawWidth / 2.0f),
            offsetY + (pos.y * globalScale) - finalDrawHeight + (TOWER_Y_OFFSET_PIXELS * globalScale),
            finalDrawWidth,
            finalDrawHeight};

        DrawTexturePro(current->texture,
                       sourceRect,
                       destRect,
                       (Vector2){0, 0}, 0.0f, WHITE);

        current = (Tower *)current->next;
    }
}

Vector2 GetTowerPosition(const Tower *tower) { return tower ? tower->position : (Vector2){0, 0}; }
TowerType GetTowerType(const Tower *tower) { return tower ? tower->type : TOWER_TYPE_1; }
int GetTowerDamage(const Tower *tower) { return tower ? tower->damage : 0; }
float GetTowerRange(const Tower *tower) { return tower ? tower->range : 0.0f; }
float GetTowerAttackSpeed(const Tower *tower) { return tower ? tower->attackSpeed : 0.0f; }
float GetTowerAttackCooldown(const Tower *tower) { return tower ? tower->attackCooldown : 0.0f; }
bool GetTowerActive(const Tower *tower) { return tower ? tower->active : false; }

void SetTowerPosition(Tower *tower, Vector2 position)
{
    if (tower)
        tower->position = position;
}
void SetTowerType(Tower *tower, TowerType type)
{
    if (tower)
        tower->type = type;
}
void SetTowerDamage(Tower *tower, int damage)
{
    if (tower)
        tower->damage = damage;
}
void SetTowerRange(Tower *tower, float range)
{
    if (tower)
        tower->range = range;
}
void SetTowerAttackSpeed(Tower *tower, float attackSpeed)
{
    if (tower)
        tower->attackSpeed = attackSpeed;
}
void SetTowerAttackCooldown(Tower *tower, float cooldown)
{
    if (tower)
        tower->attackCooldown = cooldown;
}
void SetTowerActive(Tower *tower, bool active)
{
    if (tower)
        tower->active = active;
}

Tower *GetTowerAtMapCoord(int row, int col)
{
    Tower *current = towersListHead;
    while (current != NULL)
    {
        if (current->row == row && current->col == col)
        {
            return current;
        }
        current = (Tower *)current->next;
    }
    return NULL;
}