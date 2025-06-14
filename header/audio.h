/* File        : audio.h 
* Deskripsi   : Deklarasi untuk modul Audio. 
*               Modul ini mengelola semua pemuatan, pemutaran, dan pembersihan aset audio game. 
* Dibuat oleh : Micky Ridho Pratama
* Perubahan terakhir : Sabtu, 7 Juni 2025
*/

#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"
#include "common.h" 

extern Music regularBacksound;
extern bool isRegularPlaying;

/**
 * I.S. : Perangkat audio belum siap dan musik belum dimuat.
 * F.S. : Perangkat audio siap dan semua stream musik telah dimuat ke memori.
 */
void InitGameAudio();

/**
 * I.S. : Stream musik memiliki state dari frame sebelumnya.
 * F.S. : Buffer stream musik yang aktif telah diperbarui.
 */
void UpdateGameAudio();

/**
 * I.S. : Aset musik sedang digunakan.
 * F.S. : Semua musik telah di-unload dan perangkat audio ditutup.
 */
void UnloadGameAudio();

/**
 * I.S. : Musik regular mungkin sedang tidak berjalan.
 * F.S. : Musik regular diputar, dan musik lainnya dihentikan.
 */
void PlayRegularMusic();

// Menghentikan semua musik yang sedang berjalan.
void StopAllMusic();

// Menghentikan semua musik KECUALI yang ditentukan.
void StopAllMusicExcept(Music musicToKeepPlaying);

// Mengecek apakah musik regular sedang diputar. true jika musik regular sedang diputar, false jika tidak.
bool IsRegularMusicPlaying();


#endif // AUDIO_H