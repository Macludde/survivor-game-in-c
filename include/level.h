#pragma once

#define TREE_COLLISION_RADIUS 20
#define TREE_TRUNK_WIDTH 20
#define TREE_TRUNK_HEIGHT 40
#include <stdbool.h>

#include "raylib.h"

typedef struct Level {
  int width;
  int height;
  int treeCount;
  Vector2 *trees;
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
Rectangle TreeRectangle(Vector2 tree);
bool CheckCollisionCircleTree(Vector2 pos, float radius, Vector2 tree);