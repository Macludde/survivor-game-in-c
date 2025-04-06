#include "flecs.h"
#include "modules/movement.h";
#include "modules/player.h";
#include "raylib.h"
#include "raymath.h"

extern Camera2D camera;

#define CAMERA_MOVEMENT_SPEED 2.0f
void MoveCameraToPosition(ecs_iter_t *it) {
  static int screenWidth = 0;
  static int screenHeight;
  if (screenWidth == 0 || IsWindowResized()) {
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
  }
  Position *p = ecs_field(it, Position, 0);
  Position centerPosition;
  if (it->count == 1) {
    centerPosition = p[0];
  } else if (it->count == 0) {
    centerPosition = Vector2Zero();
  } else {
    centerPosition = Vector2Zero();
    for (int i = 0; i < it->count; ++i) {
      centerPosition = Vector2Add(centerPosition, p[i]);
    }
    centerPosition = Vector2Scale(centerPosition, 1.0f / it->count);
  }

  float maxDistanceHorizontal = screenWidth * 0.4f;
  float maxDistanceVertical = screenHeight * 0.4f;
  if ((int)centerPosition.x - (int)camera.target.x > maxDistanceHorizontal) {
    camera.target.x = centerPosition.x - maxDistanceHorizontal;
  } else if ((int)centerPosition.x - (int)camera.target.x <
             -maxDistanceHorizontal) {
    camera.target.x = centerPosition.x + maxDistanceHorizontal;
  }
  if ((int)centerPosition.y - (int)camera.target.y > maxDistanceVertical) {
    camera.target.y = centerPosition.y - maxDistanceVertical;
  } else if ((int)centerPosition.y - (int)camera.target.y <
             -maxDistanceVertical) {
    camera.target.y = centerPosition.y + maxDistanceVertical;
  }
  camera.target = Vector2Lerp(camera.target, centerPosition,
                              CAMERA_MOVEMENT_SPEED * it->delta_time);
}

void MoveCameraToPlayerSystem(ecs_world_t *world) {}