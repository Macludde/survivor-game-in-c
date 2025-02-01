#pragma once

#include "flecs.h"
#include "modules/movement.h"
#include "raylib.h"

#define ENEMY_COLOR RED
#define ENEMY_MOVEMENT_SPEED 200.0f
#define ENEMY_ROTATION_SPEED 5

#define ENEMY_DEFAULT_SIZE 40

typedef struct EnemySpawner {
  int ticksBetweenSpawns;
  int currentTicks;
} EnemySpawner;

extern ECS_DECLARE(Enemy);
extern ECS_COMPONENT_DECLARE(EnemySpawner);

extern ECS_SYSTEM_DECLARE(SpawnEnemy);

void EnemiesImport(ecs_world_t *world);
Position *ClosestEnemy(Position origin, ecs_world_t *world, int maxRange);