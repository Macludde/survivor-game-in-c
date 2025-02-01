#include "modules/item.h"

#include "flecs.h"
#include "helpers.h"
#include "modules/collisions.h"
#include "modules/health.h"
#include "modules/movement.h"
#include "modules/physics.h"
#include "modules/player.h"
#include "modules/render.h"

ECS_DECLARE(Weapon);
ECS_DECLARE(Projectile);
ECS_DECLARE(TargetsClosestEnemy);
ECS_COMPONENT_DECLARE(ProjectileShooter);

ECS_SYSTEM_DECLARE(FireProjectiles);

static void FireProjectile(ecs_world_t *world, ProjectileShooter *shooter,
                           Position *p) {
  shooter->fire(world, shooter, p);
  shooter->lastShot = time_in_seconds();
}

static bool CanFire(ProjectileShooter *shooter) {
  return time_in_seconds() - shooter->lastShot >= 1.0f / shooter->attackRate;
}

void FireProjectiles(ecs_iter_t *it) {
  ecs_entity_t src1 = ecs_field_src(it, 0);
  ecs_entity_t src2 = ecs_field_src(it, 1);
  ProjectileShooter *shooter = ecs_field(it, ProjectileShooter, 0);
  Position *p = ecs_field(it, Position, 1);
  // Because p is seen as a fixed source (due to traversal), this function is
  // called once per position (can still be multiple shooters)
  for (int i = 0; i < it->count; ++i) {
    if (CanFire(&shooter[i])) FireProjectile(it->world, &shooter[i], p);
  }
}

void ItemImport(ecs_world_t *world) {
  ECS_MODULE(world, Item);
  ECS_IMPORT(world, Players);
  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Collisions);
  ECS_IMPORT(world, Health);

  ECS_COMPONENT_DEFINE(world, ProjectileShooter);
  ECS_TAG_DEFINE(world, Weapon);
  ECS_TAG_DEFINE(world, Projectile);
  ECS_TAG_DEFINE(world, TargetsClosestEnemy);

  ecs_add_pair(world, ecs_id(ProjectileShooter), EcsIsA, Weapon);

  ECS_SYSTEM_DEFINE(world, FireProjectiles, EcsOnUpdate, ProjectileShooter,
                    movement.Position(self | up));
}