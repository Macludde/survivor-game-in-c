#include "level.h"

#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "entity.h"
#include "lib/stb_ds.h"
#include "physics.h"
#include "raylib.h"
#include "raymath.h"

#define MAX_TREES 1024
void InitializeLevel(Level *level) {
  Tree *trees = malloc(MAX_TREES * sizeof(Tree));
  if (trees == NULL) {
    printf("Failed to allocate memory for trees\n");
    exit(1);
  }

  int retries = 0;
  int i = 0;
  while (retries < 10 && i < MAX_TREES) {
  tree_loop_start:
    trees[i].body.pos =
        (Vector2){GetRandomValue(-level->width, level->width),
                  GetRandomValue(-level->height, level->height)};
    if (Vector2Distance(trees[i].body.pos, Vector2Zero()) < 500) {
      ++retries;
      goto tree_loop_start;
    }
    for (int j = 0; j < i; ++j) {
      if (Vector2Distance(trees[i].body.pos, trees[j].body.pos) < 200) {
        // retry
        ++retries;
        if (retries > 10) {
          --i;
          goto tree_loop_finished;
        }
        goto tree_loop_start;
      }
    }
    retries = 0;

    trees[i].body.mass = TREE_MASS;
    trees[i].body.radius = TREE_COLLISION_RADIUS;
    trees[i].body.velocity = Vector2Zero();
    trees[i].body.acceleration = Vector2Zero();
    trees[i].health = 100;
    trees[i].type = ENTITY_TYPE_TREE;
    ++i;
  }
tree_loop_finished:
  level->treeCount = i;

  trees = realloc(trees, level->treeCount * sizeof(Tree));
  if (trees == NULL) {
    // should not be able to happen
    printf("Could not shrink tree heap, exiting");
    free(trees);
    exit(1);
  }
  arrsetcap(level->allEntities,
            level->treeCount + 100);  // +100 for player and enemies, roughly
  for (i = 0; i < level->treeCount; ++i) {
    arrput(level->allEntities, trees + i);
  }
  level->trees = trees;
}

extern Level level;

Rectangle TreeRectangle(Tree tree) {
  return (Rectangle){tree.body.pos.x - TREE_TRUNK_WIDTH / 2,
                     tree.body.pos.y - TREE_TRUNK_HEIGHT / 2, TREE_TRUNK_WIDTH,
                     TREE_TRUNK_HEIGHT};
}
void DrawLevelBackground() {
  Tree *trees = level.trees;
  for (int i = 0; i < level.treeCount; i++) {
    DrawRectangleRec(TreeRectangle(trees[i]), BROWN);
  }
}
void DrawLevelForeground() {
  Tree *trees = level.trees;
  for (int i = 0; i < level.treeCount; i++) {
    DrawCircle(trees[i].body.pos.x,
               trees[i].body.pos.y - TREE_TRUNK_HEIGHT + 10, 30, GREEN);
  }
}

void AddEntity(Entity *entity) { arrput(level.allEntities, entity); }

// Removes an entity, O(n)
void RemoveEntity(Entity *entity) {
  // since dynamic entities are added at the end of the array, it makes sense to
  // look there first and go backwards
  for (int i = arrlen(level.allEntities) - 1; i >= 0; --i) {
    if (level.allEntities[i] == entity) {
      arrdelswap(level.allEntities,
                 i);  // O(1), arrdel is O(n) because it moves the rest
      return;
    }
  }
}