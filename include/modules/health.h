#pragma once

#include "flecs.h"

typedef struct Killable {
  float health;
  float maxHealth;
} Killable;

typedef struct Despawn {
  double createdAt;
  double despawnAfter;
} Despawn;

typedef float Damage;

#define KILLABLE(health) {health, health}
#define DESPAWN_IN(time) {time_in_seconds(), time}

extern ECS_COMPONENT_DECLARE(Killable);
extern ECS_COMPONENT_DECLARE(Despawn);
extern ECS_COMPONENT_DECLARE(Damage);

extern ECS_SYSTEM_DECLARE(KillHealthless);
extern ECS_SYSTEM_DECLARE(DespawnEntities);
extern ECS_SYSTEM_DECLARE(DamageOnCollision);

void HealthImport(ecs_world_t *world);
