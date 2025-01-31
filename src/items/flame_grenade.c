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

void SplashFlameGrenade(ecs_world_t *world, ecs_entity_t e) {
  ecs_entity_t parent = ecs_get_parent(world, e);
  const ArcMotion *arc = ecs_get(world, e, ArcMotion);
  const Damage *d = ecs_get(world, e, Damage);
  ecs_entity_t splash = ecs_new(world);
  // ecs_add_pair(world, splash, EcsChildOf, parent);
  ecs_delete(world, e);
  ecs_set(world, splash, Position, {arc->to.x, arc->to.y});
  ecs_set_pair(world, splash, CircleShape, BackgroundRenderLayer,
               {.offset = {0, 0}, .radius = 50, .color = ORANGE});
  ecs_set(world, splash, Collidable, {.radius = 50});
  ecs_set(world, splash, Despawn, DESPAWN_IN(10));
  ecs_set(world, splash, Damage, {*d});
}

void FireFlameGrenade(ecs_world_t *world, ProjectileShooter *shooter,
                      Position *p) {
  Position *closestEnemy = ClosestEnemy(*p, world, shooter->range);
  if (closestEnemy == NULL) {
    return;
  }
  ecs_entity_t bullet =
      ecs_new_w_pair(world, EcsIsA, shooter->projectilePrefab);
  ecs_set(world, bullet, Position, {p->x, p->y});
  ecs_set(world, bullet, ArcMotion,
          {.from = *p, .to = *closestEnemy, .onComplete = SplashFlameGrenade});
}

ecs_entity_t FlameGrenade(ecs_world_t *world) {
  ecs_entity_t weapon = ecs_new(world);
  ecs_add(world, weapon, TargetsClosestEnemy);

  ecs_entity_t bulletPrefab = ecs_new(world);
  ecs_add_id(world, bulletPrefab, EcsPrefab);
  ecs_add(world, bulletPrefab, Projectile);
  ecs_set(world, bulletPrefab, CircleShape,
          {.offset = {0, 0}, .radius = 5, .color = ORANGE});
  ecs_set(world, bulletPrefab, Damage, {20});
  // ecs_add(world, bulletPrefab, Position);

  ecs_set(world, weapon, ProjectileShooter,
          {
              .attackRate = 0.2f,
              .range = 800,
              .projectilePrefab = bulletPrefab,
              .fire = FireFlameGrenade,
          });
  ecs_add_pair(world, bulletPrefab, EcsChildOf, weapon);
  return weapon;
}