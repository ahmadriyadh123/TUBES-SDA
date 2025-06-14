#include <raylib.h>
#include <raymath.h>
#include "enemy.h"
#include "utils.h"
#include "map.h"
#include "player_resources.h"
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

void Enemies_InitAssets() {
    enemy1_anim_data = LoadAnimSprite("assets/enemy1.png",7,10,7);
    enemy2_anim_data = LoadAnimSprite("assets/enemy2.png",4,12,4);

    allActiveEnemies = (Enemy*)calloc(maxTotalActiveEnemies, sizeof(Enemy));
    if (allActiveEnemies == NULL) {
        TraceLog(LOG_FATAL, "Failed to allocate allActiveEnemies array.");
    }
    totalActiveEnemiesCount = 0;
    InitWaveQueue(&incomingWaves);
    TraceLog(LOG_INFO, "Enemy assets initialized. Max active enemies: %d", maxTotalActiveEnemies);
}

void Enemies_ShutdownAssets()
{
    UnloadAnimSprite(&enemy1_anim_data);
    UnloadAnimSprite(&enemy2_anim_data);
    if (allActiveEnemies) {
        free(allActiveEnemies);
        allActiveEnemies = NULL;
    }
    ClearWaveQueue(&incomingWaves); 
    TraceLog(LOG_INFO, "Enemy assets unloaded.");
}

void InitEnemyQueue(EnemyQueue *q) {
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

void InitWaveQueue(WaveQueue *q) {
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}
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

bool IsEnemyQueueEmpty(EnemyQueue *q) {
    return q->front == NULL;
}
bool IsWaveQueueEmpty(WaveQueue *q) {
    return q->front == NULL;
}

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

    // Hitung lebar dan tinggi satu frame
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

void UnloadAnimSprite(AnimSprite *sprite)
{
    if (sprite->texture.id > 0)
    {
        UnloadTextureSafe(&sprite->texture);
        sprite->texture = (Texture2D){0};
    }
}

static bool IsPathTile(int row, int col)
{
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
    {
        int tileValue = GetMapTile(row, col);
        return (tileValue == 37);
    }
    return false;
}

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

void Enemies_BuildPath(int startX, int startY, EnemyWave* waveToBuild) { 
    bool visited[MAP_ROWS][MAP_COLS];
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            visited[r][c] = false;
        }
    }
    waveToBuild->pathCount = 0; 

    TraceLog(LOG_DEBUG, "Attempting to build path for wave %d from (%d, %d). Tile value: %d", 
             waveToBuild->waveNum, startX, startY, GetMapTile(startY, startX));

    if (startX >= 0 && startX < MAP_COLS && startY >= 0 && startY < MAP_ROWS &&
        IsPathTile(startY, startX)) {


        BuildPathDFS_Internal(startX, startY, visited, waveToBuild->path, &waveToBuild->pathCount);

        TraceLog(LOG_INFO, "Enemy path built for wave %d. Points: %d. Start: (%d, %d).", 
                 waveToBuild->waveNum, waveToBuild->pathCount, startX, startY);

    } else {
        TraceLog(LOG_ERROR, "Failed to build enemy path for wave %d: Start point (%d, %d) is not a valid path tile (value %d).",
                 waveToBuild->waveNum, startX, startY, GetMapTile(startY, startX));
        waveToBuild->pathCount = 0;
    }
}

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

void Enemies_Update(float deltaTime)
{
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
            isDefeated = true;
        }

        if (reachedEnd) {
            if (!isDefeated) { 
                DecreaseLife(1);
            }
            isDefeated = true;
        }

        if (isDefeated) {
            e->active = false;
            totalActiveEnemiesCount--;
            AddMoney(15);
        }
    }
}

void Enemies_Draw(const EnemyWave *wave, float globalScale, float offsetX, float offsetY)
{
    if (!allActiveEnemies || totalActiveEnemiesCount == 0) {
        TraceLog(LOG_DEBUG, "Enemies_Draw: No active enemies to draw (allActiveEnemies NULL or count is 0).");
        return;
    }

    TraceLog(LOG_DEBUG, "Enemies_Draw: Drawing %d active enemies.", totalActiveEnemiesCount);
    for (int i = 0; i < wave->total; i++)
    {
        const Enemy *e = &wave->allEnemies[i];
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

        float currentHealthWidth = (float)e->hp / (100.0f + (wave->waveNum * 10)) * healthBarWidth;
        if (currentHealthWidth < 0)
            currentHealthWidth = 0;

        DrawRectangle(screenPos.x - (healthBarWidth / 2.0f),
                      screenPos.y + healthBarOffsetY,
                      currentHealthWidth, healthBarHeight, LIME);
    }
}

// Ubah CreateWave agar mengembalikan pointer EnemyWave
EnemyWave* CreateWave(int startRow, int startCol) { // <--- UBAH DEFINISI
    EnemyWave* newWave = (EnemyWave*)malloc(sizeof(EnemyWave)); // Alokasikan memori
    if (newWave == NULL) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for new EnemyWave. Returning NULL.");
        return NULL;
    }
    *newWave = (EnemyWave){0}; // Inisialisasi memori yang dialokasikan menjadi nol (PENTING!)

    newWave->total = 20; // Atur total musuh di sini
    newWave->allEnemies = (Enemy *)malloc(sizeof(Enemy) * newWave->total); // Alokasikan musuh
    if (newWave->allEnemies == NULL) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for enemies in wave. Freeing newWave and returning NULL.");
        free(newWave);
        return NULL;
    }
    
    newWave->activeCount = 0;
    newWave->spawnedCount = 0;
    newWave->nextSpawnIndex = 0;
    newWave->spawnTimer = 1.0f; 

    newWave->waveNum = currentWaveNum; // Gunakan currentWaveNum global

    // Inisialisasi state timer untuk gelombang baru
    newWave->interWaveTimer = 0.0f; // Reset untuk setiap gelombang baru
    newWave->lastWaveSpawnTime = 0.0f; // Akan diisi saat wave ini aktif
    newWave->previousWaveSpawnTimeReference = 0.0f; // Akan diisi dari state.c
    
    SetWaveTimerCurrentTime(newWave, 0.0f); // Reset timer utama
    SetWaveTimerDuration(newWave, WAVE_TIMER_DURATION); // <--- KUNCI: Atur durasi timer di sini!
    SetWaveActive(newWave, false); // Gelombang awalnya TIDAK aktif

    if (newWave->waveNum == 1) {
        newWave->waitingForNextWave = false;
        SetWaveTimerVisible(newWave, true);
        TraceLog(LOG_INFO, "Wave 1 created: Timer visible immediately.");
    } else {
        newWave->waitingForNextWave = true;
        SetWaveTimerVisible(newWave, false); // Timer belum terlihat
        TraceLog(LOG_INFO, "Wave %d created: Waiting for inter-wave delay from previous wave's start.", newWave->waveNum);
    }
    
    SetTimerMapRow(newWave, startRow);
    SetTimerMapCol(newWave, startCol);

    newWave->timerTexture = LoadTextureSafe("assets/timer.png");
    if (newWave->timerTexture.id == 0) {
        TraceLog(LOG_WARNING, "Failed to load assets/timer.png for wave timer.");
    }

    // Panggil Enemies_BuildPath dengan pointer newWave
    Enemies_BuildPath(startCol, startRow, newWave);

    for (int i = 0; i < newWave->total; i++) {
        Enemy *e = &newWave->allEnemies[i];
        e->t = 0.0f;
        e->speed = 15.0f + (newWave->waveNum * 3.0f) + (rand() % 20);
        e->active = false;
        e->spawned = false;
        e->segment = 0;
        e->position = (Vector2){0, 0};
        e->hp = 100 + (newWave->waveNum * 10);
        e->spriteType = rand() % 2;

        if (e->spriteType == 0) {
            e->animData = enemy1_anim_data;
            e->drawScale = 0.7f;
        } else {
            e->animData = enemy2_anim_data;
            e->drawScale = 0.2f;  
        }
        e->animData.currentFrame = 0;
        e->animData.frameCounter = 0.0f;
    }
    TraceLog(LOG_INFO, "Wave %d created with %d enemies. Path points: %d. Timer Duration: %.2f", 
             newWave->waveNum, newWave->total, newWave->pathCount, newWave->timerDuration);
    return newWave; // Kembalikan pointer
}

// Ubah FreeWave agar menerima pointer ke pointer EnemyWave
void FreeWave(EnemyWave **wave) { // <--- UBAH DEFINISI
    if (wave && *wave) { // Periksa apakah pointer ke pointer valid dan pointer itu sendiri tidak NULL
        if ((*wave)->allEnemies) {
            free((*wave)->allEnemies);
            (*wave)->allEnemies = NULL;
        }
        if ((*wave)->timerTexture.id != 0) {
            UnloadTextureSafe(&(*wave)->timerTexture);
            (*wave)->timerTexture = (Texture2D){0};
        }
        free(*wave); // Bebaskan memori EnemyWave itu sendiri
        *wave = NULL; // <--- Set pointer yang dilewatkan menjadi NULL setelah dibebaskan (PENTING!)
        TraceLog(LOG_INFO, "Wave freed.");
    }
}
bool AllEnemiesInWaveFinished(const EnemyWave *wave)
{
    return (wave && wave->spawnedCount >= wave->total && wave->activeCount <= 0);
}
void UpdateWaveTimer(EnemyWave *wave, float deltaTime) {
    if (!wave) {
        TraceLog(LOG_WARNING, "UpdateWaveTimer: Wave is NULL.");
        return;
    }

    // Fase 1: Menunggu Jeda Antar-Gelombang (waitingForNextWave)
    if (wave->waitingForNextWave) {
        // Hitung waktu berlalu dari referensi waktu spawn gelombang sebelumnya
        float timeSincePreviousTimerVanished = GetTime() - wave->previousWaveSpawnTimeReference;
        
        TraceLog(LOG_DEBUG, "[W%d (num %d)] WAITING. Time since prev timer vanished: %.2f / %.2f (INTER_WAVE_DELAY). Prev ref: %.2f",
                 wave->waveNum, currentWaveNum, timeSincePreviousTimerVanished, INTER_WAVE_DELAY, wave->previousWaveSpawnTimeReference);

        if (timeSincePreviousTimerVanished >= INTER_WAVE_DELAY) {
            wave->waitingForNextWave = false; // Jeda selesai
            SetWaveTimerVisible(wave, true);  // Timer utama sekarang terlihat
            SetWaveTimerCurrentTime(wave, 0.0f); // <--- KUNCI: Reset timer utama ke 0.0f di sini!
            TraceLog(LOG_INFO, "[W%d (num %d)] Inter-wave delay finished. Timer visible, starting countdown from 0.0f. Current Time: %.2f",
                     wave->waveNum, currentWaveNum, GetTime()); //
        }
        return; // SANGAT PENTING: Jangan lanjutkan ke hitung mundur utama jika masih dalam jeda.
    }

    // Fase 2: Hitung Mundur Utama (setelah jeda selesai, atau untuk gelombang 1)
    // Hanya update jika gelombang belum aktif DAN timer terlihat.
    if (!GetWaveActive(wave) && GetWaveTimerVisible(wave)) {
        float timer = GetWaveTimerCurrentTime(wave) + deltaTime;
        SetWaveTimerCurrentTime(wave, timer);
        TraceLog(LOG_DEBUG, "[W%d (num %d)] MAIN TIMER: %.2f / %.2f (Duration: %.2f). Current Time: %.2f",
                 wave->waveNum, currentWaveNum, GetWaveTimerCurrentTime(wave), GetWaveTimerDuration(wave), WAVE_TIMER_DURATION, GetTime()); //
        
        if (timer >= GetWaveTimerDuration(wave)) {
            SetWaveTimerCurrentTime(wave, 0.0f); // Reset lagi untuk jaga-jaga
            SetWaveTimerVisible(wave, false);    // Sembunyikan timer setelah selesai
            
            // --- KUNCI: Rekam waktu saat timer ini menghilang ---
            wave->previousWaveSpawnTimeReference = GetTime(); // <--- UBAH: Ini sekarang merekam waktu timer menghilang
            // lastWaveSpawnTime tetap mencatat waktu saat gelombang ini mulai spawn
            SetWaveActive(wave, true);           // Aktifkan gelombang: musuh mulai spawn
            wave->lastWaveSpawnTime = GetTime(); // Rekam waktu spawn gelombang ini

            TraceLog(LOG_INFO, "[W%d (num %d)] Timer finished. Wave ACTIVATED (enemies will spawn)! Timer vanished time: %.2f",
                     wave->waveNum, currentWaveNum, wave->previousWaveSpawnTimeReference); //
        }
    }
}
void DrawGameTimer(const EnemyWave *wave, float globalScale, float offsetX, float offsetY, int timerRow, int timerCol)
{
    if (!wave || !GetWaveTimerVisible(wave))
        return;
    float tileScreenSize = TILE_SIZE * globalScale;
    float timerCenterX = offsetX + timerCol * tileScreenSize + tileScreenSize / 2.0f;
    float timerCenterY = offsetY + timerRow * tileScreenSize + tileScreenSize / 2.0f;
    Vector2 position = {timerCenterX, timerCenterY};
    float timerRadius = (TILE_SIZE / 2.0f) * globalScale * TIMER_OVERALL_SIZE_FACTOR;
    
    // Pastikan progress valid (antara 0.0 dan 1.0)
    float progress = GetWaveTimerCurrentTime(wave) / GetWaveTimerDuration(wave);
    if (GetWaveTimerDuration(wave) <= 0.0f) progress = 0.0f; // Hindari pembagian nol
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

Vector2 GetEnemyPosition(const Enemy *enemy) { return enemy ? enemy->position : (Vector2){0, 0}; }
int GetEnemyHP(const Enemy *enemy) { return enemy ? enemy->hp : 0; }
float GetEnemySpeed(const Enemy *enemy) { return enemy ? enemy->speed : 0.0f; }
bool GetEnemyActive(const Enemy *enemy) { return enemy ? enemy->active : false; }
bool GetEnemySpawned(const Enemy *enemy) { return enemy ? enemy->spawned : false; }
int GetEnemyPathIndex(const Enemy *enemy) { return enemy ? enemy->segment : 0; }

int GetWaveTotal(const EnemyWave *wave) { return wave ? wave->total : 0; }
int GetWaveActiveCount(const EnemyWave *wave) { return wave ? wave->activeCount : 0; }
int GetWaveNum(const EnemyWave *wave) { return wave ? wave->waveNum : 0; }
float GetWaveTimerCurrentTime(const EnemyWave *wave) { return wave ? wave->timerCurrentTime : 0.0f; }
float GetWaveTimerDuration(const EnemyWave *wave) { return wave ? wave->timerDuration : 0.0f; }
bool GetWaveTimerVisible(const EnemyWave *wave) { return wave ? wave->timerVisible : false; }
bool GetWaveActive(const EnemyWave *wave) { return wave ? wave->active : false; }
int GetTimerMapRow(const EnemyWave *wave) { return wave ? wave->timerMapRow : 0; }
int GetTimerMapCol(const EnemyWave *wave) { return wave ? wave->timerMapCol : 0; }
void SetEnemyPosition(Enemy *enemy, Vector2 position)
{
    if (enemy)
        enemy->position = position;
}
void SetEnemyHP(Enemy *enemy, int hp)
{
    if (enemy)
        enemy->hp = hp;
}
void SetEnemySpeed(Enemy *enemy, float speed)
{
    if (enemy)
        enemy->speed = speed;
}
void SetEnemyActive(Enemy *enemy, bool active)
{
    if (enemy)
        enemy->active = active;
}
void SetEnemySpawned(Enemy *enemy, bool spawned)
{
    if (enemy)
        enemy->spawned = spawned;
}
void SetEnemyPathIndex(Enemy *enemy, int index)
{
    if (enemy)
        enemy->segment = index;
}
void SetWaveTotal(EnemyWave *wave, int total)
{
    if (wave)
        wave->total = total;
}
void SetWaveActiveCount(EnemyWave *wave, int count)
{
    if (wave)
        wave->activeCount = count;
}
void SetWaveNum(EnemyWave *wave, int num)
{
    if (wave)
        wave->waveNum = num;
}
void SetWaveTimerCurrentTime(EnemyWave *wave, float time)
{
    if (wave)
        wave->timerCurrentTime = time;
}
void SetWaveTimerDuration(EnemyWave *wave, float duration)
{
    if (wave)
        wave->timerDuration = duration;
}
void SetWaveTimerVisible(EnemyWave *wave, bool visible)
{
    if (wave)
        wave->timerVisible = visible;
}
void SetWaveActive(EnemyWave *wave, bool active)
{
    if (wave)
        wave->active = active;
}
void SetTimerMapRow(EnemyWave *wave, int row)
{
    if (wave)
        wave->timerMapRow = row;
}
void SetTimerMapCol(EnemyWave *wave, int col)
{
    if (wave)
        wave->timerMapCol = col;
}