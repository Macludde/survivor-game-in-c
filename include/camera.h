#pragma once
#include <stdbool.h>

#include "raylib.h"

#define CAMERA_MOVEMENT_SPEED 2.0f
void TickCamera(Vector2 playerPos);

bool IsPointOnScreen(Vector2 pos);
bool IsCircleOnScreen(Vector2 pos, float radius);
bool IsRectOnScreen(Vector2 pos, Vector2 size);