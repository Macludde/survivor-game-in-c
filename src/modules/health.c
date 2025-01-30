#include "modules/health.h"

#include "flecs.h"
#include "helpers.h"

ECS_COMPONENT_DECLARE(Killable);

ECS_SYSTEM_DECLARE(KillHealthless);

void KillHealthless(ecs_iter_t *it) {
  Killable *k = ecs_field(it, Killable, 0);
  for (int i = 0; i < it->count; i++) {
    if (k[i].health <= EPSILON) {
      ecs_delete(it->world, it->entities[i]);
    }
  }
}

void HealthImport(ecs_world_t *world) {
  ECS_MODULE(world, Health);

  ECS_COMPONENT_DEFINE(world, Killable);

  ECS_SYSTEM_DEFINE(world, KillHealthless, EcsPreStore, Killable);
}