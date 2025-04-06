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
// float is margin between entity position and healthbar
typedef struct Healthbar {
  float width;
  float topMargin;
} Healthbar;

#define KILLABLE(health) {health, health}
#define DESPAWN_IN(time) {time_in_seconds(), time}
#define HEALTHBAR_GIVEN_RADIUS(radius) \
  {.width = radius * 2, .topMargin = radius + 10}
// adds default margin between entity and healthbar

extern ECS_COMPONENT_DECLARE(Killable);
extern ECS_COMPONENT_DECLARE(Despawn);
extern ECS_COMPONENT_DECLARE(Damage);
extern ECS_COMPONENT_DECLARE(Healthbar);

extern ECS_SYSTEM_DECLARE(KillHealthless);
extern ECS_SYSTEM_DECLARE(DespawnEntities);
extern ECS_SYSTEM_DECLARE(DamageNonPlayerOnCollision);
extern ECS_SYSTEM_DECLARE(DamagePlayerOnCollision);

void HealthImport(ecs_world_t *world);
