
#include "./physics.h"

#include "./collisions.h"
#include "./movement.h"
#include "./rigid_body_collision.h"
#include "flecs.h"
#include "raylib.h"
#include "raymath.h"

ECS_COMPONENT_DECLARE(Rigidbody);
ECS_COMPONENT_DECLARE(Bounciness);
ECS_SYSTEM_DECLARE(SolvePenetrations);
ECS_SYSTEM_DECLARE(ResolveCollisions);
ECS_SYSTEM_DECLARE(ApplyAllImpulses);
ECS_SYSTEM_DECLARE(ClearCollisions);

void SolvePenetrations(ecs_iter_t *it) {
  Position *p1 = ecs_field(it, Position, 0);
  Collidable *col1 = ecs_field(it, Collidable, 1);
  Rigidbody *r1 = ecs_field(it, Rigidbody, 2);
  // CollidesWith *c = ecs_field(it, CollidesWith, 3);
  Position *p2 = ecs_field(it, Position, 4);
  Collidable *col2 = ecs_field(it, Collidable, 5);
  Rigidbody *r2 = ecs_field(it, Rigidbody, 6);

  for (int i = 0; i < it->count; ++i) {
    SolvePenetration(&p1[i], &p2[i], col1[i].radius, col2[i].radius,
                     r1[i].inverseMass, r2[i].inverseMass);
  }
}

void ResolveCollisions(ecs_iter_t *it) {
  Rigidbody *r1 = ecs_field(it, Rigidbody, 0);
  Velocity *v1 = ecs_field(it, Velocity, 1);
  CollidesWith *c = ecs_field(it, CollidesWith, 2);
  Rigidbody *r2 = ecs_field(it, Rigidbody, 3);
  Velocity *v2 = ecs_field(it, Velocity, 4);

  for (int i = 0; i < it->count; ++i) {
    ResolveCollision(&r1[i], &r2[i], v1[i], v2[i], c[i].normal);
  }
}

// Apply impulses after all collisions are processed
void ApplyAllImpulses(ecs_iter_t *it) {
  Rigidbody *r1 = ecs_field(it, Rigidbody, 0);
  Velocity *v1 = ecs_field(it, Velocity, 1);

  for (int i = 0; i < it->count; ++i) {
    if (Vector2LengthSqr(r1[i].accumulatedImpulse) > EPSILON)
      ApplyImpulses(&r1[i], &v1[i]);
  }
}

static void ClearCollisions(ecs_iter_t *it) {
  ecs_id_t pair_id = ecs_field_id(it, 0);
  ecs_entity_t target = ecs_pair_second(it->world, pair_id);
  ecs_entity_t e2 = it->variables[1].entity;
  if (it->count > 1) {
    int _a = 0;
  }

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e1 = it->entities[i];
    ecs_remove_pair(it->world, e1, ecs_id(CollidesWith), target);
    ecs_remove_pair(it->world, e1, ecs_id(CollidesWith), e2);
  }
}

// The import function name has to follow the convention: <ModuleName>Import
void PhysicsImport(ecs_world_t *world) {
  ECS_MODULE(world, Physics);
  ECS_COMPONENT_DEFINE(world, Rigidbody);
  ECS_COMPONENT_DEFINE(world, Bounciness);

  ecs_add_pair(world, ecs_id(Rigidbody), EcsWith, ecs_id(Velocity));
  ecs_add_pair(world, ecs_id(Rigidbody), EcsWith, ecs_id(Collidable));

  ECS_SYSTEM_DEFINE(
      world, SolvePenetrations, EcsOnStore,
      movement.Position($this), [in] collisions.Collidable($this),
      [in] Rigidbody($this), [in] collisions.CollidesWith($this, $other),
      movement.Position($other), [in] collisions.Collidable($other),
      [in] Rigidbody($other));
  ECS_SYSTEM_DEFINE(world, ResolveCollisions, EcsOnStore,
                    Rigidbody($this), [in] movement.Velocity($this),
                    [in] collisions.CollidesWith($this, $other),
                    Rigidbody($other), [in] movement.Velocity($other));
  ECS_SYSTEM_DEFINE(world, ApplyAllImpulses, EcsOnStore, Rigidbody,
                    movement.Velocity);

  ECS_SYSTEM_DEFINE(world, ClearCollisions, EcsOnStore,
                    collisions.CollidesWith($this, $other));
}