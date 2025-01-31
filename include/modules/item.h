#pragma once

#include "flecs.h"
#include "items.h"
#include "modules/movement.h"

typedef struct ProjectileShooter {
  float attackRate;
  float bulletSpeed;
  float range;
  ecs_entity_t projectilePrefab;

  // this might be a performance killer, but for now it isn't (called
  // infrequently)
  void (*fire)(ecs_world_t *world, struct ProjectileShooter *shooter,
               Position *p);

  double lastShot;
} ProjectileShooter;

typedef float Damage;

extern ECS_DECLARE(Weapon);
extern ECS_DECLARE(Projectile);
extern ECS_DECLARE(TargetsClosestEnemy);
extern ECS_COMPONENT_DECLARE(ProjectileShooter);
extern ECS_COMPONENT_DECLARE(Damage);

void ItemImport(ecs_world_t *world);
