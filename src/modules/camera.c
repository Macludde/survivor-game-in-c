#include "./camera.h"

#include "./base.h"
#include "./movement.h"
#include "flecs.h"
#include "raylib.h"
#include "raymath.h"

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
  static int screenWidth;
  static int screenHeight;
  Position *p = ecs_field(it, Position, 1);
  camera.target =
      Vector2Lerp(camera.target, p[0], CAMERA_MOVEMENT_SPEED * it->delta_time);
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
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    camera.offset = (Vector2){width / 2.0f, height / 2.0f};
  }
}

void CameraImport(ecs_world_t *world) {
  camera = (Camera2D){
      .offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
      .target = {0, 0},
      .rotation = 0.0f,
      .zoom = 1.0f,
  };
  ECS_MODULE(world, Camera);
  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Base);
  Position a = {0, 0};

  ECS_SYSTEM_DEFINE(world, MoveCameraToPosition, EcsPostUpdate, base.Player,
                    movement.Position);

  // on store is 2d camera rendering, UI rendering is done in
  // any othe stage before EcsPreStore
  ECS_SYSTEM_DEFINE(world, Begin2D, EcsPreStore);
  ECS_SYSTEM_DEFINE(world, End2D, EcsPostFrame);
  ECS_SYSTEM_DEFINE(world, CameraHandleWindowResize, EcsPostUpdate);
#ifdef DEBUG
  ECS_SYSTEM_DEFINE(world, MouseScrollZoom, EcsPostUpdate);
#endif
}