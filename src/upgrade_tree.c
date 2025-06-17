/* File        : upgrade_tree.c 
* Deskripsi   : Implementasi untuk modul Pohon Upgrade (Upgrade Tree). 
* Berisi semua logika untuk menginisialisasi tree, menangani input, 
* memperbarui status, dan menggambar antarmuka pengguna (UI) untuk menu upgrade. 
* Dibuat oleh  : Ahmad Riyadh Almaliki
* Perubahan terakhir : Sabtu, 14 Juni 2025
*/

#include "upgrade_tree.h"
#include "tower.h"
#include "player_resources.h"
#include "utils.h"
#include "status.h"
#include "audio.h"

static Texture2D upgradeIcon_AttackSpeedBase = {0};
static Texture2D upgradeIcon_AttackPowerBase = {0};
static Texture2D upgradeIcon_SpecialEffectBase = {0};
static Texture2D upgradeIcon_LightningAttack = {0};
static Texture2D upgradeIcon_ChainAttack = {0};
static Texture2D upgradeIcon_AreaAttack = {0};
static Texture2D upgradeIcon_CriticalAttack = {0};
static Texture2D upgradeIcon_StunEffect = {0};
static Texture2D upgradeIcon_WideChainRange = {0};
static Texture2D upgradeIcon_LargeAoERadius = {0};
static Texture2D upgradeIcon_HighCritChance = {0};
static Texture2D upgradeIcon_LethalPoison = {0};
static Texture2D upgradeIcon_MassSlow = {0};
static Texture2D upgradeIcon_CustomStat = {0};
static Texture2D acceptIconTex = { 0 };
static Texture2D lockedIconTex;
static UpgradeNode *currentOrbitParentNode = NULL;
static UpgradeNode *prevOrbitParentNode = NULL;

TowerUpgradeTree tower1UpgradeTree;
UpgradeNode* pendingUpgradeNode = NULL;
Vector2 pendingUpgradeIconPos = { 0 };

void AddChild(UpgradeNode* parent, UpgradeNode* child) {
    if (!parent || !child) return;
    
    child->parent = parent; // Tetap set parent-nya

    // Jika parent belum punya anak sama sekali
    if (parent->firstChild == NULL) {
        parent->firstChild = child;
    } else {
        // Jika sudah punya anak, cari anak terakhir dan tambahkan child baru sebagai saudaranya
        UpgradeNode* sibling = parent->firstChild;
        while (sibling->nextSibling != NULL) {
            sibling = sibling->nextSibling;
        }
        sibling->nextSibling = child;
    }
}

/*
I.S : Parameter-parameter seperti type, name, desc, cost, parent, dan exclusiveGroupId telah siap untuk digunakan dalam pembuatan node baru.
F.S.: 'UpgradeNode' baru telah dialokasikan di memori dan semua propertinya telah diisi sesuai dengan parameter yang diberikan.
Fungsi mengembalikan pointer ke node baru tersebut. Jika alokasi memori gagal, fungsi akan mengembalikan NULL.*/
UpgradeNode *CreateUpgradeNode(UpgradeType type, const char *name, const char *desc, int cost, UpgradeNode *parent, int exclusiveGroupId)
{
    UpgradeNode *node = (UpgradeNode *)malloc(sizeof(UpgradeNode));
    if (node == NULL)
    {
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

/* I.S. : 'tree' adalah struct TowerUpgradeTree yang sembarang (belum diinisialisasi).
   F.S. : 'tree' telah diinisialisasi, semua node upgrade telah dibuat dan terhubung,
          membentuk sebuah pohon upgrade yang lengkap untuk 'type' tower yang ditentukan. */
void InitUpgradeTree(TowerUpgradeTree *tree, TowerType type)
{
    // Memuat semua tekstur ikon yang dibutuhkan
    lockedIconTex = LoadTextureSafe("assets/img/upgrade_imgs/locked.png");
    
    upgradeIcon_AttackSpeedBase = LoadTextureSafe("assets/img/upgrade_imgs/speed_upgrade.png"); 
    upgradeIcon_AttackPowerBase = LoadTextureSafe("assets/img/upgrade_imgs/power_upgrade.png"); 
    upgradeIcon_SpecialEffectBase = LoadTextureSafe("assets/img/upgrade_imgs/special_effect.png"); 
    upgradeIcon_LightningAttack = LoadTextureSafe("assets/img/upgrade_imgs/flash_upgrade.png"); 
    upgradeIcon_ChainAttack = LoadTextureSafe("assets/img/upgrade_imgs/chains_upgrade.png"); 
    upgradeIcon_AreaAttack = LoadTextureSafe("assets/img/upgrade_imgs/aoe_upgrade.png"); 
    upgradeIcon_CriticalAttack = LoadTextureSafe("assets/img/upgrade_imgs/critical_upgrade.png"); 
    upgradeIcon_StunEffect = LoadTextureSafe("assets/img/upgrade_imgs/stun_upgrade.png"); 
    upgradeIcon_WideChainRange = LoadTextureSafe("assets/img/upgrade_imgs/chains_upgrade(2).png"); 
    upgradeIcon_LargeAoERadius = LoadTextureSafe("assets/img/upgrade_imgs/aoe_upgrade(2).png"); 
    upgradeIcon_HighCritChance = LoadTextureSafe("assets/img/upgrade_imgs/critical_upgrade(2).png"); 
    upgradeIcon_LethalPoison = LoadTextureSafe("assets/img/upgrade_imgs/poison_upgrade.png"); 
    upgradeIcon_MassSlow = LoadTextureSafe("assets/img/upgrade_imgs/slow_upgrade.png"); 
    upgradeIcon_CustomStat = LoadTextureSafe("assets/img/upgrade_imgs/nodeskill.png"); 
    acceptIconTex = LoadTextureSafe("assets/img/upgrade_imgs/accept.png"); 

    TraceLog(LOG_INFO, "Initializing upgrade tree for Tower Type %d", type);

    // Membuat Node Root
    tree->root = CreateUpgradeNode(UPGRADE_NONE, "Upgrade Tower", "Pilih jalur upgrade", 0, NULL, 0);
    if (!tree->root)
        return;
    tree->root->status = UPGRADE_UNLOCKED;

    // --- TIER 1: Cabang Utama ---
    UpgradeNode *speedBase = CreateUpgradeNode(UPGRADE_ATTACK_SPEED_BASE, "Upgrade Kecepatan Serangan", "Meningkatkan kecepatan serangan dasar.", 0, tree->root, 1);
    UpgradeNode *powerBase = CreateUpgradeNode(UPGRADE_ATTACK_POWER_BASE, "Upgrade Kekuatan Serangan", "Meningkatkan kekuatan serangan dasar.", 0, tree->root, 1);
    UpgradeNode *specialBase = CreateUpgradeNode(UPGRADE_SPECIAL_EFFECT_BASE, "Upgrade Efek Khusus", "Membuka jalur efek khusus.", 0, tree->root, 1);
    
    // Menambahkan anak ke root
    AddChild(tree->root, speedBase);
    AddChild(tree->root, powerBase);
    AddChild(tree->root, specialBase);

    // --- TIER 2: Spesialisasi Awal ---
    UpgradeNode *lightning = CreateUpgradeNode(UPGRADE_LIGHTNING_ATTACK, "Serangan Kilat", "Serangan sangat cepat.", 75, speedBase, 2);
    UpgradeNode *chain = CreateUpgradeNode(UPGRADE_CHAIN_ATTACK, "Serangan Berantai", "Serangan melompat ke musuh terdekat.", 100, speedBase, 2);
    AddChild(speedBase, lightning);
    AddChild(speedBase, chain);

    UpgradeNode *area = CreateUpgradeNode(UPGRADE_AREA_ATTACK, "Serangan Area", "Serangan merusak beberapa musuh sekaligus.", 110, powerBase, 3);
    UpgradeNode *critical = CreateUpgradeNode(UPGRADE_CRITICAL_ATTACK, "Serangan Kritikal", "Meningkatkan peluang critical hit.", 120, powerBase, 3);
    AddChild(powerBase, area);
    AddChild(powerBase, critical);

    UpgradeNode *poison = CreateUpgradeNode(UPGRADE_LETHAL_POISON, "Racun Mematikan", "Serangan meracuni musuh.", 150, specialBase, 4);
    UpgradeNode *massSlow = CreateUpgradeNode(UPGRADE_MASS_SLOW, "Perlambat Massal", "Serangan memperlambat area luas.", 160, specialBase, 4);
    AddChild(specialBase, poison);
    AddChild(specialBase, massSlow);

    // --- TIER 3: Spesialisasi Lanjutan ---
    UpgradeNode *stun = CreateUpgradeNode(UPGRADE_STUN_EFFECT, "Efek Stun", "Serangan Kilat memiliki peluang stun.", 180, lightning, 5);
    UpgradeNode *wideChain = CreateUpgradeNode(UPGRADE_WIDE_CHAIN_RANGE, "Jangkauan Berantai Luas", "Meningkatkan jangkauan lompatan.", 160, chain, 6);
    UpgradeNode *largeAoE = CreateUpgradeNode(UPGRADE_LARGE_AOE_RADIUS, "Jangkauan AoE Lebih Besar", "Meningkatkan radius Serangan Area.", 170, area, 7);
    UpgradeNode *highCrit = CreateUpgradeNode(UPGRADE_HIGH_CRIT_CHANCE, "Peluang Kritikal Tinggi", "Meningkatkan peluang critical hit secara signifikan.", 180, critical, 8);

    AddChild(lightning, stun);
    AddChild(chain, wideChain);
    AddChild(area, largeAoE);
    AddChild(critical, highCrit);
}

/* Mengirimkan aset Texture2D yang berasosiasi dengan 'type' upgrade tertentu. */
Texture2D GetUpgradeIconTexture(UpgradeType type)
{
    switch (type)
    {
    case UPGRADE_ATTACK_SPEED_BASE:
        return upgradeIcon_AttackSpeedBase;
    case UPGRADE_ATTACK_POWER_BASE:
        return upgradeIcon_AttackPowerBase;
    case UPGRADE_SPECIAL_EFFECT_BASE:
        return upgradeIcon_SpecialEffectBase;
    case UPGRADE_LIGHTNING_ATTACK:
        return upgradeIcon_LightningAttack;
    case UPGRADE_CHAIN_ATTACK:
        return upgradeIcon_ChainAttack;
    case UPGRADE_AREA_ATTACK:
        return upgradeIcon_AreaAttack;
    case UPGRADE_CRITICAL_ATTACK:
        return upgradeIcon_CriticalAttack;
    case UPGRADE_STUN_EFFECT:
        return upgradeIcon_StunEffect;
    case UPGRADE_WIDE_CHAIN_RANGE:
        return upgradeIcon_WideChainRange;
    case UPGRADE_LARGE_AOE_RADIUS:
        return upgradeIcon_LargeAoERadius;
    case UPGRADE_HIGH_CRIT_CHANCE:
        return upgradeIcon_HighCritChance;
    case UPGRADE_LETHAL_POISON:
        return upgradeIcon_LethalPoison;
    case UPGRADE_MASS_SLOW:
        return upgradeIcon_MassSlow;
    default:
        if (type >= 100) { 
            return upgradeIcon_CustomStat;
        }
        return (Texture2D){0}; 
    }
}

//Mengembalikan true jika ada saudara eksklusif yang sudah dibeli, false jika tidak.
static bool IsExclusiveSiblingPurchased(const UpgradeNode *node, const struct Tower *tower) {
    if (!node || !node->parent || !tower || node->exclusiveGroupId == 0) {
        return false;
    }

    UpgradeNode* parent = node->parent;
    UpgradeNode* sibling = parent->firstChild;
    
    // Lakukan loop sepanjang rantai saudara
    while (sibling != NULL) {
        if (sibling != node && sibling->exclusiveGroupId == node->exclusiveGroupId && tower->purchasedUpgrades[sibling->type]) {
            return true; 
        }
        // Pindah ke saudara berikutnya
        sibling = sibling->nextSibling;
    }

    return false; 
}

// Fungsi rekursif internal untuk menelusuri pohon
static UpgradeNode* FindCurrentUpgradeNode_Recursive(UpgradeNode* currentNode, const struct Tower* tower) {
    if (!currentNode || !tower) return NULL;

    // Mulai iterasi dari anak pertama
    UpgradeNode* child = currentNode->firstChild;
    while (child != NULL) {
        // Cek apakah anak ini sudah dibeli
        if (tower->purchasedUpgrades[child->type]) {
            // Jika ya, lanjutkan pencarian secara rekursif dari anak ini
            return FindCurrentUpgradeNode_Recursive(child, tower);
        }
        // Pindah ke saudara berikutnya
        child = child->nextSibling;
    }
    // Jika tidak ada anak yang dibeli, berarti ini adalah node terjauh yang valid.
    // Pastikan node ini sendiri sudah dibeli (atau merupakan root)
    if (tower->purchasedUpgrades[currentNode->type] || currentNode->parent == NULL) {
        return currentNode;
    }
    return currentNode->parent;
}

// Fungsi publik yang akan dipanggil dari luar
UpgradeNode* FindCurrentUpgradeNode(const struct Tower* tower) {
    if (!tower || !tower1UpgradeTree.root) return NULL;
    return FindCurrentUpgradeNode_Recursive(tower1UpgradeTree.root, tower);
}

/* I.S. : Status dari 'node' dan semua turunannya mungkin belum sesuai dengan kondisi 'tower' saat ini.
   F.S. : Status dari 'node' dan semua turunannya telah diperbarui. */
static void UpdateAllNodesRecursive(UpgradeNode* node, const struct Tower* tower) {
    if (!node || !tower) return;

    // Logika untuk memproses node saat ini 
    if (tower->purchasedUpgrades[node->type]) {
        node->status = UPGRADE_PURCHASED;
    } else {
        if (node->parent == NULL) { 
            node->status = UPGRADE_PURCHASED; 
        } else if (node->parent->status != UPGRADE_PURCHASED) {
            node->status = UPGRADE_LOCKED; 
        } else {
            if (IsExclusiveSiblingPurchased(node, tower)) {
                node->status = UPGRADE_LOCKED_EXCLUDED; 
            } else {
                node->status = UPGRADE_UNLOCKED; 
            }
        }
    }


    UpgradeNode* child = node->firstChild;
    while (child != NULL) {
        UpdateAllNodesRecursive(child, tower);
        child = child->nextSibling; 
    }
}

/* I.S. : Status setiap node di dalam 'tree' mungkin tidak sesuai dengan upgrade yang dimiliki 'tower'.
   F.S. : Status (LOCKED, UNLOCKED, PURCHASED) dari setiap node di dalam 'tree' telah diperbarui
          untuk secara akurat merefleksikan upgrade yang sudah dibeli oleh 'tower'. */
void UpdateUpgradeTreeStatus(TowerUpgradeTree *tree, const Tower *tower)
{
    if (!tree || !tree->root || !tower) {
        TraceLog(LOG_WARNING, "UpdateUpgradeTreeStatus: Invalid parameters provided.");
        return;
    }
    
    UpdateAllNodesRecursive(tree->root, tower);
}

/* I.S. : Tampilan menu orbit bisa berada di level mana pun dalam pohon upgrade.
   F.S. : Tampilan menu orbit di-reset kembali ke level paling awal (akar/root dari pohon). */
void ResetUpgradeOrbit(void)
{
    currentOrbitParentNode = GetUpgradeTreeRoot(&tower1UpgradeTree);
    prevOrbitParentNode = NULL;
    TraceLog(LOG_INFO, "Upgrade orbit reset to root.");
}

/* I.S. : Menu orbit menampilkan anak-anak dari 'currentOrbitParentNode'.
   F.S. : Tampilan menu orbit berpindah, di mana 'currentOrbitParentNode' yang lama disimpan ke 'prevOrbitParentNode'
          dan 'currentOrbitParentNode' yang baru diatur menjadi 'targetNode'. */
void NavigateUpgradeOrbit(UpgradeNode *targetNode)
{
    if (!targetNode)
        return;
    prevOrbitParentNode = currentOrbitParentNode;
    currentOrbitParentNode = targetNode;
    TraceLog(LOG_INFO, "Navigated to orbit node: %s", targetNode->name);
}

UpgradeNode* FindNodeByType(UpgradeNode* startNode, UpgradeType type) {
    if (!startNode) {
        return NULL;
    }
    if (startNode->type == type) {
        return startNode;
    }

    UpgradeNode* foundNode = NULL;
    UpgradeNode* child = startNode->firstChild;
    while (child != NULL) {
        foundNode = FindNodeByType(child, type);
        if (foundNode) {
            return foundNode; // Ditemukan di cabang ini
        }
        child = child->nextSibling;
    }

    return NULL; // Tidak ditemukan di cabang mana pun
}

/* I.S. : 'tower' memiliki status (damage, speed, dll.) sebelum upgrade.
   F.S. : 'tower' telah dimodifikasi sesuai dengan efek dari upgrade 'type'.
          Contoh: damage bertambah, atau properti boolean seperti 'hasChainAttack' menjadi true. */
void ApplyUpgradeEffect(Tower *tower, UpgradeType type)
{
    if (!tower) return;

    tower->purchasedUpgrades[type] = true;
    UpgradeNode* appliedNode = FindNodeByType(GetUpgradeTreeRoot(&tower1UpgradeTree), type);
    TraceLog(LOG_INFO, "Applying upgrade %d to tower at (%d,%d).", type, tower->row, tower->col);

    switch (type)
    {
    
    case UPGRADE_ATTACK_SPEED_BASE:
        SetTowerAttackSpeed(tower, GetTowerAttackSpeed(tower) * 0.8f); 
        break;
    case UPGRADE_ATTACK_POWER_BASE:
        SetTowerDamage(tower, GetTowerDamage(tower) + 15);
        break;
    case UPGRADE_SPECIAL_EFFECT_BASE:        
        break;

    
    case UPGRADE_LIGHTNING_ATTACK:
        SetTowerAttackSpeed(tower, GetTowerAttackSpeed(tower) * 0.7f);
        tower->texture = tower2Texture; 
        break;
    case UPGRADE_CHAIN_ATTACK:
        tower->hasChainAttack = true;
        tower->chainJumps = 2; 
        tower->chainRange = 100.0f; 
        tower->texture = tower2Texture; 
        SetTowerDamage(tower, GetTowerDamage(tower) * 0.8f); 
        break;
    case UPGRADE_AREA_ATTACK:
        tower->hasAreaAttack = true;
        tower->areaAttackRadius = 60.0f; 
        tower->texture = tower2Texture; 
        SetTowerDamage(tower, GetTowerDamage(tower) * 0.7f); 
        break;
    case UPGRADE_CRITICAL_ATTACK:
        tower->critChance = 15; 
        tower->critMultiplier = 2.0f; 
        tower->texture = tower2Texture; 

        break;
    case UPGRADE_LETHAL_POISON:
        SetTowerDamage(tower, GetTowerDamage(tower) + 5);
        tower->texture = tower2Texture; 
        break;
    case UPGRADE_MASS_SLOW:
        tower->hasAreaAttack = true; 
        tower->areaAttackRadius = 80.0f;
        tower->texture = tower2Texture; 
        SetTowerDamage(tower, GetTowerDamage(tower) * 0.5f); 
        break;

    
    case UPGRADE_STUN_EFFECT:
        tower->hasStunEffect = true;
        tower->stunChance = 20.0f; 
        tower->stunDuration = 0.5f; 
        tower->texture = tower3Texture; 
        break;
    case UPGRADE_WIDE_CHAIN_RANGE:
        if (tower->hasChainAttack) {
            tower->chainJumps += 2; 
            tower->chainRange *= 1.5f; 
            tower->texture = tower3Texture; 
        }
        break;
    case UPGRADE_LARGE_AOE_RADIUS:
        if (tower->hasAreaAttack) {
            tower->areaAttackRadius *= 1.6f; 
            tower->texture = tower3Texture; 
        }
        break;
    case UPGRADE_HIGH_CRIT_CHANCE:
        if (tower->critChance > 0) {
            tower->critChance += 20; 
            tower->critMultiplier += 0.5f;
            tower->texture = tower3Texture; 
 
        }
        break;
    default:
        TraceLog(LOG_WARNING, "Attempted to apply unknown upgrade type: %d", type);
        break;
    }
}

/* I.S. : Menu orbit upgrade sedang ditampilkan, pemain melakukan klik pada posisi 'mousePos'.
   F.S. : Jika klik mengenai tombol yang valid (skill, kembali), aksi yang sesuai akan dijalankan
          (misalnya, navigasi atau menampilkan konfirmasi) dan fungsi mengembalikan true.
          Jika tidak, mengembalikan false. */
bool HandleUpgradeOrbitClick(Vector2 mousePos, float currentTileScale)
{
    if (!selectedTowerForDeletion || !currentOrbitParentNode) return false;

    if (pendingUpgradeNode != NULL) {
        float iconSize = 16.0f * currentTileScale * 2.0f; 
        Rectangle acceptIconRect = {
            pendingUpgradeIconPos.x - iconSize / 2,
            pendingUpgradeIconPos.y - iconSize / 2,
            iconSize,
            iconSize
        };
        
        if (CheckCollisionPointRec(mousePos, acceptIconRect)) {
            if (GetMoney() >= pendingUpgradeNode->cost) {
                Push(&statusStack, TextFormat("Upgraded: %s", pendingUpgradeNode->name));
                ApplyUpgradeEffect(selectedTowerForDeletion, pendingUpgradeNode->type);
                AddMoney(-(pendingUpgradeNode->cost));
                PlaySpendMoneySound(); // <-- PANGGIL DI SINI
                UpdateUpgradeTreeStatus(&tower1UpgradeTree, selectedTowerForDeletion);
 
                if (GetNumChildren(pendingUpgradeNode) > 0) {
                    NavigateUpgradeOrbit(pendingUpgradeNode);
                }
            } else {
                Push(&statusStack, "Not enough money!");
            }
            pendingUpgradeNode = NULL; 
            return true; 
        }
    }

    Vector2 orbitCenter = towerSelectionUIPos;
    float orbitRadius = TILE_SIZE * currentTileScale * ORBIT_RADIUS_TILE_FACTOR;
    float sellBtnSize = 20.0f * currentTileScale;
    Rectangle sellBtnRect = {
        orbitCenter.x - sellBtnSize / 2.0f,
        orbitCenter.y - sellBtnSize / 2.0f,
        sellBtnSize,
        sellBtnSize
    };
    if (CheckCollisionPointRec(mousePos, sellBtnRect)) {
        SellTower(selectedTowerForDeletion);
        // HideTowerSelectionUI() akan dipanggil di dalam SellTower -> RemoveTower
        return true; // Klik sudah ditangani
    }
    UpgradeNode *parentNode = GetCurrentOrbitParentNode();
    int numChildren = GetNumChildren(parentNode);
    int totalButtons = numChildren + (parentNode->parent != NULL ? 1 : 0);

    for (int i = 0; i < numChildren; i++) {
        UpgradeNode *childNode = GetNthChild(parentNode, i);
        if (!childNode) continue;

        Rectangle buttonRect = GetOrbitButtonRect(orbitCenter, orbitRadius, i, totalButtons, ORBIT_BUTTON_DRAW_SCALE, upgradeButtonTex);
        if (CheckCollisionPointRec(mousePos, buttonRect)) {
            if (childNode->status == UPGRADE_UNLOCKED) {
                if (childNode->cost > 0) {
                    
                    pendingUpgradeNode = childNode;
                    pendingUpgradeIconPos = (Vector2){ buttonRect.x + buttonRect.width / 2, buttonRect.y + buttonRect.height / 2 };
                } else {
                                       
                    Push(&statusStack, TextFormat("Path chosen: %s", childNode->name));
                    ApplyUpgradeEffect(selectedTowerForDeletion, childNode->type);
                    UpdateUpgradeTreeStatus(&tower1UpgradeTree, selectedTowerForDeletion);
                    NavigateUpgradeOrbit(childNode);

                    pendingUpgradeNode = NULL;
                    
                }
            } else if (childNode->status == UPGRADE_PURCHASED && GetNumChildren(childNode) > 0) {
                
                pendingUpgradeNode = NULL; 
                NavigateUpgradeOrbit(childNode);
            }
            return true; 
        }
    }

    if (pendingUpgradeNode != NULL) {
        pendingUpgradeNode = NULL;
        
        return true;
    }
    return false;
}

/* I.S. : State untuk menu orbit upgrade (seperti 'currentOrbitParentNode') telah diatur.
   F.S. : Seluruh antarmuka pengguna (UI) untuk menu orbit, termasuk tombol-tombol skill,
          ikon status, dan tombol kembali, telah digambar ke layar. */
void DrawUpgradeOrbitMenu(float currentTileScale, float mapScreenOffsetX, float mapScreenOffsetY)
{

    if (!selectedTowerForDeletion || !currentOrbitParentNode) {
        return;
    }

    Vector2 orbitCenter = towerSelectionUIPos;
    float orbitRadius = TILE_SIZE * currentTileScale * ORBIT_RADIUS_TILE_FACTOR;
    
    DrawCircleLines((int)orbitCenter.x, (int)orbitCenter.y, orbitRadius, RAYWHITE);

    if (deleteButtonTex.id != 0) {
        float sellBtnSize = 20.0f * currentTileScale;
        Rectangle sellBtnRect = {
            orbitCenter.x - sellBtnSize / 2.0f,
            orbitCenter.y - sellBtnSize / 2.0f,
            sellBtnSize,
            sellBtnSize
        };
        DrawTexturePro(deleteButtonTex, (Rectangle){0,0,(float)deleteButtonTex.width, (float)deleteButtonTex.height}, sellBtnRect, (Vector2){0,0}, 0.0f, WHITE);
    }

    UpdateUpgradeTreeStatus(&tower1UpgradeTree, selectedTowerForDeletion);

    UpgradeNode *parentNode = currentOrbitParentNode;
    int numChildren = GetNumChildren(parentNode);    
    
    int totalButtons = numChildren + (parentNode->parent != NULL ? 1 : 0); 
    
    for (int i = 0; i < numChildren; i++)
    {
        UpgradeNode *childNode = GetNthChild(parentNode, i);
        if (!childNode) continue;

        Rectangle buttonRect = GetOrbitButtonRect(orbitCenter, orbitRadius, i, totalButtons, ORBIT_BUTTON_DRAW_SCALE, upgradeButtonTex);
        
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
        if (childNode->type >= 100) {
            // Teks untuk baris pertama dan kedua
            const char* line1 = "Stat Boost";
            char line2[16];
            sprintf(line2, "$%d", childNode->cost);

            // Pengaturan Font
            float fontSize = 14.0f;
            float lineSpacing = 2.0f; // Jarak antar baris
            Color textColor = (childNode->status == UPGRADE_LOCKED) ? GRAY : WHITE;
            Color costColor = (GetMoney() >= childNode->cost) ? GOLD : RED;

            // Hitung tinggi total dari kedua baris teks
            float totalTextHeight = (fontSize * 2) + lineSpacing;

            // Hitung posisi untuk memusatkan blok teks secara vertikal
            float startY = buttonRect.y + (buttonRect.height - totalTextHeight) / 2;

            // Hitung posisi dan gambar baris pertama (nama skill)
            int textWidth1 = MeasureText(line1, fontSize);
            DrawText(line1, 
                     (int)(buttonRect.x + (buttonRect.width - textWidth1) / 2), 
                     (int)startY, 
                     fontSize, 
                     textColor);

            // Hitung posisi dan gambar baris kedua (harga)
            int textWidth2 = MeasureText(line2, fontSize);
            DrawText(line2, 
                     (int)(buttonRect.x + (buttonRect.width - textWidth2) / 2), 
                     (int)(startY + fontSize + lineSpacing), 
                     fontSize, 
                     costColor);
                     
        } else {
            // Untuk skill standar, kita tetap tampilkan nama dan harga di bawahnya
            // (Blok else ini sama seperti sebelumnya, tidak perlu diubah)
            int fontSize = 15;
            Color textColor = (childNode->status == UPGRADE_LOCKED) ? GRAY : WHITE;
            if (childNode->status == UPGRADE_PURCHASED) textColor = GOLD;

            int textWidth = MeasureText(childNode->name, fontSize);
            DrawText(childNode->name, 
                     (int)(buttonRect.x + (buttonRect.width - textWidth) / 2), 
                     (int)(buttonRect.y + buttonRect.height + 5),
                     fontSize, 
                     textColor);

            if (childNode->cost > 0 && childNode->status == UPGRADE_UNLOCKED) {
                char costText[16];
                sprintf(costText, "$%d", childNode->cost);
                int costFontSize = 14;
                int costTextWidth = MeasureText(costText, costFontSize);
                DrawText(costText, 
                         (int)(buttonRect.x + (buttonRect.width - costTextWidth) / 2), 
                         (int)(buttonRect.y + buttonRect.height + 22),
                         costFontSize, 
                         GOLD);
            }
        }
    }
    if (pendingUpgradeNode != NULL) {
        float iconSize = 16.0f * currentTileScale;
        Rectangle destRect = { 
            pendingUpgradeIconPos.x - iconSize / 2, 
            pendingUpgradeIconPos.y - iconSize / 2, 
            iconSize, 
            iconSize 
        };
        DrawTexturePro(acceptIconTex, (Rectangle){0,0, (float)acceptIconTex.width, (float)acceptIconTex.height}, destRect, (Vector2){0,0}, 0.0f, WHITE);
    }
}


/* I.S. : 'node' dan semua turunannya mungkin masih dialokasikan di memori.
 * F.S. : Memori untuk 'node' dan semua turunannya telah dibebaskan. */
static void FreeUpgradeNode(UpgradeNode *node)
{
    if (!node)
        return;

    // 1. Kunjungi dan bebaskan semua anak terlebih dahulu
    UpgradeNode* child = node->firstChild;
    while (child != NULL) {
        UpgradeNode* next = child->nextSibling;
        FreeUpgradeNode(child);
        child = next;
    }

    // TAMBAHKAN BLOK INI:
    // Hanya bebaskan nama dan deskripsi jika ini adalah skill kustom
    // (dengan asumsi ID di atas 100 adalah kustom, sesuai implementasi di research_menu.c)
    if (node->type >= 100) {
        free((void*)node->name);
        free((void*)node->description);
    }

    // Bebaskan node itu sendiri
    free(node);
}

/* I.S. : 'tree' mungkin menunjuk ke sebuah pohon upgrade yang valid.
   F.S. : Semua memori yang dialokasikan untuk setiap node di dalam 'tree' telah dibebaskan.
          'tree->root' diatur menjadi NULL. */
void FreeUpgradeTree(TowerUpgradeTree* tree) {
    if (tree && tree->root) {
        FreeUpgradeNode(tree->root);
        tree->root = NULL;
        TraceLog(LOG_INFO, "Upgrade tree freed.");
    }
}

/* Mengirimkan pointer ke node akar (root) dari 'tree'. Mengembalikan NULL jika tree kosong. */
UpgradeNode* GetUpgradeTreeRoot(TowerUpgradeTree* tree) {
    return tree ? tree->root : NULL;
}

/* I.S. : 'parent' adalah node yang valid dan 'n' adalah indeks anak yang diinginkan.
   F.S. : Mengirimkan pointer ke anak ke-'n' dari 'parent'. Mengembalikan NULL jika 'n' di luar batas. */
UpgradeNode *GetNthChild(UpgradeNode *parent, int n)
{
    if (!parent || n < 0) {
        return NULL;
    }
    // Mulai dari anak pertama
    UpgradeNode* currentChild = parent->firstChild;
    int count = 0;

    // Lakukan iterasi sepanjang rantai saudara
    while (currentChild != NULL) {
        if (count == n) {
            return currentChild;
        }
        currentChild = currentChild->nextSibling;
        count++;
    }

    return NULL; 
}

/* Mengirimkan jumlah anak langsung yang dimiliki oleh sebuah 'node'. Mengembalikan 0 jika node NULL. */
int GetNumChildren(UpgradeNode *node)
{
    if (!node) return 0;

    int count = 0;
    UpgradeNode* child = node->firstChild;
    while (child != NULL) {
        count++;
        child = child->nextSibling;
    }
    return count;
}

/* I.S. : Menu orbit mungkin berpusat pada node yang berbeda atau NULL.
   F.S. : Pointer global 'currentOrbitParentNode' diatur untuk menunjuk ke 'node'. */
void SetCurrentOrbitParentNode(UpgradeNode* node) {
    currentOrbitParentNode = node;
}

/* Mengirimkan pointer ke node yang saat ini menjadi pusat dari menu orbit. */
UpgradeNode* GetCurrentOrbitParentNode(void) {
    return currentOrbitParentNode;
}

/* I.S. : Pointer 'prevOrbitParentNode' memiliki nilai lama.
   F.S. : Pointer global 'prevOrbitParentNode' diatur untuk menunjuk ke 'node', digunakan untuk navigasi kembali. */
void SetPrevOrbitParentNode(UpgradeNode* node) {
    prevOrbitParentNode = node;
}

/* Mengirimkan pointer ke node orbit sebelumnya, digunakan untuk navigasi "kembali". */
UpgradeNode* GetPrevOrbitParentNode(void) {
    return prevOrbitParentNode;
}
