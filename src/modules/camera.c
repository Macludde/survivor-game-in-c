#include "modules/camera.h"

#include "flecs.h"
#include "modules/movement.h"
#include "modules/player.h"
#include "raylib.h"
#include "raymath.h"

static int screenWidth;
static int screenHeight;

Camera2D camera;
ECS_COMPONENT_DECLARE(FollowCam);

ECS_SYSTEM_DECLARE(MoveCameraToPosition);
ECS_SYSTEM_DECLARE(Begin2D);
ECS_SYSTEM_DECLARE(End2D);
ECS_SYSTEM_DECLARE(CameraHandleWindowResize);
#ifdef DEBUG
ECS_SYSTEM_DECLARE(MouseScrollZoom);
#endif

static void Begin2D(ecs_iter_t *it) { BeginMode2D(camera); }

static void End2D(ecs_iter_t *it) { EndMode2D(); }

static void MoveCameraToPosition(ecs_iter_t *it) {
  Position *p = ecs_field(it, Position, 1);
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

static void MouseScrollZoom(ecs_iter_t *it) {
  camera.zoom += ((float)GetMouseWheelMove() * 0.05f);
  if (camera.zoom > 3.0f)
    camera.zoom = 3.0f;
  else if (camera.zoom < 0.1f)
    camera.zoom = 0.1f;
}

bool IsPointOnScreen(Vector2 pos) {
  Vector2 topLeft = GetScreenToWorld2D((Vector2){0, 0}, camera);
  Vector2 bottomRight = GetScreenToWorld2D(
      (Vector2){GetScreenWidth(), GetScreenHeight()}, camera);
  return pos.x > topLeft.x && pos.x < bottomRight.x && pos.y > topLeft.y &&
         pos.y < bottomRight.y;
}

bool IsCircleOnScreen(Vector2 pos, float radius) {
  Vector2 topLeft = GetScreenToWorld2D((Vector2){0, 0}, camera);
  Vector2 bottomRight = GetScreenToWorld2D(
      (Vector2){GetScreenWidth(), GetScreenHeight()}, camera);
  return pos.x - radius < bottomRight.x && pos.x + radius > topLeft.x &&
         pos.y - radius < bottomRight.y && pos.y + radius > topLeft.y;
}

bool IsRectOnScreen(Vector2 pos, Vector2 size) {
  Vector2 topLeft = GetScreenToWorld2D((Vector2){0, 0}, camera);
  Vector2 bottomRight = GetScreenToWorld2D(
      (Vector2){GetScreenWidth(), GetScreenHeight()}, camera);
  return pos.x < bottomRight.x && pos.x + size.x > topLeft.x &&
         pos.y < bottomRight.y && pos.y + size.y > topLeft.y;
}

void CameraHandleWindowResize(ecs_iter_t *it) {
  if (IsWindowResized()) {
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
  }
}

// works by getting a point within an area, but not within a disallowed area
// for example, the entire level might be totalArea, while the actively viewed
// by camera area is disallowedArea
static Vector2 _RandomPointInArea(Area disallowedArea, Area totalArea) {
  Vector2 randomPoint = {0, 0};
  int iterations = 0;
  do {
    ++iterations;
    randomPoint =
        (Vector2){GetRandomValue(totalArea.topLeft.x, totalArea.bottomRight.x),
                  GetRandomValue(totalArea.topLeft.y, totalArea.bottomRight.y)};
    if (iterations > 1000) {
      printf("iterations exceeded 1000\n");
      break;
    }
    // do while inside camera bounds
  } while (randomPoint.x > disallowedArea.topLeft.x &&
           randomPoint.x < disallowedArea.bottomRight.x &&
           randomPoint.y > disallowedArea.topLeft.y &&
           randomPoint.y < disallowedArea.bottomRight.y);
  return randomPoint;
}

Area GetCameraBounds() {
  Vector2 cameraTopLeft = GetScreenToWorld2D((Vector2){0, 0}, camera);
  Vector2 cameraBottomRight = GetScreenToWorld2D(
      (Vector2){GetScreenWidth(), GetScreenHeight()}, camera);
  return (Area){cameraTopLeft, cameraBottomRight};
}

Area AreaAddBuffer(Area area, float buffer) {
  return (Area){Vector2SubtractValue(area.topLeft, buffer),
                Vector2AddValue(area.bottomRight, buffer)};
}

Vector2 RandomPointOffScreen(float minDistance, float maxDistance) {
  Area cameraBounds = AreaAddBuffer(GetCameraBounds(), minDistance);
  return _RandomPointInArea(cameraBounds,
                            AreaAddBuffer(cameraBounds, maxDistance));
}

Vector2 ClampedRandomPointOffScreen(float minDistance, float maxDistance,
                                    Area clampedArea) {
  Area cameraBounds = AreaAddBuffer(GetCameraBounds(), minDistance);
  Area outerBounds = AreaAddBuffer(cameraBounds, maxDistance);
  Area clamped = {
      Vector2Clamp(outerBounds.topLeft, clampedArea.topLeft,
                   clampedArea.bottomRight),
      Vector2Clamp(Vector2AddValue(outerBounds.bottomRight, maxDistance),
                   clampedArea.topLeft, clampedArea.bottomRight),
  };

  return _RandomPointInArea(cameraBounds, clamped);
}

void CameraImport(ecs_world_t *world) {
  screenWidth = GetScreenWidth();
  screenHeight = GetScreenHeight();

  camera = (Camera2D){
      .offset = {screenWidth / 2.0f, screenHeight / 2.0f},
      .target = {0, 0},
      .rotation = 0.0f,
      .zoom = 1.0f,
  };
  ECS_MODULE(world, Camera);
  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Players);
  Position a = {0, 0};

  ECS_SYSTEM_DEFINE(world, MoveCameraToPosition, EcsPostUpdate, players.Player,
                    movement.Position);

  // on store is 2d camera rendering, UI rendering is done in
  // any othe stage before EcsPreStore
  ECS_SYSTEM_DEFINE(world, Begin2D, EcsPreStore);
  ECS_SYSTEM_DEFINE(world, End2D, EcsPostFrame);
  ECS_SYSTEM_DEFINE(world, CameraHandleWindowResize, EcsOnLoad);
#ifdef DEBUG
  ECS_SYSTEM_DEFINE(world, MouseScrollZoom, EcsPostUpdate);
#endif
}