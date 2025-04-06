#pragma once
#include "flecs.h"

extern ecs_entity_t tick_source;

void InitializeTickSource(ecs_world_t *world);