#pragma once

#include "flecs.h"

typedef struct Killable {
  float health;
  float maxHealth;
} Killable;

#define KILLABLE(health) {health, health}

extern ECS_COMPONENT_DECLARE(Killable);

extern ECS_SYSTEM_DECLARE(KillHealthless);

void HealthImport(ecs_world_t *world);
