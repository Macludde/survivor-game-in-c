#pragma once

#include "flecs.h"
#include "raylib.h"

// Components
typedef Vector2 Position;
typedef Vector2 Velocity;
typedef Vector2 Acceleration;

extern ECS_COMPONENT_DECLARE(Position);
extern ECS_COMPONENT_DECLARE(Velocity);
extern ECS_COMPONENT_DECLARE(Acceleration);

extern ECS_SYSTEM_DECLARE(MovementMove);
extern ECS_SYSTEM_DECLARE(MovementAccelerate);

// The import function name has to follow the convention: <ModuleName>Import
void MovementImport(ecs_world_t *world);