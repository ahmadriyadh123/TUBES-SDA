/* File        : common.h 
* Deskripsi   : Deklarasi tipe data umum yang digunakan di seluruh modul program. 
* File ini berfungsi sebagai "kamus" untuk tipe data seperti GameState, 
* UpgradeType, dan struct UpgradeNode agar bisa digunakan secara konsisten 
* di berbagai file .c yang berbeda. 
* Dibuat oleh : Ahmad Riyadh Almaliki
* Perubahan terakhir : Jumat, 13 Juni 2025
*/

#ifndef COMMON_H
#define COMMON_H

#define VIRTUAL_WIDTH 1920
#define VIRTUAL_HEIGHT 1080

#include "raylib.h"
#include <stdbool.h> 
#include <stdio.h>   
#include <stdlib.h> 
#include <math.h>
#include <string.h>
#include <stddef.h>

//Enum untuk semua jenis state utamma yang ada dalam game
typedef enum {
    MAIN_MENU,
    MAIN_MENU_PLAY_SELECTION, 
    MAIN_MENU_CUSTOM_MAP_LIST,
    GAMEPLAY,
    GAME_PAUSED, 
    LEVEL_EDITOR,
    LEVEL_EDITOR_SAVE, 
    SETTINGS,
    LEVEL_COMPLETE,
    GAME_OVER,
    EXITING
} GameState;
extern GameState currentGameState;

/* Enum untuk tipe-tipe tower yang ada */
typedef enum {
    TOWER_TYPE_1,
    TOWER_TYPE_2,
    TOWER_TYPE_3
} TowerType;

/* Enum untuk semua jenis tipe upgrade yang tersedia dalam game. */
/* Digunakan untuk mengidentifikasi efek dan ikon yang sesuai. */
typedef enum {
    UPGRADE_NONE = 0,
    UPGRADE_ATTACK_SPEED_BASE,
    UPGRADE_ATTACK_POWER_BASE,
    UPGRADE_SPECIAL_EFFECT_BASE,
    UPGRADE_LIGHTNING_ATTACK,
    UPGRADE_CHAIN_ATTACK,
    UPGRADE_AREA_ATTACK,
    UPGRADE_LETHAL_POISON,
    UPGRADE_MASS_SLOW, 
    UPGRADE_CRITICAL_ATTACK,
    UPGRADE_STUN_EFFECT,
    UPGRADE_WIDE_CHAIN_RANGE,
    UPGRADE_LARGE_AOE_RADIUS,
    UPGRADE_HIGH_CRIT_CHANCE,
    
} UpgradeType;

/* Enum untuk merepresentasikan status dari sebuah node upgrade pada tower tertentu. */
typedef enum {
    UPGRADE_LOCKED,
    UPGRADE_UNLOCKED,
    UPGRADE_PURCHASED,
    UPGRADE_LOCKED_EXCLUDED
} UpgradeStatus;

/* ADT untuk satu buah 'node' dalam pohon upgrade. */
/* Setiap node merepresentasikan satu pilihan upgrade yang bisa dibeli. */
typedef struct UpgradeNode {
    UpgradeType type;
    const char* name;
    const char* description;
    int cost;
    
    UpgradeStatus status;
    struct UpgradeNode* parent; 
    
    struct UpgradeNode* children[3]; 
    int numChildren;
    
    int exclusiveGroupId;
    Vector2 uiPosition; 
    Rectangle uiRect;
    Texture2D iconTex; 
} UpgradeNode;

#endif 