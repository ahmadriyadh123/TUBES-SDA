#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>

#define TILE_SIZE 64
#define MAP_WIDTH 10
#define MAP_HEIGHT 7
#define MAX_PATH_POINTS 64
#define MAX_ENEMIES 3

int map[MAP_HEIGHT][MAP_WIDTH] = {
    {0,0,0,0,0,0,0,0,0,0},
    {1,1,1,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,1,0,0,0},
    {0,0,0,0,0,0,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0}
};

// Arah: kanan, bawah, kiri, atas
int dx[4] = {1, 0, -1, 0};
int dy[4] = {0, 1, 0, -1};

// Path dari tile hijau dengan DFS
void BuildPathDFS(int x, int y, bool visited[MAP_HEIGHT][MAP_WIDTH], Vector2 path[], int *count) {
    if (*count >= MAX_PATH_POINTS) return;
    visited[y][x] = true;
    path[*count] = (Vector2){ x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2 };
    (*count)++;

    for (int d = 0; d < 4; d++) {
        int nx = x + dx[d];
        int ny = y + dy[d];

        if (nx >= 0 && nx < MAP_WIDTH && ny >= 0 && ny < MAP_HEIGHT &&
            map[ny][nx] == 1 && !visited[ny][nx]) {
            BuildPathDFS(nx, ny, visited, path, count);
            break; // hanya satu cabang
        }
    }
}

typedef struct Enemy {
    float t;
    float speed;
    int segment;
    bool active;
} Enemy;

int main(void) {
    const int screenWidth = MAP_WIDTH * TILE_SIZE;
    const int screenHeight = MAP_HEIGHT * TILE_SIZE;

    InitWindow(screenWidth, screenHeight, "Raylib - Musuh Bergerak Linear di Tile Jalan");

    // Bangun path
    Vector2 path[MAX_PATH_POINTS] = { 0 };
    int pathCount = 0;
    bool visited[MAP_HEIGHT][MAP_WIDTH] = { false };

    // Mulai dari tile hijau pertama
    for (int y = 0; y < MAP_HEIGHT && pathCount == 0; y++) {
        for (int x = 0; x < MAP_WIDTH && pathCount == 0; x++) {
            if (map[y][x] == 1) {
                BuildPathDFS(x, y, visited, path, &pathCount);
            }
        }
    }

    // Inisialisasi musuh
    Enemy enemies[MAX_ENEMIES];
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].t = 0.0f;
        enemies[i].speed = 0.5f + i * 0.1f;
        enemies[i].segment = 0;
        enemies[i].active = true;
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Musuh mengikuti tile hijau secara linear", 10, 10, 20, DARKGRAY);

        // Gambar tile map
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                Color color = (map[y][x] == 1) ? GREEN : LIGHTGRAY;
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, color);
                DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKGRAY);
            }
        }

        // Update dan gambar musuh
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].active) continue;

            enemies[i].t += enemies[i].speed * delta;
            if (enemies[i].t > 1.0f) {
                enemies[i].t = 0.0f;
                enemies[i].segment++;
                if (enemies[i].segment >= pathCount - 1) {
                    enemies[i].active = false;
                    continue;
                }
            }

            int s = enemies[i].segment;
            Vector2 pos = Vector2Lerp(path[s], path[s + 1], enemies[i].t);
            DrawCircleV(pos, 10, RED);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
