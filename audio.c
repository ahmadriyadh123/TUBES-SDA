/* File        : audio.c
* Deskripsi   : Implementasi untuk modul Audio.
*
*  Penulis     : Micky Ridho Pratama
*  Perubahan terakhir: Sabtu, 7 Juni 2025
*/

#include "audio.h"
#include "utils.h"
#include "raylib.h"

Music regularBacksound;
Music battleBacksound;
bool isRegularPlaying = false;
bool isBattlePlaying = false;
static Sound sfxKill1;
static Sound sfxKill2;
static Sound sfxSpendMoney;
static int sfxKillToggle = 0;

/* 
 * void InitGameAudio()
 * IS : Sistem audio belum diinisialisasi, file audio belum dimuat.
 * FS : Sistem audio siap digunakan, file musik dan sound effect telah dimuat ke memori.
 */
void InitGameAudio()
{
    InitAudioDevice();  

    // Gunakan wrapper 'Safe' untuk keamanan
    regularBacksound = LoadMusicStreamSafe("assets/audio/regular.wav");
    battleBacksound = LoadMusicStreamSafe("assets/audio/battle.wav");
    sfxKill1 = LoadSound("assets/audio/kill1.mp3");
    sfxKill2 = LoadSound("assets/audio/kill2.mp3");
    sfxSpendMoney = LoadSound("assets/audio/spendmoney.mp3");

    if (regularBacksound.stream.buffer != NULL) SetMusicVolume(regularBacksound, 2.0f);
    if (battleBacksound.stream.buffer != NULL) SetMusicVolume(battleBacksound, 2.0f);
    TraceLog(LOG_INFO, "AUDIO: Game audio initialized.");
}

/* 
 * void UpdateGameAudio()
 * IS : Musik yang sedang diputar perlu diperbarui untuk streaming.
 * FS : Stream audio yang sedang aktif diperbarui agar tetap terdengar tanpa terputus.
 */
void UpdateGameAudio()
{
    // Cukup update stream yang sedang berjalan
    if (isRegularPlaying && regularBacksound.stream.buffer != NULL) {
        UpdateMusicStream(regularBacksound);
    }
    if (isBattlePlaying && battleBacksound.stream.buffer != NULL) {
        UpdateMusicStream(battleBacksound);
    }
}

/* 
 * void UnloadGameAudio()
 * IS : Musik dan sound effect masih aktif atau dimuat dalam memori.
 * FS : Semua musik dan sound effect dilepaskan dari memori, sistem audio dimatikan.
 */
void UnloadGameAudio()
{
    StopAllMusic();
    UnloadMusicStreamSafe(&regularBacksound);
    UnloadMusicStreamSafe(&battleBacksound);
    UnloadSound(sfxKill1);
    UnloadSound(sfxKill2);
    UnloadSound(sfxSpendMoney);
    CloseAudioDevice();
    TraceLog(LOG_INFO, "AUDIO: Game audio unloaded.");
}

/*
 * void StopAllMusicExcept(Music musicToKeepPlaying)
 * IS : Ada musik yang sedang berjalan (regular atau battle).
 * FS : Semua musik dihentikan kecuali musik yang ditentukan (musicToKeepPlaying).
 */
void StopAllMusicExcept(Music musicToKeepPlaying)
{
    if (regularBacksound.stream.buffer != musicToKeepPlaying.stream.buffer && IsMusicStreamPlaying(regularBacksound)) {
        StopMusicStream(regularBacksound);
        isRegularPlaying = false;
    }
    if (battleBacksound.stream.buffer != musicToKeepPlaying.stream.buffer && IsMusicStreamPlaying(battleBacksound)) {
        StopMusicStream(battleBacksound);
        isBattlePlaying = false;
    }
}

/* 
 * void StopAllMusic()
 * IS : Salah satu atau kedua musik (regular/battle) mungkin sedang diputar.
 * FS : Semua musik dihentikan, status pemutaran di-reset.
 */
void StopAllMusic()
{
    if (IsMusicStreamPlaying(regularBacksound)) StopMusicStream(regularBacksound);
    if (IsMusicStreamPlaying(battleBacksound)) StopMusicStream(battleBacksound);
    isRegularPlaying = false;
    isBattlePlaying = false;
}

/* 
 * void PlayRegularMusic()
 * IS : Musik belum diputar atau musik yang diputar bukan regular.
 * FS : Musik regular diputar, musik lainnya dihentikan.
 */
void PlayRegularMusic()
{
    if (regularBacksound.stream.buffer != NULL && !IsMusicStreamPlaying(regularBacksound))
    {
        StopAllMusicExcept(regularBacksound);
        PlayMusicStream(regularBacksound);
        isRegularPlaying = true;
        isBattlePlaying = false;
        TraceLog(LOG_INFO, "AUDIO: Playing regular music.");
    }
}

/* 
 * void PlayBattleMusic()
 * IS : Musik belum diputar atau musik yang diputar bukan battle.
 * FS : Musik battle diputar, musik lainnya dihentikan.
 */
void PlayBattleMusic()
{
    if (battleBacksound.stream.buffer != NULL && !IsMusicStreamPlaying(battleBacksound))
    {
        StopAllMusicExcept(battleBacksound);
        PlayMusicStream(battleBacksound);
        isBattlePlaying = true;
        isRegularPlaying = false;
        TraceLog(LOG_INFO, "AUDIO: Playing battle music.");
    }
}

/* 
 * void PlayEnemyDefeatedSound()
 * IS : Tidak ada efek suara "enemy defeated" yang sedang dimainkan.
 * FS : Efek suara "enemy defeated" dimainkan secara bergantian (toggle antara dua file).
 */
void PlayEnemyDefeatedSound(void)
{
    // Gunakan toggle untuk memilih suara mana yang akan diputar
    if (sfxKillToggle == 0) {
        PlaySound(sfxKill1);
    } else {
        PlaySound(sfxKill2);
    }
    // Balik nilai toggle untuk pemanggilan berikutnya
    sfxKillToggle = 1 - sfxKillToggle;
}

/* 
 * void PlaySpendMoneySound()
 * IS : Tidak ada efek suara pengeluaran uang yang sedang dimainkan.
 * FS : Efek suara pengeluaran uang dimainkan.
 */
void PlaySpendMoneySound()
{
        PlaySound(sfxSpendMoney);

}

/* 
 * bool IsRegularMusicPlaying()
 * IS/FS : Mengecek status pemutaran musik regular.
 * return : true jika musik regular sedang diputar, false jika tidak.
 */
bool IsRegularMusicPlaying() {
    return isRegularPlaying;
}

/* 
 * bool IsBattleMusicPlaying()
 * IS/FS : Mengecek status pemutaran musik battle.
 * return : true jika musik battle sedang diputar, false jika tidak.
 */
bool IsBattleMusicPlaying() {
    return isBattlePlaying;
}