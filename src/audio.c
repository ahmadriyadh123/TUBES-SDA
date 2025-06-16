/* File        : audio.c */
/* Deskripsi   : Implementasi untuk modul Audio. */

#include "audio.h"
#include "utils.h"
#include "raylib.h"

Music regularBacksound;
Music battleBacksound;
bool isRegularPlaying = false;
bool isBattlePlaying = false;
static Sound sfxKill1;
static Sound sfxKill2;
static Music sfxSpendMoney;
static int sfxKillToggle = 0;

void InitGameAudio()
{
    InitAudioDevice();  

    // Gunakan wrapper 'Safe' untuk keamanan
    regularBacksound = LoadMusicStreamSafe("assets/audio/regular.wav");
    battleBacksound = LoadMusicStreamSafe("assets/audio/battle.wav");
    sfxKill1 = LoadSound("assets/audio/kill1.mp3");
    sfxKill2 = LoadSound("assets/audio/kill2.mp3");
    sfxSpendMoney = LoadMusicStreamSafe("assets/audio/spendmoney.mp3");

    if (regularBacksound.stream.buffer != NULL) SetMusicVolume(regularBacksound, 2.0f);
    if (battleBacksound.stream.buffer != NULL) SetMusicVolume(battleBacksound, 2.0f);
    if (sfxSpendMoney.stream.buffer != NULL && IsMusicStreamPlaying(sfxSpendMoney)) {
        UpdateMusicStream(sfxSpendMoney);
    }
    TraceLog(LOG_INFO, "AUDIO: Game audio initialized.");
}

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

void UnloadGameAudio()
{
    StopAllMusic();
    UnloadMusicStreamSafe(&regularBacksound);
    UnloadMusicStreamSafe(&battleBacksound);
    UnloadSound(sfxKill1);
    UnloadSound(sfxKill2);
    UnloadMusicStreamSafe(&sfxSpendMoney);
    CloseAudioDevice();
    TraceLog(LOG_INFO, "AUDIO: Game audio unloaded.");
}

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

void StopAllMusic()
{
    if (IsMusicStreamPlaying(regularBacksound)) StopMusicStream(regularBacksound);
    if (IsMusicStreamPlaying(battleBacksound)) StopMusicStream(battleBacksound);
    isRegularPlaying = false;
    isBattlePlaying = false;
}

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

/**
 * @brief Implementasi fungsi baru untuk memutar SFX transaksi uang.
 */
void PlaySpendMoneySound(void)
{
    if (sfxSpendMoney.stream.buffer != NULL) {
        // Hentikan jika sedang diputar, agar bisa diputar dari awal lagi
        StopMusicStream(sfxSpendMoney);
        PlayMusicStream(sfxSpendMoney);
    }
}

bool IsRegularMusicPlaying() {
    return isRegularPlaying;
}

bool IsBattleMusicPlaying() {
    return isBattlePlaying;
}