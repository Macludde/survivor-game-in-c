#pragma once

#include "flecs.h"
#include "raylib.h"

// Components
typedef Vector2 Position;
typedef Vector2 Velocity;
typedef Vector2 Acceleration;
typedef struct ArcMotion {
  Vector2 from;
  Vector2 to;
  float progress;
  void (*onComplete)(ecs_world_t *world, ecs_entity_t e);
} ArcMotion;

typedef float MaxSpeed;
#define DEFAULT_FRICTION 1.5f
typedef float Friction;
typedef float Rotation;

extern ECS_COMPONENT_DECLARE(Position);
extern ECS_COMPONENT_DECLARE(Velocity);
extern ECS_COMPONENT_DECLARE(Acceleration);
extern ECS_COMPONENT_DECLARE(ArcMotion);
extern ECS_COMPONENT_DECLARE(MaxSpeed);
extern ECS_COMPONENT_DECLARE(Friction);
extern ECS_COMPONENT_DECLARE(Rotation);

extern ECS_SYSTEM_DECLARE(MovementMove);
extern ECS_SYSTEM_DECLARE(MovementMoveInArc);
extern ECS_SYSTEM_DECLARE(MovementAccelerate);
extern ECS_SYSTEM_DECLARE(CapVelocity);
extern ECS_SYSTEM_DECLARE(MovementFriction);

// The import function name has to follow the convention: <ModuleName>Import
void MovementImport(ecs_world_t *world);