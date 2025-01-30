#include "modules/controls.h"

#include "flecs.h"
#include "modules/movement.h"
#include "modules/player.h"
#include "raylib.h"
#include "raymath.h"

ECS_COMPONENT_DECLARE(InputStates);

ECS_SYSTEM_DECLARE(CaptureKeys);

static void CaptureKeys(ecs_iter_t *it) {
  InputStates *i = ecs_field(it, InputStates, 0);

  i->WASD.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
  i->WASD.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
  i->ArrowKeys.x = IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT);
  i->ArrowKeys.x = IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP);
}

void ControlsImport(ecs_world_t *world) {
  ECS_MODULE(world, Controls);

  ECS_COMPONENT_DEFINE(world, InputStates);

  ecs_singleton_set(world, InputStates, {.WASD = {0, 0}, .ArrowKeys = {0, 0}});

  ECS_SYSTEM_DEFINE(world, CaptureKeys, EcsOnLoad, InputStates);
}