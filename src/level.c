#include "level.h"

#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "entity.h"
#include "lib/stb_ds.h"
#include "physics.h"
#include "raylib.h"
#include "raymath.h"

Level *AllocateLevel(int width, int height, int treeCount) {
  Level *level = malloc(sizeof(Level));
  if (level == NULL) {
    printf("Failed to allocate memory for level\n");
    exit(1);
  }
  level->width = width;
  level->height = height;
  level->treeCount = treeCount;
  level->allEntities = NULL;
  return level;
}

void InitializeLevel(Level *level) {
  Tree *trees = malloc(level->treeCount * sizeof(Tree));
  if (trees == NULL) {
    printf("Failed to allocate memory for trees\n");
    exit(1);
  }
  arrsetlen(
      level->allEntities,
      level->treeCount + 100);  // player + enemies as well. We can afford
                                // using a little too much memory, we want to
                                // avoid rescaling during runtime if possible

  int retries = 0;
  for (int i = 0; i < level->treeCount; ++i) {
    trees[i].body.pos =
        (Vector2){GetRandomValue(-level->width, level->width),
                  GetRandomValue(-level->height, level->height)};
    for (int j = 0; j < i; ++j) {
      if (Vector2Distance(trees[i].body.pos, trees[j].body.pos) < 120) {
        // retry
        --i;
        ++retries;
        if (retries > 10) {
          level->treeCount = i;
          break;
        }
        break;
      }
    }
    trees[i].body.mass = TREE_MASS;
    trees[i].body.radius = TREE_COLLISION_RADIUS;
    trees[i].body.velocity = Vector2Zero();
    trees[i].body.acceleration = Vector2Zero();
    retries = 0;
    trees[i].health = 100;
    arrput(level->allEntities, &trees[i]);
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
#ifdef DEBUG_SHOW_HITBOXES
    DrawCircleLinesV(trees[i].body.pos, TREE_COLLISION_RADIUS, PINK);
#endif
  }
}
void DrawLevelForeground() {
  Tree *trees = level.trees;
  for (int i = 0; i < level.treeCount; i++) {
    DrawCircle(trees[i].body.pos.x,
               trees[i].body.pos.y - TREE_TRUNK_HEIGHT + 10, 30, GREEN);
  }
}

bool CheckCollisionCircleTree(Vector2 pos, float radius, Tree tree) {
  return CheckCollisionCircleRec(pos, radius, TreeRectangle(tree));
}

// @deprecated
inline PhysicsBody GetTreeBody(Tree tree) { return tree.body; }

// @deprecated
inline Entity GetTreeEntity(Tree tree) { return tree; }

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