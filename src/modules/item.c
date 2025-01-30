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

ecs_entity_t SimpleGun(ecs_world_t *world) {
  ecs_entity_t weapon = ecs_new(world);
  ecs_set(world, weapon, ProjectileShooter,
          {.damage = 10, .attackRate = 5, .bulletSpeed = 800, .range = 800});
  ecs_add(world, weapon, TargetsClosestEnemy);
  return weapon;
}

static Position *ClosestEnemy(Position playerPos, ecs_world_t *world,
                              int maxRange) {
  static ecs_query_t *enemyPositionsQuery = NULL;
  if (enemyPositionsQuery == NULL) {
    enemyPositionsQuery =
        ecs_query(world, {.expr = "enemies.Enemy, movement.Position"});
  }
  Position *closestEnemy;
  int closestEnemyDistanceSqr = maxRange * maxRange + 1;
  // find closes enemy
  ecs_iter_t targetIt = ecs_query_iter(world, enemyPositionsQuery);

  while (ecs_query_next(&targetIt)) {
    if (targetIt.count == 0) {
      continue;
    }
    Position *targetPosition = ecs_field(&targetIt, Position, 1);
    for (int i = 0; i < targetIt.count; i++) {
      int distance = Vector2DistanceSqr(playerPos, targetPosition[i]);
      if (distance <= closestEnemyDistanceSqr) {
        closestEnemy = &targetPosition[i];
        closestEnemyDistanceSqr = distance;
      }
    }
  }
  if (closestEnemyDistanceSqr == maxRange * maxRange + 1) {
    return NULL;
  }
  return closestEnemy;
}

static void FireProjectile(ecs_world_t *world, ProjectileShooter *shooter,
                           Position *p) {
  Position *closestEnemy = ClosestEnemy(*p, world, shooter->range);
  if (closestEnemy == NULL) {
    return;
  }
  Vector2 direction = SafeNormalize(Vector2Subtract(*closestEnemy, *p));
  Vector2 bulletVelocity = Vector2Scale(direction, shooter->bulletSpeed);
  ecs_entity_t bullet = ecs_new(world);
  ecs_set(world, bullet, Position, {p->x, p->y});
  ecs_set(world, bullet, Velocity, {bulletVelocity.x, bulletVelocity.y});
  ecs_set(world, bullet, CircleShape,
          {.offset = {0, 0}, .radius = 5, .color = YELLOW});
  ecs_set(world, bullet, Collidable, {.radius = 5});
  ecs_set(world, bullet, Damage, {shooter->damage});
  ecs_set(world, bullet, Friction, {0});
  ecs_set(world, bullet, Despawn, DESPAWN_IN(2));
  ecs_add_id(world, bullet, Projectile);

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
    killable[i].health -= damage[i];
    ecs_delete(it->world, bullet);
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
                    health.Killable($other), !player.Player($other),
                    Projectile($this));
}