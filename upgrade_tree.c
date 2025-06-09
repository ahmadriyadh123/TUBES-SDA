#include "upgrade_tree.h"
#include "tower.h" 
#include "player_resources.h" 
#include "utils.h" 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 


TowerUpgradeTree tower1UpgradeTree;

Texture2D upgradeIcon_AttackSpeedBase = {0};
Texture2D upgradeIcon_AttackPowerBase = {0};
Texture2D upgradeIcon_SpecialEffectBase = {0};
Texture2D upgradeIcon_LightningAttack = {0};
Texture2D upgradeIcon_ChainAttack = {0};
Texture2D upgradeIcon_AreaAttack = {0};
Texture2D upgradeIcon_CriticalAttack = {0};
Texture2D upgradeIcon_StunEffect = {0};
Texture2D upgradeIcon_WideChainRange = {0};
Texture2D upgradeIcon_LargeAoERadius = {0};
Texture2D upgradeIcon_HighCritChance = {0};
Texture2D upgradeIcon_LethalPoison = {0};
Texture2D upgradeIcon_MassSlow = {0};

void InitUpgradeTree(TowerUpgradeTree* tree, TowerType type) {
    
    lockedIconTex = LoadTextureSafe("assets/locked.png");
    unlockedIconTex = LoadTextureSafe("assets/unlocked.png");
    purchasedIconTex = LoadTextureSafe("assets/purchased.png");
    excludedIconTex = LoadTextureSafe("assets/excluded.png"); 
    
    upgradeIcon_AttackSpeedBase = LoadTextureSafe("assets/power_upgrade.png"); 
    upgradeIcon_AttackPowerBase = LoadTextureSafe("assets/speed_upgrade.png"); 
    upgradeIcon_SpecialEffectBase = LoadTextureSafe("assets/special_effect.png"); 
    upgradeIcon_LightningAttack = LoadTextureSafe("assets/flash_upgrade.png"); 
    upgradeIcon_ChainAttack = LoadTextureSafe("assets/chains_upgrade.png"); 
    upgradeIcon_AreaAttack = LoadTextureSafe("assets/aoe_upgrade.png"); 
    upgradeIcon_CriticalAttack = LoadTextureSafe("assets/critical_upgrade.png"); 
    upgradeIcon_StunEffect = LoadTextureSafe("assets/stun_upgrade.png"); 
    upgradeIcon_WideChainRange = LoadTextureSafe("assets/chains_upgrade(2).png"); 
    upgradeIcon_LargeAoERadius = LoadTextureSafe("assets/aoe_upgrade(2).png"); 
    upgradeIcon_HighCritChance = LoadTextureSafe("assets/critical_upgrade(2).png"); 
    upgradeIcon_LethalPoison = LoadTextureSafe("assets/poison_upgrade.png"); 
    upgradeIcon_MassSlow = LoadTextureSafe("assets/slow_upgrade.png"); 
    TraceLog(LOG_INFO, "Initializing upgrade tree for Tower Type %d", type);    

    tree->root = CreateUpgradeNode(UPGRADE_NONE, "Upgrade Tower", "Pilih jalur upgrade", 0, NULL, 0); 
    if (!tree->root) return;
    tree->root->status = UPGRADE_UNLOCKED;
}

UpgradeNode* CreateUpgradeNode(UpgradeType type, const char* name, const char* desc, int cost, UpgradeNode* parent, int exclusiveGroupId) {
    UpgradeNode* node = (UpgradeNode*)malloc(sizeof(UpgradeNode));
    if (node == NULL) {
        TraceLog(LOG_ERROR, "Failed to allocate UpgradeNode for %s", name);
        return NULL;
    }
    *node = (UpgradeNode){0}; 
    node->type = type;
    node->name = name;
    node->description = desc;
    node->cost = cost;
    node->parent = parent;
    node->exclusiveGroupId = exclusiveGroupId; 
    node->status = UPGRADE_LOCKED; 
    return node;
}

Texture2D GetUpgradeIconTexture(UpgradeType type) {
    switch (type) {
        case UPGRADE_ATTACK_SPEED_BASE: return upgradeIcon_AttackSpeedBase;
        case UPGRADE_ATTACK_POWER_BASE: return upgradeIcon_AttackPowerBase;
        case UPGRADE_SPECIAL_EFFECT_BASE: return upgradeIcon_SpecialEffectBase;
        case UPGRADE_LIGHTNING_ATTACK: return upgradeIcon_LightningAttack;
        case UPGRADE_CHAIN_ATTACK: return upgradeIcon_ChainAttack;
        case UPGRADE_AREA_ATTACK: return upgradeIcon_AreaAttack;
        case UPGRADE_CRITICAL_ATTACK: return upgradeIcon_CriticalAttack;
        case UPGRADE_STUN_EFFECT: return upgradeIcon_StunEffect;
        case UPGRADE_WIDE_CHAIN_RANGE: return upgradeIcon_WideChainRange;
        case UPGRADE_LARGE_AOE_RADIUS: return upgradeIcon_LargeAoERadius;
        case UPGRADE_HIGH_CRIT_CHANCE: return upgradeIcon_HighCritChance;
        case UPGRADE_LETHAL_POISON: return upgradeIcon_LethalPoison;
        case UPGRADE_MASS_SLOW: return upgradeIcon_MassSlow;
        default: return (Texture2D){0}; 
    }
}

// Fungsi helper untuk menentukan GameState berdasarkan UpgradeType dari node
GameState GetUpgradeOrbitGameState(UpgradeType type) {
    switch (type) {
        case UPGRADE_NONE:
            return GAMEPLAY_TOWER_SELECTED_ORBIT_L1; // Root (Upgrade Tower) -> Level 1
        case UPGRADE_ATTACK_SPEED_BASE:
        case UPGRADE_ATTACK_POWER_BASE:
        case UPGRADE_SPECIAL_EFFECT_BASE:
            return GAMEPLAY_TOWER_SELECTED_ORBIT_L2; // Dari L1 ke L2
        case UPGRADE_LIGHTNING_ATTACK:
        case UPGRADE_CHAIN_ATTACK:
        case UPGRADE_AREA_ATTACK:
        case UPGRADE_CRITICAL_ATTACK:
        case UPGRADE_LETHAL_POISON:
        case UPGRADE_MASS_SLOW:
            return GAMEPLAY_TOWER_SELECTED_ORBIT_L3; // Dari L2 ke L3
        default:
            return GAMEPLAY; // Jika tidak ada level lebih dalam, kembali ke gameplay
    }
}
static void FreeUpgradeNode(UpgradeNode* node) {
    if (!node) return;
    for (int i = 0; i < node->numChildren; i++) {
        FreeUpgradeNode(node->children[i]);
    }
    free(node);
}

void FreeUpgradeTree(TowerUpgradeTree* tree) {
    if (tree && tree->root) {
        FreeUpgradeNode(tree->root);
        tree->root = NULL;
        TraceLog(LOG_INFO, "Upgrade tree freed.");
    }
}