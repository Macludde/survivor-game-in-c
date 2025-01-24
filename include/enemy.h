#pragma once
#include "entity.h"
#include "level.h"
#include "physics.h"
#include "player.h"
#include "raylib.h"

// this scales with player speed
#define MAX_ENEMY_COUNT 10
#define ENEMY_COLOR RED
#define ENEMY_MOVEMENT_SPEED 100.0f
#define ENEMY_MAX_SPEED (ENEMY_MOVEMENT_SPEED / FRICTION_COEFFICIENT)
// 1/ENEMY_ROTATION_SPEED = seconds to turn at max speed
#define ENEMY_ROTATION_SPEED 5
#define ENEMY_ROTATION_SPEED_VELOCITY_RELATIVE \
  ENEMY_ROTATION_SPEED *ENEMY_MAX_SPEED

#define ENEMY_DEFAULT_SIZE 40

typedef struct Enemy {
  Entity entity;
  // in radians
  float rotation;
  // radius
  bool spawned;
  Color color;
} Enemy;

typedef struct EnemySpawner {
  Enemy *enemies;
  int enemyCount;
  int highestEnemyIndex;
  double lastSpawnTime;
} EnemySpawner;

void InitializeEnemySpawner(EnemySpawner *enemySpawner);
void RemoveAllEnemies();
// uses camera and level for spawning, call this after ticking camera
void TickEnemySpawner(Camera2D *camera, Player *player);
void DrawEnemy(Enemy *enemy);
void DrawEnemies();
float EnemyTakeDamage(Enemy *enemy, float damage);
float GetEnemyRotationSpeedGivenVelocity(Vector2 velocity);