#include "./movement.h"

#include "flecs.h"
#include "raylib.h"
#include "raymath.h"

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);
ECS_COMPONENT_DECLARE(Acceleration);

ECS_SYSTEM_DECLARE(MovementMove);
ECS_SYSTEM_DECLARE(MovementAccelerate);

#define FRICTION_COEFFICIENT 0.8f
static void MovementMove(ecs_iter_t *it) {
  // Get fields from system query
  Position *p = ecs_field(it, Position, 0);
  Velocity *v = ecs_field(it, Velocity, 1);

  for (int i = 0; i < it->count; ++i) {
    v[i] = Vector2Add(
        v[i], Vector2Scale(v[i], -FRICTION_COEFFICIENT * it->delta_time));
    if (Vector2LengthSqr(v[i]) < EPSILON)
      v[i] = Vector2Zero();  // stop completely if small enough
    p[i] = Vector2Add(p[i], Vector2Scale(v[i], it->delta_time));
  }
}

#define EPSILON 1E-2
static void MovementAccelerate(ecs_iter_t *it) {
  // Get fields from system query
  Velocity *v = ecs_field(it, Velocity, 0);
  Acceleration *a = ecs_field(it, Acceleration, 1);

  for (int i = 0; i < it->count; ++i) {
    // if (Vector2LengthSqr(a[i]) > 0) {
    v[i] = Vector2Add(v[i], Vector2Scale(a[i], it->delta_time));
    a[i] = Vector2Zero();  // reset for next frame
    // }
  }
}

void MovementImport(ecs_world_t *world) {
  ECS_MODULE(world, Movement);
  ECS_COMPONENT_DEFINE(world, Position);
  ECS_COMPONENT_DEFINE(world, Velocity);
  ECS_COMPONENT_DEFINE(world, Acceleration);
  ecs_add_pair(world, ecs_id(Velocity), EcsWith, ecs_id(Position));
  ecs_add_pair(world, ecs_id(Acceleration), EcsWith, ecs_id(Velocity));

  ECS_SYSTEM_DEFINE(world, MovementMove, EcsOnUpdate, Position, [in] Velocity);
  ECS_SYSTEM_DEFINE(world, MovementAccelerate, EcsOnUpdate, Velocity,
                    Acceleration);
}