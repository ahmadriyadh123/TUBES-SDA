// entities.c
#include "tempEntities.h"
#include "raylib.h"
#include "raymath.h"
#include "state.h"
#include "economy.h"

Tower towers[MAX_TOWERS];
static Enemy enemies[MAX_ENEMIES];


static void InitEnemies_internal() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].position = (Vector2){ -i * 80.0f, 300 };
        enemies[i].hp = 1000;
        enemies[i].active = true;
    }
}

static void InitTowers_internal() {
    for (int i = 0; i < MAX_TOWERS; i++) {
        towers[i].active = false;
    }
}

void InitGameEntities() {
    InitEnemies_internal();
    InitTowers_internal();
}

static void UpdateEnemies_internal(int screenWidth) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        enemies[i].position.x += 1.0f;

        if (enemies[i].hp <= 0) {
            enemies[i].active = false;
            AddMoney(10); 
        }

        if (enemies[i].position.x > screenWidth) {
            enemies[i].active = false;
            DecreaseLife();
        }
    }
}

static void CheckTowerHits_internal() {
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

void UpdateGameEntities(int screenWidth) {
    UpdateEnemies_internal(screenWidth);
    CheckTowerHits_internal();
}

static void DrawEnemies_internal() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            DrawRectangleV(enemies[i].position, (Vector2){20, 20}, RED);
        }
    }
}

static void DrawTowers_internal() {
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (towers[i].active) {
            DrawCircleV(towers[i].position, 10, BLUE);
            DrawCircleLines(towers[i].position.x, towers[i].position.y, towers[i].range, Fade(BLUE, 0.2f));
        }
    }
}

void DrawGameEntities() {
    DrawEnemies_internal();
    DrawTowers_internal();
}