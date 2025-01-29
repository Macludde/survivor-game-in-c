#include "./movement.h"

#include "flecs.h"
#include "raylib.h"
#include "raymath.h"

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);
ECS_COMPONENT_DECLARE(Acceleration);
ECS_COMPONENT_DECLARE(MaxSpeed);
ECS_COMPONENT_DECLARE(Friction);

ECS_SYSTEM_DECLARE(MovementMove);
ECS_SYSTEM_DECLARE(MovementAccelerate);
ECS_SYSTEM_DECLARE(CapVelocity);
ECS_SYSTEM_DECLARE(MovementFriction);

static void MovementMove(ecs_iter_t *it) {
  // Get fields from system query
  Position *p = ecs_field(it, Position, 0);
  Velocity *v = ecs_field(it, Velocity, 1);

  for (int i = 0; i < it->count; ++i) {
    if (Vector2LengthSqr(v[i]) < EPSILON)
      v[i] = Vector2Zero();  // stop completely if small enough
    else
      p[i] = Vector2Add(p[i], Vector2Scale(v[i], it->delta_time));
  }
}

#define EPSILON 1E-2
static void MovementAccelerate(ecs_iter_t *it) {
  // Get fields from system query
  Velocity *v = ecs_field(it, Velocity, 0);
  Acceleration *a = ecs_field(it, Acceleration, 1);

  for (int i = 0; i < it->count; ++i) {
    if (Vector2DotProduct(v[i], a[i]) < 0) {
      // if acceleration is in opposite direction, move velocity faster
      v[i] = Vector2Add(v[i], Vector2Scale(a[i], it->delta_time * 3));
    } else {
      v[i] = Vector2Add(v[i], Vector2Scale(a[i], it->delta_time));
    }
    a[i] = Vector2Zero();  // reset for next frame
  }
}

static void CapVelocity(ecs_iter_t *it) {
  // Get fields from system query
  Velocity *v = ecs_field(it, Velocity, 0);
  MaxSpeed *a = ecs_field(it, MaxSpeed, 1);

  for (int i = 0; i < it->count; ++i) {
    if (Vector2LengthSqr(v[i]) > a[i] * a[i]) {
      v[i] = Vector2Scale(Vector2Normalize(v[i]), a[i]);
    }
  }
}

// Example friction system
static void MovementFriction(ecs_iter_t *it) {
  Velocity *v = ecs_field(it, Velocity, 0);
  Friction *f = ecs_field(it, Friction, 1);

  for (int i = 0; i < it->count; i++) {
    float frictionVal = f ? f[i] : DEFAULT_FRICTION;
    // Scale velocity down each tick without affecting acceleration directly
    if (Vector2LengthSqr(v[i]) > EPSILON) {
      v[i] = Vector2Scale(v[i], 1.0f - frictionVal * it->delta_time);
    }
  }
}

void MovementImport(ecs_world_t *world) {
  ECS_MODULE(world, Movement);

  ECS_COMPONENT_DEFINE(world, Position);
  ECS_COMPONENT_DEFINE(world, Velocity);
  ECS_COMPONENT_DEFINE(world, Acceleration);
  ECS_COMPONENT_DEFINE(world, MaxSpeed);
  ECS_COMPONENT_DEFINE(world, Friction);

  ecs_add_pair(world, ecs_id(Velocity), EcsWith, ecs_id(Position));
  ecs_add_pair(world, ecs_id(Acceleration), EcsWith, ecs_id(Velocity));

  ECS_SYSTEM_DEFINE(world, MovementAccelerate, EcsPreUpdate, Velocity,
                    Acceleration);
  ECS_SYSTEM_DEFINE(world, CapVelocity, EcsOnUpdate, Velocity, [in] MaxSpeed);
  ECS_SYSTEM_DEFINE(world, MovementMove, EcsOnUpdate, Position, Velocity);
  ECS_SYSTEM_DEFINE(world, MovementFriction,EcsPostUpdate, Velocity, [in] ?Friction);
}