/* File        : utils.c 
* Deskripsi   : Implementasi untuk modul Utilitas (Utilities). 
* Berisi semua logika dari fungsi-fungsi pembantu, seperti pengecekan file, 
* pemuatan aset yang aman, dan operasi penyalinan string yang aman. 
* Dibuat oleh : Ahmad Riyadh Almaliki
* Perubahan terakhir : Sabtu, 14 Juni 2025
*/

#include "utils.h"
#include <string.h>
#include "raylib.h"

/* I.S. : filePath adalah path ke sebuah file gambar.
   F.S. : Mengembalikan Texture2D yang valid jika file berhasil dimuat. 
          Jika file tidak ada atau gagal dimuat, mengembalikan Texture2D kosong ({0}) 
          dan mencatat sebuah peringatan (warning). */
Texture2D LoadTextureSafe(const char *filePath) {
    Texture2D texture = {0};
    if (filePath == NULL || !FileExists(filePath)) {
        TraceLog(LOG_WARNING, "Failed to load texture: File %s not found or invalid path.", filePath ? filePath : "NULL");
        return texture;
    }

    texture = LoadTexture(filePath);
    if (texture.id == 0) {
        TraceLog(LOG_WARNING, "Failed to load texture from %s: Invalid texture ID.", filePath);
    } else {
        TraceLog(LOG_INFO, "Texture loaded successfully from %s.", filePath);
    }

    return texture;
}

/* I.S. : texture adalah pointer ke sebuah Texture2D yang mungkin valid atau tidak.
   F.S. : Jika tekstur valid, memorinya dibebaskan dan pointernya di-reset. 
          Jika tidak valid, tidak terjadi apa-apa selain mencatat peringatan. */
void UnloadTextureSafe(Texture2D *texture) {
    if (texture == NULL || texture->id == 0) {
        TraceLog(LOG_WARNING, "Attempted to unload invalid or NULL texture.");
        return;
    }

    UnloadTexture(*texture);
    *texture = (Texture2D){0};
    TraceLog(LOG_INFO, "Texture unloaded successfully.");
}

/* Mengirimkan true jika file pada 'filePath' ada, dan false jika tidak ada atau jika 'filePath' NULL. */
bool FileExistsSafe(const char *filePath) {
    if (filePath == NULL) {
        TraceLog(LOG_WARNING, "File path is NULL in FileExistsSafe.");
        return false;
    }

    bool exists = FileExists(filePath);
    TraceLog(exists ? LOG_INFO : LOG_WARNING, "File %s %s.", filePath, exists ? "exists" : "does not exist");
    return exists;
}

/* I.S. : filePath adalah path ke sebuah file musik.
F.S. : Mengembalikan stream Music yang valid jika file berhasil dimuat. 
Jika tidak, mengembalikan struct Music kosong dan mencatat peringatan. */
Music LoadMusicStreamSafe(const char *filePath) {
    // Inisialisasi struct Music ke nol sebagai nilai default jika gagal
    Music music = {0}; 

    // Pengecekan keamanan: pastikan filePath tidak NULL dan file-nya ada
    if (filePath == NULL || !FileExists(filePath)) {
        TraceLog(LOG_WARNING, "AUDIO: Failed to load music: File %s not found or invalid path.", filePath ? filePath : "NULL");
        return music; // Kembalikan struct music kosong jika gagal
    }

    // Panggil fungsi asli Raylib untuk memuat stream
    music = LoadMusicStream(filePath);
    
    // Pengecekan keberhasilan: stream yang valid akan memiliki buffer
    if (music.stream.buffer == NULL) {
        TraceLog(LOG_WARNING, "AUDIO: Failed to load music stream from %s: Invalid buffer.", filePath);
    } else {
        TraceLog(LOG_INFO, "AUDIO: Music stream loaded successfully from %s.", filePath);
    }

    return music;
}

/* I.S. : music adalah pointer ke sebuah struct Music.
F.S. : Jika stream musik valid, sumber dayanya dibebaskan dan pointernya di-reset. */
void UnloadMusicStreamSafe(Music *music) {
    // Pengecekan keamanan: pastikan pointer music dan buffer-nya tidak NULL
    if (music == NULL || music->stream.buffer == NULL) {
        TraceLog(LOG_WARNING, "AUDIO: Attempted to unload invalid or NULL music stream.");
        return; // Keluar dari fungsi jika tidak valid
    }

    // Panggil fungsi asli Raylib untuk unload
    UnloadMusicStream(*music);

    // Reset struct ke nol untuk menandakan bahwa musik sudah tidak valid
    *music = (Music){0}; 
    TraceLog(LOG_INFO, "AUDIO: Music stream unloaded successfully.");
}

/* I.S. : 'dest' adalah buffer tujuan, 'src' adalah string sumber.
   F.S. : Menyalin string dari 'src' ke 'dest' dengan aman, memastikan tidak ada buffer overflow
          dan 'dest' selalu diakhiri dengan null-terminator. */
void StrCopySafe(char *dest, const char *src, int maxLength) {
    if (dest == NULL || src == NULL || maxLength <= 0) {
        TraceLog(LOG_WARNING, "Invalid parameters in StrCopySafe: dest=%p, src=%p, maxLength=%d.", dest, src, maxLength);
        return;
    }

    strncpy(dest, src, maxLength - 1);
    dest[maxLength - 1] = '\0';
    TraceLog(LOG_INFO, "String copied: %s (maxLength=%d).", dest, maxLength);
}