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

static void RemoveTopAndShiftUp(Stack *S) {
    // Geser setiap elemen ke atas satu posisi
    for (int i = 0; i < MAX_STACK_SIZE - 1; i++) {
        strcpy(S->messages[i], S->messages[i+1]);
        S->alphas[i] = S->alphas[i+1];
        S->timers[i] = S->timers[i+1];
        S->posX[i] = S->posX[i+1];
    }
    
    S->messages[MAX_STACK_SIZE - 1][0] = '\0';
    S->alphas[MAX_STACK_SIZE - 1] = 0.0f;
    S->timers[MAX_STACK_SIZE - 1] = 0.0f;
}

/* IS : S mungkin sudah berisi pesan. */
/* FS : 'message' menjadi TOP yang baru. Jika stack penuh, elemen paling bawah akan terhapus. */
void Push(Stack S, const char message) {
    bool isStackFull = (S->messages[MAX_STACK_SIZE - 1][0] != '\0');

    if (isStackFull) {
        RemoveTopAndShiftUp(S);
    }
    Pop(S);

    strncpy(S->messages[0], message, MAX_MESSAGE_LENGTH - 1);
    S->messages[0][MAX_MESSAGE_LENGTH - 1] = '\0';
    S->alphas[0] = 1.0f;
    S->timers[0] = 3.0f; 
    S->posX[0] = -400.0f;
}

/* IS : S adalah stack status. */
/* FS : Timer dan alpha dari setiap pesan di dalam S di-update. */
// File: tubes1/status.c
void UpdateStatus(Stack *S, float deltaTime) {
    // Animasikan posisi masuk untuk semua pesan yang terlihat
    for (int i = 0; i < MAX_STACK_SIZE; i++) {
        if (S->messages[i][0] == '\0') continue;
        
        float targetX = 20.0f;
        float animSpeed = 10.0f;
        if (S->posX[i] < targetX) {
            S->posX[i] = Lerp(S->posX[i], targetX, animSpeed * deltaTime);
            if (targetX - S->posX[i] < 1.0f) {
                S->posX[i] = targetX;
            }
        }
    }

    if (S->messages[0][0] != '\0') {
        if (S->timers[0] > 0) {
            S->timers[0] -= deltaTime;
        } 
        else if (S->alphas[0] > 0) {
            S->alphas[0] -= deltaTime * 1.0f; 
        if (S->alphas[0] <= 0) {
            RemoveTopAndShiftUp(S);
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