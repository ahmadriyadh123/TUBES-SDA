/* File       : tower.c 
* Deskripsi   : Implementasi dari semua fungsi yang berkaitan dengan ADT Tower. 
*               Berisi logika untuk menempatkan, meng-update serangan, menggambar, 
*               dan mengelola tower. 
* Dibuat oleh : Ahmad Riyadh Almaliki
* Tanggal Perubahan : Sabtu, 14 Juni 2025
*/

#include "tower.h"
#include "raylib.h"
#include "utils.h"
#include "raymath.h"
#include "map.h"
#include "enemy.h"
#include "upgrade_tree.h"
#include "player_resources.h"
#include "status.h"
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

Tower *towersListHead = NULL;
Tower *selectedTowerForDeletion = NULL;
Vector2 deleteButtonScreenPos = {0, 0};
Vector2 towerSelectionUIPos = {0, 0}; 
bool isTowerSelectionUIVisible = false;
bool deleteButtonVisible = false;
Texture2D deleteButtonTex = {0};
Texture2D upgradeButtonTex = {0};
float DELETE_BUTTON_DRAW_SCALE = ORBIT_BUTTON_DRAW_SCALE; 
float UPGRADE_BUTTON_DRAW_SCALE = ORBIT_BUTTON_DRAW_SCALE;

static Texture2D towerTexture = {0};
static Shot shots[MAX_VISUAL_SHOTS]; 

/* I.S. : Aset-aset untuk tower (seperti tekstur sprite, tombol UI) belum dimuat.
   F.S. : Semua aset yang diperlukan oleh modul Tower telah dimuat ke memori. */
void InitShots(void) {
    for (int i = 0; i < MAX_VISUAL_SHOTS; i++) {
        shots[i].active = false;
    }
    TraceLog(LOG_INFO, "SHOTS: Visual shot system initialized.");
}

/* I.S. : Sistem visual tembakan (shots) belum siap digunakan.
   F.S. : Array internal untuk menampung efek visual telah diinisialisasi dan siap digunakan. */
void InitTowerAssets()
{
    towerTexture = LoadTextureSafe("assets/img/gameplay_imgs/tower1.png");
    deleteButtonTex = LoadTextureSafe("assets/img/gameplay_imgs/delete_button.png");
    upgradeButtonTex = LoadTextureSafe("assets/img/gameplay_imgs/upgrade_button.png");
    TraceLog(LOG_INFO, "Tower assets initialized.");
}

/* I.S. : Aset-aset tower sedang digunakan.
   F.S. : Semua aset yang digunakan oleh modul Tower telah dihapus dari memori (unload).
          Semua tower yang tersisa di-dealokasi. */
void ShutdownTowerAssets()
{
    UnloadTextureSafe(&towerTexture);
    UnloadTextureSafe(&deleteButtonTex);
    UnloadTextureSafe(&upgradeButtonTex);

    Tower *current = towersListHead;
    while (current != NULL)
    {
        Tower *next = (Tower *)current->next;
        free(current);
        current = next;
    }
    towersListHead = NULL;
    HideTowerOrbitUI();
    TraceLog(LOG_INFO, "Tower assets shutdown.");
}   

/* I.S. : Sistem visual tembakan sedang berjalan.
   F.S. : Tidak ada tindakan spesifik di implementasi saat ini, namun disiapkan untuk dealokasi di masa depan. */
void ShutdownShots(void) {
    TraceLog(LOG_INFO, "SHOTS: Visual shot system shutdown.");
}

/* I.S. : 'tower' adalah tower valid yang diklik oleh pemain. UI Orbit sedang tersembunyi.
   F.S. : Variabel global untuk UI seleksi diaktifkan ('isTowerSelectionUIVisible' = true).
          'selectedTowerForDeletion' diatur ke 'tower'. Posisi UI dihitung dan diatur. */
void ShowTowerOrbitUI(Tower *tower, float currentTileScale, float mapScreenOffsetX, float mapScreenOffsetY)
{
    selectedTowerForDeletion = tower; 

    
    Vector2 towerCenterPos = {
        mapScreenOffsetX + (tower->position.x * currentTileScale),
        mapScreenOffsetY + (tower->position.y * currentTileScale)};
    towerSelectionUIPos = towerCenterPos;
    isTowerSelectionUIVisible = true;
    deleteButtonVisible = true; 

    ResetUpgradeOrbit(); 

    TraceLog(LOG_INFO, "Displaying tower orbit UI for tower at (%d, %d).", tower->row, tower->col);
}


/* I.S. : UI Orbit mungkin sedang ditampilkan.
   F.S. : Variabel global untuk UI seleksi dinonaktifkan dan di-reset. */
void HideTowerOrbitUI(void)
{
    selectedTowerForDeletion = NULL;
    isTowerSelectionUIVisible = false;
    deleteButtonVisible = false; 
    ResetUpgradeOrbit(); 
    TraceLog(LOG_INFO, "Tower orbit UI hidden.");
}

/* Mengirimkan true jika UI orbit (seleksi tower) sedang ditampilkan. */
bool IsTowerOrbitUIVisible(void)
{
    return isTowerSelectionUIVisible;
}

/* I.S. : UI seleksi tower (menu orbit) mungkin sedang ditampilkan.
   F.S. : Semua variabel state yang terkait dengan UI seleksi di-reset, sehingga UI menjadi tersembunyi. */
void HideTowerSelectionUI(void) { 
    selectedTowerForDeletion = NULL;
    deleteButtonVisible = false;
    isTowerSelectionUIVisible = false; 
    TraceLog(LOG_INFO, "Tower selection UI hidden.");
}

/* Mengirimkan true jika UI seleksi tower (menu orbit) sedang ditampilkan. */
bool IsTowerSelectionUIVisible(void) {
    return deleteButtonVisible;
}

/* I.S. : Diberikan semua parameter untuk sebuah tombol di lingkaran orbit.
   F.S. : Mengembalikan sebuah Rectangle yang merepresentasikan posisi dan ukuran tombol tersebut di layar. */
Rectangle GetOrbitButtonRect(Vector2 orbitCenter, float orbitRadius, int buttonIndex, int totalButtons, float buttonScale, Texture2D buttonTexture) {
    float angleStep = 360.0f / totalButtons;
    float currentAngle = (float)buttonIndex * angleStep - 45.0f; 

    Vector2 buttonPosInOrbit = {
        orbitCenter.x + orbitRadius * cosf(DEG2RAD * currentAngle),
        orbitCenter.y + orbitRadius * sinf(DEG2RAD * currentAngle)
    };

    float visualButtonSize = 57.0f; 
    float btnDrawSize = visualButtonSize * buttonScale; 

    

    return (Rectangle){
        buttonPosInOrbit.x - btnDrawSize / 2.0f,
        buttonPosInOrbit.y - btnDrawSize / 2.0f,
        btnDrawSize,
        btnDrawSize
    };
}

/* I.S. : Diberikan posisi mouse dan semua parameter sebuah tombol orbit.
   F.S. : Mengembalikan true jika 'mousePos' berada di dalam area Rectangle tombol tersebut. */
bool CheckOrbitButtonClick(Vector2 mousePos, Vector2 orbitCenter, float orbitRadius, int buttonIndex, int totalButtons, float buttonScale, Texture2D buttonTexture) {
    Rectangle rect = GetOrbitButtonRect(orbitCenter, orbitRadius, buttonIndex, totalButtons, buttonScale, buttonTexture);
    return CheckCollisionPointRec(mousePos, rect);
}

/* I.S. : Petak di (row, col) adalah petak yang valid dan kosong. Pemain memiliki cukup uang.
   F.S. : Sebuah tower baru dibuat dan ditambahkan ke daftar tower pada posisi tersebut.
          Uang pemain berkurang, dan tile di peta diperbarui. */
void PlaceTower(int row, int col, TowerType type)
{

    if (GetTowerAtMapCoord(row, col) != NULL)
    {
        TraceLog(LOG_WARNING, "Cannot place tower: Already a tower at (%d, %d).", row, col);
        return;
    }
    if (GetMoney() < 50)
    {
        TraceLog(LOG_WARNING, "Cannot place tower: Insufficient money.");
        return;
    }
    if (GetMapTile(row, col) != 4)
    {
        TraceLog(LOG_WARNING, "Cannot place tower: Invalid tile type %d at (%d, %d).", GetMapTile(row, col), row, col);
        return;
    }

    Tower *newTower = (Tower *)malloc(sizeof(Tower));
    if (newTower == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to allocate memory for new tower.");
        return;
    }

    *newTower = (Tower){0};
    SetTowerPosition(newTower, (Vector2){col * TILE_SIZE + TILE_SIZE / 2.0f, row * TILE_SIZE + TILE_SIZE / 2.0f});
    SetTowerType(newTower, type);
    SetTowerDamage(newTower, 25);
    SetTowerRange(newTower, 100.0f);
    SetTowerAttackSpeed(newTower, 1.0f);
    SetTowerAttackCooldown(newTower, 0.3f);
    SetTowerActive(newTower, true);
    newTower->texture = towerTexture;
    newTower->frameWidth = TOWER_FRAME_WIDTH;
    newTower->frameHeight = TOWER_FRAME_HEIGHT;
    newTower->currentFrame = 0;
    newTower->frameTimer = 0.0f;
    newTower->row = row;
    newTower->col = col;
    newTower->next = NULL;

    newTower->next = (struct Tower *)towersListHead;
    towersListHead = newTower;

    AddMoney(-50);
    SetMapTile(row, col, 7);
    Push(&statusStack, "Tower placed successfully."); 
    TraceLog(LOG_INFO, "Tower placed at (%d, %d). Money: $%d.", row, col, GetMoney());
    HideTowerOrbitUI();
}

/* I.S. : 'towerToRemove' adalah pointer valid ke tower yang ada dalam daftar.
   F.S. : Tower tersebut dihapus dari daftar, memorinya dibebaskan, dan tile di peta dikembalikan ke semula. */
void RemoveTower(Tower *towerToRemove)
{
    if (towerToRemove == NULL)
        return;

    Tower *current = towersListHead;
    Tower *prev = NULL;

    while (current != NULL && current != towerToRemove)
    {
        prev = current;
        current = (Tower *)current->next;
    }

    if (current == NULL)
    {
        TraceLog(LOG_WARNING, "Attempted to remove a tower not found in the list.");
        return;
    }

    if (prev == NULL)
    {
        towersListHead = (Tower *)current->next;
    }
    else
    {
        prev->next = (struct Tower *)current->next;
    }

    SetMapTile(current->row, current->col, 4);

    free(current);
    Push(&statusStack, "Tower sold for +25 gold."); 
    TraceLog(LOG_INFO, "Tower removed from map at (%d, %d).", towerToRemove->row, towerToRemove->col);
    HideTowerOrbitUI();
}

/* I.S. : 'towerToSell' adalah tower valid yang dipilih oleh pemain.
   F.S. : Uang pemain bertambah sesuai harga jual, dan RemoveTower(towerToSell) dipanggil. */
void SellTower(Tower *towerToSell)
{
    if (towerToSell == NULL)
        return;

    AddMoney(25);
    TraceLog(LOG_INFO, "Tower at (%d, %d) sold. Money: $%d.", towerToSell->row, towerToSell->col, GetMoney());

    RemoveTower(towerToSell);
}

/* I.S. : Aksi dalam game (misalnya serangan tower) terjadi.
   F.S. : Sebuah efek visual proyektil baru yang bergerak dari 'startPos' ke 'endPos' dibuat dan diaktifkan. */
void SpawnProjectile(Vector2 startPos, Vector2 endPos, Color color, float radius, float travelTime) {
    for (int i = 0; i < MAX_VISUAL_SHOTS; i++) {
        if (!shots[i].active) {
            
            shots[i] = (Shot){0};
            
            
            shots[i].type = SHOT_TYPE_PROJECTILE;
            shots[i].active = true;
            shots[i].startPos = startPos;
            shots[i].endPos = endPos;
            shots[i].color = color;
            shots[i].radius = radius;
            shots[i].travelTime = fmax(travelTime, 0.01f);
            shots[i].currentTravelTime = 0.0f; 
            
            TraceLog(LOG_DEBUG, "Spawned PROJECTILE in slot %d.", i);
            return;
        }
    }
    TraceLog(LOG_WARNING, "FAILED to spawn projectile. No available slots.");
}

/* I.S. : Sebuah proyektil mengenai target atau sebuah serangan area terjadi.
   F.S. : Sebuah efek visual tumbukan (impact) dengan tipe tertentu ('type') dibuat dan diaktifkan di 'position'. */
void SpawnImpactEffect(Vector2 position, ShotType type, Color color) {
    for (int i = 0; i < MAX_VISUAL_SHOTS; i++) {
        if (!shots[i].active) {
            shots[i] = (Shot){0};
            shots[i].type = type;
            shots[i].active = true;
            shots[i].startPos = position;
            shots[i].color = color;
            shots[i].impactTimer = 0.0f;
            
            if (type == SHOT_TYPE_AOE_BLAST) {
                shots[i].impactDuration = 0.4f;
                shots[i].impactMaxSize = 60.0f;
            } else if (type == SHOT_TYPE_CRIT_SHATTER) {
                shots[i].impactDuration = 0.4f;
                shots[i].impactMaxSize = 25.0f;
            } else if (type == SHOT_TYPE_NORMAL_IMPACT) {
                shots[i].impactDuration = 0.2f;
                shots[i].impactMaxSize = 10.0f;
            }
            return;
        }
    }
}

/* I.S. : Posisi dan durasi semua efek visual pada frame sebelumnya.
   F.S. : Posisi proyektil diperbarui, dan durasi animasi untuk efek tumbukan dikurangi berdasarkan 'deltaTime'.*/
void UpdateShots(float deltaTime) {
    for (int i = 0; i < MAX_VISUAL_SHOTS; i++) {
        if (!shots[i].active) continue;

        
        if (shots[i].type == SHOT_TYPE_PROJECTILE) {
            shots[i].currentTravelTime += deltaTime;

            
            if (shots[i].currentTravelTime >= shots[i].travelTime) {
                
                
                shots[i].type = SHOT_TYPE_NORMAL_IMPACT;
                shots[i].startPos = shots[i].endPos; 
                shots[i].impactTimer = 0.0f;         
                shots[i].impactDuration = 0.2f;
                shots[i].impactMaxSize = 10.0f;
            }
        } 
        
        else {
            shots[i].impactTimer += deltaTime;
            if (shots[i].impactTimer >= shots[i].impactDuration) {
                shots[i].active = false; 
            }
        }
    }
}

/* I.S. : Terdapat efek visual (shots) yang aktif.
   F.S. : Semua efek visual yang 'active' digambar ke layar sesuai dengan tipe dan propertinya. */
void DrawShots(float globalScale, float offsetX, float offsetY) {
    for (int i = 0; i < MAX_VISUAL_SHOTS; i++) {
        if (shots[i].active) {
            switch (shots[i].type) {
                case SHOT_TYPE_PROJECTILE: {
                    float progress = shots[i].currentTravelTime / shots[i].travelTime;
                    Vector2 currentPos = Vector2Lerp(shots[i].startPos, shots[i].endPos, progress);
                    Vector2 screenPos = { offsetX + currentPos.x * globalScale, offsetY + currentPos.y * globalScale };
                    DrawCircleV(screenPos, shots[i].radius * globalScale, shots[i].color);
                } break;

                case SHOT_TYPE_AOE_BLAST: {
                    float progress = shots[i].impactTimer / shots[i].impactDuration;
                    float currentRadius = Lerp(0, shots[i].impactMaxSize, progress);
                    Color currentColor = Fade(shots[i].color, 1.0f - progress);
                    Vector2 screenPos = { offsetX + shots[i].startPos.x * globalScale, offsetY + shots[i].startPos.y * globalScale };
                    DrawCircleGradient(screenPos.x, screenPos.y, currentRadius * globalScale, currentColor, BLANK);
                } break;
                case SHOT_TYPE_NORMAL_IMPACT: { 
                    float progress = shots[i].impactTimer / shots[i].impactDuration;
                    float currentRadius = Lerp(0, shots[i].impactMaxSize, progress);
                    Color currentColor = Fade(shots[i].color, 1.0f - progress);
                    Vector2 screenPos = { offsetX + shots[i].startPos.x * globalScale, offsetY + shots[i].startPos.y * globalScale };
                    DrawCircleGradient(screenPos.x, screenPos.y, currentRadius * globalScale, currentColor, BLANK);
                } break;
                

                case SHOT_TYPE_CRIT_SHATTER: {
                    float progress = shots[i].impactTimer / shots[i].impactDuration;
                    Color currentColor = Fade(shots[i].color, 1.0f - progress);
                    Vector2 screenPos = { offsetX + shots[i].startPos.x * globalScale, offsetY + shots[i].startPos.y * globalScale };
                    for (int j = 0; j < 6; j++) { 
                        Vector2 endPos = { 
                            screenPos.x + cosf(DEG2RAD * (j * 60)) * (progress * shots[i].impactMaxSize * globalScale),
                            screenPos.y + sinf(DEG2RAD * (j * 60)) * (progress * shots[i].impactMaxSize * globalScale)
                        };
                        DrawLineEx(screenPos, endPos, 2.0f, currentColor);
                    }                
                } break;
            }
        }
    }
}

/* I.S. : Keadaan tower dan musuh pada frame sebelumnya.
   F.S. : Setiap tower yang aktif dan tidak dalam masa cooldown akan mencari target. Jika target
          ditemukan dalam jangkauan, tower akan menyerang dan me-reset cooldown-nya. */
void UpdateTowerAttacks(EnemyWave *wave, float deltaTime)
{
    
    if (!towersListHead || !allActiveEnemies)
        return;

    Tower *current = towersListHead;
    while (current != NULL)
    {
        
        float currentCooldown = GetTowerAttackCooldown(current);
        if (currentCooldown > 0) {
            SetTowerAttackCooldown(current, currentCooldown - deltaTime);
            current = (Tower *)current->next;
            continue;
        }

        
        for (int j = 0; j < maxTotalActiveEnemies; j++) {
            Enemy *mainTarget = &allActiveEnemies[j];

            if (!mainTarget->active) continue;

            float distance = Vector2Distance(GetTowerPosition(current), GetEnemyPosition(mainTarget));

            
            if (distance <= GetTowerRange(current)) {
                
                
                
                int damage = GetTowerDamage(current);
                Color shotColor = RAYWHITE; 
                bool isCrit = false;

                
                if (current->critChance > 0 && GetRandomValue(1, 100) <= current->critChance) {
                    isCrit = true;
                    damage = (int)(damage * current->critMultiplier);
                    shotColor = RED;
                }

                
                if (current->hasStunEffect && GetRandomValue(1, 100) <= current->stunChance) {
                    mainTarget->isStunned = true;
                    mainTarget->stunTimer = current->stunDuration;
                    shotColor = SKYBLUE; 
                }

                
                if (current->hasAreaAttack) {
                    shotColor = ORANGE;
                    SetEnemyHP(mainTarget, GetEnemyHP(mainTarget) - damage);
                    SpawnImpactEffect(GetEnemyPosition(mainTarget), SHOT_TYPE_AOE_BLAST, shotColor);
                    
                    
                    for (int k = 0; k < maxTotalActiveEnemies; k++) {
                        if (allActiveEnemies[k].active && &allActiveEnemies[k] != mainTarget) {
                            if (Vector2Distance(GetEnemyPosition(mainTarget), GetEnemyPosition(&allActiveEnemies[k])) <= current->areaAttackRadius) {
                                SetEnemyHP(&allActiveEnemies[k], GetEnemyHP(&allActiveEnemies[k]) - damage);
                            }
                        }
                    }
                } else if (current->hasChainAttack) {
                    shotColor = VIOLET;
                    Enemy* targets[10] = {0};
                    int targetCount = 0;
                    Enemy* currentTarget = mainTarget;
                    Vector2 lastPos = GetTowerPosition(current);

                    
                    for (int i = 0; i <= current->chainJumps && currentTarget != NULL; i++) {
                        SetEnemyHP(currentTarget, GetEnemyHP(currentTarget) - damage);
                        DrawLineEx(Vector2Add(lastPos, (Vector2){-1,-1}), Vector2Add(GetEnemyPosition(currentTarget), (Vector2){-1,-1}), 3.0f, Fade(shotColor, 0.5f));
                        DrawLineEx(lastPos, GetEnemyPosition(currentTarget), 2.0f, shotColor);
                        SpawnImpactEffect(GetEnemyPosition(currentTarget), SHOT_TYPE_CRIT_SHATTER, shotColor);
                        
                        lastPos = GetEnemyPosition(currentTarget);
                        targets[targetCount++] = currentTarget;
                        currentTarget = FindNextChainTarget(currentTarget, targets, targetCount, current->chainRange);
                        damage = (int)(damage * 0.75f); 
                    }
                } else {
                    
                    SetEnemyHP(mainTarget, GetEnemyHP(mainTarget) - damage);
                    
                    
                    SpawnProjectile(GetTowerPosition(current), GetEnemyPosition(mainTarget), shotColor, 3.0f, 0.05f);
                    
                    
                    if (isCrit) {
                        SpawnImpactEffect(GetEnemyPosition(mainTarget), SHOT_TYPE_CRIT_SHATTER, shotColor);
                        Push(&statusStack, "Critical Hit!");
                    }
                }
                
                SetTowerAttackCooldown(current, GetTowerAttackSpeed(current));
                goto next_tower; 
                
            }
        }

        next_tower:; 
        current = (Tower *)current->next;
    }
}

/* I.S. : Terdapat satu atau lebih tower dalam daftar tower.
   F.S. : Semua tower yang aktif digambar ke layar, beserta UI orbit jika ada tower yang terpilih. */
void DrawTowers(float globalScale, float offsetX, float offsetY)
{
    Tower *current = towersListHead;
    while (current != NULL)
    {
        if (!GetTowerActive(current))
        {
            current = (Tower *)current->next;
            continue;
        }

        Rectangle sourceRect = {
            (float)current->currentFrame * current->frameWidth,
            0.0f,
            (float)current->frameWidth,
            (float)current->frameHeight};

        Vector2 pos = GetTowerPosition(current);

        float finalDrawWidth = current->frameWidth * TOWER_DRAW_SCALE * globalScale;
        float finalDrawHeight = current->frameHeight * TOWER_DRAW_SCALE * globalScale;

        Rectangle destRect = {
            offsetX + (pos.x * globalScale) - (finalDrawWidth / 2.0f),
            offsetY + (pos.y * globalScale) - finalDrawHeight + (TOWER_Y_OFFSET_PIXELS * globalScale),
            finalDrawWidth,
            finalDrawHeight};

        DrawTexturePro(current->texture,
                       sourceRect,
                       destRect,
                       (Vector2){0, 0}, 0.0f, WHITE);

        current = (Tower *)current->next;
    }
    if (IsTowerOrbitUIVisible() && selectedTowerForDeletion != NULL && GetCurrentOrbitParentNode() == NULL) { 
        Vector2 orbitCenter = towerSelectionUIPos;
        float orbitRadius = TILE_SIZE * globalScale * ORBIT_RADIUS_TILE_FACTOR;
        
        DrawCircleLines((int)orbitCenter.x, (int)orbitCenter.y, orbitRadius, RAYWHITE);
        
        
        Rectangle deleteBtnRect = GetOrbitButtonRect(orbitCenter, orbitRadius, 0, 2, ORBIT_BUTTON_DRAW_SCALE, deleteButtonTex);
        DrawTexturePro(deleteButtonTex, (Rectangle){0,0,(float)deleteButtonTex.width,(float)deleteButtonTex.height}, deleteBtnRect, (Vector2){0,0}, 0.0f, WHITE);
        
        
        Rectangle upgradeBtnRect = GetOrbitButtonRect(orbitCenter, orbitRadius, 1, 2, ORBIT_BUTTON_DRAW_SCALE, upgradeButtonTex);
        DrawTexturePro(upgradeButtonTex, (Rectangle){0,0,(float)upgradeButtonTex.width,(float)upgradeButtonTex.height}, upgradeBtnRect, (Vector2){0,0}, 0.0f, WHITE);
    }
}

/* I.S. : (row, col) adalah koordinat yang valid di dalam peta.
   F.S. : Mengembalikan pointer ke struct Tower jika ada tower di koordinat tersebut.
          Jika tidak ada, mengembalikan NULL. */
Tower *GetTowerAtMapCoord(int row, int col)
{
    Tower *current = towersListHead;
    while (current != NULL)
    {
        if (current->row == row && current->col == col)
        {
            return current;
        }
        current = (Tower *)current->next;
    }
    return NULL;
}

/* Mengirimkan nilai posisi (Vector2) dari 'tower'. */
Vector2 GetTowerPosition(const Tower *tower) { return tower ? tower->position : (Vector2){0, 0}; }

/* Mengirimkan nilai tipe (TowerType) dari 'tower'. */
TowerType GetTowerType(const Tower *tower) { return tower ? tower->type : TOWER_TYPE_1; }

/* Mengirimkan nilai kerusakan (damage) dari 'tower'. */
int GetTowerDamage(const Tower *tower) { return tower ? tower->damage : 0; }

/* Mengirimkan nilai jangkauan serangan (range) dari 'tower'. */
float GetTowerRange(const Tower *tower) { return tower ? tower->range : 0.0f; }

/* Mengirimkan nilai kecepatan serangan (attackSpeed) dari 'tower'. */
float GetTowerAttackSpeed(const Tower *tower) { return tower ? tower->attackSpeed : 0.0f; }

/* Mengirimkan sisa waktu cooldown serangan dari 'tower'. */
float GetTowerAttackCooldown(const Tower *tower) { return tower ? tower->attackCooldown : 0.0f; }

/* Mengirimkan status 'active' dari 'tower'. */
bool GetTowerActive(const Tower *tower) { return tower ? tower->active : false; }

/* I.S. : Posisi 'tower' sembarang.
   F.S. : Properti 'tower->position' diatur menjadi nilai 'position' yang baru. */
void SetTowerPosition(Tower *tower, Vector2 position)
{
    if (tower) tower->position = position;
}

/* I.S. : Tipe 'tower' sembarang.
   F.S. : Properti 'tower->type' diatur menjadi nilai 'type' yang baru. */
void SetTowerType(Tower *tower, TowerType type)
{
    if (tower) tower->type = type;
}

/* I.S. : Kerusakan 'tower' sembarang.
   F.S. : Properti 'tower->damage' diatur menjadi nilai 'damage' yang baru. */
void SetTowerDamage(Tower *tower, int damage)
{
    if (tower) tower->damage = damage;
}

/* I.S. : Jangkauan 'tower' sembarang.
   F.S. : Properti 'tower->range' diatur menjadi nilai 'range' yang baru. */
void SetTowerRange(Tower *tower, float range)
{
    if (tower) tower->range = range;
}

/* I.S. : Kecepatan serangan 'tower' sembarang.
   F.S. : Properti 'tower->attackSpeed' diatur menjadi nilai 'attackSpeed' yang baru. */
void SetTowerAttackSpeed(Tower *tower, float attackSpeed)
{
    if (tower) tower->attackSpeed = attackSpeed;
}

/* I.S. : Waktu cooldown 'tower' sembarang.
   F.S. : Properti 'tower->attackCooldown' diatur menjadi nilai 'cooldown' yang baru. */
void SetTowerAttackCooldown(Tower *tower, float cooldown)
{
    if (tower) tower->attackCooldown = cooldown;
}

/* I.S. : Status aktif 'tower' sembarang.
   F.S. : Properti 'tower->active' diatur menjadi nilai 'active' yang baru. */
void SetTowerActive(Tower *tower, bool active)
{
    if (tower) tower->active = active;
}

