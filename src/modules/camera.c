#include "./camera.h"

#include "./base.h"
#include "./movement.h"
#include "flecs.h"
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
  float maxDistanceHorizontal = screenWidth * 0.4f;
  float maxDistanceVertical = screenHeight * 0.4f;
  if ((int)p[0].x - (int)camera.target.x > maxDistanceHorizontal) {
    camera.target.x = p[0].x - maxDistanceHorizontal;
  } else if ((int)p[0].x - (int)camera.target.x < -maxDistanceHorizontal) {
    camera.target.x = p[0].x + maxDistanceHorizontal;
  }
  if ((int)p[0].y - (int)camera.target.y > maxDistanceVertical) {
    camera.target.y = p[0].y - maxDistanceVertical;
  } else if ((int)p[0].y - (int)camera.target.y < -maxDistanceVertical) {
    camera.target.y = p[0].y + maxDistanceVertical;
  }
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
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
  }
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
  ECS_IMPORT(world, Base);
  Position a = {0, 0};

  ECS_SYSTEM_DEFINE(world, MoveCameraToPosition, EcsPostUpdate, base.Player,
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