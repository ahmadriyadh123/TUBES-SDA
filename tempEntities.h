#ifndef TEMP_ENTITIES_H
#define TEMP_ENTITIES_H

#include "raylib.h"
#include <stdbool.h>


#define MAX_ENEMIES 10
#define MAX_TOWERS  5

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

extern Tower towers[MAX_TOWERS];

void InitGameEntities();
void UpdateGameEntities(int screenWidth);
void DrawGameEntities();

#endif // ENTITIES_H