#pragma once
#include <stdbool.h>

#include "raylib.h"

// max speed is acceleration / friction_coefficient
#define FRICTION_COEFFICIENT 0.3f

typedef struct PhysicsBody {
  Vector2 pos;
  Vector2 velocity;
  Vector2 acceleration;
  float mass;
  float radius;
} PhysicsBody;

// Updates body stats given its pos, velocity and acceleration
void MoveBody(PhysicsBody *body);
void MoveBodyWithWeights(PhysicsBody *body, float accelerationRate,
                         float retardationRate);
void ApplyForce(PhysicsBody *body, Vector2 force);
void ApplyAcceleration(PhysicsBody *body, Vector2 forceByMass);
void RigidCollision(PhysicsBody *body1, PhysicsBody *body2);
void ElasticCollision(PhysicsBody *body1, PhysicsBody *body2);

bool CheckCollision(PhysicsBody body1, PhysicsBody body2);
bool CheckLenientCollision(PhysicsBody body1, PhysicsBody body2,
                           float leniency);

// angle in radians
float LerpRotationAngle(float angle, Vector2 to, float amount);
Vector2 LerpRotation(Vector2 from, Vector2 to, float amount);