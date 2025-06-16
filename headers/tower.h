/* File        : tower.h 
* Deskripsi   : Deklarasi Abstract Data Type (ADT) untuk Tower. 
*               Berisi definisi struct Tower dan semua prototipe fungsi publik 
*               yang berkaitan dengan manajemen tower, seperti penempatan, serangan, dan UI. 
* Dibuat oleh : Ahmad Riyadh Almaliki
* Tanggal Perubahan : Sabtu, 14 Juni 2025
*/

#ifndef TOWER_H
#define TOWER_H
#include "common.h"
#include "map.h"

#define TOWER_FRAME_WIDTH 70
#define TOWER_FRAME_HEIGHT 130
#define TOWER_ANIMATION_SPEED 0.1f  
#define TOWER_Y_OFFSET_PIXELS 15.0f 
#define TOWER_DRAW_SCALE 0.45f 
#define ORBIT_RADIUS_FACTOR 1.5f 
#define ORBIT_BUTTON_DRAW_SCALE 1.0f 
#define ORBIT_RADIUS_TILE_FACTOR 1.0f

#define MAX_VISUAL_SHOTS 50 

struct EnemyWave; 

/* Struct utama tower */
typedef struct Tower{
    Vector2 position;
    TowerType type;
    int damage;
    float range;
    float attackSpeed;
    float attackCooldown; 
    bool active;
    Texture2D texture;
    int frameWidth;    
    int frameHeight;   
    int currentFrame;  
    float frameTimer;  
    int row; 
    int col; 
    int totalCost;
    struct Tower *next; 
    bool purchasedUpgrades[UPGRADE_MASS_SLOW + 1]; 
    
    bool hasChainAttack;
    int chainJumps;
    float chainRange;

    bool hasAreaAttack;
    float areaAttackRadius;

    bool hasStunEffect;
    float stunChance;
    float stunDuration;

    float critChance;
    float critMultiplier;
} Tower;

//Enum untuk tipe tembakan
typedef enum {
    SHOT_TYPE_NORMAL_IMPACT, 
    SHOT_TYPE_PROJECTILE,
    SHOT_TYPE_AOE_BLAST,
    SHOT_TYPE_CRIT_SHATTER
} ShotType;

//Struct Tembakan
typedef struct {
    Vector2 startPos;  
    Vector2 endPos;    
    Color color;    
    float travelTime;  
    float currentTravelTime; 
    bool active;       
    ShotType type; 
    float radius;      
    bool isImpactEffect;     
    float impactTimer;       
    float impactDuration;    
    float impactMaxSize;
} Shot;

extern Texture2D tower1Texture;
extern Texture2D tower2Texture;
extern Texture2D tower3Texture;
extern Tower *towersListHead; 
extern Tower *selectedTowerForDeletion; 
extern Vector2 towerSelectionUIPos;   
extern bool isTowerSelectionUIVisible;
extern Vector2 deleteButtonScreenPos;   
extern bool deleteButtonVisible;        
extern float DELETE_BUTTON_DRAW_SCALE; 
extern Texture2D deleteButtonTex;       
extern Texture2D upgradeButtonTex; 

/* I.S. : Aset-aset untuk tower (seperti tekstur sprite, tombol UI) belum dimuat.
   F.S. : Semua aset yang diperlukan oleh modul Tower telah dimuat ke memori. */
void InitTowerAssets();

/* I.S. : Sistem visual tembakan (shots) belum siap digunakan.
   F.S. : Array internal untuk menampung efek visual telah diinisialisasi dan siap digunakan. */
void InitShots(void);

/* I.S. : Aset-aset tower sedang digunakan.
   F.S. : Semua aset yang digunakan oleh modul Tower telah dihapus dari memori (unload).
          Semua tower yang tersisa di-dealokasi. */
void ShutdownTowerAssets();

/* I.S. : Sistem visual tembakan sedang berjalan.
   F.S. : Tidak ada tindakan spesifik di implementasi saat ini, namun disiapkan untuk dealokasi di masa depan. */
void ShutdownShots(void);

/* I.S. : Aksi dalam game (misalnya serangan tower) terjadi.
   F.S. : Sebuah efek visual proyektil baru yang bergerak dari 'startPos' ke 'endPos' dibuat dan diaktifkan. */
void SpawnProjectile(Vector2 startPos, Vector2 endPos, Color color, float radius, float travelTime);

/* I.S. : Sebuah proyektil mengenai target atau sebuah serangan area terjadi.
   F.S. : Sebuah efek visual tumbukan (impact) dengan tipe tertentu ('type') dibuat dan diaktifkan di 'position'. */
void SpawnImpactEffect(Vector2 position, ShotType type, Color color);

/* I.S. : Posisi dan durasi semua efek visual pada frame sebelumnya.
   F.S. : Posisi proyektil diperbarui, dan durasi animasi untuk efek tumbukan dikurangi berdasarkan 'deltaTime'.*/
void UpdateShots(float deltaTime);

/* I.S. : Terdapat efek visual (shots) yang aktif.
   F.S. : Semua efek visual yang 'active' digambar ke layar sesuai dengan tipe dan propertinya. */
void DrawShots(float globalScale, float offsetX, float offsetY);

/* I.S. : Petak di (row, col) adalah petak yang valid dan kosong. Pemain memiliki cukup uang.
   F.S. : Sebuah tower baru dibuat dan ditambahkan ke daftar tower pada posisi tersebut.
          Uang pemain berkurang, dan tile di peta diperbarui. */
void PlaceTower(int row, int col, TowerType type);

/* I.S. : Keadaan tower dan musuh pada frame sebelumnya.
   F.S. : Setiap tower yang aktif dan tidak dalam masa cooldown akan mencari target. Jika target
          ditemukan dalam jangkauan, tower akan menyerang dan me-reset cooldown-nya. */
void UpdateTowerAttacks(struct EnemyWave *wave, float deltaTime);

/* I.S. : Terdapat satu atau lebih tower dalam daftar tower.
   F.S. : Semua tower yang aktif digambar ke layar, beserta UI orbit jika ada tower yang terpilih. */
void DrawTowers(float globalScale, float offsetX, float offsetY);

/* I.S. : 'towerToRemove' adalah pointer valid ke tower yang ada dalam daftar.
   F.S. : Tower tersebut dihapus dari daftar, memorinya dibebaskan, dan tile di peta dikembalikan ke semula. */
void RemoveTower(Tower *towerToRemove); 

/* I.S. : 'towerToSell' adalah tower valid yang dipilih oleh pemain.
   F.S. : Uang pemain bertambah sesuai harga jual, dan RemoveTower(towerToSell) dipanggil. */
void SellTower(Tower *towerToSell);     

/* I.S. : (row, col) adalah koordinat yang valid di dalam peta.
   F.S. : Mengembalikan pointer ke struct Tower jika ada tower di koordinat tersebut.
          Jika tidak ada, mengembalikan NULL. */
Tower* GetTowerAtMapCoord(int row, int col); 

/* I.S. : UI seleksi tower (menu orbit) mungkin sedang ditampilkan.
   F.S. : Semua variabel state yang terkait dengan UI seleksi di-reset, sehingga UI menjadi tersembunyi. */
void HideTowerSelectionUI(void); 

/* I.S. : 'tower' adalah tower valid yang diklik oleh pemain. UI Orbit sedang tersembunyi.
   F.S. : Variabel global untuk UI seleksi diaktifkan ('isTowerSelectionUIVisible' = true).
          'selectedTowerForDeletion' diatur ke 'tower'. Posisi UI dihitung dan diatur. */
void ShowTowerOrbitUI(Tower *tower, float currentTileScale, float mapScreenOffsetX, float mapScreenOffsetY); 

/* I.S. : UI Orbit mungkin sedang ditampilkan.
   F.S. : Variabel global untuk UI seleksi dinonaktifkan dan di-reset. */
void HideTowerOrbitUI(void);

/* Mengirimkan true jika UI orbit (seleksi tower) sedang ditampilkan. */
bool IsTowerOrbitUIVisible(void); 

/* I.S. : Diberikan semua parameter untuk sebuah tombol di lingkaran orbit.
   F.S. : Mengembalikan sebuah Rectangle yang merepresentasikan posisi dan ukuran tombol tersebut di layar. */
Rectangle GetOrbitButtonRect(Vector2 orbitCenter, float orbitRadius, int buttonIndex, int totalButtons, float buttonScale, Texture2D buttonTexture);

/* I.S. : Diberikan posisi mouse dan semua parameter sebuah tombol orbit.
   F.S. : Mengembalikan true jika 'mousePos' berada di dalam area Rectangle tombol tersebut. */
bool CheckOrbitButtonClick(Vector2 mousePos, Vector2 orbitCenter, float orbitRadius, int buttonIndex, int totalButtons, float buttonScale, Texture2D buttonTexture);

/* Mengirimkan true jika UI seleksi tower (menu orbit) sedang ditampilkan. */
bool IsTowerSelectionUIVisible(void);

/* Mengirimkan nilai posisi (Vector2) dari 'tower'. */
Vector2 GetTowerPosition(const Tower *tower);

/* Mengirimkan nilai tipe (TowerType) dari 'tower'. */
TowerType GetTowerType(const Tower *tower);

/* Mengirimkan nilai kerusakan (damage) dari 'tower'. */
int GetTowerDamage(const Tower *tower);

/* Mengirimkan nilai jangkauan serangan (range) dari 'tower'. */
float GetTowerRange(const Tower *tower);

/* Mengirimkan nilai kecepatan serangan (attackSpeed) dari 'tower'. */
float GetTowerAttackSpeed(const Tower *tower);

/* Mengirimkan sisa waktu cooldown serangan dari 'tower'. */
float GetTowerAttackCooldown(const Tower *tower);

/* Mengirimkan status 'active' dari 'tower'. */
bool GetTowerActive(const Tower *tower);

/* I.S. : Posisi 'tower' sembarang.
   F.S. : Properti 'tower->position' diatur menjadi nilai 'position' yang baru. */
void SetTowerPosition(Tower *tower, Vector2 position);

/* I.S. : Tipe 'tower' sembarang.
   F.S. : Properti 'tower->type' diatur menjadi nilai 'type' yang baru. */
void SetTowerType(Tower *tower, TowerType type);

/* I.S. : Kerusakan 'tower' sembarang.
   F.S. : Properti 'tower->damage' diatur menjadi nilai 'damage' yang baru. */
void SetTowerDamage(Tower *tower, int damage);

/* I.S. : Jangkauan 'tower' sembarang.
   F.S. : Properti 'tower->range' diatur menjadi nilai 'range' yang baru. */
void SetTowerRange(Tower *tower, float range);

/* I.S. : Kecepatan serangan 'tower' sembarang.
   F.S. : Properti 'tower->attackSpeed' diatur menjadi nilai 'attackSpeed' yang baru. */
void SetTowerAttackSpeed(Tower *tower, float attackSpeed);

/* I.S. : Waktu cooldown 'tower' sembarang.
   F.S. : Properti 'tower->attackCooldown' diatur menjadi nilai 'cooldown' yang baru. */
void SetTowerAttackCooldown(Tower *tower, float cooldown);

/* I.S. : Status aktif 'tower' sembarang.
   F.S. : Properti 'tower->active' diatur menjadi nilai 'active' yang baru. */
void SetTowerActive(Tower *tower, bool active);

#endif