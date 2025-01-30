#pragma once
#include "./collisions.h"
#include "./physics.h"
#include "raylib.h"

void SolvePenetration(Vector2 *posA, Vector2 *posB, float radiusA,
                      float radiusB, float massA, float massB);
void ResolveCollision(Rigidbody *body1, Rigidbody *body2, Velocity vel1,
                      Velocity vel2, Vector2 normal);
void ApplyImpulses(Rigidbody *body, Velocity *vel);