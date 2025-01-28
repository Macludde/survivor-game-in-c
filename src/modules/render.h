#pragma once
#include <stdbool.h>

#include "./movement.h"
#include "flecs.h"
#include "raylib.h"

typedef struct CircleShape {
  Vector2 offset;
  Color color;
  float radius;
} CircleShape;
typedef struct RectShape {
  Vector2 offset;
  Color color;
  Vector2 size;
  float rotation;
} RectShape;

extern ECS_COMPONENT_DECLARE(CircleShape);
extern ECS_COMPONENT_DECLARE(RectShape);

// declares for each layer. BGname, name and FGName
#define EXTERN_DECLARE_RENDER_SYSTEM(name)   \
  extern ECS_SYSTEM_DECLARE(BGRender##name); \
  extern ECS_SYSTEM_DECLARE(Render##name);   \
  extern ECS_SYSTEM_DECLARE(FGRender##name);
#define DECLARE_RENDER_SYSTEM(name)   \
  ECS_SYSTEM_DECLARE(BGRender##name); \
  ECS_SYSTEM_DECLARE(Render##name);   \
  ECS_SYSTEM_DECLARE(FGRender##name);

EXTERN_DECLARE_RENDER_SYSTEM(RenderCircle);
EXTERN_DECLARE_RENDER_SYSTEM(RenderRect);

extern ECS_DECLARE(BackgroundRenderLayer);
extern ECS_DECLARE(ForegroundRenderLayer);

// The import function name has to follow the convention: <ModuleName>Import
void RenderImport(ecs_world_t *world);