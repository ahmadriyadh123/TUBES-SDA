#include "object.h"
#include "raylib.h"

#include <stdlib.h>
#include <stdio.h>

AnimSprite LoadAnimSprite(const char *filename, int cols, int rows, int rowIndex, int speed, int frameCount) {
    AnimSprite sprite = { 0 };

    sprite.texture = LoadTexture(filename);
    sprite.frameCols = cols;
    sprite.frameRows = rows;
    sprite.animRow = rowIndex;
    sprite.frameSpeed = speed;
    sprite.frameCount = frameCount;

    sprite.currentFrame = 0;
    sprite.frameCounter = 0;

    sprite.frameWidth = sprite.texture.width / cols;
    sprite.frameHeight = sprite.texture.height / rows;

    sprite.frameRec = (Rectangle){
        0.0f,
        rowIndex * sprite.frameHeight,
        (float)sprite.frameWidth,
        (float)sprite.frameHeight
    };

    return sprite;
}

void UpdateAnimSprite(AnimSprite *sprite) {
    sprite->frameCounter++;
    if (sprite->frameCounter >= (60 / sprite->frameSpeed)) {
        sprite->frameCounter = 0;
        sprite->currentFrame++;
        if (sprite->currentFrame >= sprite->frameCount)  // ← batasi hanya sampai jumlah frame aktif
            sprite->currentFrame = 0;

        sprite->frameRec.x = sprite->currentFrame * sprite->frameWidth;
        sprite->frameRec.y = sprite->animRow * sprite->frameHeight;
    }
}


void DrawAnimSprite(AnimSprite *sprite, Vector2 position, float scale) {
    Rectangle destRec = {
        position.x,
        position.y,
        sprite->frameWidth * scale,
        sprite->frameHeight * scale
    };

    DrawTexturePro(
        sprite->texture,
        sprite->frameRec, // source rectangle dari texture
        destRec,          // target rectangle di layar (dengan skala)
        (Vector2){sprite->frameWidth * scale / 2, sprite->frameHeight * scale / 2},
        0.0f,             // rotasi
        WHITE
    );
}

void UnloadAnimSprite(AnimSprite *sprite) {
     UnloadTexture(sprite->texture);
}

void EnqueueEnemy(EnemyWave *wave, Enemy enemy) {
    if (wave->total >= MAX_PATH_POINTS) return; // batas antrian

    wave->allEnemies[wave->total++] = enemy;
}

Enemy *DequeueEnemy(EnemyWave *wave) {
    if (wave->nextSpawnIndex >= wave->total) return NULL;

    Enemy *e = &wave->allEnemies[wave->nextSpawnIndex++];
    e->active = true;
    e->spawned = true;
    return e;
}
