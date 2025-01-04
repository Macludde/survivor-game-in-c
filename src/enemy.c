#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "enemy.h"
#include "raymath.h"

void InitializeEnemySpawner(EnemySpawner *enemySpawner)
{
    *enemySpawner = (EnemySpawner){
        .enemies = calloc(MAX_ENEMY_COUNT, sizeof(Enemy)),
        .enemyCount = 0,
        .firstFreeSlot = 0,
        .lastSpawnTime = time(NULL),
    };
}

void RemoveAllEnemies(EnemySpawner *enemySpawner)
{
    for (int i = 0; i < MAX_ENEMY_COUNT; i++)
    {
        enemySpawner->enemies[i].spawned = false;
    }
    enemySpawner->enemyCount = 0;
    enemySpawner->firstFreeSlot = 0;
}

// returns true if enemy was spawned, false if not
bool SpawnEnemy(EnemySpawner *enemySpawner, Camera2D *camera, Level *level)
{
    if (enemySpawner->firstFreeSlot >= MAX_ENEMY_COUNT)
        return false;

    enemySpawner->enemies[enemySpawner->firstFreeSlot] = (Enemy){
        .pos = RandomPointOffScreen(camera, level),
        .velocity = (Vector2){0, 0},
        .health = 100,
        .spawned = true,
    };
    enemySpawner->enemyCount++;
    enemySpawner->firstFreeSlot = enemySpawner->firstFreeSlot + 1;
    for (int i = enemySpawner->firstFreeSlot; i < MAX_ENEMY_COUNT; ++i)
    {
        if (!enemySpawner->enemies[i].spawned)
        {
            enemySpawner->firstFreeSlot = i;
            break;
        }
    }
    return true;
}

void TickEnemySpawner(EnemySpawner *enemySpawner, Camera2D *camera, Level *level)
{
    // if 2 seconds has passed, spawn enemy
    // if (time(NULL) - enemySpawner->lastSpawnTime > 0.2)
    // {
    SpawnEnemy(enemySpawner, camera, level);
    // enemySpawner->lastSpawnTime = time(NULL);
    // }
    for (int i = 0; i < enemySpawner->enemyCount; i++)
    {
        TickEnemy(&enemySpawner->enemies[i]);
    }
}

void TickEnemy(Enemy *enemy)
{
    // todo
}

#define RECTANGLE_WIDTH 50
#define RECTANGLE_HEIGHT 30
#define TRIANGLE_WIDTH 20
#define TRIANGLE_HEIGHT 40

#define ENEMY_WIDTH (RECTANGLE_WIDTH + TRIANGLE_WIDTH)

void DrawEnemy(Enemy *enemy)
{
    DrawRectangle(enemy->pos.x - ENEMY_WIDTH / 2, enemy->pos.y - RECTANGLE_HEIGHT / 2, RECTANGLE_WIDTH, RECTANGLE_HEIGHT, ENEMY_COLOR);
    // Yes, triangle is larger than rectangle
    Vector2 point1 = {enemy->pos.x + ENEMY_WIDTH / 2 - TRIANGLE_WIDTH, enemy->pos.y - TRIANGLE_HEIGHT / 2};
    Vector2 point2 = {enemy->pos.x + ENEMY_WIDTH / 2 - TRIANGLE_WIDTH, enemy->pos.y + TRIANGLE_HEIGHT / 2};
    Vector2 point3 = {enemy->pos.x + ENEMY_WIDTH / 2, enemy->pos.y};
    DrawTriangle(point1, point2, point3, ENEMY_COLOR);
}

void DrawEnemies(EnemySpawner *enemySpawner)
{
    for (int i = 0; i < enemySpawner->enemyCount; i++)
        DrawEnemy(&enemySpawner->enemies[i]);
}

void printVector2(Vector2 v, char *name)
{
    printf("%s: %f, %f\n", name, v.x, v.y);
}
// generate random point just outside camera, 100 units away from camera bounds, and within level bounds
Vector2 RandomPointJustOffScreen(Camera2D *camera, Level *level)
{
    // get camera bounds, given GetScreenToWorld2D
    Vector2 cameraTopLeft = GetScreenToWorld2D((Vector2){0, 0}, *camera);
    Vector2 cameraBottomRight = GetScreenToWorld2D((Vector2){GetScreenWidth(), GetScreenHeight()}, *camera);
    printVector2(cameraTopLeft, "cameraTopLeft");
    printVector2(cameraBottomRight, "cameraBottomRight");
    // get level bounds
    Vector2 levelTopLeft = {-level->width, -level->height};
    Vector2 levelBottomRight = {level->width, level->height};

    Vector2 clampedTopLeft = Vector2Clamp(Vector2SubtractValue(cameraTopLeft, 100), levelTopLeft, levelBottomRight);
    Vector2 clampedBottomRight = Vector2Clamp(Vector2AddValue(cameraBottomRight, 100), levelTopLeft, levelBottomRight);
    Vector2 randomPoint = {0, 0};
    int iterations = 0;
    do
    {
        ++iterations;
        randomPoint = (Vector2){GetRandomValue(clampedTopLeft.x, clampedBottomRight.x), GetRandomValue(cameraTopLeft.y, clampedBottomRight.y)};
        if (iterations > 1000)
        {
            printf("iterations exceeded 1000\n");
            break;
        }
        // do while inside camera bounds
    } while (randomPoint.x > cameraTopLeft.x &&
             randomPoint.x < cameraBottomRight.x &&
             randomPoint.y > cameraTopLeft.y &&
             randomPoint.y < cameraBottomRight.y);
    printf("iterations: %d\n", iterations);
    return randomPoint;
}

Vector2 RandomPointOffScreen(Camera2D *camera, Level *level)
{
    Vector2 randomPoint = RandomPointJustOffScreen(camera, level);
}