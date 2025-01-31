#include "modules/controls.h"

#include "flecs.h"
#include "modules/movement.h"
#include "raylib.h"
#include "raymath.h"

ECS_COMPONENT_DECLARE(InputStates);

ECS_SYSTEM_DECLARE(CaptureKeys);
ECS_SYSTEM_DECLARE(WASDMove);
ECS_SYSTEM_DECLARE(ArrowsMove);

ECS_DECLARE(WASDMovable);
ECS_DECLARE(ArrowsMovable);

static void CaptureKeys(ecs_iter_t *it) {
  InputStates *i = ecs_field(it, InputStates, 0);

  i->WASD.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
  i->WASD.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
  i->ArrowKeys.x = IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT);
  i->ArrowKeys.y = IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP);
}

static void WASDMove(ecs_iter_t *it) {
  const InputStates *input = ecs_singleton_get(it->world, InputStates);
  Acceleration *a = ecs_field(it, Acceleration, 0);
  for (int i = 0; i < it->count; ++i) {
    a[i].x += input->WASD.x * 400;
    a[i].y += input->WASD.y * 400;
  }
}

static void ArrowsMove(ecs_iter_t *it) {
  const InputStates *input = ecs_singleton_get(it->world, InputStates);
  Acceleration *a = ecs_field(it, Acceleration, 0);
  for (int i = 0; i < it->count; ++i) {
    a[i].x += input->ArrowKeys.x * 400;
    a[i].y += input->ArrowKeys.y * 400;
  }
}

void ControlsImport(ecs_world_t *world) {
  ECS_MODULE(world, Controls);
  ECS_IMPORT(world, Movement);

  ECS_COMPONENT_DEFINE(world, InputStates);

  ECS_TAG_DEFINE(world, WASDMovable);
  ECS_TAG_DEFINE(world, ArrowsMovable);

  ecs_singleton_set(world, InputStates, {.WASD = {0, 0}, .ArrowKeys = {0, 0}});

  ECS_SYSTEM_DEFINE(world, CaptureKeys, EcsOnLoad, InputStates);
  ECS_SYSTEM_DEFINE(world, WASDMove, EcsPostLoad, movement.Acceleration,
                    WASDMovable);
  ECS_SYSTEM_DEFINE(world, ArrowsMove, EcsPostLoad, movement.Acceleration,
                    ArrowsMovable);
}