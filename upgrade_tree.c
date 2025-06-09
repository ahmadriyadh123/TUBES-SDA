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

    UpgradeNode* speedBase = CreateUpgradeNode(UPGRADE_ATTACK_SPEED_BASE, "Upgrade Kecepatan Serangan", "Meningkatkan kecepatan serangan dasar.", 0, tree->root, 1);
    UpgradeNode* powerBase = CreateUpgradeNode(UPGRADE_ATTACK_POWER_BASE, "Upgrade Kekuatan Serangan", "Meningkatkan kekuatan serangan dasar.", 0, tree->root, 1);
    UpgradeNode* specialBase = CreateUpgradeNode(UPGRADE_SPECIAL_EFFECT_BASE, "Upgrade Efek Khusus", "Membuka jalur efek khusus.", 0, tree->root, 1);

    tree->root->children[tree->root->numChildren++] = speedBase;
    tree->root->children[tree->root->numChildren++] = powerBase;
    tree->root->children[tree->root->numChildren++] = specialBase;

    UpgradeNode* lightning = CreateUpgradeNode(UPGRADE_LIGHTNING_ATTACK, "Serangan Kilat", "Serangan sangat cepat.", 75, speedBase, 2);
    UpgradeNode* chain = CreateUpgradeNode(UPGRADE_CHAIN_ATTACK, "Serangan Berantai", "Serangan melompat ke musuh terdekat.", 100, speedBase, 2);
    speedBase->children[speedBase->numChildren++] = lightning;
    speedBase->children[speedBase->numChildren++] = chain;

    UpgradeNode* area = CreateUpgradeNode(UPGRADE_AREA_ATTACK, "Serangan Area", "Serangan merusak beberapa musuh sekaligus.", 110, powerBase, 3);
    UpgradeNode* critical = CreateUpgradeNode(UPGRADE_CRITICAL_ATTACK, "Serangan Kritikal", "Meningkatkan peluang critical hit.", 120, powerBase, 3);
    powerBase->children[powerBase->numChildren++] = area;
    powerBase->children[powerBase->numChildren++] = critical;
    
    UpgradeNode* poison = CreateUpgradeNode(UPGRADE_LETHAL_POISON, "Racun Mematikan", "Serangan meracuni musuh.", 150, specialBase, 4);
    UpgradeNode* massSlow = CreateUpgradeNode(UPGRADE_MASS_SLOW, "Perlambat Massal", "Serangan memperlambat area luas.", 160, specialBase, 4);
    specialBase->children[specialBase->numChildren++] = poison;
    specialBase->children[specialBase->numChildren++] = massSlow;

    UpgradeNode* stun = CreateUpgradeNode(UPGRADE_STUN_EFFECT, "Efek Stun", "Serangan Kilat memiliki peluang stun.", 180, lightning, 5); 
    UpgradeNode* wideChain = CreateUpgradeNode(UPGRADE_WIDE_CHAIN_RANGE, "Jangkauan Berantai Luas", "Meningkatkan jangkauan lompatan.", 160, chain, 6); 
    UpgradeNode* largeAoE = CreateUpgradeNode(UPGRADE_LARGE_AOE_RADIUS, "Jangkauan AoE Lebih Besar", "Meningkatkan radius Serangan Area.", 170, area, 7); 
    UpgradeNode* highCrit = CreateUpgradeNode(UPGRADE_HIGH_CRIT_CHANCE, "Peluang Kritikal Tinggi", "Meningkatkan peluang critical hit secara signifikan.", 180, critical, 8); 

    lightning->children[lightning->numChildren++] = stun;
    chain->children[chain->numChildren++] = wideChain;
    area->children[area->numChildren++] = largeAoE;
    critical->children[critical->numChildren++] = highCrit;
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

GameState GetUpgradeOrbitGameState(UpgradeType type) {
    switch (type) {
        case UPGRADE_NONE:
            return GAMEPLAY_TOWER_SELECTED_ORBIT_L1; 
        case UPGRADE_ATTACK_SPEED_BASE:
        case UPGRADE_ATTACK_POWER_BASE:
        case UPGRADE_SPECIAL_EFFECT_BASE:
            return GAMEPLAY_TOWER_SELECTED_ORBIT_L2; 
        case UPGRADE_LIGHTNING_ATTACK:
        case UPGRADE_CHAIN_ATTACK:
        case UPGRADE_AREA_ATTACK:
        case UPGRADE_CRITICAL_ATTACK:
        case UPGRADE_LETHAL_POISON:
        case UPGRADE_MASS_SLOW:
            return GAMEPLAY_TOWER_SELECTED_ORBIT_L3; 
        default:
            return GAMEPLAY; 
    }
}

static void UpdateNodeAndChildrenStatus(UpgradeNode *node, const Tower *tower, int purchasedExclusiveGroupId)
{
    if (!node)
        return;
    node->status = UPGRADE_LOCKED;
    if (tower->purchasedUpgrades[node->type])
    {
        node->status = UPGRADE_PURCHASED;
    }
    else if (node->parent && node->parent->cost > 0 && !tower->purchasedUpgrades[node->parent->type])
    {
        node->status = UPGRADE_LOCKED;
    }
    else if (node->exclusiveGroupId != 0 && purchasedExclusiveGroupId == node->exclusiveGroupId)
    {
        node->status = UPGRADE_LOCKED_EXCLUDED;
    }
    else
    {
        if (node->parent && node->parent->cost == 0 && tower->purchasedUpgrades[node->parent->type])
        {
            node->status = UPGRADE_UNLOCKED;
        }
        else
        {
            node->status = UPGRADE_UNLOCKED;
        }
    }
    int nextPurchasedExclusiveGroupId = purchasedExclusiveGroupId;
    if (node->status == UPGRADE_PURCHASED && node->exclusiveGroupId != 0)
    {
        nextPurchasedExclusiveGroupId = node->exclusiveGroupId;
    }
    for (int i = 0; i < node->numChildren; i++)
    {
        UpdateNodeAndChildrenStatus(node->children[i], tower, nextPurchasedExclusiveGroupId);
    }
}

void UpdateUpgradeTreeStatus(TowerUpgradeTree *tree, const Tower *tower)
{
    if (!tree || !tree->root || !tower)
        return;

    UpdateNodeAndChildrenStatus(tree->root, tower, 0);
}

void ApplyUpgradeEffect(Tower *tower, UpgradeType type)
{
    if (!tower)
        return;

    switch (type)
    {
    case UPGRADE_ATTACK_SPEED_BASE:
        SetTowerAttackSpeed(tower, GetTowerAttackSpeed(tower) * 1.2f);
        TraceLog(LOG_INFO, "Upgrade applied: Attack Speed increased to %.2f", GetTowerAttackSpeed(tower));
        break;
    case UPGRADE_ATTACK_POWER_BASE:
        SetTowerDamage(tower, GetTowerDamage(tower) + 15);
        TraceLog(LOG_INFO, "Upgrade applied: Attack Power increased to %d", GetTowerDamage(tower));
        break;
    case UPGRADE_SPECIAL_EFFECT_BASE:
        TraceLog(LOG_INFO, "Upgrade applied: Special Effect path unlocked.");
        break;
    case UPGRADE_LIGHTNING_ATTACK:
        SetTowerAttackSpeed(tower, GetTowerAttackSpeed(tower) * 1.5f);
        TraceLog(LOG_INFO, "Upgrade applied: Lightning Attack (speed increased to %.2f)", GetTowerAttackSpeed(tower));
        break;
    case UPGRADE_CHAIN_ATTACK:
        SetTowerDamage(tower, GetTowerDamage(tower) * 0.8f);
        TraceLog(LOG_INFO, "Upgrade applied: Chain Attack unlocked.");
        break;
    case UPGRADE_AREA_ATTACK:
        SetTowerDamage(tower, GetTowerDamage(tower) * 0.7f);
        TraceLog(LOG_INFO, "Upgrade applied: Area Attack unlocked.");
        break;
    case UPGRADE_CRITICAL_ATTACK:
        SetTowerDamage(tower, GetTowerDamage(tower) + 5);
        TraceLog(LOG_INFO, "Upgrade applied: Critical Attack unlocked.");
        break;
    case UPGRADE_STUN_EFFECT:
        TraceLog(LOG_INFO, "Upgrade applied: Stun Effect added to Lightning Attack.");
        break;
    case UPGRADE_WIDE_CHAIN_RANGE:
        TraceLog(LOG_INFO, "Upgrade applied: Wide Chain Range increased.");
        break;
    case UPGRADE_LARGE_AOE_RADIUS:
        TraceLog(LOG_INFO, "Upgrade applied: Large AoE Radius increased.");
        break;
    case UPGRADE_HIGH_CRIT_CHANCE:
        SetTowerDamage(tower, GetTowerDamage(tower) + 10);
        TraceLog(LOG_INFO, "Upgrade applied: High Crit Chance increased.");
        break;
    case UPGRADE_LETHAL_POISON:
        TraceLog(LOG_INFO, "Upgrade applied: Lethal Poison effect unlocked.");
        break;
    case UPGRADE_MASS_SLOW:
        TraceLog(LOG_INFO, "Upgrade applied: Mass Slow effect unlocked.");
        break;
    default:
        TraceLog(LOG_WARNING, "Attempted to apply unknown upgrade type: %d", type);
        break;
    }
    tower->purchasedUpgrades[type] = true;
    TraceLog(LOG_INFO, "Applied upgrade %d to tower at (%d,%d).", type, tower->row, tower->col);
}

void UpdateUpgradeOrbitMenu(float deltaTime, Vector2 mousePos, float currentTileScale, float mapScreenOffsetX, float mapScreenOffsetY)
{
    Vector2 orbitCenter = towerSelectionUIPos;
    float orbitRadius = TILE_SIZE * currentTileScale * ORBIT_RADIUS_TILE_FACTOR;
    int numChildren = GetNumChildren(parentNode);
    int totalButtons = numChildren + (parentNode->parent != NULL ? 1 : 0);
    bool clickHandledByOrbitMenu = false;

    UpgradeNode *parentNode = GetCurrentOrbitParentNode();
    if (!parentNode)
    {
        TraceLog(LOG_ERROR, "UpdateUpgradeOrbitMenuLogic: Parent node is NULL!");
        HideTowerSelectionUI();
        return;
    }

    if (parentNode->parent != NULL)
    {
        Rectangle backButtonRect = GetOrbitButtonRect(orbitCenter, orbitRadius, numChildren, totalButtons, ORBIT_BUTTON_DRAW_SCALE, deleteButtonTex);
        if (CheckCollisionPointRec(mousePos, backButtonRect))
        {
            NavigateUpgradeOrbitBack();
            currentGameState = GetUpgradeOrbitGameState(GetCurrentOrbitParentNode()->type);
            clickHandledByOrbitMenu = true;
            TraceLog(LOG_INFO, "Upgrade orbit: Back button clicked. Returning to %s.", GetCurrentOrbitParentNode()->name);
            return;
        }
    }

    for (int i = 0; i < numChildren; i++)
    {
        UpgradeNode *childNode = GetNthChild(parentNode, i);
        if (!childNode)
            continue;

        Rectangle buttonRect = GetOrbitButtonRect(orbitCenter, orbitRadius, i, totalButtons, ORBIT_BUTTON_DRAW_SCALE, upgradeButtonTex);

        if (CheckCollisionPointRec(mousePos, buttonRect))
        {
            clickHandledByOrbitMenu = true;
            if (childNode->cost == 0)
            {
                TraceLog(LOG_INFO, "Upgrade category '%s' clicked. Navigating to next level.", childNode->name);

                selectedTowerForDeletion->purchasedUpgrades[childNode->type] = true;

                UpdateUpgradeTreeStatus(&tower1UpgradeTree, selectedTowerForDeletion);

                NavigateUpgradeOrbit(childNode);
                currentGameState = GetUpgradeOrbitGameState(GetCurrentOrbitParentNode()->type);
                return;
            }
            else
            {
                if (childNode->status == UPGRADE_UNLOCKED)
                {
                    if (GetMoney() >= childNode->cost)
                    {
                        ShowUpgradeAgreementPanel(childNode);

                        return;
                    }
                    else
                    {
                        TraceLog(LOG_WARNING, "Not enough money to buy '%s'. Cost: $%d, Have: $%d", childNode->name, childNode->cost, GetMoney());
                        return;
                    }
                }
                else if (childNode->status == UPGRADE_PURCHASED)
                {
                    TraceLog(LOG_INFO, "Upgrade '%s' already purchased. Moving to next level (if available).", childNode->name);

                    UpdateUpgradeTreeStatus(&tower1UpgradeTree, selectedTowerForDeletion);
                    if (GetNumChildren(childNode) > 0)
                    {
                        NavigateUpgradeOrbit(childNode);
                        currentGameState = GetUpgradeOrbitGameState(GetCurrentOrbitParentNode()->type);
                    }
                    else
                    {
                        currentGameState = GAMEPLAY;
                        HideTowerSelectionUI();
                    }
                    return;
                }
                else if (childNode->status == UPGRADE_LOCKED)
                {
                    TraceLog(LOG_WARNING, "Upgrade '%s' is locked. Purchase its parent first.", childNode->name);
                    return;
                }
                else if (childNode->status == UPGRADE_LOCKED_EXCLUDED)
                {
                    TraceLog(LOG_WARNING, "Upgrade '%s' is excluded because another option in its branch was chosen.", childNode->name);
                    return;
                }
            }
        }
    }
}

void DrawUpgradeOrbitMenu(float currentTileScale, float mapScreenOffsetX, float mapScreenOffsetY)
{
    Vector2 orbitCenter = towerSelectionUIPos;
    float orbitRadius = TILE_SIZE * currentTileScale * ORBIT_RADIUS_TILE_FACTOR;
    int numChildren = GetNumChildren(parentNode);
    int totalButtons = numChildren + (parentNode->parent != NULL ? 1 : 0);

    DrawCircleLines((int)orbitCenter.x, (int)orbitCenter.y, orbitRadius, RAYWHITE);
    UpdateUpgradeTreeStatus(&tower1UpgradeTree, selectedTowerForDeletion);

    UpgradeNode *parentNode = currentOrbitParentNode;
    if (!parentNode)
    {
        TraceLog(LOG_ERROR, "DrawUpgradeOrbitMenu: Parent node is NULL!");
        return;
    }

    for (int i = 0; i < numChildren; i++)
    {
        UpgradeNode *childNode = GetNthChild(parentNode, i);
        if (!childNode)
            continue;

        Rectangle buttonRect = GetOrbitButtonRect(orbitCenter, orbitRadius, i, totalButtons, ORBIT_BUTTON_DRAW_SCALE, upgradeButtonTex);

        Texture2D iconStatusToDraw = lockedIconTex;

        switch (childNode->status)
        {
        case UPGRADE_LOCKED:
            iconStatusToDraw = lockedIconTex;
            break;
        case UPGRADE_UNLOCKED:
            iconStatusToDraw = unlockedIconTex;
            break;
        case UPGRADE_PURCHASED:
            iconStatusToDraw = purchasedIconTex;
            break;
        case UPGRADE_LOCKED_EXCLUDED:
            iconStatusToDraw = excludedIconTex;
            break;
        }

        Texture2D upgradeIcon = GetUpgradeIconTexture(childNode->type);
        if (upgradeIcon.id != 0)
        {
            float iconDrawSize = buttonRect.width * 1.0f;
            Rectangle iconDestRect = {
                buttonRect.x + (buttonRect.width - iconDrawSize) / 2.0f,
                buttonRect.y + (buttonRect.height - iconDrawSize) / 2.0f,
                iconDrawSize,
                iconDrawSize};
            DrawTexturePro(upgradeIcon, (Rectangle){0, 0, (float)upgradeIcon.width, (float)upgradeIcon.height}, iconDestRect, (Vector2){0, 0}, 0.0f, WHITE);
        }

        float statusIconSize = buttonRect.width * 0.25f;
        Rectangle statusIconDestRect = {buttonRect.x + buttonRect.width - statusIconSize - 2, buttonRect.y + 2, statusIconSize, statusIconSize};
        if (iconStatusToDraw.id != 0)
        {
            DrawTexturePro(iconStatusToDraw, (Rectangle){0, 0, (float)iconStatusToDraw.width, (float)iconStatusToDraw.height}, statusIconDestRect, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }
    DrawTowers(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);

    if (isUpgradeAgreementPanelVisible && selectedUpgradeNodeForAgreement != NULL)
    {
        float panelWidth = GetScreenWidth() * 0.2f;
        float panelHeight = GetScreenHeight() * 0.2f;
        float panelX = (GetScreenWidth() - panelWidth) / 2.0f;
        float panelY = (GetScreenHeight() - panelHeight) / 2.0f;

        DrawRectangleRec((Rectangle){panelX, panelY, panelWidth, panelHeight}, Fade(GRAY, 0.9f));
        DrawRectangleLinesEx((Rectangle){panelX, panelY, panelWidth, panelHeight}, 3, RAYWHITE);

        const char *confirmText = TextFormat("Confirm purchase of '%s' for $%d?", selectedUpgradeNodeForAgreement->name, selectedUpgradeNodeForAgreement->cost);
        Vector2 confirmTextSize = MeasureTextEx(GetFontDefault(), confirmText, 25, 1);
        DrawText(confirmText, (int)(panelX + panelWidth / 2 - confirmTextSize.x / 2), (int)(panelY + 30), 25, RAYWHITE);

        Vector2 descTextSize = MeasureTextEx(GetFontDefault(), selectedUpgradeNodeForAgreement->description, 18, 1);
        DrawText(selectedUpgradeNodeForAgreement->description, (int)(panelX + panelWidth / 2 - descTextSize.x / 2), (int)(panelY + 70), 18, LIGHTGRAY);

        float buttonWidth = panelWidth * 0.3f;
        float buttonHeight = 40.0f;
        float buttonSpacing = 20.0f;
        Rectangle okButtonRect = {panelX + panelWidth / 2 - buttonWidth - buttonSpacing / 2, panelY + panelHeight - buttonHeight - 20, buttonWidth, buttonHeight};
        Rectangle cancelButtonRect = {panelX + panelWidth / 2 + buttonSpacing / 2, panelY + panelHeight - buttonHeight - 20, buttonWidth, buttonHeight};

        DrawRectangleRec(okButtonRect, GREEN);
        DrawText("OK", (int)(okButtonRect.x + okButtonRect.width / 2 - MeasureText("OK", 20) / 2), (int)(okButtonRect.y + 10), 20, WHITE);
        DrawRectangleRec(cancelButtonRect, RED);
        DrawText("Cancel", (int)(cancelButtonRect.x + cancelButtonRect.width / 2 - MeasureText("Cancel", 20) / 2), (int)(cancelButtonRect.y + 10), 20, WHITE);

        return;
    }
}

void ShowUpgradeAgreementPanel(UpgradeNode *node)
{
    selectedUpgradeNodeForAgreement = node;
    isUpgradeAgreementPanelVisible = true;
    currentGameState = GAMEPLAY_TOWER_UPGRADE_AGREEMENT;
    TraceLog(LOG_INFO, "Upgrade Agreement: Panel shown for '%s'.", node->name);
}

void HideUpgradeAgreementPanel(void)
{
    selectedUpgradeNodeForAgreement = NULL;
    isUpgradeAgreementPanelVisible = false;
    TraceLog(LOG_INFO, "Upgrade Agreement: Panel hidden.");
}

void ProcessUpgradeAgreement(bool confirmed)
{
    if (confirmed && selectedUpgradeNodeForAgreement != NULL && selectedTowerForDeletion != NULL)
    {
        UpgradeNode *node = selectedUpgradeNodeForAgreement;
        if (node->status == UPGRADE_UNLOCKED && GetMoney() >= node->cost)
        {
            ApplyUpgradeEffect(selectedTowerForDeletion, node->type);
            AddMoney(-node->cost);
            selectedTowerForDeletion->purchasedUpgrades[node->type] = true;

            TraceLog(LOG_INFO, "Confirmed purchase of '%s' for $%d. Money: $%d", node->name, node->cost, GetMoney());

            UpdateUpgradeTreeStatus(&tower1UpgradeTree, selectedTowerForDeletion);

            if (GetNumChildren(node) > 0)
            {
                NavigateUpgradeOrbit(node);
                currentGameState = GetUpgradeOrbitGameState(GetCurrentOrbitParentNode()->type);
            }
            else
            {

                currentGameState = GAMEPLAY;
                HideTowerSelectionUI();
            }
        }
        else
        {
            TraceLog(LOG_WARNING, "Upgrade Agreement: Purchase failed. Node status: %d, Money: %d, Cost: %d", node->status, GetMoney(), node->cost);

            currentGameState = GetUpgradeOrbitGameState(GetCurrentOrbitParentNode()->type);
        }
    }
    else
    {
        TraceLog(LOG_INFO, "Upgrade Agreement: Purchase cancelled.");

        currentGameState = GetUpgradeOrbitGameState(GetCurrentOrbitParentNode()->type);
    }
    HideUpgradeAgreementPanel();
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

UpgradeNode* GetUpgradeTreeRoot(TowerUpgradeTree* tree) {
    return tree ? tree->root : NULL;
}

UpgradeNode* GetNthChild(UpgradeNode* parent, int n) {
    if (!parent || n < 0 || n >= parent->numChildren) {
        return NULL;
    }
    return parent->children[n];
}

int GetNumChildren(UpgradeNode* node) {
    return node ? node->numChildren : 0;
}


void SetCurrentOrbitParentNode(UpgradeNode* node) {
    currentOrbitParentNode = node;
}

UpgradeNode* GetCurrentOrbitParentNode(void) {
    return currentOrbitParentNode;
}


void SetPrevOrbitParentNode(UpgradeNode* node) {
    prevOrbitParentNode = node;
}

UpgradeNode* GetPrevOrbitParentNode(void) {
    return prevOrbitParentNode;
}
