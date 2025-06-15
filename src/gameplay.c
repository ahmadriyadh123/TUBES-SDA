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

// I.S. : Aset gameplay belum dimuat.
// F.S. : Semua tekstur, data tower, musuh, dll., telah dimuat dan siap digunakan.
void InitGameplay(void) {
    Enemies_InitAssets();
    InitMapAssets();          
    InitTowerAssets(); 
    InitUpgradeTree(&tower1UpgradeTree, TOWER_TYPE_1);
    moneyIconTex = LoadTexture("assets/img/gameplay_imgs/coin.png");
    lifeIconTex = LoadTexture("assets/img/gameplay_imgs/heart.png");
    pauseButtonTex = LoadTexture("assets/img/gameplay_imgs/pause_button.png"); 
    mousePos = GetMousePosition();
    
    for(int i = 0; i < activeWavesCount; ++i) { FreeWave(&activeWaves[i]); }
    activeWavesCount = 0;  
    timeToNextWave = -1.0f;
    currentWaveNum = 1;
    
    if (selectedCustomMapIndex != -1) {
        const char* mapToLoad = customMaps[selectedCustomMapIndex].filePath;
        if (LoadLevelFromFile(mapToLoad)) {
            StrCopySafe(currentMapName, GetFileNameWithoutExt(mapToLoad), sizeof(currentMapName));
            TraceLog(LOG_INFO, "Gameplay initialized from Custom Map: %s", currentMapName);
        } else {
            StrCopySafe(currentMapName, "Default Map", sizeof(currentMapName));
            TraceLog(LOG_WARNING, "Failed to load custom map, using Default Map");
            currentGameState = MAIN_MENU;
            return;
        }
    } else {
        const char* editorFile = GetEditorMapFileName(); 
        if (editorFile && strcmp(editorFile, "maps/map.txt") != 0) {
            StrCopySafe(currentMapName, GetFileNameWithoutExt(editorFile), sizeof(currentMapName));
            TraceLog(LOG_INFO, "Gameplay started from Level Editor save: %s", currentMapName);
        } else {
            
            SetEditorStartRow(-1); SetEditorStartCol(-1);
            StrCopySafe(currentMapName, "Default Map", sizeof(currentMapName));
            TraceLog(LOG_INFO, "Gameplay started with Default Map.");
        }
    }
        if (strlen(currentMapName) == 0) {
            StrCopySafe(currentMapName, "Default Map", sizeof(currentMapName));
            TraceLog(LOG_WARNING, "currentMapName was empty, set to Default Map");
        }
    int startRow = GetEditorStartRow();
    int startCol = GetEditorStartCol();
    if (startRow == -1 || startCol == -1) { startRow = 0; startCol = 4; }
    
    EnemyWave* firstWave = CreateWave(startRow, startCol); 
    if (firstWave) {
        activeWaves[activeWavesCount++] = firstWave;
    }
    SetMoney(200); 
    SetLife(10);   
    gameplayInitialized = true;
    selectedCustomMapIndex = -1; 
    TraceLog(LOG_INFO, "Gameplay initialized. First wave created.");
}

// I.S. : Permainan mungkin sedang berjalan atau belum dimulai.
// F.S. : Semua state (uang, nyawa, tower, musuh) di-reset ke kondisi awal,
// dan permainan dimulai pada state GAMEPLAY.
void RestartGameplay(void) {
    TraceLog(LOG_INFO, "GAMEPLAY: Starting/Restarting session...");
    InitGameplay();
    
    // Pembersihan Entitas Game (untuk restart)
    for (int i = 0; i < activeWavesCount; ++i) { FreeWave(&activeWaves[i]); }
    activeWavesCount = 0;
    
    Tower *currentTower = towersListHead;
    while (currentTower != NULL) {
        Tower *next = (Tower *)currentTower->next;
        RemoveTower(currentTower);
        currentTower = next;
    }
    towersListHead = NULL;
    
    if (allActiveEnemies != NULL) {
        for (int i = 0; i < maxTotalActiveEnemies; i++) {
            allActiveEnemies[i].active = false;
        }
    }
    totalActiveEnemiesCount = 0;
    
    HideTowerSelectionUI();
    ResetUpgradeOrbit();
    CreateStatus(&statusStack);
    SetMoney(200);
    SetLife(10);
    currentWaveNum = 1;
    timeToNextWave = -1.0f;
    
    //Menentukan peta yang akan digunakan setelah restart
    if (selectedCustomMapIndex != -1) {
        
        const char* mapToLoad = customMaps[selectedCustomMapIndex].filePath;
        LoadLevelFromFile(mapToLoad);
        StrCopySafe(currentMapName, GetFileNameWithoutExt(mapToLoad), sizeof(currentMapName));
    } else {
        
        const char* editorFile = GetEditorMapFileName();
        if (editorFile && strcmp(editorFile, "maps/map.txt") != 0 && strlen(editorFile) > 0) {
            StrCopySafe(currentMapName, GetFileNameWithoutExt(editorFile), sizeof(currentMapName));
        } else {
            StrCopySafe(currentMapName, "Default Map", sizeof(currentMapName));
        }
    }
    TraceLog(LOG_INFO, "RestartGameplay: Map name set to '%s'", currentMapName);
    
    int startRow = GetEditorStartRow();
    int startCol = GetEditorStartCol();
    if (startRow == -1 || startCol == -1) { startRow = 0; startCol = 4; } 

    // Membuat objek gelombang musuh pertama dan menambahkannya ke daftar gelombang aktif.
    EnemyWave* firstWave = CreateWave(startRow, startCol);
    if (firstWave) {
        activeWaves[activeWavesCount++] = firstWave;
    }
    currentGameState = GAMEPLAY;
}

// I.S. : State semua entitas game (musuh, tower, wave) pada frame sebelumnya.
// F.S. : State semua entitas game telah diperbarui.
void UpdateGameplay(float deltaTime) {
    if (!gameplayInitialized) InitGameplay();
    if (currentGameState == MAIN_MENU) return;
    
    float screenWidth = (float)VIRTUAL_WIDTH;
    float screenHeight = (float)VIRTUAL_HEIGHT;
    float baseMapWidth = MAP_COLS * TILE_SIZE;
    float baseMapHeight = MAP_ROWS * TILE_SIZE;
    currentTileScale = fmin((float)VIRTUAL_WIDTH / baseMapWidth, (float)VIRTUAL_HEIGHT / baseMapHeight);
    mapScreenOffsetX = (screenWidth - baseMapWidth * currentTileScale) / 2.0f;
    mapScreenOffsetY = (screenHeight - baseMapHeight * currentTileScale) / 2.0f;
    UpdateStatus(&statusStack, deltaTime);
    
    // Memperbarui timer gelombang memicu spawning musuh jika gelombang aktif dan siap.
    for (int i = 0; i < activeWavesCount; i++) {
        EnemyWave* wave = activeWaves[i];
        if (!wave) continue;
        bool waveJustActivated = UpdateWaveTimer(wave, deltaTime);
        if (waveJustActivated) {
            if(timeToNextWave < 0) {
                timeToNextWave = WAVE_INTERVAL;
            }
        }
        if (wave->active && wave->nextSpawnIndex < wave->enemiesToSpawnInThisWave) {
            wave->spawnTimer += deltaTime;
            if (wave->spawnTimer >= SPAWN_DELAY) {
                wave->spawnTimer = 0.0f;
                Enemy newEnemy;
                if (!IsEnemyQueueEmpty(&wave->enemyQueue)) {
                    DequeueEnemy(&wave->enemyQueue, &newEnemy);
                    int slot = -1;
                    for(int k=0; k<maxTotalActiveEnemies; ++k) { if(!allActiveEnemies[k].active) { slot = k; break; } }
                    if(slot != -1) {
                        allActiveEnemies[slot] = newEnemy;
                        allActiveEnemies[slot].active = true;
                        allActiveEnemies[slot].spawned = true;
                        allActiveEnemies[slot].position = wave->path[0];
                        totalActiveEnemiesCount++;
                        wave->spawnedCount++;
                    } else {
                        EnqueueEnemy(&wave->enemyQueue, newEnemy);
                    }
                }
                wave->nextSpawnIndex++;
            }
        }
    }
    Enemies_Update(deltaTime);
    UpdateTowerAttacks(NULL, deltaTime);
    UpdateShots(deltaTime);

    //Menangani hitung mundur antar gelombang dan memicu gelombang berikutnya atau 
    //mengakhiri permainan jika semua gelombang selesai atau nyawa pemain habis
    if (timeToNextWave > 0) {
        timeToNextWave -= deltaTime;
        if (timeToNextWave <= 0) {
        
            bool isCustomGame = (selectedCustomMapIndex != -1) || (strcmp(GetEditorMapFileName(), "maps/map.txt") != 0);
            if (!isCustomGame || (isCustomGame && currentWaveNum < customWaveCount)) {
                if (activeWavesCount < MAX_ACTIVE_WAVES) {
                    currentWaveNum++;
                    TraceLog(LOG_INFO, "WAVE_INTERVAL finished. Creating Wave %d.", currentWaveNum);
                    
                    int startRow = GetEditorStartRow();
                    int startCol = GetEditorStartCol();
                    if (startRow == -1 || startCol == -1) { startRow = 0; startCol = 2; }
                    EnemyWave* nextWave = CreateWave(startRow, startCol);
                    if (nextWave) activeWaves[activeWavesCount++] = nextWave;
                }
            } 
            if (isCustomGame && currentWaveNum >= customWaveCount && activeWavesCount == 0 && totalActiveEnemiesCount == 0) {
                TraceLog(LOG_INFO, "All custom waves completed! Triggering LEVEL_COMPLETE state.");
                currentGameState = LEVEL_COMPLETE;
            }
            else {  
                if (!isCustomGame || (isCustomGame && currentWaveNum < customWaveCount)) {
                    if (activeWavesCount < MAX_ACTIVE_WAVES) {
                        currentWaveNum++;
                        TraceLog(LOG_INFO, "WAVE_INTERVAL finished. Creating Wave %d.", currentWaveNum);
                        
                        int startRow = GetEditorStartRow();
                        int startCol = GetEditorStartCol();
                        if (startRow == -1 || startCol == -1) { startRow = 0; startCol = 2; }
                        EnemyWave* nextWave = CreateWave(startRow, startCol);
                        if (nextWave) activeWaves[activeWavesCount++] = nextWave;
                    }
                }
            }
            timeToNextWave = -1.0f;
        }
    }
    for (int i = activeWavesCount - 1; i >= 0; i--) {
        if (AllEnemiesInWaveFinished(activeWaves[i])) {
            FreeWave(&activeWaves[i]);
            activeWaves[i] = activeWaves[activeWavesCount - 1];
            activeWaves[activeWavesCount - 1] = NULL;
            activeWavesCount--;
        }
    }
    if (IsGameOver()) {
        PlayTransitionAnimation(GAME_OVER);
        currentGameState = GAME_OVER;
        gameplayInitialized = false; 
    }
}

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