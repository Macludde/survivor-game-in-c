#pragma once

#include "flecs.h"
#include "level.h"
#include "physics_math.h"
#include "raylib.h"

// this scales with player speed
#define MAX_ENEMY_COUNT 1024
#define ENEMY_COLOR RED
#define ENEMY_MOVEMENT_SPEED 100.0f
// 1/ENEMY_ROTATION_SPEED = seconds to turn at max speed
#define ENEMY_ROTATION_SPEED 5
#define ENEMY_ROTATION_SPEED_VELOCITY_RELATIVE \
  ENEMY_ROTATION_SPEED *ENEMY_MAX_SPEED

#define ENEMY_DEFAULT_SIZE 40

typedef struct EnemySpawner {
  int ticksBetweenSpawns;
  int currentTicks;
} EnemySpawner;

extern ECS_DECLARE(Enemy);
extern ECS_COMPONENT_DECLARE(EnemySpawner);

extern ECS_SYSTEM_DECLARE(SpawnEnemy);
extern ECS_SYSTEM_DECLARE(EnemyTargetPlayer);
extern ECS_SYSTEM_DECLARE(EnemyAccelerate);

void EnemyImport(ecs_world_t *world);