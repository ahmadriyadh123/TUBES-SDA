/* File        : gameplay.c
* Deskripsi   : Implementasi untuk Gameplay. 
*               Modul ini mengelola semua logika inti yang berjalan selama state gameplay, 
*               seperti update musuh, tower, gelombang, dan input pemain. 
* Dibuat oleh : Ahmad Riyadh Almaliki
* Perubahan terakhir : Minggu, 15 Juni 2025
*/

#include "gameplay.h"
#include "enemy.h"
#include "tower.h"
#include "upgrade_tree.h"
#include "main_menu.h"  
#include "level_editor.h" 
#include "status.h"
#include "common.h"

#include "player_resources.h"
#include "transition.h" 
#include "audio.h"
#include "utils.h"

//  I.S. : Menunggu input klik dari pemain.
//  F.S. : Aksi yang sesuai dengan input pemain (membangun, upgrade, menjual) telah dieksekusi.
void HandleGameplayInput(Vector2 mousePos) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        bool clickHandled = false;
        float pauseBtnSize = 100.0f;
        Rectangle pauseBtnRect = { VIRTUAL_WIDTH - pauseBtnSize - 15, 30, pauseBtnSize, pauseBtnSize };
        // Memeriksa apakah tombol pause diklik
        if (CheckCollisionPointRec(mousePos, pauseBtnRect)) {
            previousGameState = currentGameState;
            currentGameState = GAME_PAUSED;
        }
    //Mempercepat timer gelombang jika pemain mengklik timer yang terlihat 
    for (int i = 0; i < activeWavesCount; i++) {
        EnemyWave* wave = activeWaves[i];
        if (wave && wave->timerVisible) {
            Rectangle timerAreaRect = { 
                mapScreenOffsetX + GetTimerMapCol(wave) * TILE_SIZE * currentTileScale, 
                mapScreenOffsetY + GetTimerMapRow(wave) * TILE_SIZE * currentTileScale, 
                TILE_SIZE * currentTileScale, 
                TILE_SIZE * currentTileScale 
            };
            if (CheckCollisionPointRec(mousePos, timerAreaRect)) {
                
                SetWaveTimerCurrentTime(wave, GetWaveTimerDuration(wave));
                Push(&statusStack, "Wave accelerated!");
                return; 
            }
        }
    }
}