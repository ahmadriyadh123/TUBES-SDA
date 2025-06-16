/* File        : player_resources.h.h 
*
*  Deskripsi   : mengelola dan memfasilitasi interaksi dengan dua sumber daya utama pemain
                 dalam sebuah game: uang (money) dan nyawa (life).
*  Penulis     : m. jibril fathi al-ghifari
*  Perubahan terakhir: Sabtu, 14 Juni 2025
*/

#ifndef PLAYER_RESOURCES_H
#define PLAYER_RESOURCES_H

#include "raylib.h"
#include "stdio.h"

// Menginisialisasi uang pemain.
// IS : Variabel internal `money` bisa memiliki nilai apa saja.
// FS : Variabel internal `money` diatur menjadi 0.
void InitEconomy();

// Menambahkan jumlah tertentu ke uang pemain.
// IS : Variabel internal `money` memiliki nilai tertentu; `amount` adalah bilangan bulat.
// FS : Variabel internal `money` bertambah `amount` (dibulatkan menjadi 0 jika hasilnya negatif).
void AddMoney(int amount);

// Mendapatkan jumlah uang pemain saat ini.
// IS : Variabel internal `money` memiliki nilai tertentu.
// FS : Mengembalikan nilai variabel internal `money`.
int GetMoney();

// Mengatur uang pemain ke jumlah tertentu.
// IS : Variabel internal `money` memiliki nilai tertentu; `amount` adalah bilangan bulat.
// FS : Variabel internal `money` diatur menjadi `amount` (dibulatkan menjadi 0 jika `amount` negatif).
void SetMoney(int amount);

// Menginisialisasi status permainan.
// IS : Variabel internal `life` bisa memiliki nilai apa saja.
// FS : Variabel internal `life` diatur menjadi 10.
void InitGameState();

// Mengurangi nyawa pemain sebanyak satu.
// IS : Variabel internal `life` memiliki nilai tertentu.
// FS : Variabel internal `life` berkurang 1 (dibulatkan menjadi 0 jika hasilnya negatif).
void LoseLife();

// Memeriksa apakah permainan sudah berakhir.
// IS : Variabel internal `life` memiliki nilai tertentu.
// FS : Mengembalikan `true` jika `life` <= 0, jika tidak `false`.
bool IsGameOver();

// Mendapatkan jumlah nyawa pemain saat ini.
// IS : Variabel internal `life` memiliki nilai tertentu.
// FS : Mengembalikan nilai variabel internal `life`.
int GetLife();

// Mengatur nyawa pemain ke jumlah tertentu.
// IS : Variabel internal `life` memiliki nilai tertentu; `amount` adalah bilangan bulat.
// FS : Variabel internal `life` diatur menjadi `amount` (dibulatkan menjadi 0 jika `amount` negatif).
void SetLife(int amount);

// Menggambar jumlah nyawa saat ini di layar.
// IS : Konteks gambar Raylib aktif; variabel internal `life` memiliki nilai tertentu.
// FS : Teks "Life: [current_life]" digambar di layar.
void DrawLife();

// Mengurangi nyawa pemain sebesar jumlah tertentu.
// IS : Variabel internal `life` memiliki nilai tertentu; `amount` adalah bilangan bulat non-negatif.
// FS : Variabel internal `life` berkurang `amount` (pesan game over jika `life` <= 0).
void DecreaseLife(int amount);

#endif