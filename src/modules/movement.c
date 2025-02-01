#include "modules/movement.h"

#include <math.h>

#include "flecs.h"
#include "raylib.h"
#include "raymath.h"

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);
ECS_COMPONENT_DECLARE(Acceleration);

ECS_COMPONENT_DECLARE(MaxSpeed);
ECS_COMPONENT_DECLARE(Friction);
ECS_COMPONENT_DECLARE(Rotation);

ECS_COMPONENT_DECLARE(ArcMotion);
ECS_COMPONENT_DECLARE(CircularMotion);
ECS_DECLARE(OrbitsParent);

ECS_SYSTEM_DECLARE(MovementMove);
ECS_SYSTEM_DECLARE(MovementAccelerate);
ECS_SYSTEM_DECLARE(MovementFriction);
ECS_SYSTEM_DECLARE(CapVelocity);

ECS_SYSTEM_DECLARE(ArcMove);
ECS_SYSTEM_DECLARE(MoveCircleCenter);
ECS_SYSTEM_DECLARE(FollowOrbitCenter);
ECS_SYSTEM_DECLARE(RotateCircle);

static void MovementMove(ecs_iter_t *it) {
  // Get fields from system query
  Position *p = ecs_field(it, Position, 0);
  Velocity *v = ecs_field(it, Velocity, 1);

  for (int i = 0; i < it->count; ++i) {
    if (Vector2LengthSqr(v[i]) < EPSILON)
      v[i] = Vector2Zero();  // stop completely if small enough
    else
      p[i] = Vector2Add(p[i], Vector2Scale(v[i], it->delta_time));
    if (!(p[i].x < 1000 || p[i].x > 1000) || p[i].y == NAN) {
      int a = 0;
    }
  }
}

static void ArcMove(ecs_iter_t *it) {
  Position *p = ecs_field(it, Position, 0);
  ArcMotion *a = ecs_field(it, ArcMotion, 1);

  for (int i = 0; i < it->count; ++i) {
    a[i].progress += it->delta_time;
    if (a[i].progress >= 1) {
      a[i].progress = 1;
      a[i].onComplete(it->world, it->entities[i]);
    }
    p[i] = Vector2Lerp(a[i].from, a[i].to, a[i].progress);
  }
}

static void MovementAccelerate(ecs_iter_t *it) {
  // Get fields from system query
  Velocity *v = ecs_field(it, Velocity, 0);
  Acceleration *a = ecs_field(it, Acceleration, 1);

  for (int i = 0; i < it->count; ++i) {
    if (Vector2LengthSqr(a[i]) < EPSILON) {
      continue;
    }
    if (Vector2DotProduct(v[i], a[i]) < 0) {
      // if acceleration is in opposite direction, move velocity faster
      v[i] = Vector2Add(v[i], Vector2Scale(a[i], it->delta_time * 3));
    } else {
      v[i] = Vector2Add(v[i], Vector2Scale(a[i], it->delta_time));
    }
    if (!(v[i].x < 1000 || v[i].x > 1000) || v[i].y == NAN) {
      int _a = 0;
    }
    a[i] = Vector2Zero();  // reset for next frame
  }
}

static void CapVelocity(ecs_iter_t *it) {
  // Get fields from system query
  Velocity *v = ecs_field(it, Velocity, 0);
  MaxSpeed *max = ecs_field(it, MaxSpeed, 1);

  for (int i = 0; i < it->count; ++i) {
    if (Vector2LengthSqr(v[i]) > max[i] * max[i]) {
      v[i] = Vector2Scale(Vector2Normalize(v[i]), max[i]);
    }
  }
}

// Example friction system
static void MovementFriction(ecs_iter_t *it) {
  Velocity *v = ecs_field(it, Velocity, 0);
  Friction *f = ecs_field(it, Friction, 1);

  for (int i = 0; i < it->count; i++) {
    float frictionVal = f != NULL ? f[i] : DEFAULT_FRICTION;
    // Scale velocity down each tick without affecting acceleration directly
    if (Vector2LengthSqr(v[i]) > EPSILON) {
      v[i] = Vector2Scale(v[i], 1.0f - frictionVal * it->delta_time);
    }
  }
}

static void MoveCircleCenter(ecs_iter_t *it) {
  Velocity *v = ecs_field(it, Velocity, 0);
  CircularMotion *c = ecs_field(it, CircularMotion, 1);

  for (int i = 0; i < it->count; ++i) {
    c[i].center = Vector2Add(c[i].center, Vector2Scale(v[i], it->delta_time));
  }
}

static void FollowOrbitCenter(ecs_iter_t *it) {
  CircularMotion *c = ecs_field(it, CircularMotion, 0);
  Position *p = ecs_field(it, Position, 2);

  for (int i = 0; i < it->count; ++i) {
    c[i].center = p[i];
  }
}

static void RotateCircle(ecs_iter_t *it) {
  Position *p = ecs_field(it, Position, 0);
  CircularMotion *c = ecs_field(it, CircularMotion, 1);

  for (int i = 0; i < it->count; ++i) {
    c[i].angle += c[i].speed * it->delta_time;
    if (c[i].angle > 2 * PI) {
      c[i].angle -= 2 * PI;
    } else if (c[i].angle < 0) {
      c[i].angle += 2 * PI;
    }
    p[i] = Vector2Add(c[i].center, (Vector2){c[i].radius * cos(c[i].angle),
                                             c[i].radius * sin(c[i].angle)});
  }
}

void MovementImport(ecs_world_t *world) {
  ECS_MODULE(world, Movement);

  ECS_COMPONENT_DEFINE(world, Position);
  ECS_COMPONENT_DEFINE(world, Velocity);
  ECS_COMPONENT_DEFINE(world, Acceleration);

  ECS_COMPONENT_DEFINE(world, MaxSpeed);
  ECS_COMPONENT_DEFINE(world, Friction);
  ECS_COMPONENT_DEFINE(world, Rotation);

  ECS_COMPONENT_DEFINE(world, ArcMotion);
  ECS_COMPONENT_DEFINE(world, CircularMotion);
  ECS_TAG_DEFINE(world, OrbitsParent);

  ecs_add_pair(world, ecs_id(Velocity), EcsWith, ecs_id(Position));
  ecs_add_pair(world, ecs_id(Acceleration), EcsWith, ecs_id(Velocity));

  ECS_SYSTEM_DEFINE(world, MovementAccelerate, EcsPreUpdate, Velocity,
                    Acceleration);
  ECS_SYSTEM_DEFINE(world, CapVelocity, EcsOnUpdate, Velocity, [in] MaxSpeed);
  ECS_SYSTEM_DEFINE(world, MovementMove, EcsOnUpdate, Position, Velocity);
  ECS_SYSTEM_DEFINE(world, MovementFriction,EcsPostUpdate, Velocity, [in]
  ?Friction);

  ECS_SYSTEM_DEFINE(world, ArcMove, EcsOnUpdate, Position, ArcMotion);
  ECS_SYSTEM_DEFINE(world, MoveCircleCenter, EcsOnUpdate, [in] Velocity,
                    CircularMotion);
  ECS_SYSTEM_DEFINE(world, FollowOrbitCenter, EcsOnUpdate, CircularMotion,
                    OrbitsParent, [in] Position(up));
  ECS_SYSTEM_DEFINE(world, RotateCircle, EcsOnUpdate, Position, CircularMotion);
}