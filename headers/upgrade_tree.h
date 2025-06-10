#ifndef UPGRADE_TREE_H
#define UPGRADE_TREE_H

#include "raylib.h"
#include "common.h"
#include "tower.h"

typedef struct
{
    UpgradeNode *root;
} TowerUpgradeTree;

extern TowerUpgradeTree tower1UpgradeTree;

extern Texture2D upgradeIcon_AttackSpeedBase;
extern Texture2D upgradeIcon_AttackPowerBase;
extern Texture2D upgradeIcon_SpecialEffectBase;
extern Texture2D upgradeIcon_LightningAttack;
extern Texture2D upgradeIcon_ChainAttack;
extern Texture2D upgradeIcon_AreaAttack;
extern Texture2D upgradeIcon_CriticalAttack;
extern Texture2D upgradeIcon_StunEffect;
extern Texture2D upgradeIcon_WideChainRange;
extern Texture2D upgradeIcon_LargeAoERadius;
extern Texture2D upgradeIcon_HighCritChance;
extern Texture2D upgradeIcon_LethalPoison;
extern Texture2D upgradeIcon_MassSlow;

extern bool isUpgradeAgreementPanelVisible; 
extern UpgradeNode* selectedUpgradeNodeForAgreement;


bool IsUpgradeOrbitMenuVisible(void); 

Texture2D GetUpgradeIconTexture(UpgradeType type);
UpgradeNode *CreateUpgradeNode(UpgradeType type, const char *name, const char *desc, int cost, UpgradeNode *parent, int exclusiveGroupId);
void InitUpgradeTree(TowerUpgradeTree *tree, TowerType type);

GameState GetUpgradeOrbitGameState(UpgradeType type);

void UpdateUpgradeTreeStatus(TowerUpgradeTree *tree, const Tower *tower);
void UpdateUpgradeOrbitMenu(float deltaTime, Vector2 mousePos, float currentTileScale, float mapScreenOffsetX, float mapScreenOffsetY);
void DrawUpgradeOrbitMenu(float currentTileScale, float mapScreenOffsetX, float mapScreenOffsetY);

void ApplyUpgradeEffect(Tower *tower, UpgradeType type);

void NavigateUpgradeOrbit(UpgradeNode *targetNode);
void NavigateUpgradeOrbitBack(void);

void ResetUpgradeOrbit(void);
void ShowUpgradeAgreementPanel(UpgradeNode* node);
void HideUpgradeAgreementPanel(void);
void ProcessUpgradeAgreement(bool confirmed);

void SetCurrentOrbitParentNode(UpgradeNode *node);
void SetPrevOrbitParentNode(UpgradeNode *node);
UpgradeNode *GetUpgradeTreeRoot(TowerUpgradeTree *tree);
UpgradeNode *GetNthChild(UpgradeNode *parent, int n);
int GetNumChildren(UpgradeNode *node);
UpgradeNode *GetCurrentOrbitParentNode(void);
UpgradeNode *GetPrevOrbitParentNode(void);

void FreeUpgradeTree(TowerUpgradeTree *tree);

#endif