#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

#include "level.h"

Level *AllocateLevel(int width, int height, int treeCount)
{
	Level *level = malloc(sizeof(Level));
	if (level == NULL)
	{
		printf("Failed to allocate memory for level\n");
		exit(1);
	}
	level->width = width;
	level->height = height;
	level->treeCount = treeCount;
	return level;
}

void InitializeLevel(Level *level)
{
	Vector2 *trees = malloc(level->treeCount * sizeof(Vector2));
	if (trees == NULL)
	{
		printf("Failed to allocate memory for trees\n");
		exit(1);
	}
	for (int i = 0; i < level->treeCount; ++i)
	{
		trees[i] = (Vector2){GetRandomValue(-level->width, level->width), GetRandomValue(-level->height, level->height)};
		for (int j = 0; j < i; ++j)
		{
			if (Vector2Distance(trees[i], trees[j]) < 120)
			{
				--i;
				break;
			}
		}
	}
	level->trees = trees;
}

Rectangle TreeRectangle(Vector2 tree)
{
	return (Rectangle){tree.x - TREE_TRUNK_WIDTH / 2, tree.y - TREE_TRUNK_HEIGHT / 2, TREE_TRUNK_WIDTH, TREE_TRUNK_HEIGHT};
}
void DrawLevelBackground(Level *level)
{
	Vector2 *trees = level->trees;
	for (int i = 0; i < level->treeCount; i++)
	{
		DrawRectangleRec(TreeRectangle(trees[i]), BROWN);
		DrawCircleLinesV(trees[i], TREE_COLLISION_RADIUS, PINK);
	}
}
void DrawLevelForeground(Level *level)
{
	Vector2 *trees = level->trees;
	for (int i = 0; i < level->treeCount; i++)
	{
		DrawCircle(trees[i].x, trees[i].y - TREE_TRUNK_HEIGHT + 10, 30, GREEN);
	}
}

bool CheckCollisionCircleTree(Vector2 pos, float radius, Vector2 tree)
{
	return CheckCollisionCircleRec(pos, radius, TreeRectangle(tree));
}