#include "level.h"

#include "flecs.h"
#include "helpers.h"
#include "modules/collisions.h"
#include "modules/movement.h"
#include "modules/render.h"
#include "modules/rigidbody.h"
#include "raylib.h"
#include "raymath.h"

ecs_entity_t treePrefab = 0;

ecs_entity_t TreePrefab(ecs_world_t *world) {
  ecs_entity_t tree =
      ecs_entity(world, {.name = "Tree", .add = ecs_ids(EcsPrefab)});
  ecs_add(world, tree, Position);
  ecs_set(world, tree, Velocity, {0, 0});
  ecs_set_pair(world, tree, RectShape, BackgroundRenderLayer,
               {
                   .offset = {0, 0},
                   .size = {TREE_TRUNK_WIDTH, TREE_TRUNK_HEIGHT},
                   .color = BROWN,
               });
  ecs_set_pair(world, tree, CircleShape, ForegroundRenderLayer,
               {
                   .offset = {0, -TREE_TRUNK_HEIGHT / 2},
                   .radius = TREE_COLLISION_RADIUS,
                   .color = GREEN,
               });

  ecs_set(world, tree, Collidable, {.radius = TREE_TRUNK_HEIGHT / 2});
  ecs_set(world, tree, Rigidbody,
          RIGIDBODY(INT16_MAX));  // somewhat large number

  return tree;
}

ecs_entity_t CreateTree(ecs_world_t *world, Vector2 pos) {
  ecs_entity_t tree = ecs_new_w_pair(world, EcsIsA, treePrefab);
  ecs_set(world, tree, Position, {pos.x, pos.y});

  return tree;
}

void InitializeLevel(ecs_world_t *world, Level *level) {
  // split level up into grid of 200x200, spawn a tree within each grid. Gap of
  // 50 pixels around so 100x100 area to pick from
  if (treePrefab == 0) treePrefab = TreePrefab(world);
  int gridWidth = level->width / 200;
  int gridHeight = level->height / 200;
  for (int i = -gridWidth; i < gridWidth; ++i) {
    for (int j = -gridHeight; j < gridHeight; ++j) {
      if (i == 0 && j == 0) continue;
      Vector2 treePos = {
          GetRandomValue(-50, 50) + i * 200 + 100,
          GetRandomValue(-50, 50) + j * 200 + 100,
      };
      CreateTree(world, treePos);
    }
  }
}