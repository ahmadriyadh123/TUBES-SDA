/* File        : utils.h 
* Deskripsi   : Deklarasi untuk modul Utilitas (Utilities). 
*               Modul ini berisi kumpulan fungsi-fungsi pembantu (helper functions) 
*               yang bersifat umum dan sebagai debugging
* Dibuat oleh : Ahmad Riyadh Almaliki
* Perubahan terakhir : Minggu, 1 Juni 2025

*/
#ifndef UTILS_H
#define UTILS_H
#include "raylib.h"

/* I.S. : filePath adalah path ke sebuah file gambar.
   F.S. : Mengembalikan Texture2D yang valid jika file berhasil dimuat. 
          Jika file tidak ada atau gagal dimuat, mengembalikan Texture2D kosong ({0}) 
          dan mencatat sebuah peringatan (warning). */
Texture2D LoadTextureSafe(const char *filePath);

/* I.S. : texture adalah pointer ke sebuah Texture2D yang mungkin valid atau tidak.
   F.S. : Jika tekstur valid, memorinya dibebaskan dan pointernya di-reset. 
          Jika tidak valid, tidak terjadi apa-apa selain mencatat peringatan. */
void UnloadTextureSafe(Texture2D *texture);

/* I.S. : filePath adalah path ke sebuah file musik.
F.S. : Mengembalikan stream Music yang valid jika file berhasil dimuat. 
Jika tidak, mengembalikan struct Music kosong dan mencatat peringatan. */
Music LoadMusicStreamSafe(const char *filePath);

/* I.S. : music adalah pointer ke sebuah struct Music.
F.S. : Jika stream musik valid, sumber dayanya dibebaskan dan pointernya di-reset. */
void UnloadMusicStreamSafe(Music *music);

/* Mengirimkan true jika file pada 'filePath' ada, dan false jika tidak ada atau jika 'filePath' NULL. */
bool FileExistsSafe(const char *filePath);

/* I.S. : 'dest' adalah buffer tujuan, 'src' adalah string sumber.
   F.S. : Menyalin string dari 'src' ke 'dest' dengan aman, memastikan tidak ada buffer overflow
          dan 'dest' selalu diakhiri dengan null-terminator. */
void StrCopySafe(char *dest, const char *src, int maxLength);

#endif