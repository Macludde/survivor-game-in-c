#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "enemy.h"
#include "enemy_collisions.h"
#include "raymath.h"
#include "helpers.h"
#include "player.h"
#include "raylib.h"
#include "debug.h"

void TickEnemy(Enemy *enemy, Vector2 target, Enemy *allEnemies, int enemyCount)
{
    Vector2 direction = Vector2Normalize(Vector2Subtract(target, enemy->pos));
    enemy->targetVelocity = Vector2Lerp(enemy->targetVelocity, direction, ENEMY_ROTATION_SPEED * GetFrameTime());
    enemy->velocity = Vector2Lerp(enemy->velocity, enemy->targetVelocity, ENEMY_ROTATION_SPEED * GetFrameTime());
    enemy->pos = Vector2Add(enemy->pos, Vector2Scale(enemy->velocity, ENEMY_MOVEMENT_SPEED * GetFrameTime()));

    int iterations = 0;
    Enemy *collided;
    ++iterations;
    // collided = CollidesWithAnyEnemy(enemy, allEnemies, enemyCount);
    // for (int i = 0; collided != NULL && i < 10; i++)
    // {
    //     HandleEnemiesCollision(enemy, collided, allEnemies, enemyCount);
    //     collided = CollidesWithAnyEnemy(enemy, allEnemies, enemyCount);
    // }
}

void DrawEnemyWithRotation(Enemy *enemy)
{
    Vector2 rotateTowards = enemy->targetVelocity;
    float angle = atan2(rotateTowards.y, rotateTowards.x) * RAD2DEG;
    Rectangle rect = {enemy->pos.x, enemy->pos.y, enemy->size * 2, enemy->size};
    DrawRectanglePro(rect, (Vector2){enemy->size, enemy->size / 2}, angle, enemy->color);
}
void DrawEnemy(Enemy *enemy)
{
    DrawEnemyWithRotation(enemy);
#ifdef DEBUG_SHOW_HITBOXES
    DrawCircleLinesV(enemy->pos, enemy->size, PINK);
    DrawCircleLinesV(enemy->pos, 1, PINK);
#endif
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
    return randomPoint;
}

Vector2 RandomPointOffScreen(Camera2D *camera, Level *level)
{
    Vector2 randomPoint = RandomPointJustOffScreen(camera, level);
}

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
        .targetVelocity = (Vector2){0, 0},
        .health = 100,
        .spawned = true,
        .size = ENEMY_DEFAULT_SIZE * (GetRandomValue(8, 12) / 10.0f),
        .color = SlightColorVariation(ENEMY_COLOR),
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
    for (int i = 0; i < enemySpawner->enemyCount; ++i)
        TickEnemy(&enemySpawner->enemies[i], player->pos, enemySpawner->enemies, enemySpawner->enemyCount);
    HandleAllEnemyCollisions(enemySpawner->enemies, enemySpawner->enemyCount, level, player);
}