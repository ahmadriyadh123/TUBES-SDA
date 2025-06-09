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