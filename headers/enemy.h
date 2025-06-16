/* File        : enemy.h 
*
*  Deskripsi   : Deklarasi Abstract Data Type (ADT) untuk Enemy dan EnemyWave. 
*                Mengelola semua data dan perilaku yang terkait dengan musuh dan gelombang musuh.
* 
*  Penulis     : Micky Ridho Pratama
*  Perubahan terakhir: Sabtu, 14 Juni 2025
*/

#ifndef ENEMY_H
#define ENEMY_H

#include "common.h"
#define TIMER_OVERALL_SIZE_FACTOR 0.8f
#define TIMER_IMAGE_DISPLAY_FACTOR 0.8f
#define MAX_PATH_POINTS 100
#define WAVE_TIMER_DURATION 10.0f
#define SPAWN_DELAY 1.5f 
#define WAVE_INTERVAL 10.0f

// DEFINISI TIPE ADT (ABSTRACT DATA TYPE)
typedef struct EnemyWave EnemyWave;

// ADT untuk Animasi Sprite
typedef struct {
    Texture2D texture;
    int frameCols;        
    int frameSpeed;   
    int frameCount;   
    int currentFrame; 
    float frameCounter;
    int frameWidth;   
    int frameHeight;  
    Rectangle frameRec;
} AnimSprite;

// ADT untuk satu entitas Enemy
typedef struct {    
    AnimSprite animData; 
    Vector2 position;
    int hp;
    float speed;
    bool active;
    bool spawned;
    int segment;     
    float t;         
    int spriteType;     
    float drawScale;   
    int waveNum;     
    EnemyWave* parentWave; 
    bool isStunned;
    float stunTimer;
} Enemy;

// ADT untuk Antrian Musuh (Queue) yang akan di-spawn dalam satu wave
typedef struct EnemyQueueNode {
    Enemy enemy;
    struct EnemyQueueNode *next;
} EnemyQueueNode;

typedef struct {
    EnemyQueueNode *front; 
    EnemyQueueNode *rear;  
    int count;             
} EnemyQueue;

// ADT untuk satu Gelombang Musuh (Wave)
struct EnemyWave {
    EnemyQueue enemyQueue; 
    Enemy *activeEnemies;  
    int maxActiveEnemies;  
    int currentActiveCount;
    Enemy *allEnemies; 
    int activeCount;
    
    Texture2D timerTexture; 
    Vector2 path[MAX_PATH_POINTS];
    int pathCount;
    int waveNum;

    int enemiesToSpawnInThisWave; 
    float spawnTimer;      
    int spawnedCount;
    
    int nextSpawnIndex;    
    float timerCurrentTime;
    float timerDuration;
    bool timerVisible;
    bool active;
    float lastWaveSpawnTime; 
    int timerMapRow;
    int timerMapCol;
    float interWaveTimer;
    float waitingForNextWave;
    float previousWaveSpawnTimeReference;
};

typedef struct WaveQueueNode {
    EnemyWave *wave; 
    struct WaveQueueNode *next;
} WaveQueueNode;

typedef struct WaveQueue {
    WaveQueueNode *front; 
    WaveQueueNode *rear;  
    int count;             
} WaveQueue;

extern EnemyWave *currentWave;
extern WaveQueue incomingWaves;     
extern Enemy *allActiveEnemies;      
extern int maxTotalActiveEnemies;   
extern int totalActiveEnemiesCount; 
extern int currentWaveNum;          

//DEKLARASI MODUL (PROTOTIPE FUNGSI)
// I.S. : Aset-aset untuk musuh belum dimuat.
// F.S. : Semua aset yang diperlukan oleh modul Enemy telah dimuat ke memori.
void Enemies_InitAssets();

// I.S. : Aset-aset musuh mungkin sedang digunakan.
// F.S. : Semua aset yang digunakan oleh modul Enemy telah dihapus dari memori.
void Enemies_ShutdownAssets();

// I.S. : q adalah antrian sembarang.
// F.S. : q adalah antrian kosong yang siap digunakan (front dan rear NULL, count 0).
void InitEnemyQueue(EnemyQueue *q);

// I.S. : q terdefinisi, mungkin kosong.
// F.S. : 'enemy' ditambahkan sebagai elemen terakhir (rear) dari antrian q.
void EnqueueEnemy(EnemyQueue *q, Enemy enemy);

// I.S. : q tidak kosong.
// F.S. : Elemen pertama (front) dari q dihapus, nilainya disalin ke 'enemy', dan fungsi mengembalikan true.
// Jika q kosong, fungsi mengembalikan false.
bool DequeueEnemy(EnemyQueue *q, Enemy *enemy); 

// I.S. : q terdefinisi.
// F.S. : Mengembalikan true jika q tidak memiliki elemen.
bool IsEnemyQueueEmpty(EnemyQueue *q);    

// I.S. : q terdefinisi, mungkin berisi elemen.
// F.S. : q menjadi antrian kosong, semua node yang dialokasikan telah dibebaskan.
void ClearEnemyQueue(EnemyQueue *q);

// I.S. : q adalah antrian sembarang.
// F.S. : q adalah antrian kosong yang siap digunakan (front dan rear NULL, count 0).
void InitWaveQueue(WaveQueue *q);

// I.S. : q terdefinisi, mungkin kosong.
// F.S. : 'wave' ditambahkan sebagai elemen terakhir (rear) dari antrian q.
void EnqueueWave(WaveQueue *q, EnemyWave *wave);

// I.S. : q tidak kosong.
// F.S. : Elemen pertama (front) dari q dihapus dan pointernya dikembalikan.
// Jika q kosong, fungsi mengembalikan NULL.
EnemyWave* DequeueWave(WaveQueue *q); 

// I.S. : q terdefinisi.
// F.S. : Mengembalikan true jika q tidak memiliki elemen.
bool IsWaveQueueEmpty(WaveQueue *q);

// I.S. : q terdefinisi, mungkin berisi elemen.
// F.S. : q menjadi antrian kosong, semua node yang dialokasikan telah dibebaskan.
void ClearWaveQueue(WaveQueue *q);

// I.S. : Sprite belum dimuat
// F.S. : Mengembalikan sebuah struct AnimSprite yang sudah diinisialisasi dengan tekstur
// dari 'filename' dan properti animasi yang sesuai.
AnimSprite LoadAnimSprite(const char *filename, int cols, int speed, int frameCount);

// I.S. : 'sprite' berada pada frame animasi tertentu.
// F.S. : 'sprite->currentFrame' mungkin bertambah berdasarkan 'deltaTime' dan 'frameSpeed'.
void UpdateAnimSprite(AnimSprite *sprite);

// I.S. : 'sprite' terdefinisi.
// F.S. : Frame animasi saat ini dari 'sprite' digambar ke layar pada 'position' yang ditentukan.
void DrawAnimSprite(const AnimSprite *sprite, Vector2 position, float scale, Color tint);

//I.S : Sprite terdefinisi
//F.S : Semua sprite dibebaskan dari memori
void UnloadAnimSprite(AnimSprite *sprite);

// I.S. : 'waveToBuild->path' kosong.
// F.S. : 'waveToBuild->path' telah diisi dengan koordinat jalur yang ditemukan dari titik awal.
// 'waveToBuild->pathCount' diperbarui.
void Enemies_BuildPath(int startX, int startY, EnemyWave* waveToBuild); 

// I.S. : Diberikan sebuah target saat ini dan daftar target yang sudah dikecualikan.
// F.S. : Mengembalikan pointer ke musuh terdekat berikutnya dalam jangkauan 'range' yang belum ada
// di dalam daftar 'excludedTargets'. Mengembalikan NULL jika tidak ada.
Enemy* FindNextChainTarget(Enemy* currentTarget, Enemy* excludedTargets[], int excludedCount, float range);

// I.S. : Posisi dan status musuh pada frame sebelumnya.
// F.S. : Posisi semua musuh yang aktif di 'allActiveEnemies' diperbarui sesuai 'deltaTime' dan jalurnya.
void Enemies_Update(float deltaTime);

// I.S. : 'allActiveEnemies' berisi data musuh yang akan digambar.
// F.S. : Semua musuh yang 'active' telah digambar ke layar pada posisi dan skala yang tepat.
void Enemies_Draw(float globalScale, float offsetX, float offsetY);

// I.S. : EnemyWave belum dibuat
// F.S. : Sebuah 'EnemyWave' dibuat, dialokasikan di memori, dan dikembalikan.
EnemyWave* CreateWave(int startRow, int startCol); 

// I.S. : 'wave' menunjuk ke sebuah EnemyWave yang valid.
// F.S. : Semua memori yang dialokasikan untuk 'wave' dan antrian di dalamnya telah dibebaskan.
// Pointer 'wave' diatur menjadi NULL.
void FreeWave(EnemyWave **wave);

// I.S. : 'wave' memiliki timer yang sedang berjalan atau tidak aktif.
// F.S. : 'timerCurrentTime' dari 'wave' diperbarui. Jika timer selesai, 'wave->active' menjadi true
// dan fungsi mengembalikan 'true'. Jika tidak, mengembalikan 'false'.
bool UpdateWaveTimer(EnemyWave *wave, float deltaTime);

// I.S. : 'wave' adalah gelombang yang sedang atau telah berjalan.
// F.S. : Mengembalikan 'true' jika semua musuh dari 'wave' ini sudah di-spawn DAN sudah tidak ada lagi
// yang aktif di layar. Mengembalikan 'false' jika sebaliknya.
bool AllEnemiesInWaveFinished(const EnemyWave *wave);

// I.S. : 'wave' adalah gelombang yang sedang dalam fase hitung mundur.
// F.S. : Visual timer (lingkaran merah) digambar ke layar jika 'wave->timerVisible' adalah true.
void DrawGameTimer(const EnemyWave *wave, float globalScale, float offsetX, float offsetY, int mapRow, int mapCol);

// Mengembalikan posisi Vector2 dari 'enemy'.
Vector2 GetEnemyPosition(const Enemy *enemy);

// Mengembalikan nilai HP dari 'enemy'.
int GetEnemyHP(const Enemy *enemy);

// Mengirimkan nilai waktu saat ini (progress) dari timer 'wave'.
float GetWaveTimerCurrentTime(const EnemyWave *wave);

// Mengirimkan true jika visual timer untuk 'wave' sedang ditampilkan.
bool GetWaveTimerVisible(const EnemyWave *wave);

// Mengirimkan true jika 'wave' sedang dalam fase aktif (men-spawn musuh).
bool GetWaveActive(const EnemyWave *wave);

// Mengirimkan total durasi timer dari 'wave'.
float GetWaveTimerDuration(const EnemyWave *wave);

// Mengirimkan posisi baris di peta untuk timer 'wave'.
int GetTimerMapRow(const EnemyWave *wave);

// Mengirimkan posisi kolom di peta untuk timer 'wave'.
int GetTimerMapCol(const EnemyWave *wave);

// I.S. : HP 'enemy' memiliki nilai lama.
// F.S. : HP 'enemy' diatur menjadi nilai 'hp' yang baru.
void SetEnemyHP(Enemy *enemy, int hp);

// I.S. : Waktu progress timer 'wave' sembarang.
// F.S. : Properti 'wave->timerCurrentTime' diatur menjadi nilai 'time' yang baru.
void SetWaveTimerCurrentTime(EnemyWave *wave, float time);

// I.S. : Durasi timer 'wave' sembarang.
// F.S. : 'wave->timerDuration' diatur menjadi nilai 'duration' yang baru.
void SetWaveTimerDuration(EnemyWave *wave, float duration);

// I.S. : Status visibilitas timer pada 'wave' sembarang.
// F.S. : Properti 'wave->timerVisible' diatur menjadi nilai 'visible' yang baru.
void SetWaveTimerVisible(EnemyWave *wave, bool visible);

// I.S. : Status aktif 'wave' (apakah sedang men-spawn musuh) sembarang.
// F.S. : Properti 'wave->active' diatur menjadi nilai 'active' yang baru.
void SetWaveActive(EnemyWave *wave, bool active);

// I.S. : Posisi baris untuk visual timer 'wave' di peta sembarang.
// F.S. : Properti 'wave->timerMapRow' diatur menjadi nilai 'row' yang baru.
void SetTimerMapRow(EnemyWave *wave, int row);

// I.S. : Posisi kolom untuk visual timer 'wave' di peta sembarang.
// F.S. : Properti 'wave->timerMapCol' diatur menjadi nilai 'col' yang baru.
void SetTimerMapCol(EnemyWave *wave, int col);

#endif