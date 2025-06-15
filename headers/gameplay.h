/* File        : gameplay.h
* Deskripsi   : Deklarasi untuk Gameplay. 
*              Modul ini mengelola semua logika inti yang berjalan selama state gameplay. 
* Dibuat oleh : Ahmad Riyadh Almaliki
* Perubahan terakhir : Minggu, 15 Juni 2025
*/

#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "common.h"

extern Vector2 mousePos;
extern bool gameplayInitialized;
extern float currentTileScale;
extern float mapScreenOffsetX;
extern float mapScreenOffsetY;
#define MAX_ACTIVE_WAVES 10

// I.S. : Aset gameplay belum dimuat.
// F.S. : Semua tekstur, data tower, musuh, dll., telah dimuat dan siap digunakan.
void InitGameplay(void);

// I.S. : Permainan mungkin sedang berjalan atau belum dimulai.
// F.S. : Semua state (uang, nyawa, tower, musuh) di-reset ke kondisi awal,
// dan permainan dimulai pada state GAMEPLAY.
void RestartGameplay(void);

// I.S. : State semua entitas game (musuh, tower, wave) pada frame sebelumnya.
// F.S. : State semua entitas game telah diperbarui.
void UpdateGameplay(float deltaTime);

//  I.S. : Menunggu input klik dari pemain.
//  F.S. : Aksi yang sesuai dengan input pemain (membangun, upgrade, menjual) telah dieksekusi.
void HandleGameplayInput(Vector2 mousePos);

/* I.S. : Permainan dalam state GAME_PAUSED, menunggu input pemain.
   F.S. : Memproses klik mouse pada tombol "Resume", "Restart", atau "Main Menu".
          currentGameState akan diubah sesuai dengan tombol yang diklik. */
void UpdatePauseMenu();

/* I.S. : Permainan dalam state GAME_PAUSED.
   F.S. : Overlay gelap dan panel menu jeda dengan semua tombolnya telah
          digambar di atas tampilan gameplay yang dijeda. */
void DrawPauseMenu();

// I.S. : Elemen-elemen HUD belum digambar.
// F.S. : Seluruh elemen HUD telah digambar dengan rapi di posisinya masing-masing.
void DrawHUD(const char* mapName, int money, int life, Vector2 mousePos);

// I.S. : State semua entitas game siap untuk digambar.
// F.S. : Peta, musuh, tower, proyektil, dan HUD telah digambar.
void DrawGameplay(void);

// I.S. : Aset-aset gameplay sedang berada di memori.
// F.S. : Semua aset gameplay telah dibebaskan dari memori.
void UnloadGameplay(void);

//Berguna untuk modul UI lain yang perlu menggambar relatif terhadap ukuran peta.
float GetCurrentTileScale(void);

//Mengirimkan nilai offset X layar peta saat ini.
float GetMapScreenOffsetX(void);

//Mengirimkan nilai offset Y layar peta saat ini.
float GetMapScreenOffsetY(void);

#endif