#include "physics_math.h"

#include <stdbool.h>

#include "raylib.h"
#include "raymath.h"

#define DEFAULT_ACCELERATION_SPEED 10.0f
#define DEFAULT_RETARDATION_SPEED 15.0f
// Updates body stats given its pos, velocity and acceleration

void ApplyForce(PhysicsBody *body, Vector2 force) {
  body->acceleration =
      Vector2Add(body->acceleration, Vector2Scale(force, 1 / body->mass));
}

// Like ApplyForce but doesn't care about mass
void ApplyAcceleration(PhysicsBody *body, Vector2 forceByMass) {
  body->acceleration = Vector2Add(body->acceleration, forceByMass);
}

Vector2 CollisionPoint(Vector2 pos1, float radius1, Vector2 pos2,
                       float radius2) {
  return (Vector2){
      ((pos1.x * radius2) + (pos2.x * radius1)) / (radius1 + radius2),
      ((pos1.y * radius2) + (pos2.y * radius1)) / (radius1 + radius2),
  };
}

#define BOUNCINESS 0.5f
// mass = 0 means it won't change velocity
// void RigidCollision(Vector2 collPoint, float overlap, Vector2 *pos,
//                     Vector2 *velocity, float mass, Vector2 *otherPos,
//                     Vector2 *otherVelocity, float otherMass) {
//   // Calculate new velocities
//   Vector2 normal = Vector2Normalize(Vector2Subtract(*pos, *otherPos));
//   Vector2 relativeVelocity = Vector2Subtract(*velocity, *otherVelocity);
//   float velocityAlongNormal = Vector2DotProduct(relativeVelocity, normal);

//   if (velocityAlongNormal > 0) return;

//   float e = BOUNCINESS;  // Coefficient of restitution
//   float j = -(1 + e) * velocityAlongNormal;
//   float invMass1 = (mass == 0) ? 0 : 1.0f / mass;
//   float invMass2 = (otherMass == 0) ? 0 : 1.0f / otherMass;
//   j /= invMass1 + invMass2;

//   Vector2 impulse = Vector2Scale(normal, j);
//   *velocity = Vector2Subtract(*velocity, Vector2Scale(impulse, 1.0f / mass));
//   *otherVelocity =
//       Vector2Add(*otherVelocity, Vector2Scale(impulse, 1.0f / otherMass));

//   // Adjust positions to avoid overlap
//   Vector2 correction =
//       Vector2Scale(normal, overlap / ((1.0f / mass) + (1.0f / otherMass)));
//   *pos = Vector2Add(*pos, Vector2Scale(correction, 1.0f / mass));
//   *otherPos =
//       Vector2Subtract(*otherPos, Vector2Scale(correction, 1.0f / otherMass));
// }

void ElasticCollision(PhysicsBody *body1, PhysicsBody *body2) {
  Vector2 delta = Vector2Subtract(body1->pos, body2->pos);
  Vector2 awayDirection = Vector2Normalize(delta);
  float nearness = 1 - Vector2Length(delta) / (body1->radius + body2->radius);
  float force = nearness / (body1->mass + body2->mass);
  ApplyAcceleration(body1, Vector2Scale(awayDirection, force * body2->mass));
  ApplyAcceleration(body2, Vector2Scale(awayDirection, -force * body1->mass));
}

bool CheckCollision(PhysicsBody body1, PhysicsBody body2) {
  return CheckCollisionCircles(body1.pos, body1.radius, body2.pos,
                               body2.radius);
}

// Check if collision if sizes were leniency times bigger
bool CheckLenientCollision(PhysicsBody body1, PhysicsBody body2,
                           float leniency) {
  return CheckCollisionCircles(body1.pos, body1.radius * leniency, body2.pos,
                               body2.radius * leniency);
}
