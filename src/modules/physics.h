#pragma once
#include "flecs.h"
#include "raylib.h"
#include "raymath.h"

typedef float Bounciness;
typedef struct {
  Vector2 accumulatedImpulse;
  float inverseMass;
} Rigidbody;

#define RIGIDBODY(mass) \
  {.inverseMass = SafeInverseMass(mass), .accumulatedImpulse = {0, 0}}

static inline float SafeInverseMass(float mass) {
  return (mass <= 1) ? 1.0f : 1.0f / mass;
}

static inline Vector2 SafeNormalize(Vector2 vec) {
  float length = Vector2Length(vec);
  if (length < EPSILON) return Vector2Zero();
  return Vector2Scale(vec, 1.0f / length);
}

extern ECS_COMPONENT_DECLARE(Rigidbody);
extern ECS_COMPONENT_DECLARE(Bounciness);

extern ECS_SYSTEM_DECLARE(SolvePenetrations);
extern ECS_SYSTEM_DECLARE(ResolveCollisions);
extern ECS_SYSTEM_DECLARE(ApplyAllImpulses);
extern ECS_SYSTEM_DECLARE(ClearCollisions);

// The import function name has to follow the convention: <ModuleName>Import
void PhysicsImport(ecs_world_t *world);