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