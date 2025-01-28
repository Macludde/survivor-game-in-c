#pragma once
#include "raylib.h"

double time_in_seconds();
Color SlightColorVariation(Color base);
// Returns float between 0.8 and 1.2
inline static float SlightVariation() {
  return GetRandomValue(800, 1200) / 1000.0f;
}

Vector2 CalculateMidpoint(Vector2 origin, Vector2 target);
Vector2 PointAlongArc(Vector2 origin, Vector2 target, float progress);