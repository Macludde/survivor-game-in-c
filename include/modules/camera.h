#pragma once
#include <stdbool.h>

#include "flecs.h"
#include "raylib.h"

// declare the Component type

extern ECS_SYSTEM_DECLARE(Begin2D);
extern ECS_SYSTEM_DECLARE(End2D);
extern ECS_SYSTEM_DECLARE(CameraHandleWindowResize);
#ifdef DEBUG
extern ECS_SYSTEM_DECLARE(MouseScrollZoom);
#endif

typedef struct Area {
  Vector2 topLeft;
  Vector2 bottomRight;
} Area;

void CameraImport(ecs_world_t *world);

bool IsPointOnScreen(Vector2 pos);
bool IsCircleOnScreen(Vector2 pos, float radius);
bool IsRectOnScreen(Vector2 pos, Vector2 size);

Vector2 RandomPointOffScreen(float minDistance, float maxDistance);
Vector2 ClampedRandomPointOffScreen(float minDistance, float maxDistance,
                                    Area clamp);