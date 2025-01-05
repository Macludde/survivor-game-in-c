#pragma once
#include "raylib.h"
#include "level.h"
#include "player.h"

// this scales with player speed
#define MAX_ENEMY_COUNT 500
#define ENEMY_COLOR RED
#define ENEMY_ROTATION_SPEED 2.0f
#define ENEMY_MOVEMENT_SPEED 100.0f

typedef struct Enemy
{
    Vector2 pos;
    Vector2 velocity;
    float health;
    bool spawned;
} Enemy;

typedef struct EnemySpawner
{
    Enemy *enemies;
    int enemyCount;
    int firstFreeSlot;
    double lastSpawnTime;
} EnemySpawner;

void InitializeEnemySpawner(EnemySpawner *enemySpawner);
void RemoveAllEnemies(EnemySpawner *enemySpawner);
// uses camera and level for spawning, call this after ticking camera
void TickEnemySpawner(EnemySpawner *enemySpawner, Camera2D *camera, Level *level, Player *player);
void DrawEnemy(Enemy *enemy);
void DrawEnemies(EnemySpawner *enemySpawner);
Vector2 RandomPointOffScreen(Camera2D *camera, Level *level);