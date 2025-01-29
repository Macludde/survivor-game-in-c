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

void ApplyForce(PhysicsBody *body, Vector2 force);
void ApplyAcceleration(PhysicsBody *body, Vector2 forceByMass);
Vector2 CollisionPoint(Vector2 pos1, float radius1, Vector2 pos2,
                       float radius2);

void RigidCollision(Vector2 collPoint, float overlap, Vector2 *pos,
                    Vector2 *velocity, float mass, Vector2 *otherPos,
                    Vector2 *otherVelocity, float otherMass);
void ElasticCollision(PhysicsBody *body1, PhysicsBody *body2);

bool CheckCollision(PhysicsBody body1, PhysicsBody body2);
bool CheckLenientCollision(PhysicsBody body1, PhysicsBody body2,
                           float leniency);

// angle in radians
float LerpRotationAngle(float angle, Vector2 to, float amount);
Vector2 LerpRotation(Vector2 from, Vector2 to, float amount);
