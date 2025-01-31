#include "flecs.h"
#include "helpers.h"
#include "modules/collisions.h"
#include "modules/health.h"
#include "modules/item.h"
#include "modules/movement.h"
#include "modules/render.h"

#ifndef ITEMS
#define ITEMS

// List to all item entries
ecs_entity_t SimpleGun(ecs_world_t *world);
ecs_entity_t FlameGrenade(ecs_world_t *world);
ecs_entity_t OrbitOrb(ecs_world_t *world);

#endif