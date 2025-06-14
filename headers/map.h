/* File        : map.h 
* Deskripsi   : Deklarasi untuk modul Peta (Map). 
*               Modul ini mengelola semua data dan aset yang berkaitan dengan 
*               grid peta permainan, termasuk ukuran, data tile, dan tekstur. 
* Dibuat oleh : Ahmad Riyadh Almaliki
* Perubahan terakhir : Senin, 9 Juni 2025
*/
#ifndef MAP_H
#define MAP_H
#include "raylib.h"

#define MAP_ROWS 14
#define MAP_COLS 23
#define TILE_SIZE 27

extern int gameMap[MAP_ROWS][MAP_COLS];
extern Texture2D tileSheetTex;
extern Texture2D emptyCircleTex;

/*
Deskripsi: Menginisialisasi aset peta dengan memuat tekstur.
I.S: Keadaan awal: Tekstur tidak diinisialisasi.
F.S:  Keadaan akhir: Tekstur dimuat dan siap digunakan.
*/
void InitMapAssets();

/*
Deskripsi: Membongkar aset peta.
I.S: Keadaan awal: Tekstur dimuat.
F.S:  Keadaan akhir: Tekstur dibongkar.
*/
void ShutdownMapAssets();

/*
Deskripsi: Menggambar peta game menggunakan ubin dan tekstur.
I.S: Keadaan awal: Peta dan tekstur diinisialisasi.
F.S:  Keadaan akhir: Peta dirender ke tampilan.
*/
void DrawMap(float globalScale, float offsetX, float offsetY);

// Mengembalikan persegi untuk ubin berdasarkan indeksnya.
// Nilai pengembalian: persegi panjang yang menentukan posisi ubin di tilesheet.
Rectangle GetTileSourceRect(int index);

// Mengambil nilai ubin pada koordinat peta yang ditentukan.
// Nilai pengembalian: Nilai ubin integer atau 0 jika koordinat tidak valid.
int GetMapTile(int row, int col);

// Deskripsi: Mengatur nilai ubin pada koordinat peta yang ditentukan.
// I.S: Peta dengan nilai ubin yang ada.
// F.S: Ubin yang ditentukan diperbarui dengan nilai baru.
void SetMapTile(int row, int col, int value);

// Mengembalikan tekstur tilesheet untuk rendering.
// Nilai Pengembalian: Tekstur2D yang berisi tilesheet. 
Texture2D GetTileSheetTexture();

// Mengembalikan jumlah poin di jalur.
// Nilai Pengembalian: Hitungan Integer dari titik jalur.
int GetPathCount();

// Mengembalikan titik jalur pada indeks yang ditentukan.
// Nilai pengembalian: titik vektor2 atau (0,0) jika indeks tidak valid.
Vector2 GetPathPoint(int index);

#endif