#ifndef UPGRADE_TREE_H
#define UPGRADE_TREE_H

#include "common.h"

struct Tower;
struct UpgradeNode;

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
extern Texture2D acceptIconTex;

extern UpgradeNode* pendingUpgradeNode;
extern Vector2 pendingUpgradeIconPos;  

Texture2D GetUpgradeIconTexture(UpgradeType type);
UpgradeNode *GetUpgradeTreeRoot(TowerUpgradeTree *tree);
UpgradeNode *GetCurrentOrbitParentNode(void);
UpgradeNode *GetNthChild(UpgradeNode *parent, int n);
UpgradeNode *GetPrevOrbitParentNode(void);

bool IsUpgradeOrbitMenuVisible(void); 

UpgradeNode* GetCurrentOrbitParentNode(void); 

void InitUpgradeTree(TowerUpgradeTree *tree, TowerType type);
void FreeUpgradeTree(TowerUpgradeTree *tree);
int GetNumChildren(UpgradeNode *node);

void UpdateUpgradeTreeStatus(TowerUpgradeTree *tree, const struct Tower *tower);
bool HandleUpgradeOrbitClick(Vector2 mousePos, float currentTileScale);
void DrawUpgradeOrbitMenu(float currentTileScale, float mapScreenOffsetX, float mapScreenOffsetY);

void ApplyUpgradeEffect(struct Tower *tower, UpgradeType type);
void SetCurrentOrbitParentNode(UpgradeNode *node);
void SetPrevOrbitParentNode(UpgradeNode *node);
void NavigateUpgradeOrbit(UpgradeNode *targetNode);
void NavigateUpgradeOrbitBack(void);
void ResetUpgradeOrbit(void);
void ShowUpgradeAgreementPanel(UpgradeNode* node);
void HideUpgradeAgreementPanel(void);
void ProcessUpgradeAgreement(bool confirmed);

#endif