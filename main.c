/*
 * File: main.c
 * Description:
 *   Titik masuk utama (entry point) dari program game Tower Defense 2D.
 *   File ini bertanggung jawab untuk menginisialisasi window, memulai game loop,
 *   dan memanggil fungsi-fungsi utama seperti penggambaran peta, penanganan input,
 *   dan penggambaran menara.
 *
 *   Fitur utama:
 *     - Inisialisasi window dengan resolusi 1600x1400.
 *     - Pengaturan frame rate ke 60 FPS.
 *     - Loop utama game yang mencakup:
 *         - Penanganan input mouse untuk menempatkan menara.
 *         - Penggambaran peta dan menara secara real-time.
 *     - Pemuatan dan pembebasan resource menggunakan fungsi dari `map.h`.
*/

#include "raylib.h"
#include "map.h"

int main(void)
{
    InitWindow(TILE_SIZE * TILE_SCALE * COLS , TILE_SIZE * TILE_SCALE * ROWS, "Tower Defense with Spritesheet");
    SetTargetFPS(60);

    InitResources();
    while (!WindowShouldClose())
    {
        handleInput();
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawMap();
        DrawTowers();
        DrawGameTimer(TILE_SCALE, 0.0f, 0.0f, timerMapRow, timerMapCol);
        DrawDecorations(TILE_SCALE, 0.0f, 0.0f);

        EndDrawing();
    }

    UnloadResources();
    CloseWindow();
    return 0;
}