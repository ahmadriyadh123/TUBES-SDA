#include "player_resources.h"
#include "raylib.h"

static int money = 0;
static int life = 10;

// Menginisialisasi uang pemain.
// IS : `money` bisa memiliki nilai apa saja sebelum fungsi dipanggil.
// FS : `money` diatur menjadi 0.
void InitEconomy() {
    money = 0;
    TraceLog(LOG_INFO, "Ekonomi diinisialisasi.");
}

// Menambahkan jumlah tertentu ke uang pemain.
// IS : `money` memiliki nilai tertentu; `amount` adalah bilangan bulat.
// FS : `money` bertambah `amount` (dibulatkan menjadi 0 jika hasilnya negatif).
void AddMoney(int amount) {
    money += amount;
    if (money < 0) money = 0;
}

// Mendapatkan jumlah uang pemain saat ini.
// IS : `money` memiliki nilai tertentu.
// FS : Mengembalikan nilai `money`.
int GetMoney() {
    return money;
}

// Mengatur uang pemain ke jumlah tertentu.
// IS : `money` memiliki nilai tertentu; `amount` adalah bilangan bulat.
// FS : `money` diatur menjadi `amount` (dibulatkan menjadi 0 jika `amount` negatif).
void SetMoney(int amount) {
    money = amount >= 0 ? amount : 0;
    TraceLog(LOG_INFO, "Uang diatur menjadi: $%d.", money);
}

// ---
// ## Modul Status Permainan

// Menginisialisasi status permainan.
// IS : `life` bisa memiliki nilai apa saja sebelum fungsi dipanggil.
// FS : `life` diatur menjadi 10.
void InitGameState() {
    life = 10;
    TraceLog(LOG_INFO, "Status permainan diinisialisasi dengan %d nyawa.", life);
}

// Mengurangi nyawa pemain sebanyak satu.
// IS : `life` memiliki nilai tertentu.
// FS : `life` berkurang 1 (dibulatkan menjadi 0 jika hasilnya negatif).
void LoseLife() {
    life--;
    if (life < 0) life = 0;
    TraceLog(LOG_INFO, "Kehilangan satu nyawa. Sisa nyawa: %d.", life);
}

// Memeriksa apakah permainan sudah berakhir.
// IS : `life` memiliki nilai tertentu.
// FS : Mengembalikan `true` jika `life` <= 0, jika tidak `false`.
bool IsGameOver() {
    bool gameOver = life <= 0;
    if (gameOver) {
        TraceLog(LOG_INFO, "Permainan berakhir: Tidak ada nyawa tersisa.");
    }
    return gameOver;
}

// Mendapatkan jumlah nyawa pemain saat ini.
// IS : `life` memiliki nilai tertentu.
// FS : Mengembalikan nilai `life`.
int GetLife() {
    return life;
}

// Mengatur nyawa pemain ke jumlah tertentu.
// IS : `life` memiliki nilai tertentu; `amount` adalah bilangan bulat.
// FS : `life` diatur menjadi `amount` (dibulatkan menjadi 0 jika `amount` negatif).
void SetLife(int amount) {
    life = amount >= 0 ? amount : 0;
    TraceLog(LOG_INFO, "Nyawa diatur menjadi: %d.", life);
}

// Menggambar jumlah nyawa saat ini di layar.
// IS : Konteks gambar Raylib aktif; `life` memiliki nilai tertentu.
// FS : Teks "Life: [current_life]" digambar di layar.
void DrawLife() {
    char text[32];
    snprintf(text, sizeof(text), "Life: %d", life);
    DrawText(text, 10, 40, 20, BLACK);
}

// Mengurangi nyawa pemain sebesar jumlah tertentu.
// IS : `life` memiliki nilai tertentu; `amount` adalah bilangan bulat non-negatif.
// FS : `life` berkurang `amount` (pesan game over jika `life` <= 0).
void DecreaseLife(int amount) {
    life -= amount;
    if (life <= 0) {
        TraceLog(LOG_INFO, "Permainan berakhir: Nyawa mencapai nol.");
    }
}