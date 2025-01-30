#pragma once

#include "flecs.h"

typedef struct ProjectileShooter {
  float damage;
  float attackRate;
  float bulletSpeed;
  float range;

  long lastShot;
} ProjectileShooter;

typedef float Damage;

extern ECS_DECLARE(Weapon);
extern ECS_DECLARE(Projectile);
extern ECS_DECLARE(TargetsClosestEnemy);
extern ECS_COMPONENT_DECLARE(ProjectileShooter);
extern ECS_COMPONENT_DECLARE(Damage);

extern ECS_DECLARE(Holds);  // Holds(Player, Weapon)

ecs_entity_t SimpleGun(ecs_world_t *world);
void ItemImport(ecs_world_t *world);
