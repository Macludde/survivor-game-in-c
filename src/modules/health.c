#include "modules/health.h"

#include <stdio.h>

#include "flecs.h"
#include "helpers.h"
#include "modules/collisions.h"
#include "modules/item.h"
#include "modules/movement.h"
#include "modules/player.h"
#include "raylib.h"

ECS_COMPONENT_DECLARE(Killable);
ECS_COMPONENT_DECLARE(Despawn);

ECS_SYSTEM_DECLARE(ShowHealth);
ECS_SYSTEM_DECLARE(KillHealthless);
ECS_SYSTEM_DECLARE(DespawnEntities);
ECS_SYSTEM_DECLARE(DamageOnCollision);

void ShowHealth(ecs_iter_t *it) {
  Killable *k = ecs_field(it, Killable, 0);
  Position *p = ecs_field(it, Position, 1);
  for (int i = 0; i < it->count; i++) {
    if (k[i].health < k[i].maxHealth) {
      // draw healthbar
      DrawRectangle(p[i].x - 20, p[i].y - 60, 40, 5, BLACK);
      DrawRectangle(p[i].x - 20, p[i].y - 60,
                    40 * (k[i].health / k[i].maxHealth), 5, GREEN);
// draw text above healthbar
#if DEBUG
      DrawText(TextFormat("%.0f / %.0f", k[i].health, k[i].maxHealth),
               p[i].x - 20, p[i].y - 80, 10, BLACK);
#endif
    }
  }
}

void KillHealthless(ecs_iter_t *it) {
  Killable *k = ecs_field(it, Killable, 0);
  for (int i = 0; i < it->count; i++) {
    if (k[i].health <= EPSILON) {
      ecs_delete(it->world, it->entities[i]);
    }
  }
}

void DespawnEntities(ecs_iter_t *it) {
  Despawn *d = ecs_field(it, Despawn, 0);
  for (int i = 0; i < it->count; i++) {
    if (time_in_seconds() - d[i].createdAt >= d[i].despawnAfter) {
      ecs_delete(it->world, it->entities[i]);
    }
  }
}

void DamageOnCollision(ecs_iter_t *it) {
  Damage *damage = ecs_field(it, Damage, 0);
  Killable *killable = ecs_field(it, Killable, 2);
  for (int i = 0; i < it->count; ++i) {
    ecs_entity_t damagingEntity = it->entities[i];
    float damageToDeal;
    if (ecs_has_id(it->world, damagingEntity, Projectile)) {
      // deal entire damage and remove
      damageToDeal = damage[i];
      ecs_delete(it->world, damagingEntity);
    } else {
      damageToDeal = damage[i] * it->delta_time;
    }
    killable[i].health -= damageToDeal;
    printf("Entity %d took %.2f damage\n", it->entities[i], damageToDeal);
  }
}

void HealthImport(ecs_world_t *world) {
  ECS_MODULE(world, Health);
  ECS_IMPORT(world, Collisions);
  ECS_IMPORT(world, Player);

  ECS_COMPONENT_DEFINE(world, Killable);
  ECS_COMPONENT_DEFINE(world, Despawn);
  ECS_COMPONENT_DEFINE(world, Damage);

  ECS_SYSTEM_DEFINE(world, KillHealthless, EcsPreStore, Killable);
  ECS_SYSTEM_DEFINE(world, DespawnEntities, EcsPreStore, Despawn);
  ECS_SYSTEM_DEFINE(world, ShowHealth, EcsOnStore, Killable, movement.Position);

  ECS_SYSTEM_DEFINE(world, DamageOnCollision, EcsPostUpdate, Damage($this),
                    collisions.CollidesWith($this, $other), Killable($other),
                    !player.Player($other));
}