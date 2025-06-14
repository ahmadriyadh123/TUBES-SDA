/* File        : audio.c */
/* Deskripsi   : Implementasi untuk modul Audio. */

#include "audio.h"
#include "utils.h"

Music regularBacksound;
bool isRegularPlaying = false;

void InitGameAudio()
{
    InitAudioDevice();  

    // Gunakan wrapper 'Safe' untuk keamanan
    regularBacksound = LoadMusicStreamSafe("assets/audio/regular.wav");

    if (regularBacksound.stream.buffer != NULL) SetMusicVolume(regularBacksound, 1.0f);
    TraceLog(LOG_INFO, "AUDIO: Game audio initialized.");
}

void UpdateGameAudio()
{
    // Cukup update stream yang sedang berjalan
    if (isRegularPlaying && regularBacksound.stream.buffer != NULL) {
        UpdateMusicStream(regularBacksound);
    }
}

void UnloadGameAudio()
{
    StopAllMusic();
    UnloadMusicStreamSafe(&regularBacksound);
    UnloadMusicStreamSafe(&battleBacksound);
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

bool IsRegularMusicPlaying() {
    return isRegularPlaying;
}
