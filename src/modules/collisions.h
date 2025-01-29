#pragma once
#include <stdbool.h>

#include "./movement.h"
#include "flecs.h"
#include "raylib.h"

typedef struct Collidable {
  float radius;
} Collidable;

typedef struct CollidesWith {
  Vector2 collisionPoint;
  float overlap;
} CollidesWith;

extern ECS_COMPONENT_DECLARE(Collidable);
extern ECS_COMPONENT_DECLARE(CollidesWith);
extern ECS_SYSTEM_DECLARE(Collide);

// The import function name has to follow the convention: <ModuleName>Import
void CollisionsImport(ecs_world_t *world);