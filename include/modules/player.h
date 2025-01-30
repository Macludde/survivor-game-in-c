#pragma once

#include "flecs.h"

extern ECS_DECLARE(Player);

void PlayerImport(ecs_world_t *world);

// HeldItem(Player, ...)