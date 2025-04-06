#include "modules/health.h"

#include <stdio.h>

#include "flecs.h"
#include "helpers.h"
#include "modules/collisions.h"
#include "modules/enemies.h"
#include "modules/item.h"
#include "modules/movement.h"
#include "modules/player.h"
#include "raylib.h"
#include "tick.h"

ECS_COMPONENT_DECLARE(Killable);
ECS_COMPONENT_DECLARE(Despawn);
ECS_COMPONENT_DECLARE(Damage);
ECS_COMPONENT_DECLARE(Healthbar);

ECS_SYSTEM_DECLARE(ShowHealth);
ECS_SYSTEM_DECLARE(KillHealthless);
ECS_SYSTEM_DECLARE(DespawnEntities);
ECS_SYSTEM_DECLARE(DamageNonPlayerOnCollision);
ECS_SYSTEM_DECLARE(DamagePlayerOnCollision);

void ShowHealth(ecs_iter_t *it) {
  Killable *k = ecs_field(it, Killable, 0);
  Position *p = ecs_field(it, Position, 1);
  Healthbar *bar = ecs_field(it, Healthbar, 2);
  for (int i = 0; i < it->count; i++) {
    if (k[i].health < k[i].maxHealth) {
      // draw healthbar
      Rectangle backgroundBar = {p[i].x - bar[i].width / 2,
                                 p[i].y - bar[i].topMargin, bar[i].width, 5};
      Rectangle health = {p[i].x - bar[i].width / 2, p[i].y - bar[i].topMargin,
                          bar[i].width * (k[i].health / k[i].maxHealth), 5};
      DrawRectangleRounded(backgroundBar, 0.8f, 4, BLACK);
      DrawRectangleRounded(health, 0.8f, 4, GREEN);
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
      if (ecs_has(it->world, it->entities[i], Player)) {
        // player died, todo
        // exit(1);
      } else {
        ecs_delete(it->world, it->entities[i]);
      }
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

static void _DamageOnCollision(ecs_iter_t *it) {
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
  }
}
inline static void DamageNonPlayerOnCollision(ecs_iter_t *it) {
  _DamageOnCollision(it);
}
inline static void DamagePlayerOnCollision(ecs_iter_t *it) {
  _DamageOnCollision(it);
}

void HealthImport(ecs_world_t *world) {
  ECS_MODULE(world, Health);
  ECS_IMPORT(world, Collisions);
  ECS_IMPORT(world, Enemies);
  ECS_IMPORT(world, Players);

  ECS_COMPONENT_DEFINE(world, Killable);
  ECS_COMPONENT_DEFINE(world, Despawn);
  ECS_COMPONENT_DEFINE(world, Damage);
  ECS_COMPONENT_DEFINE(world, Healthbar);

  ECS_SYSTEM_DEFINE(world, KillHealthless, EcsPreStore, Killable);
  ECS_SYSTEM_DEFINE(world, DespawnEntities, EcsPreStore, Despawn);
  ECS_SYSTEM_DEFINE(world, ShowHealth, EcsOnStore, Killable, movement.Position,
                    Healthbar);

  ECS_SYSTEM_DEFINE(
      world, DamageNonPlayerOnCollision, EcsPostUpdate, [in] Damage($this),
      [in] collisions.CollidesWith($this, $other), [inout] Killable($other),
      !enemies.Enemy($this), !players.PlayerTeam($other));
  ECS_SYSTEM_DEFINE(
      world, DamagePlayerOnCollision, EcsPostUpdate, [in] Damage($this),
      [in] collisions.CollidesWith($this, $other), [inout] Killable($other),
      enemies.Enemy($this), players.PlayerTeam($other));

  // ecs_set_tick_source(world, ecs_id(DamageNonPlayerOnCollision),
  // tick_source); ecs_set_tick_source(world, ecs_id(DamagePlayerOnCollision),
  // tick_source);
}