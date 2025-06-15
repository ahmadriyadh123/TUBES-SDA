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

char currentMapName[256]; 
bool gameplayInitialized = false;
static EnemyWave* activeWaves[MAX_ACTIVE_WAVES] = {0};
static int activeWavesCount = 0;
static float timeToNextWave = -1.0f;

float currentTileScale = 1.0f;
float mapScreenOffsetX = 0.0f;
float mapScreenOffsetY = 0.0f;

static GameState previousGameState;
static Texture2D moneyIconTex;
static Texture2D lifeIconTex;
static Texture2D pauseButtonTex;
static int maxWavesForCurrentLevel = -1;

Vector2 mousePos = {0};

// I.S. : Aset gameplay belum dimuat.
// F.S. : Semua tekstur, data tower, musuh, dll., telah dimuat dan siap digunakan.
void InitGameplay(void) {
    if (gameplayInitialized) return;
    TraceLog(LOG_INFO, "GAMEPLAY: Initializing assets...");

    Enemies_InitAssets();
    InitMapAssets();          
    InitTowerAssets(); 
    InitUpgradeTree(&tower1UpgradeTree, TOWER_TYPE_1);

    moneyIconTex = LoadTexture("assets/img/gameplay_imgs/coin.png");
    lifeIconTex = LoadTexture("assets/img/gameplay_imgs/heart.png");
    pauseButtonTex = LoadTexture("assets/img/gameplay_imgs/pause_button.png"); 

    gameplayInitialized = true;

    TraceLog(LOG_INFO, "Gameplay initialized. First wave created.");
}

// I.S. : Permainan mungkin sedang berjalan atau belum dimulai.
// F.S. : Semua state (uang, nyawa, tower, musuh) di-reset ke kondisi awal,
// dan permainan dimulai pada state GAMEPLAY.
void RestartGameplay(void) {
    TraceLog(LOG_INFO, "GAMEPLAY: Full restart initiated...");
    InitGameplay();

    // Bersihkan semua gelombang yang mungkin masih aktif atau ada di antrian
    for (int i = 0; i < activeWavesCount; ++i) {
        FreeWave(&activeWaves[i]);
    }
    activeWavesCount = 0;
    
    // Bersihkan semua tower yang ada di peta
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
    maxWavesForCurrentLevel = -1;

    //Menentukan peta yang akan digunakan setelah restart
    if (selectedCustomMapIndex != -1) {
        TraceLog(LOG_ERROR, "DEBUG_TRACE (Restart): Mengambil jalur 'Custom Map'.");
        const char* mapToLoad = customMaps[selectedCustomMapIndex].filePath;
        if (LoadLevelFromFile(mapToLoad)) {
            for (int r = 0; r < MAP_ROWS; r++) {
                for (int c = 0; c < MAP_COLS; c++) {
                    SetMapTile(r, c, GetEditorMapTile(r, c));
                }
            }
            maxWavesForCurrentLevel = GetEditorWaveCount();
            StrCopySafe(currentMapName, GetFileNameWithoutExt(mapToLoad), sizeof(currentMapName));
            TraceLog(LOG_INFO, "RestartGameplay: Loaded Custom Map '%s'", currentMapName);
        } else {
            TraceLog(LOG_ERROR, "DEBUG_TRACE (Restart): Mengambil jalur 'Default/Editor'.");
            StrCopySafe(currentMapName, "Default Map", sizeof(currentMapName));
        }

    } else {
        const char* editorFile = GetEditorMapFileName();
        if (editorFile && strcmp(editorFile, "maps/map.txt") != 0) {
            for (int r = 0; r < MAP_ROWS; r++) {
                for (int c = 0; c < MAP_COLS; c++) {
                    SetMapTile(r, c, GetEditorMapTile(r, c));
                }
            }
            maxWavesForCurrentLevel = GetEditorWaveCount();
            StrCopySafe(currentMapName, GetFileNameWithoutExt(editorFile), sizeof(currentMapName));
        } else {
            ResetMapToDefault();
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

        if (firstWave->pathCount == 0) {
            Push(&statusStack, "Error: Map has no valid path from start point!"); 
            TraceLog(LOG_ERROR, "GAMEPLAY: Failed to start. Map has no valid path.");
            FreeWave(&firstWave);     
            currentGameState = MAIN_MENU; 
            return;               
        }
        activeWaves[activeWavesCount++] = firstWave;
    }
    
    currentGameState = GAMEPLAY;
    selectedCustomMapIndex = -1;
    TraceLog(LOG_INFO, "GAMEPLAY: Restart complete. Starting new session on map '%s'.", currentMapName);
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
void HandleGameplayInput(Vector2 mousePos) 
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
    {
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
        //Mengelola klik pada menu orbit tower (sell/upgrade) dan navigasi di pohon upgrade
        if (selectedTowerForDeletion != NULL) {
            if (GetCurrentOrbitParentNode() != NULL) {
                clickHandled = HandleUpgradeOrbitClick(mousePos, currentTileScale);
            }
            else {
                Vector2 orbitCenter = towerSelectionUIPos;
                float orbitRadius = TILE_SIZE * currentTileScale * ORBIT_RADIUS_TILE_FACTOR;
                Rectangle deleteBtnRect = GetOrbitButtonRect(orbitCenter, orbitRadius, 0, 2, ORBIT_BUTTON_DRAW_SCALE, deleteButtonTex);
                Rectangle upgradeBtnRect = GetOrbitButtonRect(orbitCenter, orbitRadius, 1, 2, ORBIT_BUTTON_DRAW_SCALE, upgradeButtonTex);
                if (CheckCollisionPointRec(mousePos, deleteBtnRect)) {
                    SellTower(selectedTowerForDeletion);
                    HideTowerSelectionUI(); 
                    clickHandled = true;
                } else if (CheckCollisionPointRec(mousePos, upgradeBtnRect)) {
                    SetCurrentOrbitParentNode(GetUpgradeTreeRoot(&tower1UpgradeTree));
                    clickHandled = true;
                }
            }
            if (!clickHandled) {
                HideTowerSelectionUI();
            }
        }
        else {
            //Jika tidak ada UI orbit yang aktif, periksa klik pada tower yang sudah ada 
            // atau coba tempatkan tower baru di petak kosong
            Tower* clickedTower = NULL;
            Tower* currentTowerNode = towersListHead;
                while (currentTowerNode != NULL) {
                    float towerDrawWidth = currentTowerNode->frameWidth * TOWER_DRAW_SCALE * currentTileScale;
                    float towerDrawHeight = currentTowerNode->frameHeight * TOWER_DRAW_SCALE * currentTileScale;
                    Rectangle towerClickRect = {
                        mapScreenOffsetX + (currentTowerNode->position.x * currentTileScale) - (towerDrawWidth / 2.0f),
                        mapScreenOffsetY + (currentTowerNode->position.y * currentTileScale) - towerDrawHeight + (TOWER_Y_OFFSET_PIXELS * currentTileScale),
                        towerDrawWidth, towerDrawHeight
                    };
                    if (CheckCollisionPointRec(mousePos, towerClickRect)) {
                        ShowTowerOrbitUI(currentTowerNode, currentTileScale, mapScreenOffsetX, mapScreenOffsetY); 
                        clickHandled = true; 
                        return; 
                    }
                currentTowerNode = (Tower*)currentTowerNode->next;
            }
            if (clickedTower) {
                ShowTowerOrbitUI(clickedTower, currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
                clickHandled = true;
            }
        }
        // Jika tidak ada menara yang ada diklik, menempatkan yang baru
        if (!clickHandled) {
            int col = (int)((mousePos.x - mapScreenOffsetX) / (TILE_SIZE * currentTileScale));
            int row = (int)((mousePos.y - mapScreenOffsetY) / (TILE_SIZE * currentTileScale));
            if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS && GetMapTile(row, col) == 4 && GetTowerAtMapCoord(row, col) == NULL) {
                PlaceTower(row, col, TOWER_TYPE_1);
            }
        }
    }
}

// I.S. : Permainan dalam state GAME_PAUSED, menunggu input pemain.
// F.S. : Memproses klik mouse pada tombol "Resume", "Restart", atau "Main Menu".
//        currentGameState akan diubah sesuai dengan tombol yang diklik. 
void UpdatePauseMenu(void) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        float panelWidth = 300;
        float panelHeight = 280;
        float panelX = (VIRTUAL_WIDTH- panelWidth) / 2.0f;
        float panelY = (VIRTUAL_HEIGHT- panelHeight) / 2.0f;
        Rectangle resumeBtn = { panelX + 50, panelY + 50, 200, 50 };
        Rectangle restartBtn = { panelX + 50, panelY + 110, 200, 50 };
        Rectangle menuBtn = { panelX + 50, panelY + 170, 200, 50 };
        if (CheckCollisionPointRec(mousePos, resumeBtn)) {
            currentGameState = previousGameState; 
        } else if (CheckCollisionPointRec(mousePos, restartBtn)) {
            RestartGameplay();
        } else if (CheckCollisionPointRec(mousePos, menuBtn)) {
            PlayTransitionAnimation(MAIN_MENU);
            currentGameState = MAIN_MENU;
        }
    }
}

// I.S. : Permainan dalam state GAME_PAUSED.
// F.S. : Overlay gelap dan panel menu jeda dengan semua tombolnya telah
//        digambar di atas tampilan gameplay yang dijeda.
void DrawPauseMenu(){
    DrawMap(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
    DrawTowers(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
    DrawUpgradeOrbitMenu(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
    if (totalActiveEnemiesCount > 0) {
        Enemies_Draw(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
    }
    DrawShots(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
    DrawHUD(currentMapName, GetMoney(), GetLife(), mousePos);
    DrawStatus(statusStack);
    DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Fade(BLACK, 0.6f));
    float panelWidth = 300;
    float panelHeight = 280;
    float panelX = (VIRTUAL_WIDTH - panelWidth) / 2.0f;
    float panelY = (VIRTUAL_HEIGHT - panelHeight) / 2.0f;
    DrawRectangleRounded((Rectangle){panelX, panelY, panelWidth, panelHeight}, 0.1f, 10, RAYWHITE);
    DrawText("Paused", panelX + (panelWidth - MeasureText("Paused", 40)) / 2, panelY + 10, 40, BLACK);
    Rectangle resumeBtn = { panelX + 50, panelY + 70, 200, 50 };
    DrawRectangleRec(resumeBtn, LIGHTGRAY);
    DrawText("Resume", resumeBtn.x + (resumeBtn.width - MeasureText("Resume", 30)) / 2, resumeBtn.y + 10, 30, BLACK);
    Rectangle restartBtn = { panelX + 50, panelY + 130, 200, 50 };
    DrawRectangleRec(restartBtn, LIGHTGRAY);
    DrawText("Restart", restartBtn.x + (restartBtn.width - MeasureText("Restart", 30)) / 2, restartBtn.y + 10, 30, BLACK);
    Rectangle menuBtn = { panelX + 50, panelY + 190, 200, 50 };
    DrawRectangleRec(menuBtn, LIGHTGRAY);
    DrawText("Main Menu", menuBtn.x + (menuBtn.width - MeasureText("Main Menu", 30)) / 2, menuBtn.y + 10, 30, BLACK);
}

// I.S. : Elemen-elemen HUD belum digambar untuk frame ini.
// F.S. : Seluruh elemen HUD telah digambar dengan rapi di posisinya masing-masing.
void DrawHUD(const char* mapName, int money, int life, Vector2 mousePos) {  
    const char *moneyText = TextFormat("%d", GetMoney());
    const char *lifeText = TextFormat("%d", GetLife());
    int fontSize = 24;
    float padding = 10.0f;
    float spacing = 8.0f;
    float iconSize = 28.0f;
    float moneyTextWidth = MeasureText(moneyText, fontSize);
    float lifeTextWidth = MeasureText(lifeText, fontSize);
    float moneyBlockWidth = iconSize + spacing + moneyTextWidth;
    float lifeBlockWidth = iconSize + spacing + lifeTextWidth;
    float panelstatusWidth = padding + moneyBlockWidth + (padding * 2) + lifeBlockWidth + padding;
    float panelstatusHeight = iconSize + padding * 2;
    float panelstatusX = (VIRTUAL_WIDTH- panelstatusWidth) / 2.0f;
    float panelstatusY = 15.0f;
    DrawRectangleRounded(
        (Rectangle){panelstatusX, panelstatusY, panelstatusWidth, panelstatusHeight}, 
        0.3f, 10, Fade(BLACK, 0.6f)
    );
    float moneyIconX = panelstatusX + padding;
    float moneyIconY = panelstatusY + (panelstatusHeight - iconSize) / 2.0f;
    if (moneyIconTex.id > 0) {
        DrawTextureEx(moneyIconTex, (Vector2){moneyIconX, moneyIconY}, 0.0f, iconSize / moneyIconTex.height, WHITE);
    }
    DrawText(moneyText, moneyIconX + iconSize + spacing, panelstatusY + (panelstatusHeight - fontSize) / 2.0f, fontSize, WHITE);
    float lifeIconX = panelstatusX + padding + moneyBlockWidth + (padding * 2);
    float lifeIconY = panelstatusY + (panelstatusHeight - iconSize) / 2.0f;
    if (lifeIconTex.id > 0) {
        DrawTextureEx(lifeIconTex, (Vector2){lifeIconX, lifeIconY}, 0.0f, iconSize / lifeIconTex.height, WHITE);
    }
    DrawText(lifeText, lifeIconX + iconSize + spacing, panelstatusY + (panelstatusHeight - fontSize) / 2.0f, fontSize, WHITE);
    
    float mapNameFontSize = 20;
    float mapNamePadding = 10.0f;
    float mapNameWidth = MeasureText(currentMapName, mapNameFontSize) + mapNamePadding * 2;
    float mapNameHeight = mapNameFontSize + mapNamePadding * 2;
    float mapNameX = panelstatusX + (panelstatusWidth - mapNameWidth) / 2.0f; 
    float mapNameY = panelstatusY + panelstatusHeight + mapNamePadding; 
    DrawRectangleRounded(
        (Rectangle){mapNameX, mapNameY, mapNameWidth, mapNameHeight},
        0.3f, 10, Fade(BLACK, 0.6f)
    );
    DrawText(
        currentMapName,
        mapNameX + mapNamePadding,
        mapNameY + mapNamePadding,
        mapNameFontSize,
        WHITE
    );
    float pauseBtnSize = 100.0f;
    Rectangle pauseBtnRect = { VIRTUAL_WIDTH - pauseBtnSize - 15, 30, pauseBtnSize, pauseBtnSize };
    if (pauseButtonTex.id > 0) {
        Color tint = CheckCollisionPointRec(mousePos, pauseBtnRect) ? Fade(WHITE, 0.8f) : WHITE;
        DrawTextureEx(pauseButtonTex, (Vector2){pauseBtnRect.x, pauseBtnRect.y}, 0.0f, pauseBtnSize / pauseButtonTex.width, tint);
    } else {
        Color rectColor = CheckCollisionPointRec(mousePos, pauseBtnRect) ? Fade(RED, 0.8f) : RED;
        DrawRectangleRec(pauseBtnRect, rectColor);
    }
}

// I.S. : State semua entitas game siap untuk digambar.
// F.S. : Peta, musuh, tower, proyektil, dan HUD telah digambar.
void DrawGameplay(void) {
    if (!gameplayInitialized) return;
    DrawMap(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
    if (totalActiveEnemiesCount > 0)
    {
        Enemies_Draw(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
    }
    DrawTowers(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
    DrawUpgradeOrbitMenu(currentTileScale, mapScreenOffsetX, mapScreenOffsetY);
    for (int i = 0; i < activeWavesCount; i++) {
        EnemyWave* wave = activeWaves[i];
        DrawGameTimer(wave, currentTileScale, mapScreenOffsetX, mapScreenOffsetY, GetTimerMapRow(wave), GetTimerMapCol(wave));
    }
    DrawShots(currentTileScale, mapScreenOffsetX, mapScreenOffsetY); 
    DrawHUD(currentMapName, GetMoney(), GetLife(), GetMousePosition());
    DrawStatus(statusStack);
}

//Berguna untuk modul UI lain yang perlu menggambar relatif terhadap ukuran peta.
float GetCurrentTileScale(void) { return currentTileScale; }

//Mengirimkan nilai offset X layar peta saat ini.
float GetMapScreenOffsetX(void) { return mapScreenOffsetX; }

//Mengirimkan nilai offset Y layar peta saat ini.
float GetMapScreenOffsetY(void) { return mapScreenOffsetY; }

// I.S. : Aset-aset gameplay sedang berada di memori.
// F.S. : Semua aset gameplay telah dibebaskan dari memori.
void UnloadGameplay(){
    FreeWave(&currentWave);
    Enemies_ShutdownAssets();
    ShutdownTowerAssets();
    ShutdownShots();
    ShutdownMapAssets();
    FreeUpgradeTree(&tower1UpgradeTree); 
    UnloadTexture(moneyIconTex);
    UnloadTexture(lifeIconTex); 
    UnloadTexture(pauseButtonTex);
    for (int i = 0; i < activeWavesCount; ++i) {
        FreeWave(&activeWaves[i]);
    }
    activeWavesCount = 0;
    gameplayInitialized = false;
    TraceLog(LOG_INFO, "GAMEPLAY: Shutdown complete.");
}
