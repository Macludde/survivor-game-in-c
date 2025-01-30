#include "helpers.h"

#include <stdio.h>
#include <time.h>

#include "raylib.h"
#include "raymath.h"

#ifdef _WIN32

double time_in_seconds() {
  return (double)clock() / CLOCKS_PER_SEC;  // Convert clock ticks to seconds
}

#else
#include <sys/time.h>

double time_in_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec +
         tv.tv_usec / 1e6;  // Convert seconds and microseconds to seconds
}
#endif

Color SlightColorVariation(Color base) {
  return ColorLerp(base,
                   (Color){
                       .r = GetRandomValue(-10, 10),
                       .g = GetRandomValue(-10, 10),
                       .b = GetRandomValue(-10, 10),
                       .a = 255,
                   },
                   0.1f);
}

Vector2 CalculateMidpoint(Vector2 origin, Vector2 target) {
  Vector2 midpoint;
  midpoint.x = (origin.x + target.x) / 2;
  midpoint.y = (origin.y + target.y) / 2;
  return midpoint;
}

Vector2 PointAlongArc(Vector2 origin, Vector2 target, float progress) {
  if (progress < 0.0f || progress > 1.0f) {
    printf("Progress must be between 0 and 1\n");
    return Vector2Zero();
  }

  // Calculate control point for the arc (parabolic midpoint)
  Vector2 control = CalculateMidpoint(origin, target);
  float height = fabs(target.x - origin.x) / 4.0f;  // Adjust arc steepness here
  control.y -= height;

  // Quadratic Bezier interpolation formula
  float t = progress;
  float oneMinusT = 1.0f - t;

  Vector2 point;
  point.x = (int)(oneMinusT * oneMinusT * origin.x +
                  2 * oneMinusT * t * control.x + t * t * target.x);

  point.y = (int)(oneMinusT * oneMinusT * origin.y +
                  2 * oneMinusT * t * control.y + t * t * target.y);

  return point;
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

Vector2 LerpRotation(Vector2 from, Vector2 to, float amount) {
  float angle = Vector2Angle((Vector2){1, 0}, from);
  angle = LerpRotationAngle(angle, to, amount);

  return Vector2Rotate((Vector2){1, 0}, angle);
}