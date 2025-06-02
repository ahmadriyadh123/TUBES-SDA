#include "enemy.h"
#include "map.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>

#define TIMER_OVERALL_SIZE_FACTOR 0.8f
#define TIMER_IMAGE_DISPLAY_FACTOR 0.8f

static EnemyTypeProperties enemy_properties[2]; // Index 0 untuk enemy1, Index 1 untuk enemy2

static AnimSprite enemy1_anim_data = {0};
static AnimSprite enemy2_anim_data = {0};

static int dx_path[] = {0, 1, 0, -1};
static int dy_path[] = {-1, 0, 1, 0};

EnemyWave currentWave = {0};

void Enemies_InitAssets(void)
{
    enemy1_sprite = LoadAnimSprite("assets/enemy1.png", 14, 5, 4, 10, 7);
    enemy2_sprite = LoadAnimSprite("assets/enemy2.png", 4, 6, 0, 12, 4);
    TraceLog(LOG_INFO, "Enemy assets loaded.");
}

AnimSprite LoadAnimSprite(const char *filename, int cols, int rows, int rowIndex, int speed, int frameCount)
{
    AnimSprite sprite = {0};
    sprite.texture = LoadTexture(filename);
    if (sprite.texture.id == 0)
    {
        TraceLog(LOG_WARNING, "Gagal memuat tekstur: %s", filename);
    }
    sprite.frameCols = cols;
    sprite.frameRows = rows;
    sprite.animRow = rowIndex;
    sprite.frameSpeed = speed;
    sprite.frameCount = frameCount;
    sprite.currentFrame = 0;
    sprite.frameCounter = 0;
    sprite.frameWidth = sprite.texture.width / cols;
    sprite.frameHeight = sprite.texture.height / rows;
    sprite.frameRec = (Rectangle){0.0f, (float)rowIndex * sprite.frameHeight, (float)sprite.frameWidth, (float)sprite.frameHeight};
    return sprite;
}

void UpdateAnimSprite(AnimSprite *sprite)
{
    if (sprite->texture.id == 0)
        return;
    sprite->frameCounter++;
    if (sprite->frameCounter >= (GetFPS() / sprite->frameSpeed))
    {
        sprite->frameCounter = 0;
        sprite->currentFrame++;
        if (sprite->currentFrame >= sprite->frameCount)
        {
            sprite->currentFrame = 0;
        }
        sprite->frameRec.x = (float)sprite->currentFrame * sprite->frameWidth;
    }
}

void DrawAnimSprite(AnimSprite *sprite, Vector2 position, float scale, Color tint)
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
        UnloadTexture(sprite->texture);
        sprite->texture.id = 0;
    }
}

EnemyWave CreateWave(int count, int waveNum)
{
    EnemyWave wave = {0};
    if (count <= 0)
    {
        wave.total = 0;
        return wave;
    }
    wave.allEnemies = (Enemy *)malloc(sizeof(Enemy) * count);
    if (wave.allEnemies == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for enemies in wave.");
        wave.total = 0;
        return wave;
    }
    wave.total = count;
    wave.spawnedCount = 0;
    wave.activeCount = 0;
    wave.nextSpawnIndex = 0;
    wave.spawnTimer = 0.0f;

    for (int i = 0; i < count; i++)
    {
        wave.allEnemies[i].t = 0.0f;
        wave.allEnemies[i].speed = 30.0f + (waveNum * 3.0f) + (rand() % 20);
        wave.allEnemies[i].segment = 0;
        wave.allEnemies[i].active = false;
        wave.allEnemies[i].spawned = false;
        wave.allEnemies[i].spriteType = rand() % 2;
        if (pathCount > 0)
        {
            wave.allEnemies[i].position = path[0];
        }
        else
        {
            wave.allEnemies[i].position = (Vector2){0, 0};
        }
    }
    TraceLog(LOG_INFO, "Wave %d created with %d enemies.", waveNum, count);
    return wave;
}

static void BuildPathDFS_Internal(int x, int y, bool visited[MAP_ROWS][MAP_COLS],
                                  Vector2 outPath[], int *count)
{
    if (y < 0 || y >= MAP_ROWS || x < 0 || x >= MAP_COLS)
        return;

    if (visited[y][x] || !IsPathTile(y, x))
        return;
    if (*count >= MAX_PATH_POINTS)
    {
        TraceLog(LOG_WARNING, "MAX_PATH_POINTS reached in BuildPathDFS_Internal");
        return;
    }

    visited[y][x] = true;
    outPath[*count] = (Vector2){x * (float)TILE_SIZE_PX + TILE_SIZE_PX / 2.0f,
                                y * (float)TILE_SIZE_PX + TILE_SIZE_PX / 2.0f};
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

void Enemies_BuildPath(int startX, int startY)
{
    bool visited[MAP_ROWS][MAP_COLS];
    for (int r = 0; r < MAP_ROWS; r++)
    {
        for (int c = 0; c < MAP_COLS; c++)
        {
            visited[r][c] = false;
        }
    }
    pathCount = 0;

    if (startX >= 0 && startX < MAP_COLS && startY >= 0 && startY < MAP_ROWS &&
        IsPathTile(startY, startX))
    {
        BuildPathDFS_Internal(startX, startY, visited, path, &pathCount);
        TraceLog(LOG_INFO, "Enemy path built using visual.c gameMap and IsPathTile. Points: %d. Start: (%d, %d).",
                 pathCount, startX, startY);

        for (int i = 0; i < pathCount; ++i)
        {
            TraceLog(LOG_DEBUG, "Path point %d: (%.1f, %.1f)", i, path[i].x, path[i].y);
        }
    }
    else
    {
        TraceLog(LOG_ERROR, "Failed to build enemy path: Start point (%d, %d) is not a valid path tile according to IsPathTile.",
                 startX, startY);
    }
}

static bool IsPathTile(int row, int col)
{
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
    {
        int tileValue = gameMap[row][col];
        return (tileValue == 37);
    }
    return false;
}

void Enemies_ShutdownAssets(void)
{
    UnloadAnimSprite(&enemy1_sprite);
    UnloadAnimSprite(&enemy2_sprite);
    TraceLog(LOG_INFO, "Enemy assets unloaded.");
}

void FreeWave(EnemyWave *wave)
{
    if (wave && wave->allEnemies)
    {
        free(wave->allEnemies);
        wave->allEnemies = NULL;
    }
    if (wave)
    {
        *wave = (EnemyWave){0};
    }
}

bool AllEnemiesInWaveFinished(EnemyWave wave)
{
    if (wave.total == 0 && wave.spawnedCount == 0)
        return true;
    if (wave.spawnedCount < wave.total)
        return false;
    return (wave.activeCount <= 0);
}

void Enemies_BuildPath(int startX, int startY)
{
    bool visited[MAP_ROWS][MAP_COLS];
    for (int r = 0; r < MAP_ROWS; r++)
    {
        for (int c = 0; c < MAP_COLS; c++)
        {
            visited[r][c] = false;
        }
    }
    currentWave.pathCount = 0;

    TraceLog(LOG_DEBUG, "Attempting to build path from (%d, %d). Tile value: %d", startX, startY, GetMapTile(startY, startX));

    if (startX >= 0 && startX < MAP_COLS && startY >= 0 && startY < MAP_ROWS &&
        IsPathTile(startY, startX))
    {

        BuildPathDFS_Internal(startX, startY, visited, currentWave.path, &currentWave.pathCount);

        TraceLog(LOG_INFO, "Enemy path built. Points: %d. Start: (%d, %d).", currentWave.pathCount, startX, startY);

#ifdef _DEBUG
        if (currentWave.pathCount > 0)
        {
            for (int i = 0; i < currentWave.pathCount; ++i)
            {
                TraceLog(LOG_DEBUG, "Path Point %d: (X=%.1f, Y=%.1f) (Map Row: %d, Map Col: %d)",
                         i, currentWave.path[i].x, currentWave.path[i].y,
                         (int)(currentWave.path[i].y / TILE_SIZE_PX),
                         (int)(currentWave.path[i].x / TILE_SIZE_PX));
            }
        }
        else
        {
            TraceLog(LOG_ERROR, "Path not built or pathCount is 0 after DFS!");
        }
#endif
    }
    else
    {
        TraceLog(LOG_ERROR, "Failed to build enemy path: Start point (%d, %d) is not a valid path tile (value %d).",
                 startX, startY, GetMapTile(startY, startX));
        currentWave.pathCount = 0;
    }
}

void Enemies_InitAssets(void) {
    // --- INISIALISASI PROPERTI MUSUH (MENGGANTIKAN #define) ---
    enemy_properties[0] = (EnemyTypeProperties){
        .sheetWidth = 672,
        .sheetHeight = 150,
        .totalCols = 14,
        .totalRows = 5,
        .animRow = 4,
        .animSpeed = 10,
        .frameCount = 7,
        .drawScale = 0.7f,
        .texturePath = "assets/enemy1.png"
    };

    enemy_properties[1] = (EnemyTypeProperties){
        .sheetWidth = 1240,
        .sheetHeight = 1860,
        .totalCols = 4,
        .totalRows = 6,
        .animRow = 0,
        .animSpeed = 12,
        .frameCount = 4,
        .drawScale = 0.05f,
        .texturePath = "assets/enemy2.png"
    };
    // --- AKHIR INISIALISASI PROPERTI MUSUH ---

    // Menggunakan properti yang baru diinisialisasi
    enemy1_anim_data = LoadAnimSprite(
        enemy_properties[0].texturePath,
        enemy_properties[0].totalCols,
        enemy_properties[0].totalRows,
        enemy_properties[0].animRow,
        enemy_properties[0].animSpeed,
        enemy_properties[0].frameCount
    );
    enemy2_anim_data = LoadAnimSprite(
        enemy_properties[1].texturePath,
        enemy_properties[1].totalCols,
        enemy_properties[1].totalRows,
        enemy_properties[1].animRow,
        enemy_properties[1].animSpeed,
        enemy_properties[1].frameCount
    );
    TraceLog(LOG_INFO, "Enemy assets loaded.");
}

void Enemies_ShutdownAssets(void)
{
    UnloadAnimSprite(&enemy1_anim_data);
    UnloadAnimSprite(&enemy2_anim_data);
    TraceLog(LOG_INFO, "Enemy assets unloaded.");
}

void Enemies_Update(EnemyWave *wave, float delta)
{
    if (!wave || !wave->allEnemies || pathCount < 2)
        return;

    if (wave->nextSpawnIndex < wave->total)
    {
        wave->spawnTimer += delta;
        if (wave->spawnTimer >= SPAWN_DELAY)
        {

            if (wave->nextSpawnIndex < wave->total && wave->allEnemies[wave->nextSpawnIndex].spawned == false)
            {
                wave->allEnemies[wave->nextSpawnIndex].active = true;
                wave->allEnemies[wave->nextSpawnIndex].spawned = true;
                if (pathCount > 0)
                    wave->allEnemies[wave->nextSpawnIndex].position = path[0];
                wave->allEnemies[wave->nextSpawnIndex].segment = 0;
                wave->allEnemies[wave->nextSpawnIndex].t = 0.0f;
                wave->spawnedCount++;
                wave->activeCount++;
            }
            wave->nextSpawnIndex++;
            wave->spawnTimer = 0.0f;
        }
    }

    for (int i = 0; i < wave->total; i++)
    {
        Enemy *e = &wave->allEnemies[i];
        if (!e->active)
            continue;

        int s = e->segment;
        if (s < pathCount - 1)
        {
            Vector2 startPoint = path[s];
            Vector2 endPoint = path[s + 1];
            float segmentLength = Vector2Distance(startPoint, endPoint);

            if (segmentLength > 0)
            {
                e->t += (e->speed * delta) / segmentLength;
            }
            else
            {
                e->t = 1.0f;
            }

            if (e->t >= 1.0f)
            {
                e->segment++;
                s = e->segment;
                if (s >= pathCount - 1)
                {
                    e->active = false;
                    wave->activeCount--;
                }
                else
                {
                    e->t = e->t - 1.0f;

                    e->position = path[s];
                }
            }

            if (e->active && s < pathCount - 1)
            {
                e->position = Vector2Lerp(path[s], path[s + 1], e->t);
            }
            else if (e->active && s == pathCount - 1)
            {
                e->position = path[pathCount - 1];
            }
        }
        else
        {
            e->active = false;
            wave->activeCount--;
        }
    }
}

void Enemies_Draw(const EnemyWave *wave, float globalScale, float offsetX, float offsetY)
{
    if (!wave || !wave->allEnemies)
        return;

    for (int i = 0; i < wave->total; i++)
    {
        const Enemy *e = &wave->allEnemies[i];
        if (!e->active)
            continue;

        Vector2 screenPos = {
            offsetX + e->position.x * globalScale,
            offsetY + e->position.y * globalScale};

        DrawAnimSprite(&e->animData, screenPos, e->drawScale * globalScale, WHITE);

        float healthBarWidth = TILE_SIZE_PX * globalScale * 0.8f;
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

Vector2 GetEnemyPosition(const Enemy *enemy) { return enemy ? enemy->position : (Vector2){0, 0}; }
int GetEnemyHP(const Enemy *enemy) { return enemy ? enemy->hp : 0; }
float GetEnemySpeed(const Enemy *enemy) { return enemy ? enemy->speed : 0.0f; }
bool GetEnemyActive(const Enemy *enemy) { return enemy ? enemy->active : false; }
bool GetEnemySpawned(const Enemy *enemy) { return enemy ? enemy->spawned : false; }
int GetEnemyPathIndex(const Enemy *enemy) { return enemy ? enemy->segment : 0; }
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
