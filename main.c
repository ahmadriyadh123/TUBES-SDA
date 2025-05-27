#include "raylib.h"
#include "raymath.h"
#include "object.h"

#include <stdbool.h>
#include <stdlib.h>

#define COLS 15
#define ROWS 10
#define MAX_PATH_POINTS 60
#define MAX_WAVES 5
#define SPAWN_DELAY 1.0f

// Peta path (1 = path, 0 = tanah)
int map[ROWS][COLS] = {
    { 0,0,0,1,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,1,0,0,0,0,0,0,0,0,0,1,1 },
    { 0,0,0,1,0,0,0,0,0,0,0,0,0,1,0 },
    { 0,0,0,1,0,0,0,0,0,0,0,0,0,1,0 },
    { 0,0,0,1,0,0,0,0,0,0,0,0,0,1,0 },
    { 0,0,0,1,1,1,1,1,1,1,1,1,1,1,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
};

int dx[4] = {1, 0, -1, 0};
int dy[4] = {0, 1, 0, -1};

void BuildPathDFS(int x, int y, bool visited[ROWS][COLS], Vector2 path[], int *count, int tileSize) {
    if (*count >= MAX_PATH_POINTS) return;
    visited[y][x] = true;
    path[*count] = (Vector2){ x * (float)tileSize + tileSize / 2.0f, y * (float)tileSize + tileSize / 2.0f };
    (*count)++;

    for (int d = 0; d < 4; d++) {
        int nx = x + dx[d];
        int ny = y + dy[d];

        if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS &&
            map[ny][nx] == 1 && !visited[ny][nx]) {
            BuildPathDFS(nx, ny, visited, path, count, tileSize);
            break;
        }
    }
}

for (int i = 0; i < count; i++) {
    Enemy e = {
        .t = 0.0f,
        .speed = 0.5f + (rand() % 3) * 0.5f,
        .segment = 0,
        .active = false,
        .spawned = false,
            .spriteType = rand() % 2
    };
    EnqueueEnemy(&wave, e);
}


void FreeWave(EnemyWave* wave) {
    if (wave->allEnemies) {
        free(wave->allEnemies);
        wave->allEnemies = NULL;
        wave->total = 0;
        wave->nextSpawnIndex = 0;
    }
}

bool AllEnemiesFinished(EnemyWave wave) {
    for (int i = 0; i < wave.total; i++) {
        if (wave.allEnemies[i].spawned && wave.allEnemies[i].active)
            return false;
    }
    return (wave.nextSpawnIndex >= wave.total);
}

int main(void) {
    InitWindow(800, 600, "Tower Defense");

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    int tileWidth = screenWidth / COLS;
    int tileHeight = screenHeight / ROWS;
    int tileSize = (tileWidth < tileHeight) ? tileWidth : tileHeight;

    Vector2 path[MAX_PATH_POINTS] = { 0 };
    int pathCount = 0;
    bool visited[ROWS][COLS] = { false };

    for (int y = 0; y < ROWS && pathCount == 0; y++) {
        for (int x = 0; x < COLS && pathCount == 0; x++) {
            if (map[y][x] == 1) {
                BuildPathDFS(x, y, visited, path, &pathCount, tileSize);
            }
        }
    }

    int currentWave = 0;
    EnemyWave wave = CreateWave(10 + currentWave * 2);  // jumlah musuh bertambah setiap wave
    AnimSprite enemy1 = LoadAnimSprite("assets/enemy1.png", 14, 5, 4, 7, 7);
    AnimSprite enemy2 = LoadAnimSprite("assets/enemy2.png", 4, 6, 0, 8, 8); // Baris ke-1 (index 0)

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateAnimSprite(&enemy1);
        UpdateAnimSprite(&enemy2);
        
        float delta = GetFrameTime();

        if (wave.nextSpawnIndex < wave.total) {
            wave.spawnTimer += delta;
            if (wave.spawnTimer >= SPAWN_DELAY) {
    DequeueEnemy(&wave);
    wave.spawnTimer = 0.0f;
}

        }

        for (int i = 0; i < wave.total; i++) {
    Enemy* e = &wave.allEnemies[i];
    if (!e->active) continue;

    int s = e->segment;
    if (s < pathCount - 1) {
        e->t += e->speed * delta; // Tambah t sesuai kecepatan dan delta time

        // Jika t lebih dari 1, pindah ke segmen berikutnya
        if (e->t >= 1.0f) {
            e->t = 0.0f;
            e->segment++;

            // Jika sudah mencapai akhir path, matikan enemy
            if (e->segment >= pathCount - 1) {
                e->active = false;
                continue;
            }
        }

        Vector2 pos = Vector2Lerp(path[e->segment], path[e->segment + 1], e->t);

        if (e->spriteType == 0) {
            DrawAnimSprite(&enemy1, pos, 2.f);
        } else {
            DrawAnimSprite(&enemy2, pos, 0.2f);
        }
    }
}

        if (AllEnemiesFinished(wave) && currentWave < MAX_WAVES - 1) {
            FreeWave(&wave);
            currentWave++;
            wave = CreateWave(3 + currentWave * 2);
        }

        // BeginDrawing();
                // BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText(TextFormat("Wave %d / %d", currentWave + 1, MAX_WAVES), 10, 10, 20, DARKBLUE);
        for (int y = 0; y < ROWS; y++) {
            for (int x = 0; x < COLS; x++) {
                Color color = (map[y][x] == 1) ? GREEN : LIGHTGRAY;
                DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, color);
                DrawRectangleLines(x * tileSize, y * tileSize, tileSize, tileSize, DARKGRAY);
            }
        }
        
        // Gambar musuh hanya sekali dengan spriteType
        for (int i = 0; i < wave.total; i++) {
            Enemy* e = &wave.allEnemies[i];
            if (!e->active) continue;

            int s = e->segment;
            if (s < pathCount - 1) {
                Vector2 pos = Vector2Lerp(path[s], path[s + 1], e->t);
                if (e->spriteType == 0) {
                    DrawAnimSprite(&enemy1, pos, 2.f);
                } else {
                    DrawAnimSprite(&enemy2, pos, 0.2f);
                }
            }
        }

        EndDrawing();

    }

    UnloadAnimSprite(&enemy1);
    UnloadAnimSprite(&enemy2);
    FreeWave(&wave);
    CloseWindow();
    return 0;
}
