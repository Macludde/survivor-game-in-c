#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "enemy.h"
#include "raymath.h"
#include "helpers.h"
#include "player.h"
#include "raylib.h"

#define ENEMY_WIDTH 50
#define ENEMY_HEIGHT 30
#define TRIANGLE_WIDTH 20
#define TRIANGLE_HEIGHT 40

void TickEnemy(Enemy *enemy, Vector2 target, Enemy *allEnemies, int enemyCount)
{
    Vector2 direction = Vector2Normalize(Vector2Subtract(target, enemy->pos));
    enemy->velocity = Vector2Lerp(enemy->velocity, direction, ENEMY_ROTATION_SPEED * GetFrameTime());
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
    Vector2 rotateTowards = enemy->velocity;
    float angle = atan2(rotateTowards.y, rotateTowards.x) * RAD2DEG;
    DrawRectanglePro((Rectangle){enemy->pos.x - enemy->size, enemy->pos.y - enemy->size / 2, enemy->size * 2, enemy->size},
                     (Vector2){enemy->size, enemy->size / 2}, angle, enemy->color);
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
        .velocity = (Vector2){0, 0},
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

void HandleALlEnemyCollisions(Enemy *allEnemies, int enemyCount);
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
    HandleALlEnemyCollisions(enemySpawner->enemies, enemySpawner->enemyCount);
}

void HandleCollisionByTurningAwayBothEnemies(Enemy *enemy, Enemy *collided)
{
    Vector2 delta = Vector2Subtract(enemy->pos, collided->pos);
    Vector2 invDelta = Vector2Scale(delta, -1);
    // overlap is same for both
    float totalSize = enemy->size + collided->size;
    float overlap = (totalSize - Vector2Length(delta)) / totalSize; // lower => more overlap

    float rotationSpeed = 4 * ENEMY_ROTATION_SPEED * GetFrameTime() * overlap;
    // float ratio = (collided->size / enemy->size) * (collided->size / enemy->size);
    float ratio = (collided->size / (collided->size + enemy->size));
    enemy->velocity = Vector2Lerp(enemy->velocity, Vector2Normalize(delta), rotationSpeed * ratio);
    collided->velocity = Vector2Lerp(collided->velocity, Vector2Normalize(invDelta), rotationSpeed * (1 - ratio));
}

#define RELAXATION 3.0f // higher => bigger jumps, more jitter?
void HandleALlEnemyCollisions(Enemy *allEnemies, int enemyCount)
{
    for (int i = 0; i < enemyCount; ++i)
    {
        if (!allEnemies[i].spawned)
            continue;
        for (int j = i + 1; j < enemyCount; ++j)
        {
            if (!allEnemies[j].spawned)
                continue;
            Vector2 delta = Vector2Subtract(allEnemies[i].pos, allEnemies[j].pos);
            float distanceSqr = Vector2LengthSqr(delta);
            float desiredDistance = allEnemies[i].size + allEnemies[j].size;
            if (distanceSqr >= desiredDistance * desiredDistance)
                continue;

            float distance = sqrtf(distanceSqr);
            float scale = (desiredDistance - distance) * RELAXATION * GetFrameTime();
            Vector2 totalForce = Vector2Scale(Vector2Normalize(delta), scale);
            // float ratio = (allEnemies[j].size / allEnemies[i].size) * (allEnemies[j].size / allEnemies[i].size);
            float ratio = (allEnemies[j].size / (allEnemies[j].size + allEnemies[i].size));
            if (ratio > 1 || ratio < 0)
                ratio = ratio;
            allEnemies[i].pos = Vector2Add(allEnemies[i].pos, Vector2Scale(totalForce, ratio));
            allEnemies[j].pos = Vector2Add(allEnemies[j].pos, Vector2Scale(totalForce, ratio - 1)); // -1 * (1-ratio) = ratio - 1
            HandleCollisionByTurningAwayBothEnemies(&allEnemies[i], &allEnemies[j]);
        }
    }
}