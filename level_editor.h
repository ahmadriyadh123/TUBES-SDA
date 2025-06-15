/* File        : level_editor.h
* Deskripsi   : Deklarasi untuk modul Level Editor.
*               Modul ini berisi semua tipe data, state, dan prototipe fungsi
*               yang diperlukan untuk membuat dan mengedit peta permainan secara interaktif. 
* Dibuat oleh  : Mohamad Jibril Fathi Al-ghifari
* Perubahan terakhir : Rabu, 11 Juni 2025
*/

#ifndef LEVEL_EDITOR_H
#define LEVEL_EDITOR_H

#include "raylib.h"
#include "map.h"

#define EDITOR_BUTTON_WIDTH_FACTOR 1.0f
#define EDITOR_BUTTON_HEIGHT_FACTOR 1.0f
#define EDITOR_BUTTON_SPACING_FACTOR 0.2f
#define EDITOR_PANEL_WIDTH_FACTOR 0.3f
#define EDITOR_PANEL_HEIGHT_FACTOR 0.3f
#define EDITOR_PANEL_PADDING 50.0f
#define EDITOR_WAVE_BUTTON_SIZE 50.0f
#define EDITOR_VIEW_PADDING_TOP_FACTOR 0.1f
#define EDITOR_VIEW_PADDING_BOTTOM_FACTOR 0.2f
#define EDITOR_VIEW_PADDING_SIDE_FACTOR 0.05f

/* Enum untuk menentukan alat (tool) yang sedang aktif di level editor. */
typedef enum
{
    TOOL_NONE,
    TOOL_PATH,
    TOOL_TOWER,
    TOOL_WAVE,
    TOOL_START_POINT 
} EditorTool;

/* ADT untuk menyimpan keseluruhan state dari Level Editor saat ini. */
/* Struct ini menampung semua data yang dibutuhkan selama sesi editing. */
typedef struct
{
    int map[MAP_ROWS][MAP_COLS];
    int baseGameMap[MAP_ROWS][MAP_COLS];
    EditorTool selectedTool;
    bool wavePanelActive;
    int startRow; 
    int startCol;
    int waveCount;
    bool isDraggingPath;
    Texture2D pathButtonTex;
    Texture2D towerButtonTex;
    Texture2D waveButtonTex;
    Texture2D startPointButtonTex; 
    Texture2D minusButtonTex;
    Texture2D plusButtonTex;
    Texture2D okButtonTex;
    Texture2D saveButtonTex;
    bool requestSaveAndPlay;
    char mapFileName[256];
    char mapDisplayName[64]; 
    char textInputBuffer[64]; 
    int letterCount;       
} LevelEditorState;

extern int customWaveCount;
extern LevelEditorState editorState;

/* I.S. : State level editor belum terinisialisasi.
   F.S. : Seluruh state editor, termasuk salinan peta, tool yang dipilih, dan aset UI,
          telah diinisialisasi dan siap digunakan. */
void InitializeLevelEditor(const int current_game_map[MAP_ROWS][MAP_COLS]);

/* I.S. : Aset-aset untuk level editor (seperti tekstur tombol) sedang dimuat di memori.
   F.S. : Semua aset yang dialokasikan untuk level editor telah dibebaskan dari memori. */
void UnloadLevelEditor();

/* I.S. : Level editor dalam keadaan menunggu input dari pengguna (misalnya, klik mouse).
   F.S. : Input dari pengguna telah diproses. State dari peta editor (misalnya, penambahan
          tile jalur atau tower) atau state editor (misalnya, tool yang dipilih) telah diperbarui. */
void HandleLevelEditorInput(float globalScale, float offsetX, float offsetY);

/* I.S. : State level editor telah terdefinisi (peta, tool, dll.).
   F.S. : Seluruh antarmuka pengguna (UI) untuk level editor, termasuk grid peta dan
          tombol-tombol alat, telah digambar ke layar. */
void DrawLevelEditor(float globalScale, float offsetX, float offsetY);

/* I.S. : State editor berisi data peta lama atau kosong.
   F.S. : State editor (peta, wave count, start point) telah diisi dengan data yang dibaca
          dari 'fileName'. Mengembalikan true jika berhasil, false jika gagal. */
bool LoadLevelFromFile(const char *fileName);

/* I.S. : State editor berisi data peta yang akan disimpan.
   F.S. : Data peta dari state editor telah ditulis dan disimpan ke dalam file 'fileName'. */
void SaveLevelToFile(const char *fileName);

/* Mengirimkan nilai enum dari tool yang sedang aktif dipilih oleh pemain. */
EditorTool GetEditorSelectedTool();

/* Mengirimkan status boolean dari flag 'requestSaveAndPlay'. */
bool GetEditorRequestSaveAndPlay();

/* Mengirimkan ID tile pada posisi (row, col) dari peta yang ada di editor. */
int GetEditorMapTile(int row, int col);

/* Mengirimkan path file dari peta yang sedang dikerjakan. */
const char *GetEditorMapFileName();

/* Mengirimkan jumlah gelombang yang telah diatur di editor. */
int GetEditorWaveCount();

/* Mengirimkan posisi baris titik awal musuh yang telah diatur. */
int GetEditorStartRow(); 

/* Mengirimkan posisi kolom titik awal musuh yang telah diatur. */
int GetEditorStartCol(); 

/* I.S. : Tool yang aktif adalah tool sebelumnya.
   F.S. : State tool yang aktif diubah menjadi 'tool'. */
void SetEditorSelectedTool(EditorTool tool);

/* I.S. : Panel pengaturan gelombang memiliki status visibilitas lama.
   F.S. : Status visibilitas panel pengaturan gelombang diatur menjadi 'active'. */
void SetEditorWavePanelActive(bool active);

/* I.S. : Flag 'requestSaveAndPlay' memiliki nilai lama.
   F.S. : Nilai flag 'requestSaveAndPlay' diatur menjadi 'value'. */
void SetEditorRequestSaveAndPlay(bool value);

/* I.S. : Tile di posisi (row, col) pada peta editor memiliki nilai lama.
   F.S. : Nilai tile di posisi (row, col) diubah menjadi 'value'. */
void SetEditorMapTile(int row, int col, int value);

/* I.S. : State editor menyimpan path file yang lama.
   F.S. : Path file kerja di dalam state editor diubah menjadi 'fileName'. */
void SetEditorMapFileName(const char *fileName);

/* I.S. : Jumlah gelombang memiliki nilai lama.
   F.S. : Jumlah gelombang di state editor diatur menjadi 'count' (dengan validasi minimal 1). */
void SetEditorWaveCount(int count);

/* I.S. : Posisi baris titik awal memiliki nilai lama.
   F.S. : Posisi baris titik awal di state editor diatur menjadi 'row'. */
void SetEditorStartRow(int row); 

/* I.S. : Posisi kolom titik awal memiliki nilai lama.
   F.S. : Posisi kolom titik awal di state editor diatur menjadi 'col'. */
void SetEditorStartCol(int col); 

#endif