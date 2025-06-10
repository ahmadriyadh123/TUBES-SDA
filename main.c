#include "raylib.h"
#include "common.h" 
#include "state.h"  
#include <stdio.h>

int customWaveCount = 3; 

int main() {
    
    InitWindow(2560, 1575, "Tower Defense");
    SetTargetFPS(60); 
    
    SetTraceLogLevel(LOG_DEBUG); 
    
    InitGameAudio(); 
    InitAllGameModules(); 

    while (!WindowShouldClose() && currentGameState != EXITING) {
        float deltaTime = GetFrameTime(); 

        BeginDrawing();
        ClearBackground(RAYWHITE);
      
        UpdateGameState(deltaTime); 
        UpdateGameAudio(currentGameState); 
        
   
        DrawGameState(); 

        EndDrawing(); 
    }
    UnloadAllGameModules(); 
    CloseAudioDevice(); 
    CloseWindow();
    return 0; 
}