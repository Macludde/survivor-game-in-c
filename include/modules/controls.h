#pragma once

#include "flecs.h"

// Components
typedef struct Controllable2D {
  int8_t x;
  int8_t y;
} Controllable2D;

typedef struct InputStates {
  Controllable2D WASD;
  Controllable2D ArrowKeys;
} InputStates;

extern ECS_COMPONENT_DECLARE(InputStates);

extern ECS_SYSTEM_DECLARE(CaptureKeys);

// The import function name has to follow the convention: <ModuleName>Import
void ControlsImport(ecs_world_t *world);