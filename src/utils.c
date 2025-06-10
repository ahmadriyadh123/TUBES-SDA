#include "utils.h"
#include <string.h>
#include "raylib.h"

Texture2D LoadTextureSafe(const char *filePath) {
    Texture2D texture = {0};
    if (filePath == NULL || !FileExists(filePath)) {
        TraceLog(LOG_WARNING, "Failed to load texture: File %s not found or invalid path.", filePath ? filePath : "NULL");
        return texture;
    }

    texture = LoadTexture(filePath);
    if (texture.id == 0) {
        TraceLog(LOG_WARNING, "Failed to load texture from %s: Invalid texture ID.", filePath);
    } else {
        TraceLog(LOG_INFO, "Texture loaded successfully from %s.", filePath);
    }

    return texture;
}

void UnloadTextureSafe(Texture2D *texture) {
    if (texture == NULL || texture->id == 0) {
        TraceLog(LOG_WARNING, "Attempted to unload invalid or NULL texture.");
        return;
    }

    UnloadTexture(*texture);
    *texture = (Texture2D){0};
    TraceLog(LOG_INFO, "Texture unloaded successfully.");
}

bool FileExistsSafe(const char *filePath) {
    if (filePath == NULL) {
        TraceLog(LOG_WARNING, "File path is NULL in FileExistsSafe.");
        return false;
    }

    bool exists = FileExists(filePath);
    TraceLog(exists ? LOG_INFO : LOG_WARNING, "File %s %s.", filePath, exists ? "exists" : "does not exist");
    return exists;
}

void StrCopySafe(char *dest, const char *src, int maxLength) {
    if (dest == NULL || src == NULL || maxLength <= 0) {
        TraceLog(LOG_WARNING, "Invalid parameters in StrCopySafe: dest=%p, src=%p, maxLength=%d.", dest, src, maxLength);
        return;
    }

    strncpy(dest, src, maxLength - 1);
    dest[maxLength - 1] = '\0';
    TraceLog(LOG_INFO, "String copied: %s (maxLength=%d).", dest, maxLength);
}