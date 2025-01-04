#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "enemy.h"
#include "raymath.h"
#include "helpers.h"
#include "player.h"

void InitializeEnemySpawner(EnemySpawner *enemySpawner)
{
    *enemySpawner = (EnemySpawner){
        .enemies = calloc(MAX_ENEMY_COUNT, sizeof(Enemy)),
        .enemyCount = 0,
        .firstFreeSlot = 0,
        .lastSpawnTime = time_in_seconds(),
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

void TickEnemySpawner(EnemySpawner *enemySpawner, Camera2D *camera, Level *level, Player *player)
{
    // if 2 seconds has passed, spawn enemy
    if (time_in_seconds() - enemySpawner->lastSpawnTime > 0.2)
    {
        SpawnEnemy(enemySpawner, camera, level);
        enemySpawner->lastSpawnTime = time_in_seconds();
    }
    for (int i = 0; i < enemySpawner->enemyCount; i++)
        TickEnemy(&enemySpawner->enemies[i], player->pos);
}

void TickEnemy(Enemy *enemy, Vector2 target)
{
    Vector2 direction = Vector2Normalize(Vector2Subtract(target, enemy->pos));
    enemy->velocity = Vector2Lerp(enemy->velocity, direction, ENEMY_ROTATION_SPEED * GetFrameTime());
    enemy->pos = Vector2Add(enemy->pos, Vector2Scale(enemy->velocity, ENEMY_MOVEMENT_SPEED * GetFrameTime()));
}

#define ENEMY_WIDTH 50
#define ENEMY_HEIGHT 30
#define TRIANGLE_WIDTH 20
#define TRIANGLE_HEIGHT 40

// unused
void DrawEnemyWithoutRotation(Enemy *enemy)
{

    DrawRectangle(enemy->pos.x - ENEMY_WIDTH / 2, enemy->pos.y - ENEMY_HEIGHT / 2, ENEMY_WIDTH, ENEMY_HEIGHT, ENEMY_COLOR);
    // Yes, triangle is larger than rectangle
    Vector2 point1 = {enemy->pos.x + ENEMY_WIDTH / 2, enemy->pos.y - TRIANGLE_HEIGHT / 2};
    Vector2 point2 = {enemy->pos.x + ENEMY_WIDTH / 2, enemy->pos.y + TRIANGLE_HEIGHT / 2};
    Vector2 point3 = {enemy->pos.x + ENEMY_WIDTH / 2 + TRIANGLE_WIDTH, enemy->pos.y};
    DrawTriangle(point1, point2, point3, ENEMY_COLOR);
}

void DrawEnemyWithRotation(Enemy *enemy)
{
    Vector2 rotateTowards = enemy->velocity;
    float angle = atan2(rotateTowards.y, rotateTowards.x) * RAD2DEG;
    DrawRectanglePro((Rectangle){enemy->pos.x - ENEMY_WIDTH / 2, enemy->pos.y - ENEMY_HEIGHT / 2, ENEMY_WIDTH, ENEMY_HEIGHT},
                     (Vector2){ENEMY_WIDTH / 2, ENEMY_HEIGHT / 2}, angle, ENEMY_COLOR);
    // skip triangle
}
void DrawEnemy(Enemy *enemy)
{
    DrawEnemyWithRotation(enemy);
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