#include "modules/health.h"

#include <stdio.h>

#include "flecs.h"
#include "modules/collisions.h"
#include "modules/enemies.h"
#include "modules/movement.h"
#include "modules/player.h"
#include "raylib.h"

ECS_COMPONENT_DECLARE(Killable);
ECS_COMPONENT_DECLARE(Damage);

ECS_SYSTEM_DECLARE(KillHealthless);
ECS_SYSTEM_DECLARE(DamageAll);

void KillHealthless(ecs_iter_t *it) {
  Killable *k = ecs_field(it, Killable, 0);
  for (int i = 0; i < it->count; i++) {
    if (k[i].health <= 1e-6) {
      ecs_delete(it->world, it->entities[i]);
    }
  }
}

void DamageAll(ecs_iter_t *it) {
  Killable *killable = ecs_field(it, Killable, 0);
  for (int i = 0; i < it->count; ++i) {
    killable[i].health -= 100.0f * it->delta_time;
  }
}

void HealthImport(ecs_world_t *world) {
  ECS_MODULE(world, Health);
  ECS_IMPORT(world, Collisions);
  ECS_IMPORT(world, Enemies);
  ECS_IMPORT(world, Players);

  ECS_COMPONENT_DEFINE(world, Killable);
  ECS_COMPONENT_DEFINE(world, Damage);

  ECS_SYSTEM_DEFINE(world, KillHealthless, EcsPreStore, Killable);
  ECS_SYSTEM_DEFINE(world, DamageAll, EcsPostUpdate, [inout] Killable);
}