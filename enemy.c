#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TIMER_OVERALL_SIZE_FACTOR 0.8f
#define TIMER_IMAGE_DISPLAY_FACTOR 0.8f

static EnemyTypeProperties enemy_properties[2]; // Index 0 untuk enemy1, Index 1 untuk enemy2

static AnimSprite enemy1_anim_data = {0};
static AnimSprite enemy2_anim_data = {0};

static int dx_path[] = {0, 1, 0, -1};
static int dy_path[] = {-1, 0, 1, 0};

EnemyWave currentWave = {0};

AnimSprite LoadAnimSprite(const char *filename, int cols, int rows, int rowIndex, int speed, int frameCount)
{
    AnimSprite sprite = {0};
    sprite.texture = LoadTextureSafe(filename);
    if (sprite.texture.id == 0)
    {
        TraceLog(LOG_ERROR, "ERROR: LoadAnimSprite failed for: %s", filename);
    }
    sprite.frameCols = cols;
    sprite.frameRows = rows;
    sprite.animRow = rowIndex;
    sprite.frameSpeed = speed;
    sprite.frameCount = frameCount;
    sprite.currentFrame = 0;
    sprite.frameCounter = 0.0f;
    sprite.frameWidth = sprite.texture.width / cols;
    sprite.frameHeight = sprite.texture.height / rows;
    sprite.frameRec = (Rectangle){0.0f, (float)rowIndex * sprite.frameHeight, (float)sprite.frameWidth, (float)sprite.frameHeight};
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

void Enemies_Update(EnemyWave *wave, float deltaTime)
{

    if (!wave || !wave->allEnemies || wave->pathCount < 2)
    {
        if (wave && wave->pathCount < 2)
        {
            TraceLog(LOG_DEBUG, "[UPDATE] Wave %d: Path too short (%d points). Skipping update.", wave->waveNum, wave->pathCount);
        }
        return;
    }

    if (wave->nextSpawnIndex < wave->total)
    {
        wave->spawnTimer += deltaTime;

        if (wave->spawnTimer >= SPAWN_DELAY)
        {
            Enemy *e = &wave->allEnemies[wave->nextSpawnIndex];

            if (e->spawned == false)
            {
                e->active = true;
                e->spawned = true;
                e->position = wave->path[0];
                e->segment = 0;
                e->t = 0.0f;
                wave->spawnedCount++;
                wave->activeCount++;
                TraceLog(LOG_INFO, "[SPAWN] Enemy %d (Type %d) spawned at (%.1f, %.1f) in Wave %d! Active Count: %d", wave->nextSpawnIndex, e->spriteType, e->position.x, e->position.y, wave->waveNum, wave->activeCount);
            }

            wave->nextSpawnIndex++;
            wave->spawnTimer = 0.0f;
        }
    }

    for (int i = 0; i < wave->total; i++)
    {
        Enemy *e = &wave->allEnemies[i];

        if (!e->active || !e->spawned)
            continue;

        UpdateAnimSprite(&e->animData);

        int s = e->segment;

        if (s < wave->pathCount - 1)
        {
            Vector2 startPoint = wave->path[s];
            Vector2 endPoint = wave->path[s + 1];
            float segmentLength = Vector2Distance(startPoint, endPoint);

            if (segmentLength > 0)
            {
                e->t += (e->speed * deltaTime) / segmentLength;
            }
            else
            {
                e->t = 1.0f;
                TraceLog(LOG_WARNING, "[MOVE] Enemy %d: Segment length is 0 at seg %d! Auto advancing t.", i, s);
            }

            if (e->t >= 1.0f)
            {
                e->segment++;
                s = e->segment;
                e->t = fmod(e->t, 1.0f);

                if (s >= wave->pathCount)
                {
                    e->active = false;
                    wave->activeCount--;
                    TraceLog(LOG_INFO, "[END] Enemy %d reached end of path (seg %d / %d).", i, s, wave->pathCount);
                    DecreaseLife(1);
                }
                else
                {
                    e->position = wave->path[s];
                    TraceLog(LOG_INFO, "[MOVE] Enemy %d: Advanced to Segment %d. New pos: (%.1f, %.1f)", i, s, e->position.x, e->position.y);
                }
            }

            if (e->active && s < wave->pathCount - 1)
            {
                e->position = Vector2Lerp(wave->path[s], wave->path[s + 1], e->t);
            }
            else if (e->active && s == wave->pathCount - 1)
            {
                e->position = Vector2Lerp(wave->path[s], wave->path[s], 1.0f);
            }
        }
        else
        {
            if (e->active)
            {
                e->active = false;
                wave->activeCount--;
                TraceLog(LOG_INFO, "[END] Enemy %d finished path (beyond last segment).", i);
                DecreaseLife(1);
            }
        }

        if (e->active && e->hp <= 0)
        {
            e->active = false;
            wave->activeCount--;
            AddMoney(10 + (wave->waveNum * 2));
            TraceLog(LOG_INFO, "Enemy defeated. Money added. Current money: $%d", GetMoney());
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

void InitWaveAssets(void)
{
    TraceLog(LOG_INFO, "Wave assets initialized (no global textures loaded here, handled by CreateWave).");
}

void ShutdownWaveAssets(void)
{
    TraceLog(LOG_INFO, "Wave assets shutdown (textures unloaded by FreeWave).");
}

EnemyWave CreateWave(int startRow, int startCol)
{
    currentWave = (EnemyWave){0};
    SetWaveTotal(&currentWave, 5);

    currentWave.allEnemies = (Enemy *)malloc(sizeof(Enemy) * currentWave.total);
    if (currentWave.allEnemies == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for enemies in wave. Returning empty wave.");
        currentWave.total = 0;
        return currentWave;
    }
    SetWaveActiveCount(&currentWave, 0);
    currentWave.spawnedCount = 0;
    currentWave.nextSpawnIndex = 0;
    currentWave.spawnTimer = 0.0f;
    SetWaveNum(&currentWave, 1);
    SetWaveTimerCurrentTime(&currentWave, 0.0f);
    SetWaveTimerDuration(&currentWave, WAVE_TIMER_DURATION);
    SetWaveTimerVisible(&currentWave, true);
    SetWaveActive(&currentWave, false);
    SetTimerMapRow(&currentWave, startRow);
    SetTimerMapCol(&currentWave, startCol);

    currentWave.timerTexture = LoadTextureSafe("assets/timer.png");
    if (currentWave.timerTexture.id == 0)
    {
        TraceLog(LOG_WARNING, "Failed to load assets/timer.png for wave timer.");
    }

    Enemies_BuildPath(startCol, startRow);

    for (int i = 0; i < currentWave.total; i++)
    {
        Enemy *e = &currentWave.allEnemies[i];
        e->t = 0.0f;
        e->speed = 30.0f + (currentWave.waveNum * 3.0f) + (rand() % 20);
        e->active = false;
        e->spawned = false;
        e->segment = 0;
        e->position = (Vector2){0, 0};
        e->hp = 100 + (currentWave.waveNum * 10);
        e->spriteType = rand() % 2;
        EnemyTypeProperties *props = &enemy_properties[e->spriteType]; // Ambil properti yang sesuai

        if (e->spriteType == 0)
        {
            e->animData = enemy1_anim_data;
            e->drawScale = props->drawScale;
        }
        else
        {
            e->animData = enemy2_anim_data;
            e->drawScale = props->drawScale;
        }

        if (e->animData.texture.id == 0)
        {
            TraceLog(LOG_ERROR, "ERROR: Enemy %d (Type %d) has an invalid AnimSprite texture ID after assignment!", i, e->spriteType);
        }
        else
        {
            TraceLog(LOG_DEBUG, "Enemy %d (Type %d) AnimSprite texture ID: %d, Width: %d, Height: %d", i, e->spriteType, e->animData.texture.id, e->animData.texture.width, e->animData.texture.height);
        }
        e->animData.currentFrame = 0;
        e->animData.frameCounter = 0.0f;
    }
    TraceLog(LOG_INFO, "Wave %d created with %d enemies. Path points: %d.", currentWave.waveNum, currentWave.total, currentWave.pathCount);
    return currentWave;
}
void FreeWave(EnemyWave *wave)
{
    if (wave)
    {
        if (wave->allEnemies)
        {
            free(wave->allEnemies);
            wave->allEnemies = NULL;
        }
        if (wave->timerTexture.id != 0)
        {
            UnloadTextureSafe(&wave->timerTexture);
            wave->timerTexture = (Texture2D){0};
        }
        *wave = (EnemyWave){0};
    }
}
bool AllEnemiesInWaveFinished(const EnemyWave *wave)
{
    return (wave && wave->spawnedCount >= wave->total && wave->activeCount <= 0);
}
void UpdateWaveTimer(EnemyWave *wave, float deltaTime)
{
    if (!wave)
        return;
    if (GetWaveActive(wave))
    {
        return;
    }
    if (GetWaveTimerVisible(wave))
    {
        float timer = GetWaveTimerCurrentTime(wave) + deltaTime;
        SetWaveTimerCurrentTime(wave, timer);
        TraceLog(LOG_DEBUG, "Wave %d Timer: %.2f / %.2f", wave->waveNum, timer, GetWaveTimerDuration(wave));
        if (timer >= GetWaveTimerDuration(wave))
        {
            SetWaveTimerCurrentTime(wave, 0.0f);
            SetWaveTimerVisible(wave, false);
            SetWaveActive(wave, true);
            TraceLog(LOG_INFO, "Wave %d timer finished. Wave ACTIVATED!", wave->waveNum);
        }
    }
}
void DrawGameTimer(const EnemyWave *wave, float globalScale, float offsetX, float offsetY, int timerRow, int timerCol)
{
    if (!wave || !GetWaveTimerVisible(wave))
        return;
    float tileScreenSize = TILE_SIZE_PX * globalScale;
    float timerCenterX = offsetX + timerCol * tileScreenSize + tileScreenSize / 2.0f;
    float timerCenterY = offsetY + timerRow * tileScreenSize + tileScreenSize / 2.0f;
    Vector2 position = {timerCenterX, timerCenterY};
    float timerRadius = (TILE_SIZE_PX / 2.0f) * globalScale * TIMER_OVERALL_SIZE_FACTOR;
    float progress = GetWaveTimerCurrentTime(wave) / GetWaveTimerDuration(wave);
    if (progress < 0.0f)
        progress = 0.0f;
    if (progress > 1.0f)
        progress = 1.0f;
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