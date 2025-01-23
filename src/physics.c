#include "physics.h"

#include <stdbool.h>

#include "raylib.h"
#include "raymath.h"

#define DEFAULT_ACCELERATION_SPEED 10.0f
#define DEFAULT_RETARDATION_SPEED 15.0f
// Updates body stats given its pos, velocity and acceleration
void MoveBody(PhysicsBody *body) {
  MoveBodyWithWeights(body, DEFAULT_ACCELERATION_SPEED,
                      DEFAULT_RETARDATION_SPEED);
}

void MoveBodyWithWeights(PhysicsBody *body, float accelerationRate,
                         float retardationRate) {
  // add friction
  body->acceleration = Vector2Add(
      body->acceleration, Vector2Scale(body->velocity, -FRICTION_COEFFICIENT));

  // update velocity
  if (Vector2DotProduct(body->acceleration, body->velocity) > 0) {
    // less than 90 degrees between them
    body->velocity = Vector2Add(
        body->velocity,
        Vector2Scale(body->acceleration, accelerationRate * GetFrameTime()));
  } else {
    body->velocity = Vector2Add(
        body->velocity,
        Vector2Scale(body->acceleration, retardationRate * GetFrameTime()));
  }

  // update pos
  body->pos =
      Vector2Add(body->pos, Vector2Scale(body->velocity, GetFrameTime()));

  // Stop completely if velocity is very small
  if (Vector2Length(body->velocity) < 0.01f) {
    body->velocity = Vector2Zero();
  }
  // Reset acceleration for the next frame
  body->acceleration = Vector2Zero();
}

void ApplyForce(PhysicsBody *body, Vector2 force) {
  body->acceleration =
      Vector2Add(body->acceleration, Vector2Scale(force, 1 / body->mass));
}

// Like ApplyForce but doesn't care about mass
void ApplyAcceleration(PhysicsBody *body, Vector2 forceByMass) {
  body->acceleration = Vector2Add(body->acceleration, forceByMass);
}

// mass = 0 means it won't change velocity
void RigidCollision(PhysicsBody *body1, PhysicsBody *body2) {
  // Calculate collision point
  Vector2 collPoint = {
      ((body1->pos.x * body2->radius) + (body2->pos.x * body1->radius)) /
          (body1->radius + body2->radius),
      ((body1->pos.y * body2->radius) + (body2->pos.y * body1->radius)) /
          (body1->radius + body2->radius),
  };

  // Calculate new velocities
  Vector2 velTotal = Vector2Subtract(body1->velocity, body2->velocity);
  // v1_n = (v1*(m1-m2) + v2*m2*2)/(m1+m2)
  Vector2 newVel1 =
      body1->mass > 0
          ? Vector2Scale(
                Vector2Add(
                    Vector2Scale(body1->velocity, body1->mass - body2->mass),
                    Vector2Scale(body2->velocity, body2->mass * 2)),
                1 / (body1->mass + body2->mass))
          : body1->velocity;
  Vector2 newVel2 =
      Vector2Add(velTotal, newVel1);  // same total velocity afterwards

  body1->velocity = newVel1;
  body2->velocity = newVel2;
  // set their position so they don't overlap
  Vector2 delta = Vector2Subtract(body1->pos, body2->pos);
  float dist = Vector2Length(delta);
  if (dist == 0) {
    // just move it away so they don't overlap
    body1->pos = Vector2AddValue(body2->pos, body1->radius + body2->radius);
    return;
  }
  Vector2 direction = Vector2Normalize(delta);
  body1->pos = Vector2Add(collPoint, Vector2Scale(direction, body1->radius));
  body2->pos = Vector2Add(collPoint, Vector2Scale(direction, -body2->radius));
}

void ElasticCollision(PhysicsBody *body1, PhysicsBody *body2) {
  Vector2 delta = Vector2Subtract(body1->pos, body2->pos);
  Vector2 awayDirection = Vector2Normalize(delta);
  float nearness = 1 - Vector2Length(delta) / (body1->radius + body2->radius);
  float force =
      nearness * body1->mass * body2->mass / (body1->mass + body2->mass);
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

// angle in radians, vector should be normalized
float LerpRotationAngle(float angle, Vector2 to, float amount) {
  Vector3 unit = (Vector3){1, 0, 0};
  Quaternion currentRotation = QuaternionFromEuler(0, 0, angle);
  Quaternion targetRotation =
      QuaternionFromVector3ToVector3(unit, (Vector3){to.x, to.y, 0});
  currentRotation =
      QuaternionSlerp(currentRotation, targetRotation, amount > 1 ? 1 : amount);
  return QuaternionToEuler(currentRotation).z;
}

// vector should be normalized
Vector2 LerpRotation(Vector2 from, Vector2 to, float amount) {
  float angle = Vector2Angle((Vector2){1, 0}, from);
  angle = LerpRotationAngle(angle, to, amount);

  return Vector2Rotate((Vector2){1, 0}, angle);
}