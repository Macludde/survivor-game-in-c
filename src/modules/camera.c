#include "./camera.h"

#include "./movement.h"
#include "flecs.h"
#include "raylib.h"
#include "raymath.h"

extern ecs_world_t *world;
ECS_COMPONENT_DECLARE(FollowCam);

ECS_SYSTEM_DECLARE(MoveCameraToPosition);
ECS_SYSTEM_DECLARE(Begin2D);
ECS_SYSTEM_DECLARE(End2D);
#ifdef DEBUG
ECS_SYSTEM_DECLARE(MouseScrollZoom);
#endif

static void Begin2D(ecs_iter_t *it) {
  FollowCam *camera = ecs_field(it, FollowCam, 0);
  BeginMode2D(camera[0]);
}

static void End2D(ecs_iter_t *it) { EndMode2D(); }

static void MoveCameraToPosition(ecs_iter_t *it) {
  static int screenWidth;
  static int screenHeight;
  FollowCam *camera = ecs_field(it, FollowCam, 0);
  Position *p = ecs_field(it, Position, 1);
  if (GetScreenWidth() != screenWidth || GetScreenHeight() != screenHeight) {
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    camera[0].offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
  }

  camera[0].target = Vector2Lerp(camera[0].target, p[0],
                                 CAMERA_MOVEMENT_SPEED * it->delta_time);
}

static void MouseScrollZoom(ecs_iter_t *it) {
  FollowCam *camera = ecs_field(it, FollowCam, 0);
  camera[0].zoom += ((float)GetMouseWheelMove() * 0.05f);
  if (camera[0].zoom > 3.0f)
    camera[0].zoom = 3.0f;
  else if (camera[0].zoom < 0.1f)
    camera[0].zoom = 0.1f;
}

bool IsPointOnScreen(Vector2 pos) {
  const FollowCam *camera =
      ecs_get(world, ecs_lookup(world, "Player"), FollowCam);
  Vector2 topLeft = GetScreenToWorld2D((Vector2){0, 0}, *camera);
  Vector2 bottomRight = GetScreenToWorld2D(
      (Vector2){GetScreenWidth(), GetScreenHeight()}, *camera);
  return pos.x > topLeft.x && pos.x < bottomRight.x && pos.y > topLeft.y &&
         pos.y < bottomRight.y;
}

bool IsCircleOnScreen(Vector2 pos, float radius) {
  const FollowCam *camera =
      ecs_get(world, ecs_lookup(world, "Player"), FollowCam);
  Vector2 topLeft = GetScreenToWorld2D((Vector2){0, 0}, *camera);
  Vector2 bottomRight = GetScreenToWorld2D(
      (Vector2){GetScreenWidth(), GetScreenHeight()}, *camera);
  return pos.x - radius < bottomRight.x && pos.x + radius > topLeft.x &&
         pos.y - radius < bottomRight.y && pos.y + radius > topLeft.y;
}

bool IsRectOnScreen(Vector2 pos, Vector2 size) {
  const FollowCam *camera =
      ecs_get(world, ecs_lookup(world, "Player"), FollowCam);
  Vector2 topLeft = GetScreenToWorld2D((Vector2){0, 0}, *camera);
  Vector2 bottomRight = GetScreenToWorld2D(
      (Vector2){GetScreenWidth(), GetScreenHeight()}, *camera);
  return pos.x < bottomRight.x && pos.x + size.x > topLeft.x &&
         pos.y < bottomRight.y && pos.y + size.y > topLeft.y;
}

void CameraImport(ecs_world_t *_world) {
  ECS_MODULE(_world, Camera);
  ECS_IMPORT(_world, Movement);
  ECS_COMPONENT_DEFINE(_world, FollowCam);
  Position a = {0, 0};

  ECS_SYSTEM_DEFINE(_world, MoveCameraToPosition, EcsPostUpdate, FollowCam,
                    movement.Position);

  // on store is 2d camera rendering, UI rendering is done in
  // any othe stage before EcsPreStore
  ECS_SYSTEM_DEFINE(_world, Begin2D, EcsPreStore, FollowCam);
  ECS_SYSTEM_DEFINE(_world, End2D, EcsPostFrame, FollowCam);
#ifdef DEBUG
  ECS_SYSTEM_DEFINE(world, MouseScrollZoom, EcsPostUpdate, FollowCam($));
#endif
}