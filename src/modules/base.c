#include "flecs.h"

ECS_DECLARE(Player);

void BaseImport(ecs_world_t *world) {
  ECS_MODULE(world, Base);
  ECS_TAG_DEFINE(world, Player);
}