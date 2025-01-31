#include "modules/item.h"

#include "flecs.h"
#include "helpers.h"
#include "modules/collisions.h"
#include "modules/health.h"
#include "modules/movement.h"
#include "modules/player.h"
#include "modules/render.h"
#include "modules/rigidbody.h"

ECS_DECLARE(Weapon);
ECS_DECLARE(Projectile);
ECS_DECLARE(TargetsClosestEnemy);
ECS_COMPONENT_DECLARE(ProjectileShooter);
ECS_COMPONENT_DECLARE(Damage);

ECS_DECLARE(Holds);  // Holds(Player, Weapon)

ECS_SYSTEM_DECLARE(FireProjectiles);
ECS_SYSTEM_DECLARE(DamageOnCollision);

static void FireProjectile(ecs_world_t *world, ProjectileShooter *shooter,
                           Position *p) {
  shooter->fire(world, shooter, p);
  shooter->lastShot = time_in_seconds();
}

static bool CanFire(ProjectileShooter *shooter) {
  return time_in_seconds() - shooter->lastShot >= 1.0f / shooter->attackRate;
}

void FireProjectiles(ecs_iter_t *it) {
  //   ecs_id_t pair_id = ecs_field_id(it, 1);
  //   ecs_entity_t shooter = ecs_pair_second(it->world, pair_id);
  ProjectileShooter *shooter = ecs_field(it, ProjectileShooter, 2);
  Position *p = ecs_field(it, Position, 3);
  long now = time_in_seconds();
  for (int i = 0; i < it->count; ++i) {
    if (CanFire(&shooter[i])) FireProjectile(it->world, &shooter[i], &p[i]);
  }
}

void DamageOnCollision(ecs_iter_t *it) {
  Damage *damage = ecs_field(it, Damage, 0);
  Killable *killable = ecs_field(it, Killable, 2);
  for (int i = 0; i < it->count; ++i) {
    ecs_entity_t bullet = it->entities[i];
    if (ecs_has_id(it->world, bullet, Projectile)) {
      killable[i].health -= damage[i];
      ecs_delete(it->world, bullet);
    } else {
      killable[i].health -= damage[i] * it->delta_time;
    }
  }
}

void ItemImport(ecs_world_t *world) {
  ECS_MODULE(world, Item);
  ECS_IMPORT(world, Player);
  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Collisions);
  ECS_IMPORT(world, Health);

  ECS_COMPONENT_DEFINE(world, ProjectileShooter);
  ECS_COMPONENT_DEFINE(world, Damage);
  ECS_TAG_DEFINE(world, Weapon);
  ECS_TAG_DEFINE(world, Projectile);
  ECS_TAG_DEFINE(world, TargetsClosestEnemy);
  ECS_TAG_DEFINE(world, Holds);

  ecs_add_pair(world, ecs_id(ProjectileShooter), EcsIsA, Weapon);

  ECS_SYSTEM_DEFINE(world, FireProjectiles, EcsOnUpdate, player.Player($this),
                    Holds($this, $other), ProjectileShooter($other), movement.Position($this), ?TargetsClosestEnemy);
  ECS_SYSTEM_DEFINE(world, DamageOnCollision, EcsOnUpdate, Damage($this),
                    collisions.CollidesWith($this, $other),
                    health.Killable($other), !player.Player($other));
}