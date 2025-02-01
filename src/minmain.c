#include "flecs.h"

typedef struct DoSomethingOnDeath {
  int x;
} DoSomethingOnDeath;
ECS_COMPONENT_DECLARE(DoSomethingOnDeath);

typedef struct SpawnedAfterDeath {
  int y;
} SpawnedAfterDeath;
ECS_COMPONENT_DECLARE(SpawnedAfterDeath);

void onDeath(ecs_iter_t *it) {
  DoSomethingOnDeath *d = ecs_field(it, DoSomethingOnDeath, 0);
  for (int i = 0; i < it->count; ++i) {
    ecs_entity_t newEntity = ecs_new(it->world);
    ecs_set(it->world, newEntity, SpawnedAfterDeath, {d[i].x});  // crashes
  }
}

void SpawnEntities(ecs_iter_t *it) {
  for (int i = 0; i < 10000; ++i) {
    ecs_entity_t toBeKilled = ecs_new(it->world);
    ecs_set(it->world, toBeKilled, DoSomethingOnDeath, {10});
  }
}

void KillEntities(ecs_iter_t *it) {
  DoSomethingOnDeath *d = ecs_field(it, DoSomethingOnDeath, 0);
  for (int i = 0; i < it->count; ++i) {
    ecs_entity_t entity = it->entities[i];
    if (entity % 3 != 2) ecs_delete(it->world, entity);
  }
}

int main2() {
  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, DoSomethingOnDeath);
  ECS_COMPONENT_DEFINE(world, SpawnedAfterDeath);

  ECS_OBSERVER(world, onDeath, EcsOnRemove, DoSomethingOnDeath);
  ECS_SYSTEM(world, SpawnEntities, EcsPreUpdate);
  ECS_SYSTEM(world, KillEntities, EcsPostUpdate, [inout] DoSomethingOnDeath);

  for (int i = 0; i < 10000; ++i) {
    ecs_entity_t toBeKilled = ecs_new(world);
    ecs_set(world, toBeKilled, DoSomethingOnDeath, {10});
  }

  while (true) {
    ecs_progress(world, 1.0f);
  }

  ecs_fini(world);
  return 0;
}
