#pragma once

#define TREE_HEALTH 100
// collision/physics
#define TREE_COLLISION_RADIUS 20
#define TREE_MASS 1000
// rendering
#define TREE_TRUNK_WIDTH 20
#define TREE_TRUNK_HEIGHT 40
#include <stdbool.h>

#include "entity.h"
#include "physics.h"
#include "raylib.h"

typedef struct Tree {
  Vector2 pos;
  float health;
} Tree;

typedef struct Level {
  int width;
  int height;
  int treeCount;
  Tree *trees;
} Level;

Level *AllocateLevel(int width, int height, int treeCount);
/**
 * Assumes level already has width, height and treeCount set
 */
void InitializeLevel(Level *level);
void DrawLevelBackground(
    Level *level);  // Draws stuff which should appear behind everything else
void DrawLevelForeground(Level *level);  // Draws stuff which should appear in
                                         // front of everything else
Rectangle TreeRectangle(Tree tree);
bool CheckCollisionCircleTree(Vector2 pos, float radius, Tree tree);
PhysicsBody GetTreeBody(Tree tree);
Entity GetTreeEntity(Tree tree);