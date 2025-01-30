#include "flecs.h"

ECS_DECLARE(Player);

void PlayerImport(ecs_world_t *world) {
  ECS_MODULE(world, Player);
  ECS_TAG_DEFINE(world, Player);
}