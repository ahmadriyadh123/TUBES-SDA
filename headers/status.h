/* File        : status.h 
* Deskripsi   : Deklarasi stack untuk menampilkan status aksi pemain. 
*               Diimplementasikan dengan array statis. 
* Dibuat oleh  : Mohamad Jibril Fathi Al-ghifari
* Perubahan terakhir : Jumat, 13 Juni 2025
*/

#ifndef STATUS_H
#define STATUS_H

#include "raylib.h"
#include <stdbool.h>

#define MAX_STACK_SIZE 4
#define MAX_MESSAGE_LENGTH 128


typedef char infotype[MAX_MESSAGE_LENGTH];

typedef struct {
    infotype messages[MAX_STACK_SIZE];
    float alphas[MAX_STACK_SIZE];
    float timers[MAX_STACK_SIZE];
    float posX[MAX_STACK_SIZE]; 
} Stack;

extern Stack statusStack;

/* IS : S adalah stack sembarang. */
/* FS : Membuat sebuah stack S yang kosong dan siap digunakan. */
void CreateStatus(Stack *S);

/* Mengirim true jika stack status kosong (tidak ada pesan untuk ditampilkan). */
/* Dalam kasus kita, ini bisa digunakan untuk mengecek apakah ada pesan pertama. */
bool IsStackEmpty(Stack S);

/* Menambahkan 'message' sebagai elemen baru di puncak (TOP) stack S. */
/* IS : S mungkin sudah berisi pesan. */
/* FS : 'message' menjadi TOP yang baru. Jika stack penuh, elemen paling bawah akan terhapus. */
void Push(Stack *S, const char* message);

/* IS : S adalah stack status. */
/* FS : Timer dan alpha dari setiap pesan di dalam S di-update. */
void UpdateStatus(Stack *S, float deltaTime);

/* Menggambar isi stack ke layar, dengan elemen TOP di paling atas. */
void DrawStatus(Stack S);

#endif 