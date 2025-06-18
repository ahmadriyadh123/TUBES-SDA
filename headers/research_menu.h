/*
 * File        : research_menu.h
 * Deskripsi   : Deklarasi untuk modul Antarmuka Menu Riset (Research Menu).
 * Modul ini mendefinisikan fungsi-fungsi publik yang diperlukan untuk
 * mengelola dan menampilkan layar menu riset, di mana pemain dapat
 * melihat pohon skill dan menambahkan cabang upgrade kustom baru secara dinamis.
 * Author      : Ahmad Riyadh Almaliki
 * Tanggal     : 18 Juni 2025
*/

#ifndef RESEARCH_MENU_H
#define RESEARCH_MENU_H

#include "raylib.h"

/**
 * @brief Menginisialisasi semua sumber daya yang dibutuhkan oleh menu riset.
 * Fungsi ini bisa dipanggil sekali di awal jika ada aset yang perlu dimuat.
 */
void InitResearchMenu(void);

/**
 * @brief Meng-update logika untuk menu riset.
 * Menangani input klik dari pemain untuk membeli riset atau kembali ke menu sebelumnya.
 */
void UpdateResearchMenu(void);

/**
 * @brief Menggambar semua elemen antarmuka (UI) untuk menu riset.
 * Menampilkan pohon riset, status (terkunci/terbuka), dan jumlah emas global.
 */
void DrawResearchMenu(void);

/**
 * @brief Membersihkan dan membebaskan semua sumber daya yang digunakan menu riset.
 * Dipanggil saat program akan ditutup.
 */
void UnloadResearchMenu(void);

// Definisikan semua kemungkinan riset yang bisa dibuka
typedef enum {
    GLOBAL_SKILL_UNLOCK_POISON_TOWER,
    GLOBAL_SKILL_ADD_ICE_TO_AOE_PATH,
    GLOBAL_SKILL_PASSIVE_STARTING_MONEY,
    MAX_GLOBAL_SKILLS
} GlobalSkillID;

// Struktur untuk menyimpan semua progres pemain
typedef struct {
    long long globalGold;
    bool unlockedSkills[MAX_GLOBAL_SKILLS];
} PlayerProgress;

extern PlayerProgress playerProgress;

// Deklarasi fungsi untuk menyimpan dan memuat progres
void SaveProgress(const PlayerProgress* progress);
bool LoadProgress(PlayerProgress* progress);

#endif // RESEARCH_MENU_H