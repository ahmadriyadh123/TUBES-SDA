/* File        : main_menu.h
*  Deskripsi   : Deklarasi untuk modul Antarmuka Menu Utama (Main Menu).
*               Modul ini berisi semua deklarasi fungsi, variabel, dan konstanta 
*               yang diperlukan untuk menampilkan dan mengelola menu utama, 
*               termasuk sub-menu pemilihan level. 
* Dibuat oleh  : Mohamad Jibril Fathi Al-ghifari
* Perubahan terakhir : Minggu, 8 Juni 2025
*/

#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "common.h"

#define MENU_BG_DRAW_SCALE 2.0f      
#define MENU_BUTTON_DRAW_SCALE 1.5f  
#define BUTTON_SPACING_PX 5.0f
#define MAP_LIST_ITEM_HEIGHT 60
#define MAP_LIST_VIEW_HEIGHT 200 
#define PANEL_WIDTH_FACTOR 0.4f
#define PANEL_HEIGHT_FACTOR 0.5f
#define PANEL_PADDING 20.0f
#define PANEL_BUTTON_HEIGHT 50.0f 
#define PANEL_BUTTON_SPACING 10.0f

#define MAX_CUSTOM_MAPS 10
#define MAP_FILENAME_MAX_LEN 256

extern Texture2D backgroundTex; 
extern Texture2D menuBgTex;
extern Texture2D startButtonTex;
extern Texture2D levelEditorButtonTex;
extern Texture2D settingsButtonTex;
extern Texture2D exitButtonTex;
extern Texture2D backButtonTex; 

/* ADT untuk menyimpan informasi satu entri peta kustom. */
/* Digunakan untuk menampilkan daftar peta di menu dan memuat file yang benar. */
typedef struct {
    char name[MAP_FILENAME_MAX_LEN];
    char filePath[MAP_FILENAME_MAX_LEN];  
    int startOffset;                      
    int endOffset;                        
} CustomMapEntry;

extern CustomMapEntry customMaps[MAX_CUSTOM_MAPS];
extern int customMapCount;
extern int selectedCustomMapIndex;  

extern GameState currentGameState; 

/* I.S. : Aset-aset gambar untuk menu utama (background, tombol, dll.) belum dimuat ke memori.
   F.S. : Semua aset gambar yang diperlukan untuk menu utama telah dimuat ke dalam variabel-variabel Texture2D. */
void LoadMainMenuResources();

/* I.S. : Aset-aset gambar menu utama sedang berada di dalam memori.
   F.S. : Semua memori GPU yang digunakan oleh tekstur menu utama telah dibebaskan. */
void UnloadMainMenuResources();

/* I.S. : Game berada dalam state MAIN_MENU dan aset telah dimuat.
   F.S. : Tampilan utama dari menu (background, logo, tombol-tombol utama) telah digambar ke layar. */
void DrawMainMenu();

/* I.S. : Diberikan parameter untuk posisi, ukuran, teks, dan warna.
   F.S. : Sebuah tombol sederhana berupa persegi panjang dengan teks di tengahnya telah digambar ke layar. */
void DrawButton(Rectangle rect, const char* text, Color bgColor, Color textColor, int fontSize);

/* Mengirimkan true jika posisi mouse 'mousePos' berada di dalam area persegi 'rect', false jika tidak. */
bool CheckButtonClick(Rectangle rect, Vector2 mousePos);

/* I.S. : Game berada dalam state MAIN_MENU, menunggu input dari pemain.
   F.S. : Jika pemain mengklik salah satu tombol utama, currentGameState diubah ke state yang sesuai
          (misalnya, MAIN_MENU_PLAY_SELECTION, LEVEL_EDITOR, dll.). */
void HandleMainMenuInput();

/* I.S. : Game berada dalam state MAIN_MENU_PLAY_SELECTION.
   F.S. : Jika pemain mengklik tombol 'Default' atau 'Custom', currentGameState diubah
          ke state berikutnya yang sesuai. */
void UpdatePlaySelectionMenu();

/* I.S. : Game berada dalam state MAIN_MENU_PLAY_SELECTION.
   F.S. : Tampilan untuk memilih mode permainan (Default Game / Custom Game) telah digambar ke layar. */
void DrawPlaySelectionMenu();

/* I.S. : Game berada dalam state MAIN_MENU_CUSTOM_MAP_LIST.
F.S. : Menangani input scroll mouse dan klik pada item daftar peta. Jika sebuah peta dipilih,
currentGameState diubah ke GAMEPLAY. */
void UpdateCustomMapListMenu();

/* I.S. : Game berada dalam state MAIN_MENU_CUSTOM_MAP_LIST dan daftar peta telah dimuat.
   F.S. : Daftar peta kustom yang dapat di-scroll telah digambar ke layar. */
void DrawCustomMapListMenu();

/* I.S. : Daftar peta kustom di dalam program mungkin kosong atau berisi data lama.
   F.S. : Program telah memindai direktori 'maps/', dan daftar peta kustom (array customMaps)
          telah diisi dengan nama-nama file peta yang valid. */
void LoadCustomMapList();

#endif