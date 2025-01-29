#pragma once
#include "flecs.h"

typedef float Mass;
typedef float Bounciness;

extern ECS_COMPONENT_DECLARE(Mass);
extern ECS_COMPONENT_DECLARE(Bounciness);

extern ECS_SYSTEM_DECLARE(MoveOutCollisions);
extern ECS_SYSTEM_DECLARE(ClearCollisions);

// The import function name has to follow the convention: <ModuleName>Import
void PhysicsImport(ecs_world_t *world);