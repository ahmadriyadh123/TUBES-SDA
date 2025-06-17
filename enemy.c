/* File        : enemy.h 
*
*  Deskripsi   : Deklarasi Abstract Data Type (ADT) untuk Enemy dan EnemyWave. 
*                Mengelola semua data dan perilaku yang terkait dengan musuh dan gelombang musuh.
* 
*  Penulis     : Micky Ridho Pratama
*  Perubahan terakhir: Sabtu, 14 Juni 2025
*/

#include <raylib.h>
#include <raymath.h>
#include "enemy.h"
#include "utils.h"
#include "map.h"
#include "player_resources.h"
#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static AnimSprite enemy1_anim_data = {0};
static AnimSprite enemy2_anim_data = {0};

static int dx_path[] = {0, 1, 0, -1};
static int dy_path[] = {-1, 0, 1, 0};

EnemyWave *currentWave = NULL;
WaveQueue incomingWaves; 
Enemy *allActiveEnemies = NULL;  
int maxTotalActiveEnemies = 200; 
int totalActiveEnemiesCount = 0; 
int currentWaveNum = 1;

// I.S. : Aset-aset untuk musuh belum dimuat.
// F.S. : Semua aset yang diperlukan oleh modul Enemy telah dimuat ke memori.
void Enemies_InitAssets() {
    enemy1_anim_data = LoadAnimSprite("assets/img/gameplay_imgs/enemy1.png",7,10,7);
    enemy2_anim_data = LoadAnimSprite("assets/img/gameplay_imgs/enemy2.png",4,12,4);

    
    allActiveEnemies = (Enemy*)calloc(maxTotalActiveEnemies, sizeof(Enemy));
    if (allActiveEnemies == NULL) {
        TraceLog(LOG_FATAL, "Failed to allocate allActiveEnemies array.");
    }
    totalActiveEnemiesCount = 0;
    InitWaveQueue(&incomingWaves); 
    TraceLog(LOG_INFO, "Enemy assets initialized. Max active enemies: %d", maxTotalActiveEnemies);
}

// I.S. : Aset-aset musuh mungkin sedang digunakan.
// F.S. : Semua aset yang digunakan oleh modul Enemy telah dihapus dari memori.
void Enemies_ShutdownAssets() {
    UnloadAnimSprite(&enemy1_anim_data);
    UnloadAnimSprite(&enemy2_anim_data);
    if (allActiveEnemies) {
        free(allActiveEnemies);
        allActiveEnemies = NULL;
    }
    ClearWaveQueue(&incomingWaves); 
    TraceLog(LOG_INFO, "Enemy assets shutdown.");
}

// I.S. : q adalah antrian sembarang.
// F.S. : q adalah antrian kosong yang siap digunakan (front dan rear NULL, count 0).
void InitEnemyQueue(EnemyQueue *q) {
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

// I.S. : q adalah antrian sembarang.
// F.S. : q adalah antrian kosong yang siap digunakan (front dan rear NULL, count 0).
void InitWaveQueue(WaveQueue *q) {
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

// I.S. : q tidak kosong.
// F.S. : Elemen pertama (front) dari q dihapus, nilainya disalin ke 'enemy', dan fungsi mengembalikan true.
// Jika q kosong, fungsi mengembalikan false.
void EnqueueEnemy(EnemyQueue *q, Enemy enemy) {
    EnemyQueueNode *newNode = (EnemyQueueNode*)malloc(sizeof(EnemyQueueNode));
    if (newNode == NULL) {
        TraceLog(LOG_ERROR, "Failed to allocate new EnemyQueueNode.");
        return;
    }
    newNode->enemy = enemy;
    newNode->next = NULL;

    if (q->rear == NULL) { 
        q->front = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->count++;
}

// I.S. : q terdefinisi, mungkin kosong.
// F.S. : 'wave' ditambahkan sebagai elemen terakhir (rear) dari antrian q.
void EnqueueWave(WaveQueue *q, EnemyWave *wave) {
    WaveQueueNode *newNode = (WaveQueueNode*)malloc(sizeof(WaveQueueNode));
    if (newNode == NULL) {
        TraceLog(LOG_ERROR, "Failed to allocate new WaveQueueNode.");
        return;
    }
    newNode->wave = wave;
    newNode->next = NULL;

    if (q->rear == NULL) { 
        q->front = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->count++;
    TraceLog(LOG_INFO, "Wave %d enqueued. Total waves in queue: %d", wave->waveNum, q->count);
}

// I.S. : q tidak kosong.
// F.S. : Elemen pertama (front) dari q dihapus, nilainya disalin ke 'enemy', dan fungsi mengembalikan true.
// Jika q kosong, fungsi mengembalikan false.
bool DequeueEnemy(EnemyQueue *q, Enemy *enemy) {
    if (q->front == NULL) { 
        return false;
    }
    EnemyQueueNode *temp = q->front;
    *enemy = temp->enemy; 
    q->front = q->front->next;

    if (q->front == NULL) { 
        q->rear = NULL;
    }
    free(temp);
    q->count--;
    return true;
}

// I.S. : q tidak kosong.
// F.S. : Elemen pertama (front) dari q dihapus dan pointernya dikembalikan.
// Jika q kosong, fungsi mengembalikan NULL.
EnemyWave* DequeueWave(WaveQueue *q) {
    if (q->front == NULL) { 
        return NULL;
    }
    WaveQueueNode *temp = q->front;
    EnemyWave *dequeuedWave = temp->wave;
    q->front = q->front->next;

    if (q->front == NULL) { 
        q->rear = NULL;
    }
    free(temp);
    q->count--;
    TraceLog(LOG_INFO, "Wave %d dequeued. Total waves in queue: %d", dequeuedWave->waveNum, q->count);
    return dequeuedWave;
}

// I.S. : q terdefinisi.
// F.S. : Mengembalikan true jika q tidak memiliki elemen.
bool IsEnemyQueueEmpty(EnemyQueue *q) {
    return q->front == NULL;
}

// I.S. : q terdefinisi.
// F.S. : Mengembalikan true jika q tidak memiliki elemen.
bool IsWaveQueueEmpty(WaveQueue *q) {
    return q->front == NULL;
}

// I.S. : q terdefinisi, mungkin berisi elemen.
// F.S. : q menjadi antrian kosong, semua node yang dialokasikan telah dibebaskan.
void ClearEnemyQueue(EnemyQueue *q) {
    EnemyQueueNode *current = q->front;
    while (current != NULL) {
        EnemyQueueNode *next = current->next;
        free(current);
        current = next;
    }
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

// I.S. : q terdefinisi, mungkin berisi elemen.
// F.S. : q menjadi antrian kosong, semua node yang dialokasikan telah dibebaskan.
void ClearWaveQueue(WaveQueue *q) {
    WaveQueueNode *current = q->front;
    while (current != NULL) {
        WaveQueueNode *next = current->next;
        FreeWave(&current->wave); 
        free(current);
        current = next;
    }
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
    TraceLog(LOG_INFO, "WaveQueue cleared.");
}

// I.S. : Sprite belum dimuat
// F.S. : Mengembalikan sebuah struct AnimSprite yang sudah diinisialisasi dengan tekstur
// dari 'filename' dan properti animasi yang sesuai.
AnimSprite LoadAnimSprite(const char *filename, int cols, int speed, int frameCount) {
    AnimSprite sprite = {0};
    sprite.texture = LoadTextureSafe(filename);
    if (sprite.texture.id == 0) {
        TraceLog(LOG_ERROR, "ERROR: LoadAnimSprite failed to load texture: %s", filename);
        sprite.frameWidth = 0;
        sprite.frameHeight = 0;
        sprite.frameCount = 0;
        return sprite;
    }

    sprite.frameCols = cols;
    sprite.frameSpeed = speed;
    sprite.frameCount = frameCount;

    sprite.currentFrame = 0;
    sprite.frameCounter = 0.0f;

    if (cols > 0) {
        sprite.frameWidth = sprite.texture.width / cols;
    } else {
        sprite.frameWidth = sprite.texture.width;
        TraceLog(LOG_WARNING, "LoadAnimSprite: 'cols' parameter is 0, assuming 1 column for %s.", filename);
    }

    
    sprite.frameHeight = sprite.texture.height; 
        
    sprite.frameRec = (Rectangle){
        0.0f,
        0.0f, 
        (float)sprite.frameWidth,
        (float)sprite.frameHeight
    };
    
    TraceLog(LOG_INFO, "Loaded AnimSprite: %s",
             filename, cols, sprite.frameWidth, sprite.frameHeight, frameCount);

    return sprite;
}

// I.S. : 'sprite' berada pada frame animasi tertentu.
// F.S. : 'sprite->currentFrame' mungkin bertambah berdasarkan 'deltaTime' dan 'frameSpeed'.
void UpdateAnimSprite(AnimSprite *sprite)
{
    if (sprite->texture.id == 0)
        return;

    sprite->frameCounter += GetFrameTime();
    float frameDuration = 1.0f / sprite->frameSpeed;

    if (sprite->frameCounter >= frameDuration)
    {
        sprite->frameCounter -= frameDuration;
        sprite->currentFrame++;
        if (sprite->currentFrame >= sprite->frameCount)
        {
            sprite->currentFrame = 0;
        }
        sprite->frameRec.x = (float)sprite->currentFrame * sprite->frameWidth;
    }
}

// I.S. : 'sprite' terdefinisi.
// F.S. : Frame animasi saat ini dari 'sprite' digambar ke layar pada 'position' yang ditentukan.
void DrawAnimSprite(const AnimSprite *sprite, Vector2 position, float scale, Color tint)
{
    if (sprite->texture.id == 0)
        return;
    Rectangle destRec = {
        position.x - (sprite->frameWidth * scale / 2.0f),
        position.y - (sprite->frameHeight * scale / 2.0f),
        sprite->frameWidth * scale,
        sprite->frameHeight * scale};
    DrawTexturePro(sprite->texture, sprite->frameRec, destRec, (Vector2){0, 0}, 0.0f, tint);
}

//I.S : Sprite terdefinisi
//F.S : Semua sprite dibebaskan dari memori
void UnloadAnimSprite(AnimSprite *sprite)
{
    if (sprite->texture.id > 0)
    {
        UnloadTextureSafe(&sprite->texture);
        sprite->texture = (Texture2D){0};
    }
}

//Mengembalikan jalur musuh bergerak
static bool IsPathTile(int row, int col)
{
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
    {
        int tileValue = GetMapTile(row, col);
        return (tileValue == 1);
    }
    return false;
}

//Mengembalikan pengecekan tile atau bukan
static void BuildPathDFS_Internal(int x, int y, bool visited[MAP_ROWS][MAP_COLS], Vector2 outPath[], int *count)
{
    if (y < 0 || y >= MAP_ROWS || x < 0 || x >= MAP_COLS)
        return;

    if (visited[y][x] || !IsPathTile(y, x))
        return;

    if (*count >= MAX_PATH_POINTS)
    {
        TraceLog(LOG_WARNING, "MAX_PATH_POINTS reached in BuildPathDFS_Internal. Path may be incomplete.");
        return;
    }

    visited[y][x] = true;
    outPath[*count] = (Vector2){x * (float)TILE_SIZE + TILE_SIZE / 2.0f,
                                y * (float)TILE_SIZE + TILE_SIZE / 2.0f};
    (*count)++;

    for (int d = 0; d < 4; d++)
    {
        int nx = x + dx_path[d];
        int ny = y + dy_path[d];

        if (nx >= 0 && nx < MAP_COLS && ny >= 0 && ny < MAP_ROWS &&
            IsPathTile(ny, nx) && !visited[ny][nx])
        {
            BuildPathDFS_Internal(nx, ny, visited, outPath, count);
        }
    }
}

// I.S. : 'waveToBuild->path' kosong.
// F.S. : 'waveToBuild->path' telah diisi dengan koordinat jalur yang ditemukan dari titik awal.
// 'waveToBuild->pathCount' diperbarui.
void Enemies_BuildPath(int startX, int startY, EnemyWave* waveToBuild) {
    
    bool visited[MAP_ROWS][MAP_COLS] = {false};
    waveToBuild->pathCount = 0;
    
    if (startX < 0 || startX >= MAP_COLS || startY < 0 || startY >= MAP_ROWS || !IsPathTile(startY, startX)) {
        TraceLog(LOG_ERROR, "Pathfinding failed: Start point (%d, %d) is not a valid path tile.", startX, startY);
        return; 
    }

    int currentX = startX;
    int currentY = startY;

    while (waveToBuild->pathCount < MAX_PATH_POINTS) {
        
        visited[currentY][currentX] = true;

        waveToBuild->path[waveToBuild->pathCount] = (Vector2){
            currentX * (float)TILE_SIZE + TILE_SIZE / 2.0f,
            currentY * (float)TILE_SIZE + TILE_SIZE / 2.0f
        };
        waveToBuild->pathCount++;
        bool foundNextStep = false;

        for (int d = 0; d < 4; d++) {
            int nextX = currentX + dx_path[d];
            int nextY = currentY + dy_path[d];

            if (IsPathTile(nextY, nextX) && !visited[nextY][nextX]) {
                
                currentX = nextX;
                currentY = nextY;
                foundNextStep = true;
                break; 
            }
        }
        if (!foundNextStep) {
            break; 
        }
    }

    if (waveToBuild->pathCount > 0) {
         TraceLog(LOG_INFO, "Path built using Smart Tracer. Points: %d. Start: (%d, %d).",
                 waveToBuild->pathCount, startX, startY);
    } else {
        TraceLog(LOG_WARNING, "Pathfinding failed to build any path from start (%d, %d).", startX, startY);
    }
}

// I.S. : 'waveToBuild->path' kosong.
// F.S. : 'waveToBuild->path' telah diisi dengan koordinat jalur yang ditemukan dari titik awal.
// 'waveToBuild->pathCount' diperbarui.
Enemy* FindNextChainTarget(Enemy* currentTarget, Enemy* excludedTargets[], int excludedCount, float range) {
    Enemy* bestTarget = NULL;
    float minDistance = range;

    for (int i = 0; i < maxTotalActiveEnemies; i++) {
        Enemy* potentialTarget = &allActiveEnemies[i];
        if (!potentialTarget->active) continue;

        bool isExcluded = false;
        for (int j = 0; j < excludedCount; j++) {
            if (potentialTarget == excludedTargets[j]) {
                isExcluded = true;
                break;
            }
        }

        if (!isExcluded) {
            float distance = Vector2Distance(GetEnemyPosition(currentTarget), GetEnemyPosition(potentialTarget));
            if (distance < minDistance) {
                minDistance = distance;
                bestTarget = potentialTarget;
            }
        }
    }
    return bestTarget;
}

void Enemies_Update(float deltaTime) {
    for (int i = 0; i < maxTotalActiveEnemies; i++) { 
        Enemy *e = &allActiveEnemies[i];

        if (!e->active) continue;
       
        if (e->isStunned) {
            e->stunTimer -= deltaTime;
            if (e->stunTimer <= 0) {
                e->isStunned = false;
            } else {
                UpdateAnimSprite(&e->animData); 
                continue; 
            }
        }
        UpdateAnimSprite(&e->animData);
        
        if (!e->parentWave || e->parentWave->pathCount < 2) {
            continue; 
        }

        bool isDefeated = false;
        bool reachedEnd = false;
        
        int s = e->segment;
        if (s < e->parentWave->pathCount - 1) {
            Vector2 startPoint = e->parentWave->path[s];
            Vector2 endPoint = e->parentWave->path[s + 1];
            float segmentLength = Vector2Distance(startPoint, endPoint);

            if (segmentLength > 0) {
                e->t += (e->speed * deltaTime) / segmentLength;
            } else {
                e->t = 1.0f;
            }

            if (e->t >= 1.0f) {
                e->segment++;
                s = e->segment;
                e->t = fmod(e->t, 1.0f);
                if (s >= e->parentWave->pathCount) {
                    reachedEnd = true;
                }
            }
            
            if (e->active && s < e->parentWave->pathCount - 1) { 
                e->position = Vector2Lerp(e->parentWave->path[s], e->parentWave->path[s + 1], e->t);
            }
        } else {
            reachedEnd = true;
        }
        
        
        if (e->hp <= 0) {
            e->active = false;
            totalActiveEnemiesCount--;
            AddMoney(15); 
            PlayEnemyDefeatedSound();
            continue; 
        }

        
        if (reachedEnd) {
            e->active = false;
            totalActiveEnemiesCount--;
            DecreaseLife(1); 
            
            continue; 
        }

        if (isDefeated) {
            e->active = false;
            totalActiveEnemiesCount--;
            AddMoney(15);
            PlayEnemyDefeatedSound();
        }
    }
}



void Enemies_Draw(float globalScale, float offsetX, float offsetY) {
    if (!allActiveEnemies || totalActiveEnemiesCount == 0) {
        return;
    }

    for (int i = 0; i < maxTotalActiveEnemies; i++) {
        const Enemy *e = &allActiveEnemies[i];
        if (!e->active) 
            continue;

        Vector2 screenPos = {
            offsetX + e->position.x * globalScale,
            offsetY + e->position.y * globalScale};

        DrawAnimSprite(&e->animData, screenPos, e->drawScale * globalScale, WHITE);

        
        float healthBarWidth = TILE_SIZE * globalScale * 0.8f;
        float healthBarHeight = 5.0f * globalScale;
        float healthBarOffsetY = -((float)e->animData.frameHeight * e->drawScale * globalScale / 2.0f) - (healthBarHeight / 2.0f) - (5.0f * globalScale);

        DrawRectangle(screenPos.x - (healthBarWidth / 2.0f),
                      screenPos.y + healthBarOffsetY,
                      healthBarWidth, healthBarHeight, BLACK);

        float currentHealthWidth = (float)e->hp / (100.0f + ((float)e->waveNum -1) * 10.0f) * healthBarWidth;
        if (currentHealthWidth < 0) currentHealthWidth = 0;

        DrawRectangle(screenPos.x - (healthBarWidth / 2.0f),
                      screenPos.y + healthBarOffsetY,
                      currentHealthWidth, healthBarHeight, LIME);
    }
}

// I.S. : EnemyWave belum dibuat
// F.S. : Sebuah 'EnemyWave' dibuat, dialokasikan di memori, dan dikembalikan.
EnemyWave* CreateWave(int startRow, int startCol) { 
    EnemyWave* newWave = (EnemyWave*)malloc(sizeof(EnemyWave)); 
    if (newWave == NULL) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for new EnemyWave. Returning NULL.");
        return NULL;
    }
    *newWave = (EnemyWave){0}; 

    newWave->waveNum = currentWaveNum; 
    
    newWave->enemiesToSpawnInThisWave = 5 + (newWave->waveNum - 1); 
    InitEnemyQueue(&newWave->enemyQueue);
    
    for (int i = 0; i < newWave->enemiesToSpawnInThisWave; i++) {
        Enemy tempEnemy = {0}; 
        tempEnemy.waveNum = newWave->waveNum; 
        tempEnemy.parentWave = newWave;
        tempEnemy.t = 0.0f;
        tempEnemy.speed = 15.0f + (newWave->waveNum * 3.0f) + (rand() % 20); 
        tempEnemy.active = false; 
        tempEnemy.spawned = false;
        tempEnemy.segment = 0;
        tempEnemy.position = (Vector2){0, 0};
        tempEnemy.hp = 100 + ((newWave->waveNum - 1) * 10); 
        tempEnemy.spriteType = rand() % 2;

        if (tempEnemy.spriteType == 0) {
            tempEnemy.animData = enemy1_anim_data;
            tempEnemy.drawScale = 0.7f;
        } else {
            tempEnemy.animData = enemy2_anim_data;
            tempEnemy.drawScale = 0.2f;   
        }
        tempEnemy.animData.currentFrame = 0;
        tempEnemy.animData.frameCounter = 0.0f;
        EnqueueEnemy(&newWave->enemyQueue, tempEnemy);
    }
    TraceLog(LOG_INFO, "Wave %d initialized with %d enemies in queue.", newWave->waveNum, newWave->enemyQueue.count);

    newWave->spawnedCount = 0; 
    newWave->nextSpawnIndex = 0; 
    newWave->spawnTimer = 0.0f; 
                                                            
    newWave->timerCurrentTime = 0.0f; 
    SetWaveTimerDuration(newWave, WAVE_TIMER_DURATION); 
    newWave->timerVisible = false; 
    newWave->active = false; 
    newWave->lastWaveSpawnTime = 0.0f; 
    
    newWave->timerVisible = true;
    TraceLog(LOG_INFO, "Wave %d created: Timer is set to be visible upon dequeue.", newWave->waveNum);
    
    SetTimerMapRow(newWave, startRow);
    SetTimerMapCol(newWave, startCol); 

    newWave->timerTexture = LoadTextureSafe("assets/img/gameplay_imgs/timer.png");
    if (newWave->timerTexture.id == 0) {
        TraceLog(LOG_WARNING, "Failed to load assets/timer.png for wave timer.");
    }

    Enemies_BuildPath(startCol, startRow, newWave);
    return newWave;
}

// I.S. : 'wave' menunjuk ke sebuah EnemyWave yang valid.
// F.S. : Semua memori yang dialokasikan untuk 'wave' dan antrian di dalamnya telah dibebaskan.
// Pointer 'wave' diatur menjadi NULL.
void FreeWave(EnemyWave **wave) { 
    if (wave && *wave) { 
        ClearEnemyQueue(&(*wave)->enemyQueue);
                
        if ((*wave)->timerTexture.id != 0) {
            UnloadTextureSafe(&(*wave)->timerTexture);
            (*wave)->timerTexture = (Texture2D){0};
        }
        free(*wave); 
        *wave = NULL; 
        TraceLog(LOG_INFO, "Wave freed.");
    }
}

// I.S. : 'wave' memiliki timer yang sedang berjalan atau tidak aktif.
// F.S. : 'timerCurrentTime' dari 'wave' diperbarui. Jika timer selesai, 'wave->active' menjadi true
// dan fungsi mengembalikan 'true'. Jika tidak, mengembalikan 'false'.
bool UpdateWaveTimer(EnemyWave *wave, float deltaTime) { 
    if (!wave) {
        TraceLog(LOG_WARNING, "UpdateWaveTimer: Wave is NULL.");
        return false; 
    }

    if (!wave->active && wave->timerVisible) {
        float timer = wave->timerCurrentTime + deltaTime;
        wave->timerCurrentTime = timer;
        
        if (timer >= wave->timerDuration) {
            wave->timerCurrentTime = 0.0f;
            wave->timerVisible = false;    
            wave->active = true; 
            wave->lastWaveSpawnTime = GetTime(); 

            TraceLog(LOG_INFO, "[W%d (num %d)] Timer finished. Wave ACTIVATED (enemies will spawn)! Last wave spawn time: %.2f",
                     wave->waveNum, currentWaveNum, wave->lastWaveSpawnTime);
            return true; 
        }
    }
    return false; 
}

// I.S. : 'wave' adalah gelombang yang sedang atau telah berjalan.
// F.S. : Mengembalikan 'true' jika semua musuh dari 'wave' ini sudah di-spawn DAN sudah tidak ada lagi
// yang aktif di layar. Mengembalikan 'false' jika sebaliknya.
bool AllEnemiesInWaveFinished(const EnemyWave *wave) {
    if (!wave) return false; 
    
    bool allSpawnedFromThisWave = (wave->spawnedCount >= wave->enemiesToSpawnInThisWave);

    int activeFromThisWave = 0;
    for (int i = 0; i < maxTotalActiveEnemies; i++) {
        if (allActiveEnemies[i].active && allActiveEnemies[i].waveNum == wave->waveNum) {
            activeFromThisWave++;
        }
    }
    return (allSpawnedFromThisWave && activeFromThisWave == 0);
}
// I.S. : 'wave' adalah gelombang yang sedang dalam fase hitung mundur.
// F.S. : Visual timer (lingkaran merah) digambar ke layar jika 'wave->timerVisible' adalah true.
void DrawGameTimer(const EnemyWave *wave, float globalScale, float offsetX, float offsetY, int timerRow, int timerCol)
{
    if (!wave || !GetWaveTimerVisible(wave))
        return;
    float tileScreenSize = TILE_SIZE * globalScale;
    float timerCenterX = offsetX + timerCol * tileScreenSize + tileScreenSize / 2.0f;
    float timerCenterY = offsetY + timerRow * tileScreenSize + tileScreenSize / 2.0f;
    Vector2 position = {timerCenterX, timerCenterY};
    float timerRadius = (TILE_SIZE / 2.0f) * globalScale * TIMER_OVERALL_SIZE_FACTOR;
    
    
    float progress = GetWaveTimerCurrentTime(wave) / GetWaveTimerDuration(wave);
    if (GetWaveTimerDuration(wave) <= 0.0f) progress = 0.0f; 
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;

    float angle = 360.0f * progress;
    DrawCircleSector(position, timerRadius, -90, -90 + angle, 100, RED);
    DrawCircle(position.x, position.y, timerRadius * 0.9f, LIGHTGRAY);
    float iconDiameter = timerRadius * TIMER_IMAGE_DISPLAY_FACTOR;
    Rectangle destination = {
        position.x - iconDiameter,
        position.y - iconDiameter,
        iconDiameter * 2.0f,
        iconDiameter * 2.0f};
    Rectangle source = {0, 0, (float)wave->timerTexture.width, (float)wave->timerTexture.height};
    if (wave->timerTexture.id != 0)
    {
        DrawTexturePro(wave->timerTexture, source, destination, (Vector2){0, 0}, 0.0f, WHITE);
    }
}

// Mengembalikan posisi Vector2 dari 'enemy'.
Vector2 GetEnemyPosition(const Enemy *enemy) { return enemy ? enemy->position : (Vector2){0, 0}; }

// Mengembalikan nilai HP dari 'enemy'.
int GetEnemyHP(const Enemy *enemy) { return enemy ? enemy->hp : 0; }

// Mengirimkan nilai waktu saat ini (progress) dari timer 'wave'.
float GetWaveTimerCurrentTime(const EnemyWave *wave) { return wave ? wave->timerCurrentTime : 0.0f; }

// Mengirimkan total durasi timer dari 'wave'.
float GetWaveTimerDuration(const EnemyWave *wave) { return wave ? wave->timerDuration : 0.0f; }

// Mengirimkan true jika visual timer untuk 'wave' sedang ditampilkan.
bool GetWaveTimerVisible(const EnemyWave *wave) { return wave ? wave->timerVisible : false; }

// Mengirimkan posisi baris di peta untuk timer 'wave'.
int GetTimerMapRow(const EnemyWave *wave) { return wave ? wave->timerMapRow : 0; }

// Mengirimkan posisi kolom di peta untuk timer 'wave'.
int GetTimerMapCol(const EnemyWave *wave) { return wave ? wave->timerMapCol : 0; }

// I.S. : HP 'enemy' memiliki nilai lama.
// F.S. : HP 'enemy' diatur menjadi nilai 'hp' yang baru.
void SetEnemyHP(Enemy *enemy, int hp)
{
    if (enemy)
        enemy->hp = hp;
}

// I.S. : Waktu progress timer 'wave' sembarang.
// F.S. : Properti 'wave->timerCurrentTime' diatur menjadi nilai 'time' yang baru.
void SetWaveTimerCurrentTime(EnemyWave *wave, float time)
{
    if (wave)
        wave->timerCurrentTime = time;
}

// I.S. : Durasi timer 'wave' sembarang.
// F.S. : 'wave->timerDuration' diatur menjadi nilai 'duration' yang baru.
void SetWaveTimerDuration(EnemyWave *wave, float duration)
{
    if (wave)
        wave->timerDuration = duration;
}

// I.S. : Status visibilitas timer pada 'wave' sembarang.
// F.S. : Properti 'wave->timerVisible' diatur menjadi nilai 'visible' yang baru.
void SetWaveTimerVisible(EnemyWave *wave, bool visible)
{
    if (wave)
        wave->timerVisible = visible;
}

// I.S. : Status aktif 'wave' (apakah sedang men-spawn musuh) sembarang.
// F.S. : Properti 'wave->active' diatur menjadi nilai 'active' yang baru.
void SetWaveActive(EnemyWave *wave, bool active)
{
    if (wave)
        wave->active = active;
}

// I.S. : Posisi baris untuk visual timer 'wave' di peta sembarang.
// F.S. : Properti 'wave->timerMapRow' diatur menjadi nilai 'row' yang baru.
void SetTimerMapRow(EnemyWave *wave, int row)
{
    if (wave)
        wave->timerMapRow = row;
}

// I.S. : Posisi kolom untuk visual timer 'wave' di peta sembarang.
// F.S. : Properti 'wave->timerMapCol' diatur menjadi nilai 'col' yang baru.
void SetTimerMapCol(EnemyWave *wave, int col)
{
    if (wave)
        wave->timerMapCol = col;
}