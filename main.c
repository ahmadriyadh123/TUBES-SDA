#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdbool.h>

#include "economy.h"
#include "state.h"

#define MAX_ENEMIES 10
#define MAX_TOWERS  5
#define TOWER_COST 30
#define SELL_REFUND_PERCENT 0.7f

typedef struct {
    Vector2 position;
    int hp;
    bool active;
} Enemy;

typedef struct {
    Vector2 position;
    int damage;
    float range;
    bool active;
} Tower;

Enemy enemies[MAX_ENEMIES];
Tower towers[MAX_TOWERS];

// ================= INIT ====================
void InitEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].position = (Vector2){ -i * 80.0f, 300 };
        enemies[i].hp = 1000;
        enemies[i].active = true;
    }
}

void InitTowers() {
    for (int i = 0; i < MAX_TOWERS; i++) towers[i].active = false;
}

// ================= UPDATE ====================
void UpdateEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        enemies[i].position.x += 1.0f;

        if (enemies[i].hp <= 0) {
            enemies[i].active = false;
            AddMoney(10);
        }

        if (enemies[i].position.x > 800) {
            enemies[i].active = false;
            DecreaseLife();
        }
    }
}

void CheckTowerHits() {
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (!towers[i].active) continue;

        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!enemies[j].active) continue;

            float dist = Vector2Distance(towers[i].position, enemies[j].position);
            if (dist <= towers[i].range) {
                enemies[j].hp -= towers[i].damage;
            }
        }
    }
}

void BuildTower(Vector2 pos) {
    if (GetMoney() < TOWER_COST) return;

    for (int i = 0; i < MAX_TOWERS; i++) {
        if (!towers[i].active) {
            towers[i] = (Tower){ .position = pos, .damage = 10, .range = 100, .active = true };
            SpendMoney(TOWER_COST);
            break;
        }
    }
}

void SellTower(Vector2 mouse) {
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (towers[i].active && CheckCollisionPointCircle(mouse, towers[i].position, 10)) {
            towers[i].active = false;
            AddMoney((int)(TOWER_COST * SELL_REFUND_PERCENT));
            break;
        }
    }
}

// ================= DRAW ====================
void DrawEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            DrawRectangleV(enemies[i].position, (Vector2){20, 20}, RED);
        }
    }
}

void DrawTowers() {
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (towers[i].active) {
            DrawCircleV(towers[i].position, 10, BLUE);
            DrawCircleLines(towers[i].position.x, towers[i].position.y, towers[i].range, Fade(BLUE, 0.2f));
        }
    }
}

// ================= MAIN ====================
int main() {
    InitWindow(800, 600, "Tower Defense - Modular Economy & State");
    SetTargetFPS(60);

    InitEnemies();
    InitTowers();
    InitEconomy();
    InitGameState();

    while (!WindowShouldClose()) {
        if (!IsGameOver()) {
            UpdateEnemies();
            CheckTowerHits();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 pos = GetMousePosition();
                BuildTower(pos);
            }

            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                Vector2 pos = GetMousePosition();
                SellTower(pos);
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawLine(0, 320, 800, 320, GRAY); // path
        DrawEnemies();
        DrawTowers();
        DrawMoney();
        DrawLives();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
