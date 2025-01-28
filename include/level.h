#pragma once

#define TREE_HEALTH 100
// collision/physics
#define TREE_COLLISION_RADIUS 20
#define TREE_MASS 10000
// rendering
#define TREE_TRUNK_WIDTH 20
#define TREE_TRUNK_HEIGHT 40

#include "flecs.h"

typedef struct Level {
  int width;
  int height;
} Level;

void InitializeLevel(ecs_world_t *world, Level *level);