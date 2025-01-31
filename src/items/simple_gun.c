#include "flecs.h"
#include "helpers.h"
#include "modules/collisions.h"
#include "modules/enemies.h"
#include "modules/health.h"
#include "modules/item.h"
#include "modules/movement.h"
#include "modules/player.h"
#include "modules/render.h"
#include "modules/rigidbody.h"

void FireSimpleGun(ecs_world_t *world, ProjectileShooter *shooter,
                   Position *p) {
  Position *closestEnemy = ClosestEnemy(*p, world, shooter->range);
  if (closestEnemy == NULL) {
    return;
  }
  Vector2 direction = SafeNormalize(Vector2Subtract(*closestEnemy, *p));
  Vector2 bulletVelocity = Vector2Scale(direction, shooter->bulletSpeed);

  ecs_entity_t bullet =
      ecs_new_w_pair(world, EcsIsA, shooter->projectilePrefab);
  ecs_set(world, bullet, Position, {p->x, p->y});
  ecs_set(world, bullet, Velocity, {bulletVelocity.x, bulletVelocity.y});
  ecs_set(world, bullet, Despawn, DESPAWN_IN(2));
}

ecs_entity_t SimpleGun(ecs_world_t *world) {
  ecs_entity_t weapon = ecs_new(world);
  ecs_add(world, weapon, TargetsClosestEnemy);

  ecs_entity_t bulletPrefab = ecs_new(world);
  ecs_add_id(world, bulletPrefab, EcsPrefab);
  ecs_add(world, bulletPrefab, Projectile);
  ecs_set(world, bulletPrefab, CircleShape,
          {.offset = {0, 0}, .radius = 5, .color = YELLOW});
  ecs_set(world, bulletPrefab, Collidable, {.radius = 5});
  ecs_set(world, bulletPrefab, Damage, {10});
  ecs_set(world, bulletPrefab, Friction, {0});
  ecs_add(world, bulletPrefab, Position);
  ecs_add(world, bulletPrefab, Velocity);
  ecs_add_pair(world, bulletPrefab, EcsChildOf, weapon);

  ecs_set(world, weapon, ProjectileShooter,
          {
              .attackRate = 2,
              .bulletSpeed = 800,
              .range = 400,
              .projectilePrefab = bulletPrefab,
              .fire = FireSimpleGun,
          });
  return weapon;
}