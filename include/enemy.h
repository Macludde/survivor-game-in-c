#pragma once
#include "level.h"
#include "player.h"
#include "raylib.h"

// this scales with player speed
#define MAX_ENEMY_COUNT 10
#define ENEMY_COLOR RED
#define ENEMY_ROTATION_SPEED 2.0f
#define ENEMY_MOVEMENT_SPEED 300.0f

#define ENEMY_DEFAULT_SIZE 40

typedef struct Enemy {
  Vector2 pos;
  // radius
  float size;
  Vector2 targetVelocity;
  Vector2 velocity;
  float health;
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
void EnemyTakeDamage(Enemy *enemy, float damage);