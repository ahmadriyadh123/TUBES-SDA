#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "raylib.h"

typedef enum
{
    MAIN_MENU,
    GAMEPLAY,
    LEVEL_EDITOR,
    SETTINGS, // Digunakan untuk "GUIDE"
    LEVEL_COMPLETE,
    GAME_OVER,
    EXITING,
} GameState;

extern GameState currentGameState;

extern Texture2D backgroundTex;
extern Texture2D menuBgTex; // Ini akan menjadi logo "TOWER DEFENSE" utama Anda

// Tidak lagi memerlukan tekstur tombol individual jika digambar secara prosedural
// extern Texture2D startButtonTex;
// extern Texture2D levelEditorButtonTex;
// extern Texture2D settingsButtonTex; // Tombol "GUIDE" akan menggunakan state SETTINGS
// extern Texture2D exitButtonTex;

void LoadMainMenuResources();
void UnloadMainMenuResources();
void DrawMainMenu();
void HandleMainMenuInput();

#endif // MAIN_MENU_H