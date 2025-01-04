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

void DrawLevel(Level *level)
{
	Vector2 *trees = level->trees;
	for (int i = 0; i < level->treeCount; i++)
	{
		DrawRectangle(trees[i].x, trees[i].y, 20, 40, BROWN);
		DrawCircle(trees[i].x + 10, trees[i].y - 10, 30, GREEN);
	}
}