#ifndef UTILS_H
#define UTILS_H
#include "raylib.h"

Texture2D LoadTextureSafe(const char *filePath);
void UnloadTextureSafe(Texture2D *texture);
bool FileExistsSafe(const char *filePath);
void StrCopySafe(char *dest, const char *src, int maxLength);

#endif