#pragma once

#define TREE_HEALTH 100
// collision/physics
#define TREE_COLLISION_RADIUS 20
#define TREE_MASS 10000
// rendering
#define TREE_TRUNK_WIDTH 20
#define TREE_TRUNK_HEIGHT 40
#include <stdbool.h>

#include "entity.h"
#include "physics.h"
#include "raylib.h"

typedef Entity Tree;

typedef struct Level {
  int width;
  int height;
  int treeCount;
  Tree *trees;
  Entity **allEntities;  // dynamic array of entity pointers
} Level;

Level *AllocateLevel(int width, int height, int treeCount);
/**
 * Assumes level already has width, height and treeCount set
 */
void InitializeLevel(Level *level);
void DrawLevelBackground();  // Draws stuff which should appear behind
                             // everything else
void DrawLevelForeground();  // Draws stuff which should appear in
                             // front of everything else
Rectangle TreeRectangle(Tree tree);
// @deprecated
inline static PhysicsBody GetTreeBody(Tree tree) { return tree.body; }
// @deprecated
inline static Entity GetTreeEntity(Tree tree) { return tree; }

void AddEntity(Entity *entity);
void RemoveEntity(Entity *entity);