#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"

typedef enum {
    MAIN_MENU,
    MAIN_MENU_PLAY_SELECTION, 
    MAIN_MENU_CUSTOM_MAP_LIST,
    GAMEPLAY,
    GAMEPLAY_TOWER_SELECTED_ORBIT_L1, 
    GAMEPLAY_TOWER_SELECTED_ORBIT_L2, 
    GAMEPLAY_TOWER_SELECTED_ORBIT_L3, 
    GAMEPLAY_TOWER_UPGRADE_AGREEMENT,
    LEVEL_EDITOR,
    SETTINGS,
    LEVEL_COMPLETE,
    GAME_OVER,
    EXITING
} GameState;
extern GameState currentGameState;

typedef enum {
    UPGRADE_NONE = 0,
    UPGRADE_ATTACK_SPEED_BASE,
    UPGRADE_ATTACK_POWER_BASE,
    UPGRADE_SPECIAL_EFFECT_BASE,
    UPGRADE_LIGHTNING_ATTACK,
    UPGRADE_CHAIN_ATTACK,
    UPGRADE_AREA_ATTACK,
    UPGRADE_CRITICAL_ATTACK,
    UPGRADE_STUN_EFFECT,
    UPGRADE_WIDE_CHAIN_RANGE,
    UPGRADE_LARGE_AOE_RADIUS,
    UPGRADE_HIGH_CRIT_CHANCE,
    UPGRADE_LETHAL_POISON,
    UPGRADE_MASS_SLOW, 
    
} UpgradeType;

typedef enum {
    UPGRADE_LOCKED,
    UPGRADE_UNLOCKED,
    UPGRADE_PURCHASED,
    UPGRADE_LOCKED_EXCLUDED
} UpgradeStatus;

typedef struct UpgradeNode {
    UpgradeType type;
    const char* name;
    const char* description;
    int cost;
    
    UpgradeStatus status;
    struct UpgradeNode* parent; 
    
    struct UpgradeNode* children[3]; 
    int numChildren;
    
    int exclusiveGroupId;
    Vector2 uiPosition; 
    Rectangle uiRect;
    Texture2D iconTex; 
} UpgradeNode;

#endif 