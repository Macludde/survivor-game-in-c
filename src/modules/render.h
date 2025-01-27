#pragma once
#include <stdbool.h>

#include "./movement.h"
#include "flecs.h"
#include "raylib.h"

// Components
typedef struct CircleShape {
  Vector2 offset;  // from center
  float radius;
  Color color;
} CircleShape;

typedef struct RectShape {
  Vector2 offset;  // from center
  Vector2 size;
  Color color;
  float rotation;
} RectShape;

typedef float Rotation;

extern ECS_COMPONENT_DECLARE(CircleShape);
extern ECS_COMPONENT_DECLARE(RectShape);
extern ECS_COMPONENT_DECLARE(Rotation);

extern ECS_SYSTEM_DECLARE(RenderCircle);
extern ECS_SYSTEM_DECLARE(RenderRect);
extern ECS_SYSTEM_DECLARE(RenderRotatedRect);

// The import function name has to follow the convention: <ModuleName>Import
void RenderImport(ecs_world_t *world);