/* File        : status.c 
* Deskripsi   : Implementasi untuk modul Stack Status (Status Stack). 
* Berisi semua logika untuk mengelola (Push, Pop) dan menampilkan (Draw) 
* tumpukan pesan status aksi pemain di layar. 
* Dibuat oleh  : Mohamad Jibril Fathi Al-ghifari
* Perubahan terakhir : Jumat, 13 Juni 2025
*/

#include "status.h"
#include "raymath.h"
#include "stdbool.h"
#include <string.h>

Stack statusStack;

/* IS : S adalah stack sembarang. */
/* FS : Membuat sebuah stack S yang kosong dan siap digunakan. */
void CreateStatus(Stack *S) {
    for (int i = 0; i < MAX_STACK_SIZE; i++) {
        S->messages[i][0] = '\0';
        S->alphas[i] = 0.0f;
        S->timers[i] = 0.0f;
        S->posX[i] = -400.0f; 
    }
}

/* Mengirim true jika stack status kosong (tidak ada pesan untuk ditampilkan). */
/* Dalam kasus kita, ini bisa digunakan untuk mengecek apakah ada pesan pertama. */
bool IsStackEmpty(Stack S) {
    
    return S.messages[0][0] == '\0';
}

static void Pop(Stack *S){
        for (int i = MAX_STACK_SIZE - 1; i > 0; i--) {
        strcpy(S->messages[i], S->messages[i-1]);
        S->alphas[i] = S->alphas[i-1];
        S->timers[i] = S->timers[i-1];
        S->posX[i] = S->posX[i-1]; 
    }
}

/* IS : S mungkin sudah berisi pesan. */
/* FS : 'message' menjadi TOP yang baru. Jika stack penuh, elemen paling bawah akan terhapus. */
void Push(Stack *S, const char* message) {
    Pop(S);
    
    strncpy(S->messages[0], message, MAX_MESSAGE_LENGTH - 1);
    S->messages[0][MAX_MESSAGE_LENGTH - 1] = '\0';
    S->alphas[0] = 1.0f;
    S->timers[0] = 3.0f; 
    S->posX[0] = -400.0f; 
}

/* IS : S adalah stack status. */
/* FS : Timer dan alpha dari setiap pesan di dalam S di-update. */
void UpdateStatus(Stack *S, float deltaTime) {
    float targetX = 20.0f; 
    float animSpeed = 10.0f; 

    for (int i = 0; i < MAX_STACK_SIZE; i++) {
        
        if (S->posX[i] < targetX) {
            S->posX[i] = Lerp(S->posX[i], targetX, animSpeed * deltaTime);
            
            if (targetX - S->posX[i] < 1.0f) {
                S->posX[i] = targetX;
            }
        }

        
        if (S->messages[i][0] != '\0') {
            if (S->timers[i] > 0) {
                S->timers[i] -= deltaTime;
            } else if (S->alphas[i] > 0) {
                S->alphas[i] -= deltaTime * 0.5f;
                if (S->alphas[i] < 0) S->alphas[i] = 0;
            }
        }
    }
}

/* 
I.S : Isi stack belum ditampilkan yang terbaru ke layar
F.S : Isi stack dirender ke layar, dengan elemen TOP di paling atas. */
void DrawStatus(Stack S) {
    int startY = 20;
    int fontSize = 20;
    int yStep = fontSize + 12; 

    for (int i = 0; i < MAX_STACK_SIZE; i++) {
        if (S.alphas[i] > 0) { 
            float marginX = S.posX[i]; 
            float currentY = startY + (i * yStep);
            
            
            float textWidth = MeasureText(S.messages[i], fontSize);
            
            
            DrawRectangleRec(
                (Rectangle){ marginX - 5, currentY - 3, textWidth + 10, (float)fontSize + 6 }, 
                Fade(BLACK, S.alphas[i] * 0.6f) 
            );
            
            
            DrawText(S.messages[i], marginX, currentY, fontSize, Fade(RAYWHITE, S.alphas[i]));
        }
    }
}