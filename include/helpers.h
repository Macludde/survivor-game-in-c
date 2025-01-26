#pragma once
#include "raylib.h"

double time_in_seconds();
Color SlightColorVariation(Color base);

Vector2 CalculateMidpoint(Vector2 origin, Vector2 target);
Vector2 PointAlongArc(Vector2 origin, Vector2 target, float progress);