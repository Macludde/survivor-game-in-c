#include "debug.h"
#include "raylib.h"

int targetFps = 60;
void HandleDebuggingKeys()
{
    if (IsKeyPressed(KEY_F10))
    {
        if (targetFps == 60)
            targetFps = 40;
        else if (targetFps == 40)
            targetFps = 20;
        else if (targetFps == 20)
            targetFps = 0;
        else
            targetFps = 60;
        SetTargetFPS(targetFps);
    }
}