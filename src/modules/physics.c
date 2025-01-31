
#include "modules/physics.h"

#include <float.h>
#include <math.h>

#include "flecs.h"
#include "modules/collisions.h"
#include "modules/movement.h"
#include "raylib.h"
#include "raymath.h"

ECS_COMPONENT_DECLARE(Rigidbody);
ECS_COMPONENT_DECLARE(Bounciness);
ECS_SYSTEM_DECLARE(SolvePenetrations);
ECS_SYSTEM_DECLARE(ResolveCollisions);
ECS_SYSTEM_DECLARE(ApplyAllImpulses);

static void SolvePenetration(Vector2 *posA, Vector2 *posB, float radiusA,
                             float radiusB, float invMassA, float invMassB) {
  Vector2 delta = Vector2Subtract(*posB, *posA);
  float distance = Vector2Length(delta);
  float penetration = radiusA + radiusB - distance;

  if (penetration > 0) {
    Vector2 correctionDir = SafeNormalize(delta);
    float totalInvMass = invMassA + invMassB;
    float correction = penetration * 0.8f;

    *posA = Vector2Add(
        *posA,
        Vector2Scale(correctionDir, -correction * (invMassB / totalInvMass)));
    // *posB = Vector2Add(
    //     *posB,
    //     Vector2Scale(correctionDir, correction * (invMassA / totalInvMass)));
  }
}

#define BOUNCINESS 0.8f
#define MAX_IMPULSE 100000.0f
static void ResolveCollision(Rigidbody *bodyA, Rigidbody *bodyB, Velocity velA,
                             Velocity velB, Vector2 normal) {
  // Relative velocity at contact point
  Vector2 relVel = Vector2Subtract(velA, velB);
  float velAlongNormal = Vector2DotProduct(relVel, normal);

  // Restitution (bounciness)
  // float e = fminf(rbA->restitution, rbB->restitution);
  float e = BOUNCINESS;
  float j = -(1.0f + e) * velAlongNormal;
  j /= bodyA->inverseMass + bodyB->inverseMass;

  // Clamp impulse to prevent instability
  j = fmaxf(j, -MAX_IMPULSE);
  j = fminf(j, MAX_IMPULSE);

  Vector2 impulse = Vector2Scale(normal, j);
  bodyA->accumulatedImpulse = Vector2Add(
      bodyA->accumulatedImpulse, Vector2Scale(impulse, bodyA->inverseMass));
  // bodyB->accumulatedImpulse = Vector2Subtract(
  //     bodyB->accumulatedImpulse, Vector2Scale(impulse, bodyB->inverseMass));
}

// Apply impulses after all collisions are processed
static void ApplyImpulses(Rigidbody *body, Velocity *vel) {
  *vel = Vector2Add(*vel, body->accumulatedImpulse);
  body->accumulatedImpulse = Vector2Zero();  // Reset
}

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

// The import function name has to follow the convention: <ModuleName>Import
void PhysicsImport(ecs_world_t *world) {
  ECS_MODULE(world, Physics);
  ECS_COMPONENT_DEFINE(world, Rigidbody);
  ECS_COMPONENT_DEFINE(world, Bounciness);

  ecs_add_pair(world, ecs_id(Rigidbody), EcsWith, ecs_id(Collidable));

  ECS_SYSTEM_DEFINE(world, ResolveCollisions, EcsPostUpdate,
                    Rigidbody($this), [in] movement.Velocity($this),
                    [in] collisions.CollidesWith($this, $other),
                    Rigidbody($other), [in] movement.Velocity($other));
  ECS_SYSTEM_DEFINE(
      world, SolvePenetrations, EcsPostUpdate,
      movement.Position($this), [in] collisions.Collidable($this),
      [in] Rigidbody($this), [in] collisions.CollidesWith($this, $other),
      movement.Position($other), [in] collisions.Collidable($other),
      [in] Rigidbody($other));
  ECS_SYSTEM_DEFINE(world, ApplyAllImpulses, EcsPostUpdate, Rigidbody,
                    movement.Velocity);
}