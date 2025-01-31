#pragma once
#include "flecs.h"
#include "helpers.h"
#include "raylib.h"
#include "raymath.h"

typedef float Bounciness;
typedef struct {
  Vector2 accumulatedImpulse;
  float inverseMass;
} Rigidbody;

#define RIGIDBODY(mass) \
  {.inverseMass = SafeInverseMass(mass), .accumulatedImpulse = {0, 0}}

extern ECS_COMPONENT_DECLARE(Rigidbody);
extern ECS_COMPONENT_DECLARE(Bounciness);

extern ECS_SYSTEM_DECLARE(SolvePenetrations);
extern ECS_SYSTEM_DECLARE(ResolveCollisions);
extern ECS_SYSTEM_DECLARE(ApplyAllImpulses);

// The import function name has to follow the convention: <ModuleName>Import
void PhysicsImport(ecs_world_t *world);