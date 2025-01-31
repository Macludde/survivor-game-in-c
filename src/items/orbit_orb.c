#include "flecs.h"
#include "items.h"
#include "modules/physics.h"

void OnHoldOrbitOrb(ecs_iter_t *it) {
  Position *p = ecs_field(it, Position, 0);
  CircularMotion *c = ecs_field(it, CircularMotion, 3);
  for (int i = 0; i < it->count; ++i) {
    ecs_add_pair(it->world, it->entities[i], EcsChildOf, it->entities[i]);
  }
}

ecs_entity_t OrbitOrb(ecs_world_t *world) {
  ecs_entity_t weapon = ecs_new(world);

  ecs_entity_t orb = ecs_new(world);
  ecs_set(world, orb, CircleShape,
          {.offset = {0, 0}, .radius = 15, .color = PURPLE});
  ecs_set(world, orb, Collidable, {.radius = 20});
  ecs_set(world, orb, Damage, {100.0f});
  ecs_set(world, orb, Position, {0, 0});
  ecs_set(world, orb, CircularMotion,
          {.center = {0, 0}, .radius = 100, .angle = 0, .speed = 30 * DEG2RAD});
  ecs_add(world, orb, OrbitsParent);
  ecs_add_pair(world, orb, EcsChildOf, weapon);

  ecs_add(world, weapon, Weapon);
  return weapon;
}