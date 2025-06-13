/*
* Nama file: 
* Deskripsi: Menerapkan fungsionalitas terkait peta untuk game berbasis ubin, 
* menyediakan fungsi untuk memuat/membongkar tekstur, menggambar peta game, dan mengelola data ubin dan titik jalur.
*
* Penulis: Ahmad Riyadh Almaliki
*/

#include "map.h"
#include "utils.h"
#include <stdio.h>

#define MAX_PATH_POINTS 100

int gameMap[MAP_ROWS][MAP_COLS] = {
    {0, 0, 0, 2, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 2, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 2, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 2, 1, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 2, 1, 9, 7, 7, 7, 4, 7, 7, 7, 4, 7, 7, 4, 7, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0, 0},
    {0, 0, 0, 0, 8, 8, 8, 4, 8, 8, 8, 4, 8, 8, 8, 8, 4, 1, 3, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 1, 4, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

Texture2D tileSheetTex = {0};
Texture2D emptyCircleTex = {0};
static Vector2 path[MAX_PATH_POINTS] = {0};
static int pathCount = 0;

/*
Deskripsi: Menginisialisasi aset peta dengan memuat tekstur.
I.S: Keadaan awal: Tekstur tidak diinisialisasi.
F.S:  Keadaan akhir: Tekstur dimuat dan siap digunakan.
*/
void InitMapAssets() {
    tileSheetTex = LoadTextureSafe("assets/img/gameplay_imgs/tilesheet.png");
    emptyCircleTex = LoadTextureSafe("assets/img/gameplay_imgs/kosong2.png");
    TraceLog(LOG_INFO, "Map assets initialized.");
}

/*
Deskripsi: Membongkar aset peta.
I.S: Keadaan awal: Tekstur dimuat.
F.S:  Keadaan akhir: Tekstur dibongkar.
*/
void ShutdownMapAssets() {
    UnloadTextureSafe(&tileSheetTex);
    UnloadTextureSafe(&emptyCircleTex);
    TraceLog(LOG_INFO, "Map assets unloaded.");
}

/*
Deskripsi: Menggambar peta game menggunakan ubin dan tekstur.
I.S: Keadaan awal: Peta dan tekstur diinisialisasi.
F.S:  Keadaan akhir: Peta dirender ke tampilan.
*/
void DrawMap(float globalScale, float offsetX, float offsetY) {
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            int tileIndex = gameMap[r][c]; 
            Rectangle sourceRect = GetTileSourceRect(tileIndex); 

            
            Rectangle destRect = {
                offsetX + c * TILE_SIZE * globalScale,
                offsetY + r * TILE_SIZE * globalScale,
                TILE_SIZE * globalScale,
                TILE_SIZE * globalScale
            };
            
            DrawTexturePro(tileSheetTex, sourceRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
            
            if (tileIndex == 4) {
                
                Rectangle circleSource = {0, 0, (float)emptyCircleTex.width, (float)emptyCircleTex.height};
                DrawTexturePro(emptyCircleTex, circleSource, destRect, (Vector2){0, 0}, 0.0f, WHITE);
            }
            
        }
    }
}

// Mengembalikan persegi untuk ubin berdasarkan indeksnya.
// Nilai pengembalian: persegi panjang yang menentukan posisi ubin di tilesheet.
Rectangle GetTileSourceRect(int tileIndex) {
    const int tile = 32;
    switch (tileIndex) {
        case 0: return (Rectangle){0, 0, tile, tile};   
        case 1: return (Rectangle){5 * tile, 4 * tile, tile, tile};  
        case 2: return (Rectangle){0, 2 * tile, tile, tile};   
        case 3: return (Rectangle){4 * tile, 2 * tile, tile, tile};  
        case 7: return (Rectangle){2 * tile, 0, tile, tile};  
        case 8: return (Rectangle){2 * tile, 4 * tile, tile, tile};  
        case 9: return (Rectangle){5 * tile, 2 * tile, tile, tile};  
        default: return (Rectangle){0, 0, tile, tile}; 
    }
}

// Mengambil nilai ubin pada koordinat peta yang ditentukan.
// Nilai pengembalian: Nilai ubin integer atau 0 jika koordinat tidak valid.
int GetMapTile(int row, int col) {
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS) {
        return gameMap[row][col];
    }
    return 0;
}

// Deskripsi: Mengatur nilai ubin pada koordinat peta yang ditentukan.
// I.S: Peta dengan nilai ubin yang ada.
// F.S: Ubin yang ditentukan diperbarui dengan nilai baru.
void SetMapTile(int row, int col, int value) {
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS) {
        gameMap[row][col] = value;
    }
}

// Mengembalikan tekstur tilesheet untuk rendering.
// Nilai Pengembalian: Tekstur2D yang berisi tilesheet. 
Texture2D GetTileSheetTexture() {
    return tileSheetTex;
}

// Mengembalikan jumlah poin di jalur.
// Nilai Pengembalian: Hitungan Integer dari titik jalur.
int GetPathCount() {
    return pathCount;
}

// Mengembalikan titik jalur pada indeks yang ditentukan.
// Nilai pengembalian: titik vektor2 atau (0,0) jika indeks tidak valid.
Vector2 GetPathPoint(int index) {
    if (index >= 0 && index < pathCount) {
        return path[index];
    }
    return (Vector2){0, 0};
}