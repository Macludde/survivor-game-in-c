#pragma once
#include "flecs.h"

ecs_entity_t tick_source;

static void InitializeTickSource(ecs_world_t *world) {
  tick_source = ecs_set_interval(world, 0, 10.0f);  // 10 times a second
}