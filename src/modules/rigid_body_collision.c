#include "./rigid_body_collision.h"

#include <float.h>
#include <math.h>

#include "./collisions.h"
#include "./physics.h"
#include "flecs.h"
#include "raylib.h"
#include "raymath.h"

void SolvePenetration(Vector2 *posA, Vector2 *posB, float radiusA,
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
    *posB = Vector2Add(
        *posB,
        Vector2Scale(correctionDir, correction * (invMassA / totalInvMass)));
  }
}

#define BOUNCINESS 0.8f
#define MAX_IMPULSE 100000.0f
void ResolveCollision(Rigidbody *bodyA, Rigidbody *bodyB, Velocity velA,
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
  bodyB->accumulatedImpulse = Vector2Subtract(
      bodyB->accumulatedImpulse, Vector2Scale(impulse, bodyB->inverseMass));
}

// Apply impulses after all collisions are processed
void ApplyImpulses(Rigidbody *body, Velocity *vel) {
  *vel = Vector2Add(*vel, body->accumulatedImpulse);
  body->accumulatedImpulse = Vector2Zero();  // Reset
}
