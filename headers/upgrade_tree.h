/* File        : upgrade_tree.h 
* Deskripsi    : Deklarasi untuk Abstract Data Type (ADT) Upgrade Tree. 
*                Modul ini mengelola semua data, struktur, status, dan logika navigasi 
*                yang berkaitan dengan sistem upgrade tower. 
* Dibuat oleh  : Ahmad Riyadh Almaliki
* Perubahan terakhir : Sabtu, 14 Juni 2025
*/

#ifndef UPGRADE_TREE_H
#define UPGRADE_TREE_H

#include "common.h"

struct Tower;
struct UpgradeNode;

/* ADT untuk merepresentasikan keseluruhan Pohon Upgrade. */
/* Strukturnya hanya menyimpan pointer ke root. */
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

/* I.S. : 'tree' adalah struct TowerUpgradeTree yang sembarang (belum diinisialisasi).
   F.S. : 'tree' telah diinisialisasi, semua node upgrade telah dibuat dan terhubung,
          membentuk sebuah pohon upgrade yang lengkap untuk 'type' tower yang ditentukan. */
void InitUpgradeTree(TowerUpgradeTree *tree, TowerType type);

/*
I.S : Parameter-parameter seperti type, name, desc, cost, parent, dan exclusiveGroupId telah siap untuk digunakan dalam pembuatan node baru.
F.S.: 'UpgradeNode' baru telah dialokasikan di memori dan semua propertinya telah diisi sesuai dengan parameter yang diberikan.
Fungsi mengembalikan pointer ke node baru tersebut. Jika alokasi memori gagal, fungsi akan mengembalikan NULL.*/
UpgradeNode *CreateUpgradeNode(UpgradeType type, const char *name, const char *desc, int cost, UpgradeNode *parent, int exclusiveGroupId)

/* I.S. : Status setiap node di dalam 'tree' mungkin tidak sesuai dengan upgrade yang dimiliki 'tower'.
   F.S. : Status (LOCKED, UNLOCKED, PURCHASED) dari setiap node di dalam 'tree' telah diperbarui
          untuk secara akurat merefleksikan upgrade yang sudah dibeli oleh 'tower'. */
void UpdateUpgradeTreeStatus(TowerUpgradeTree *tree, const struct Tower *tower);

/* I.S. : Menu orbit upgrade sedang ditampilkan, pemain melakukan klik pada posisi 'mousePos'.
   F.S. : Jika klik mengenai tombol yang valid (skill, kembali), aksi yang sesuai akan dijalankan
          (misalnya, navigasi atau menampilkan konfirmasi) dan fungsi mengembalikan true.
          Jika tidak, mengembalikan false. */
bool HandleUpgradeOrbitClick(Vector2 mousePos, float currentTileScale);

/* I.S. : State untuk menu orbit upgrade (seperti 'currentOrbitParentNode') telah diatur.
   F.S. : Seluruh antarmuka pengguna (UI) untuk menu orbit, termasuk tombol-tombol skill,
          ikon status, dan tombol kembali, telah digambar ke layar. */
void DrawUpgradeOrbitMenu(float currentTileScale, float mapScreenOffsetX, float mapScreenOffsetY);

/* I.S. : 'tower' memiliki status (damage, speed, dll.) sebelum upgrade.
   F.S. : 'tower' telah dimodifikasi sesuai dengan efek dari upgrade 'type'.
          Contoh: damage bertambah, atau properti boolean seperti 'hasChainAttack' menjadi true. */
void ApplyUpgradeEffect(struct Tower *tower, UpgradeType type);

/* I.S. : Menu orbit mungkin berpusat pada node yang berbeda atau NULL.
   F.S. : Pointer global 'currentOrbitParentNode' diatur untuk menunjuk ke 'node'. */
void SetCurrentOrbitParentNode(UpgradeNode *node);

/* I.S. : Pointer 'prevOrbitParentNode' memiliki nilai lama.
   F.S. : Pointer global 'prevOrbitParentNode' diatur untuk menunjuk ke 'node', digunakan untuk navigasi kembali. */
void SetPrevOrbitParentNode(UpgradeNode *node);

/* I.S. : Menu orbit menampilkan anak-anak dari 'currentOrbitParentNode'.
   F.S. : Tampilan menu orbit berpindah, di mana 'currentOrbitParentNode' yang lama disimpan ke 'prevOrbitParentNode'
          dan 'currentOrbitParentNode' yang baru diatur menjadi 'targetNode'. */
void NavigateUpgradeOrbit(UpgradeNode *targetNode);

/* I.S. : Menu orbit menampilkan anak-anak dari sebuah node.
   F.S. : Tampilan menu orbit berpindah kembali ke parent dari node saat ini. */
void NavigateUpgradeOrbitBack(void);

/* I.S. : Tampilan menu orbit bisa berada di level mana pun dalam pohon upgrade.
   F.S. : Tampilan menu orbit di-reset kembali ke level paling awal (akar/root dari pohon). */
void ResetUpgradeOrbit(void);

/* I.S. : 'tree' mungkin menunjuk ke sebuah pohon upgrade yang valid.
   F.S. : Semua memori yang dialokasikan untuk setiap node di dalam 'tree' telah dibebaskan.
          'tree->root' diatur menjadi NULL. */
void FreeUpgradeTree(TowerUpgradeTree *tree);

/* Mengirimkan jumlah anak langsung yang dimiliki oleh sebuah 'node'. Mengembalikan 0 jika node NULL. */
int GetNumChildren(UpgradeNode *node);

/* Mengirimkan aset Texture2D yang berasosiasi dengan 'type' upgrade tertentu. */
Texture2D GetUpgradeIconTexture(UpgradeType type);

/* Mengirimkan pointer ke node akar (root) dari 'tree'. Mengembalikan NULL jika tree kosong. */
UpgradeNode *GetUpgradeTreeRoot(TowerUpgradeTree *tree);

/* Mengirimkan pointer ke node yang saat ini menjadi pusat dari menu orbit. */
UpgradeNode *GetCurrentOrbitParentNode(void);

/* I.S. : 'parent' adalah node yang valid dan 'n' adalah indeks anak yang diinginkan.
   F.S. : Mengirimkan pointer ke anak ke-'n' dari 'parent'. Mengembalikan NULL jika 'n' di luar batas. */
UpgradeNode *GetNthChild(UpgradeNode *parent, int n);

/* Mengirimkan pointer ke node orbit sebelumnya, digunakan untuk navigasi "kembali". */
UpgradeNode *GetPrevOrbitParentNode(void);

#endif