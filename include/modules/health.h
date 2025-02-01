#pragma once

#include "flecs.h"

typedef struct Killable {
  float health;
  float maxHealth;
} Killable;

typedef float Damage;
// float is margin between entity position and healthbar

#define KILLABLE(health) {health, health}
#define HEALTHBAR_GIVEN_RADIUS(radius) \
  {.width = radius * 2, .topMargin = radius + 10}
// adds default margin between entity and healthbar

extern ECS_COMPONENT_DECLARE(Killable);
extern ECS_COMPONENT_DECLARE(Damage);

extern ECS_SYSTEM_DECLARE(KillHealthless);
extern ECS_SYSTEM_DECLARE(DamageAll);

void HealthImport(ecs_world_t *world);
