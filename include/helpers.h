#pragma once
#include "raylib.h"
#include "raymath.h"

double time_in_seconds();
Color SlightColorVariation(Color base);
// Returns float between 0.8 and 1.2
inline static float SlightVariation() {
  return GetRandomValue(800, 1200) / 1000.0f;
}

Vector2 CalculateMidpoint(Vector2 origin, Vector2 target);
Vector2 PointAlongArc(Vector2 origin, Vector2 target, float progress);

// vector should be normalized
float LerpRotationAngle(float angle, Vector2 to, float amount);

// vector should be normalized
Vector2 LerpRotation(Vector2 from, Vector2 to, float amount);

static inline float SafeInverseMass(float mass) {
  return (mass <= 1) ? 1.0f : 1.0f / mass;
}

static inline Vector2 SafeNormalize(Vector2 vec) {
  float length = Vector2Length(vec);
  if (length < EPSILON) return Vector2Zero();
  return Vector2Scale(vec, 1.0f / length);
}