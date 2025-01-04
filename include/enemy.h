#pragma once
#include "raylib.h"
#include "time.h"
#include "level.h"

// this scales with player speed
#define MAX_ENEMY_COUNT 1000
#define ENEMY_COLOR RED

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
    time_t lastSpawnTime;
} EnemySpawner;

void InitializeEnemySpawner(EnemySpawner *enemySpawner);
void RemoveAllEnemies(EnemySpawner *enemySpawner);
void TickEnemy(Enemy *enemy);
// uses camera and level for spawning, call this after ticking camera
void TickEnemySpawner(EnemySpawner *enemySpawner, Camera2D *camera, Level *level);
void DrawEnemy(Enemy *enemy);
void DrawEnemies(EnemySpawner *enemySpawner);
Vector2 RandomPointOffScreen(Camera2D *camera, Level *level);