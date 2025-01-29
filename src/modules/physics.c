
#include "./physics.h"

#include "./collisions.h"
#include "./movement.h"
#include "flecs.h"
#include "raylib.h"
#include "raymath.h"

ECS_COMPONENT_DECLARE(Mass);
ECS_COMPONENT_DECLARE(Bounciness);
ECS_SYSTEM_DECLARE(MoveOutCollisions);
ECS_SYSTEM_DECLARE(ClearCollisions);

#define BOUNCINESS 0.2f
// mass = 0 means it won't change velocity
static void RigidCollision(Vector2 collPoint, float overlap, Vector2 *pos,
                           Vector2 *velocity, float mass, Vector2 *otherPos,
                           Vector2 *otherVelocity, float otherMass) {
  if (mass == 0) mass = EPSILON;
  if (otherMass == 0) otherMass = EPSILON;
  // Calculate new velocities
  Vector2 normal = Vector2Normalize(Vector2Subtract(*pos, *otherPos));
  Vector2 relativeVelocity = Vector2Subtract(*velocity, *otherVelocity);
  float velocityAlongNormal = Vector2DotProduct(relativeVelocity, normal);

  if (velocityAlongNormal > 0) return;

  float e = BOUNCINESS;  // Coefficient of restitution
  float j = -(1 + e) * velocityAlongNormal;
  float invMass1 = 1.0f / mass;
  float invMass2 = 1.0f / otherMass;
  j /= invMass1 + invMass2;

  Vector2 impulse = Vector2Scale(normal, j);
  *velocity = Vector2Add(*velocity, Vector2Scale(impulse, 1.0f / mass));
  *otherVelocity =
      Vector2Subtract(*otherVelocity, Vector2Scale(impulse, 1.0f / otherMass));

  // Adjust positions to avoid overlap
  Vector2 correction =
      Vector2Scale(normal, overlap / ((1.0f / mass) + (1.0f / otherMass)));
  *pos = Vector2Add(*pos, Vector2Scale(correction, 1.0f / mass));
  *otherPos =
      Vector2Subtract(*otherPos, Vector2Scale(correction, 1.0f / otherMass));
}

static void MoveOutCollisions(ecs_iter_t *it) {
  Position *p1 = ecs_field(it, Position, 0);
  Velocity *v1 = ecs_field(it, Velocity, 1);
  Mass *m1 = ecs_field(it, Mass, 2);
  CollidesWith *c = ecs_field(it, CollidesWith, 3);
  Position *p2 = ecs_field(it, Position, 4);
  Velocity *v2 = ecs_field(it, Velocity, 5);
  Mass *m2 = ecs_field(it, Mass, 6);

  for (int i = 0; i < it->count; i++) {
    RigidCollision(c[i].collisionPoint, c[i].overlap, &p1[i], &v1[i], m1[i],
                   &p2[i], &v2[i], m2[i]);
  }
}

static void ClearCollisions(ecs_iter_t *it) {
  ecs_id_t pair_id = ecs_field_id(&it, 0);
  ecs_entity_t target = ecs_pair_second(it->world, pair_id);
  ecs_entity_t e2 = it->variables[1].entity;

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e1 = it->entities[i];
    ecs_remove_pair(it->world, e1, ecs_id(CollidesWith), e2);
  }
}

// The import function name has to follow the convention: <ModuleName>Import
void PhysicsImport(ecs_world_t *world) {
  ECS_MODULE(world, Physics);
  ECS_COMPONENT_DEFINE(world, Mass);
  ECS_COMPONENT_DEFINE(world, Bounciness);

  ECS_SYSTEM_DEFINE(
      world, MoveOutCollisions, EcsPostUpdate, movement.Position,
      movement.Velocity, Mass, collisions.CollidesWith($this, $other),
      movement.Position($other), movement.Velocity($other), Mass($other));
  //   ECS_SYSTEM_DEFINE(world, CollisionBounce, EcsPostUpdate, Velocity
  //   Collidable,
  //                     (CollidesWith, Velocity));

  ECS_SYSTEM_DEFINE(world, ClearCollisions, EcsOnStore,
                    collisions.CollidesWith($this, $other));
}