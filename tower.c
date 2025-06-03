#include "raylib.h"
#include "tower.h"
#include "utils.h"
#include "map.h"
#include "economy.h"
#include <stddef.h>
#include <stdlib.h>
#include "raymath.h" 
#include "enemy.h"

void InitTowerAssets(void) {
    towerTexture = LoadTextureSafe("assets/tower1.png");
    deleteButtonTex = LoadTextureSafe("assets/delete.png"); 
    TraceLog(LOG_INFO, "Tower assets initialized.");
}

void ShutdownTowerAssets(void) {
    UnloadTextureSafe(&towerTexture);
    UnloadTextureSafe(&deleteButtonTex); 
    Tower *current = towersListHead;
    while (current != NULL) {
        Tower *next = (Tower*)current->next; 
        free(current); 
        current = next;
    }
    towersListHead = NULL;
    HideDeleteButton();
    TraceLog(LOG_INFO, "Tower assets shutdown.");
}

void HideDeleteButton() {
    selectedTowerForDeletion = NULL;
    deleteButtonVisible = false;
    TraceLog(LOG_INFO, "UI elements for tower deletion cleared.");
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
    SetTowerDamage(newTower, 10);
    SetTowerRange(newTower, 100.0f);
    SetTowerAttackSpeed(newTower, 1.0f);
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
    HideDeleteButton();
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
    HideDeleteButton();
}

void SellTower(Tower *towerToSell)
{
    if (towerToSell == NULL)
        return;

    AddMoney(25);
    TraceLog(LOG_INFO, "Tower at (%d, %d) sold. Money: $%d.", towerToSell->row, towerToSell->col, GetMoney());

    RemoveTower(towerToSell);
}

Vector2 GetTowerPosition(const Tower *tower) { return tower ? tower->position : (Vector2){0, 0}; }
TowerType GetTowerType(const Tower *tower) { return tower ? tower->type : TOWER_TYPE_1; }
int GetTowerDamage(const Tower *tower) { return tower ? tower->damage : 0; }
float GetTowerRange(const Tower *tower) { return tower ? tower->range : 0.0f; }
float GetTowerAttackSpeed(const Tower *tower) { return tower ? tower->attackSpeed : 0.0f; }
float GetTowerAttackCooldown(const Tower *tower) { return tower ? tower->attackCooldown : 0.0f; }
bool GetTowerActive(const Tower *tower) { return tower ? tower->active : false; }
int GetTowerCount(void) { return towerCount; }

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