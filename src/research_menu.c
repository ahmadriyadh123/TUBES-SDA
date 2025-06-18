/*
 * File        : research_menu.c
 * Deskripsi   : Implementasi untuk modul Antarmuka Menu Riset (Research Menu).
 * Berkas ini berisi semua logika internal untuk menangani input,
 * memperbarui state, dan menggambar semua elemen UI dari menu riset.
 * Ini mencakup logika untuk menampilkan pohon skill, memproses klik
 * untuk membuka pop-up dan panel editor, menangani input untuk pembuatan
 * skill kustom, serta menghitung biayanya secara dinamis berdasarkan
 * efek dan tier dari skill induknya.
 * Author      : Ahmad Riyadh Almaliki
 * Tanggal     : 18 Juni 2025
 *
 */

#include "research_menu.h"
#include "main_menu.h"      
#include "common.h"         
#include "status.h"         
#include "audio.h"          
#include "utils.h"
#include "raylib.h"
#include "upgrade_tree.h"
#include "gameplay.h"
#include "audio.h"

PlayerProgress playerProgress;
const char* SAVE_FILE_NAME = "progress.txt";
static Texture2D emptySkillNodeTex;
static Texture2D research_moneyIconTex; 
static bool isSpecEditorPanelVisible = false;
static UpgradeNode* parentNodeForNewSkill = NULL; 
static int newSkillCost = 0; 
static int nextCustomSkillId = 100; 
static char newSkillNameBuffer[64];      
static int newSkillNameLetterCount = 0;  
static bool isNameInputActive = false;
static int   newBonus_damage = 0;
static float newBonus_range = 0;
static float newBonus_attack_speed_percent = 0.0f;
static int   newBonus_chain_jumps = 0;
static float newBonus_crit_chance = 0.0f;
static float newBonus_crit_multiplier = 0.0f;
static float newBonus_stun_chance = 0.0f;
static float newBonus_stun_duration = 0.0f;
static UpgradeNode* nodeForNewBranch = NULL;    
static Rectangle addBranchButtonRect;

/*
 * I.S. : node adalah pointer valid ke sebuah UpgradeNode.
 * F.S. : Mengembalikan sebuah integer yang merepresentasikan kedalaman node.
 */
static int GetNodeTier(UpgradeNode* node) {
    int tier = 0;
    UpgradeNode* current = node;
    while (current->parent != NULL) {
        tier++;
        current = current->parent;
    }
    return tier;
}

/*
 * I.S. : Variabel global 'parentNodeForNewSkill' dan variabel 'newBonus_...' memiliki nilai tertentu.
 * F.S. : Variabel global 'newSkillCost' diperbarui dengan nilai biaya yang baru,
 * yang dihitung berdasarkan tier induk dan nilai bonus yang sedang diatur.
 */
static void CalculateNewSkillCost() {
    if (!parentNodeForNewSkill) {
        newSkillCost = 0;
        return;
    }
    int tier = GetNodeTier(parentNodeForNewSkill);
    int cost = 150 + (tier * 0.5f);
    switch (parentNodeForNewSkill->type) {
        case UPGRADE_CRITICAL_ATTACK:
        case UPGRADE_HIGH_CRIT_CHANCE:
            cost += (int)(newBonus_crit_chance * 10);
            cost += (int)(newBonus_crit_multiplier * 10);
            break;
        case UPGRADE_CHAIN_ATTACK:
        case UPGRADE_WIDE_CHAIN_RANGE:
            cost += newBonus_chain_jumps * 10;
            break;
        case UPGRADE_STUN_EFFECT: 
            cost += (int)(newBonus_stun_chance * 10);
            cost += (int)(newBonus_stun_duration * 12); 
            break;
        default:
            cost += newBonus_damage * 15;
            cost += (int)(newBonus_range * 10);
            cost += (int)(newBonus_attack_speed_percent * 15);
            break;
    }
    newSkillCost = cost;
}

/*
 * I.S. : Tekstur dan aset untuk menu riset belum dimuat ke memori.
 * F.S. : Semua tekstur yang diperlukan (ikon, dll.) telah dimuat dan siap digunakan.
 */
void InitResearchMenu(void) {
    emptySkillNodeTex = LoadTextureSafe("assets/img/mainmenu_imgs/nodeskill.png");
    research_moneyIconTex = LoadTextureSafe("assets/img/gameplay_imgs/coin.png"); 
}

 /*
  * I.S. : Variabel global 'nodeForNewBranch' bisa NULL atau menunjuk ke sebuah node.
 * F.S. : Jika 'nodeForNewBranch' tidak NULL, sebuah pop-up digambar di layar di sebelah
 * node tersebut. Posisi tombol di dalam pop-up disimpan ke 'addBranchButtonRect'.
 */
static void DrawAddBranchPopup() {
    if (nodeForNewBranch == NULL) return;
    float popupWidth = 150;
    float popupHeight = 50;
    float popupX = nodeForNewBranch->uiRect.x + nodeForNewBranch->uiRect.width + 10;
    float popupY = nodeForNewBranch->uiRect.y + (nodeForNewBranch->uiRect.height - popupHeight) / 2;
    
    addBranchButtonRect = (Rectangle){ popupX, popupY, popupWidth, popupHeight };
    DrawRectangleRec(addBranchButtonRect, DARKGRAY);
    DrawRectangleLinesEx(addBranchButtonRect, 2, GOLD);
    DrawText("Tambah Cabang Baru", (int)(popupX + 10), (int)(popupY + 15), 15, WHITE);
}

/*
 * I.S. : Posisi UI (uiRect) pada setiap node mungkin belum sesuai dengan frame saat ini.
 * F.S. : Posisi UI (uiRect) dari semua node di pohon telah dihitung ulang. Jika ada klik pada
 * node yang valid, state 'nodeForNewBranch' akan diperbarui untuk memicu pop-up.
 */
static void UpdateAndProcessTreeInteraction(UpgradeNode* node, int posX, int posY, int level, Vector2 mousePos, float scale) {
    if (!node) return;
    float iconSize = 50.0f * scale;
    node->uiRect.x = posX - iconSize / 2;
    node->uiRect.y = posY - iconSize / 2;
    node->uiRect.width = iconSize;
    node->uiRect.height = iconSize;
    int numChildren = GetNumChildren(node);
    if (node->cost > 0 || node->type >= 100) {
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, node->uiRect)) {
            
            nodeForNewBranch = node;
            isSpecEditorPanelVisible = false; 
        }
    }
     
    float h_spacing = (350.0f - (level * 200.0f)) * scale;
    float v_spacing = 80.0f * scale;
    int startX = posX - ( (numChildren - 1) * h_spacing ) / 2;
    
    UpgradeNode* child = node->firstChild;
    int childIndex = 0;
    while(child != NULL) {
        
        int childX = startX + (childIndex * h_spacing);
        int childY = posY + v_spacing;
        UpdateAndProcessTreeInteraction(child, childX, childY, level + 1, mousePos, scale);
        
        child = child->nextSibling;
        childIndex++;
    }
}

/*
 * I.S. : State dari menu riset pada frame sebelumnya.
 * F.S. : Semua input pengguna (klik mouse, keyboard) telah diproses dan state dari menu
 * (seperti 'isSpecEditorPanelVisible', 'nodeForNewBranch', nilai bonus, dll.)
 * telah diperbarui untuk frame saat ini.
 */
void UpdateResearchMenu(void) {
    Vector2 mousePos = GetMousePosition();
    
    Rectangle backButtonRect = { 20, 20, (float)backButtonTex.width, (float)backButtonTex.height };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, backButtonRect)) {
        currentGameState = MAIN_MENU;
        
        isSpecEditorPanelVisible = false;
        nodeForNewBranch = NULL;
        return;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (isSpecEditorPanelVisible) isSpecEditorPanelVisible = false;
        else if (nodeForNewBranch != NULL) nodeForNewBranch = NULL;
        return;
    }

    if (isSpecEditorPanelVisible) {
        Rectangle panelRect = { VIRTUAL_WIDTH / 2 - 300, VIRTUAL_HEIGHT / 2 - 250, 600, 500 };
        Rectangle textBox = { panelRect.x + 30, panelRect.y + 100, panelRect.width - 60, 40 };
        Rectangle confirmBtnRect = { panelRect.x + panelRect.width / 2 - 100, panelRect.y + panelRect.height - 70, 200, 50};
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            isNameInputActive = CheckCollisionPointRec(mousePos, textBox);
        }
        
    if (isNameInputActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (newSkillNameLetterCount < 63)) {
                newSkillNameBuffer[newSkillNameLetterCount] = (char)key;
                newSkillNameLetterCount++;
                newSkillNameBuffer[newSkillNameLetterCount] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (newSkillNameLetterCount > 0) {
                newSkillNameLetterCount--;
                newSkillNameBuffer[newSkillNameLetterCount] = '\0';
            }
        }
    }
        
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(mousePos, textBox)) {
        float buttonSize = 40.0f;
        float rightEdge = panelRect.x + panelRect.width - 30;
        float plusButtonX = rightEdge - buttonSize;
        float valueWidth = 60; 
        float minusButtonX = plusButtonX - valueWidth - buttonSize;
        int posY = (int)panelRect.y + 180;
        switch(parentNodeForNewSkill->type) {
            case UPGRADE_CRITICAL_ATTACK: case UPGRADE_HIGH_CRIT_CHANCE:
                if (CheckCollisionPointRec(mousePos, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize})) newBonus_crit_chance += 1.0f;
                if (CheckCollisionPointRec(mousePos, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}) && newBonus_crit_chance > 0) newBonus_crit_chance -= 1.0f;
                posY += 60;
                if (CheckCollisionPointRec(mousePos, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize})) newBonus_crit_multiplier += 0.1f;
                if (CheckCollisionPointRec(mousePos, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}) && newBonus_crit_multiplier > 0) newBonus_crit_multiplier -= 0.1f;
                break;
            case UPGRADE_CHAIN_ATTACK: case UPGRADE_WIDE_CHAIN_RANGE:
                if (CheckCollisionPointRec(mousePos, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize})) newBonus_chain_jumps += 1;
                if (CheckCollisionPointRec(mousePos, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}) && newBonus_chain_jumps > 0) newBonus_chain_jumps -= 1;
                break;
            case UPGRADE_STUN_EFFECT: 
                if (CheckCollisionPointRec(mousePos, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize})) newBonus_stun_chance += 1.0f;
                if (CheckCollisionPointRec(mousePos, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}) && newBonus_stun_chance > 0) newBonus_stun_chance -= 1.0f;
                posY += 60;
                if (CheckCollisionPointRec(mousePos, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize})) newBonus_stun_duration += 0.1f;
                if (CheckCollisionPointRec(mousePos, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}) && newBonus_stun_duration > 0) newBonus_stun_duration -= 0.1f;
                break;
            default:
                if (CheckCollisionPointRec(mousePos, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize})) newBonus_damage += 5;
                if (CheckCollisionPointRec(mousePos, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}) && newBonus_damage > 0) newBonus_damage -= 5;
                posY += 60;
                if (CheckCollisionPointRec(mousePos, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize})) newBonus_range += 5;
                if (CheckCollisionPointRec(mousePos, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}) && newBonus_range > 0) newBonus_range -= 5;
                break;
        }
        CalculateNewSkillCost();
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, confirmBtnRect)) {
        if (newSkillNameLetterCount == 0) { Push(&statusStack, "Nama skill tidak boleh kosong!"); return; }
        if (playerProgress.globalGold < newSkillCost) { Push(&statusStack, "Emas global tidak mencukupi!"); return; }
        
        playerProgress.globalGold -= newSkillCost;
        UpgradeNode* newNode = CreateUpgradeNode((UpgradeType)nextCustomSkillId, strdup(newSkillNameBuffer), "Skill Kustom", newSkillCost, parentNodeForNewSkill, 0);
        
        if (newNode) {
            switch(parentNodeForNewSkill->type) {
                case UPGRADE_CRITICAL_ATTACK: case UPGRADE_HIGH_CRIT_CHANCE:
                    newNode->effectType = EFFECT_CRIT_MODIFIER;
                    newNode->bonusData.crit.chance = newBonus_crit_chance;
                    newNode->bonusData.crit.multiplier = newBonus_crit_multiplier;
                    break;
                case UPGRADE_CHAIN_ATTACK: case UPGRADE_WIDE_CHAIN_RANGE:
                    newNode->effectType = EFFECT_CHAIN_MODIFIER;
                    newNode->bonusData.chain.jumps = newBonus_chain_jumps;
                    break;
                case UPGRADE_STUN_EFFECT: 
                    newNode->effectType = EFFECT_STUN_MODIFIER;
                    newNode->bonusData.stun.chance = newBonus_stun_chance;
                    newNode->bonusData.stun.duration = newBonus_stun_duration;
                    break;
                default:
                    newNode->effectType = EFFECT_GENERIC_STATS;
                    newNode->bonusData.generic.damage = newBonus_damage;
                    newNode->bonusData.generic.range = newBonus_range;
                    break;
            }
            AddChild(parentNodeForNewSkill, newNode);
            SaveProgress(&playerProgress);
            Push(&statusStack, TextFormat("Skill kustom '%s' berhasil dibuat!", newNode->name));
            nextCustomSkillId++;
            isSpecEditorPanelVisible = false;
        }
    }
    } 
    
    else if (nodeForNewBranch != NULL) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePos, addBranchButtonRect)) {
                isSpecEditorPanelVisible = true;
                parentNodeForNewSkill = nodeForNewBranch;
                nodeForNewBranch = NULL;
                
                
                newSkillNameBuffer[0] = '\0';
                newSkillNameLetterCount = 0;
                isNameInputActive = false;
                newBonus_damage = 0; newBonus_range = 0; newBonus_attack_speed_percent = 0.0f;
                newBonus_chain_jumps = 0; newBonus_crit_chance = 0.0f; newBonus_crit_multiplier = 0.0f;
                newBonus_stun_chance = 0.0f; newBonus_stun_duration = 0.0f;
                CalculateNewSkillCost();
            } else {      
                nodeForNewBranch = NULL;
            }
        }
    } 
    
    else {
        UpgradeNode* gameTreeRoot = GetUpgradeTreeRoot(&tower1UpgradeTree);
        if (gameTreeRoot) {
            
            UpdateAndProcessTreeInteraction(gameTreeRoot, VIRTUAL_WIDTH / 2, 200, 0, mousePos, 2.0f);
        }
    }
}

/*
 * I.S. : Setiap node di dalam pohon memiliki data posisi (uiRect) yang mungkin lama atau belum diinisialisasi.
 * F.S. : Representasi visual dari pohon upgrade (ikon, nama, dan garis penghubung)
 * telah digambar ke layar sesuai data posisi yang tersimpan di setiap node.
 */
static void DrawUpgradeTreeForResearch(UpgradeNode* node, float scale) {
    if (!node) return;
    
    
    Texture2D nodeIcon = GetUpgradeIconTexture(node->type);
    if (nodeIcon.id > 0) {
        DrawTexturePro(nodeIcon, 
                       (Rectangle){0, 0, (float)nodeIcon.width, (float)nodeIcon.height},
                       node->uiRect, 
                       (Vector2){0,0}, 0.0f, WHITE);
    }
    int scaledFontSize = (int)(10.0f * scale) > 8 ? (int)(10.0f * scale) : 8;
    DrawText(node->name, node->uiRect.x + (node->uiRect.width - MeasureText(node->name, scaledFontSize)) / 2, node->uiRect.y + node->uiRect.height + 5, scaledFontSize, WHITE);
    
    UpgradeNode* child = node->firstChild;
    while (child != NULL) {
        
        DrawLine(node->uiRect.x + node->uiRect.width / 2, 
                 node->uiRect.y + node->uiRect.height / 2, 
                 child->uiRect.x + child->uiRect.width / 2, 
                 child->uiRect.y + child->uiRect.height / 2, 
                 GRAY);
        DrawUpgradeTreeForResearch(child, scale); 
        child = child->nextSibling;
    }
}

/*
 * I.S. : Variabel global 'isSpecEditorPanelVisible' bisa true atau false.
 * F.S. : Jika 'isSpecEditorPanelVisible' adalah true, keseluruhan UI panel editor skill
 * (termasuk input nama dan kontrol stat yang kontekstual) telah digambar ke layar.
 */
static void DrawSpecEditorPanel() {
    if (!isSpecEditorPanelVisible) return;
    
    DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Fade(BLACK, 0.7f));
    Rectangle panelRect = { VIRTUAL_WIDTH / 2 - 300, VIRTUAL_HEIGHT / 2 - 250, 600, 500 };
    DrawRectangleRec(panelRect, DARKGRAY);
    DrawRectangleLinesEx(panelRect, 2, WHITE);
    
    if (parentNodeForNewSkill) {
        char title[128];
        sprintf(title, "Menambah Skill dari: %s", parentNodeForNewSkill->name);
        DrawText(title, panelRect.x + 20, panelRect.y + 20, 20, GOLD);
    }
    
    DrawText("Nama Skill Khusus:", panelRect.x + 30, panelRect.y + 70, 20, WHITE);
    Rectangle textBox = { panelRect.x + 30, panelRect.y + 100, panelRect.width - 60, 40 };
    DrawRectangleRec(textBox, LIGHTGRAY);
    if (isNameInputActive) DrawRectangleLinesEx(textBox, 2, GOLD);
    else DrawRectangleLinesEx(textBox, 1, DARKGRAY);
    DrawText(newSkillNameBuffer, (int)textBox.x + 10, (int)textBox.y + 10, 20, BLACK);
    if (isNameInputActive && ((int)(GetTime() * 2.0f) % 2) == 0) {
        DrawText("|", (int)textBox.x + 10 + MeasureText(newSkillNameBuffer, 20), (int)textBox.y + 12, 20, BLACK);
    }
    int posY = panelRect.y + 180;
    float buttonSize = 40.0f;
    
    float rightEdge = panelRect.x + panelRect.width - 30;
    float plusButtonX = rightEdge - buttonSize;
    float valueWidth = 60;
    float minusButtonX = plusButtonX - valueWidth - buttonSize;
    switch (parentNodeForNewSkill->type) {
        case UPGRADE_CRITICAL_ATTACK: case UPGRADE_HIGH_CRIT_CHANCE:
            DrawText("Tambah Peluang Kritis (%):", panelRect.x + 30, posY, 20, WHITE);
            DrawText(TextFormat("%.1f", newBonus_crit_chance), minusButtonX + buttonSize, posY, 20, WHITE);
            DrawTexturePro(minusButtonTex, (Rectangle){0,0,(float)minusButtonTex.width,(float)minusButtonTex.height}, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            DrawTexturePro(plusButtonTex, (Rectangle){0,0,(float)plusButtonTex.width,(float)plusButtonTex.height}, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            posY += 60;
            DrawText("Tambah Pengali Kritis (x):", panelRect.x + 30, posY, 20, WHITE);
            DrawText(TextFormat("%.2f", newBonus_crit_multiplier), minusButtonX + buttonSize, posY, 20, WHITE);
            DrawTexturePro(minusButtonTex, (Rectangle){0,0,(float)minusButtonTex.width,(float)minusButtonTex.height}, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            DrawTexturePro(plusButtonTex, (Rectangle){0,0,(float)plusButtonTex.width,(float)plusButtonTex.height}, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            break;
        case UPGRADE_CHAIN_ATTACK: case UPGRADE_WIDE_CHAIN_RANGE:
            DrawText("Tambah Jumlah Lompatan:", panelRect.x + 30, posY, 20, WHITE);
            DrawText(TextFormat("%d", newBonus_chain_jumps), minusButtonX + buttonSize, posY, 20, WHITE);
            DrawTexturePro(minusButtonTex, (Rectangle){0,0,(float)minusButtonTex.width,(float)minusButtonTex.height}, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            DrawTexturePro(plusButtonTex, (Rectangle){0,0,(float)plusButtonTex.width,(float)plusButtonTex.height}, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            break;
        case UPGRADE_STUN_EFFECT: 
            DrawText("Tambah Peluang Stun (%):", panelRect.x + 30, posY, 20, WHITE);
            DrawText(TextFormat("%.1f", newBonus_stun_chance), minusButtonX + buttonSize, posY, 20, WHITE);
            DrawTexturePro(minusButtonTex, (Rectangle){0,0,(float)minusButtonTex.width,(float)minusButtonTex.height}, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            DrawTexturePro(plusButtonTex, (Rectangle){0,0,(float)plusButtonTex.width,(float)plusButtonTex.height}, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            posY += 60;
            DrawText("Tambah Durasi Stun (s):", panelRect.x + 30, posY, 20, WHITE);
            DrawText(TextFormat("%.2f", newBonus_stun_duration), minusButtonX + buttonSize, posY, 20, WHITE);
            DrawTexturePro(minusButtonTex, (Rectangle){0,0,(float)minusButtonTex.width,(float)minusButtonTex.height}, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            DrawTexturePro(plusButtonTex, (Rectangle){0,0,(float)plusButtonTex.width,(float)plusButtonTex.height}, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            break;
        default:
            DrawText("Tambah Besar Serangan:", panelRect.x + 30, posY, 20, WHITE);
            DrawText(TextFormat("%d", newBonus_damage), minusButtonX + buttonSize, posY, 20, WHITE);
            DrawTexturePro(minusButtonTex, (Rectangle){0,0,(float)minusButtonTex.width,(float)minusButtonTex.height}, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            DrawTexturePro(plusButtonTex, (Rectangle){0,0,(float)plusButtonTex.width,(float)plusButtonTex.height}, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            posY += 60;
            DrawText("Tambah Radius Serangan:", panelRect.x + 30, posY, 20, WHITE);
            DrawText(TextFormat("%.0f", newBonus_range), minusButtonX + buttonSize, posY, 20, WHITE);
            DrawTexturePro(minusButtonTex, (Rectangle){0,0,(float)minusButtonTex.width,(float)minusButtonTex.height}, (Rectangle){minusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            DrawTexturePro(plusButtonTex, (Rectangle){0,0,(float)plusButtonTex.width,(float)plusButtonTex.height}, (Rectangle){plusButtonX, posY - 10, buttonSize, buttonSize}, (Vector2){0,0},0,WHITE);
            break;
    }
    Rectangle confirmBtn = { panelRect.x + panelRect.width / 2 - 100, panelRect.y + panelRect.height - 70, 200, 50};
    DrawRectangleRec(confirmBtn, LIME);
    DrawText("Konfirmasi Skill", confirmBtn.x + 25, confirmBtn.y + 15, 20, BLACK);
    
    char costText[64];
    sprintf(costText, "Biaya: %d Emas", newSkillCost);
    int costTextWidth = MeasureText(costText, 20);
    Color costColor = (playerProgress.globalGold >= newSkillCost) ? GOLD : RED;
    DrawText(costText, confirmBtn.x + (confirmBtn.width - costTextWidth) / 2, confirmBtn.y - 30, 20, costColor);
}

/*
 * I.S. : State dari semua elemen menu riset telah terdefinisi untuk frame saat ini.
 * F.S. : Seluruh antarmuka pengguna untuk menu riset, termasuk pohon skill, pop-up,
 * dan panel editor, telah digambar ke layar.
 */
void DrawResearchMenu(void) {
    ClearBackground(BLACK);
    DrawText("Pohon Riset & Pengembangan Skill", VIRTUAL_WIDTH / 2 - MeasureText("Pohon Riset & Pengembangan Skill", 40) / 2, 40, 40, GOLD);
    
    DrawTexture(backButtonTex, 20, 20, WHITE);
    
    if (research_moneyIconTex.id > 0) {
        char goldText[32];
        sprintf(goldText, "%lld", playerProgress.globalGold);
        int fontSize = 24;
        float padding = 10.0f;
        float spacing = 8.0f;
        float iconSize = 28.0f;
        float textWidth = MeasureText(goldText, fontSize);
        float blockWidth = iconSize + spacing + textWidth;
        
        float panelX = VIRTUAL_WIDTH - blockWidth - (padding * 3);
        float panelY = 20.0f;
        float panelWidth = blockWidth + (padding * 2);
        float panelHeight = iconSize + (padding * 2);
        DrawRectangleRounded((Rectangle){panelX, panelY, panelWidth, panelHeight}, 0.3f, 10, Fade(DARKGRAY, 0.8f));
        DrawTextureEx(research_moneyIconTex, (Vector2){panelX + padding, panelY + padding}, 0.0f, iconSize / research_moneyIconTex.height, WHITE);
        DrawText(goldText, panelX + padding + iconSize + spacing, panelY + padding + (iconSize - fontSize) / 2, fontSize, GOLD);
    }
    UpgradeNode* gameTreeRoot = GetUpgradeTreeRoot(&tower1UpgradeTree);
    if (gameTreeRoot) {
        DrawUpgradeTreeForResearch(gameTreeRoot, 2.0f);
    }
    DrawAddBranchPopup();
    DrawSpecEditorPanel();
}

/*
 * I.S. : Tekstur dan aset menu riset sedang berada di dalam memori.
 * F.S. : Semua tekstur yang dialokasikan telah dibebaskan dari memori.
 */
void UnloadResearchMenu(void) {    
    UnloadTextureSafe(&emptySkillNodeTex);
    UnloadTextureSafe(&research_moneyIconTex);
}

// Fungsi ini bertanggung jawab untuk menyimpan progres pemain ke dalam sebuah file.
void SaveProgress(const PlayerProgress* progress) {
    FILE* file = fopen(SAVE_FILE_NAME, "w"); 
    if (file) {
        fprintf(file, "globalGold %lld\n", progress->globalGold);
        fclose(file);
    }
}

// Fungsi ini bertanggung jawab untuk memuat progres pemain dari sebuah file.
bool LoadProgress(PlayerProgress* progress) {
    progress->globalGold = 5000; 
    for (int i = 0; i < MAX_GLOBAL_SKILLS; i++) {
        progress->unlockedSkills[i] = false;
    }
    FILE* file = fopen(SAVE_FILE_NAME, "r"); 
    if (file) {
        char key[64];
        long long value;
        while (fscanf(file, "%s %lld", key, &value) == 2) {
            if (strcmp(key, "globalGold") == 0) {
                progress->globalGold = value;
            }
            
        }
        fclose(file);
        return true;
    }

    return false;
}