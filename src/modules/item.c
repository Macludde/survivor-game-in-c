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

  ecs_add_pair(world, ecs_id(ProjectileShooter), EcsIsA, Weapon);

  // todo: items are still weird with the new traversal queries. Simple gun
  // shoots from {0,0}
  ECS_SYSTEM_DEFINE(world, FireProjectiles, EcsOnUpdate, ProjectileShooter,
                    movement.Position(self | up));

  ECS_SYSTEM_DEFINE(world, DamageOnCollision, EcsOnUpdate, Damage($this),
                    collisions.CollidesWith($this, $other),
                    health.Killable($other), !player.Player($other));
}