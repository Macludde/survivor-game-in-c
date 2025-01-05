// #include <stdbool.h>
#include "enemy.h"
#include "level.h"
#include "player.h"
#include "raymath.h"
#include "raylib.h"

void TurnAwayEnemiesFromEachother(Enemy *enemy, Enemy *collided)
{
    Vector2 delta = Vector2Subtract(enemy->pos, collided->pos);
    Vector2 invDelta = Vector2Scale(delta, -1);
    // overlap is same for both
    float totalSize = enemy->size + collided->size;
    float overlap = (totalSize - Vector2Length(delta)) / totalSize; // lower => more overlap

    float rotationSpeed = 4 * ENEMY_ROTATION_SPEED * GetFrameTime() * overlap;
    float ratio = (collided->size / (collided->size + enemy->size));
    enemy->targetVelocity = Vector2Lerp(enemy->targetVelocity, Vector2Normalize(delta), rotationSpeed * ratio);
    collided->targetVelocity = Vector2Lerp(collided->targetVelocity, Vector2Normalize(invDelta), rotationSpeed * (1 - ratio));
}

#define ENEMY_INTERNAL_KNOCKBACK 3.0f // higher => bigger jumps, more jitter?
// ratio between 0 and 1. 1 Means enemy is the only one moving, 0 means entity is the only one moving
void EnemyCollisionWithEntity(Enemy *enemy, Vector2 *entityPos, float entitySize, float ratio, Vector2 delta, float distanceSqr, float desiredDistance)
{
    float distance = sqrtf(distanceSqr);
    float scale = (desiredDistance - distance) * ENEMY_INTERNAL_KNOCKBACK * GetFrameTime();
    Vector2 totalForce = Vector2Scale(Vector2Normalize(delta), scale);
    enemy->pos = Vector2Add(enemy->pos, Vector2Scale(totalForce, ratio));
    *entityPos = Vector2Add(*entityPos, Vector2Scale(totalForce, ratio - 1)); // -1 * (1-ratio) = ratio - 1
}
void EnemyEnemyCollision(Enemy *enemy1, Enemy *enemy2, Vector2 delta, float distanceSqr, float desiredDistance)
{
    float ratio = (enemy2->size / (enemy2->size + enemy1->size));
    EnemyCollisionWithEntity(enemy1, &enemy2->pos, enemy2->size, ratio, delta, distanceSqr, desiredDistance);
    TurnAwayEnemiesFromEachother(enemy1, enemy2);
}

void EnemyTreeCollision(Enemy *enemy, Vector2 tree, Vector2 delta, float distanceSqr, float desiredDistance)
{
    float distance = sqrtf(distanceSqr);
    float overlap = desiredDistance - distance;
    Vector2 direction = Vector2Normalize(delta);
    enemy->pos = Vector2Add(enemy->pos, Vector2Scale(direction, overlap));
    enemy->targetVelocity = Vector2Lerp(enemy->targetVelocity, Vector2Reflect(enemy->targetVelocity, direction), 3 * GetFrameTime());
}

void CollisionsWithOtherEnemies(Enemy *enemy, Enemy *allEnemies, int startAt, int enemyCount)
{
    for (int j = startAt; j < enemyCount; ++j)
    {
        if (!allEnemies[j].spawned)
            continue;
        Vector2 delta = Vector2Subtract(enemy->pos, allEnemies[j].pos);
        float distanceSqr = Vector2LengthSqr(delta);
        float desiredDistance = enemy->size + allEnemies[j].size;
        if (distanceSqr < desiredDistance * desiredDistance)
            EnemyEnemyCollision(enemy, &allEnemies[j], delta, distanceSqr, desiredDistance);
    }
}

void CollisionsWithTrees(Enemy *enemy, Level *level)
{
    for (int treeIndex = 0; treeIndex < level->treeCount; ++treeIndex)
    {
        Vector2 delta = Vector2Subtract(enemy->pos, level->trees[treeIndex]);
        float distanceSqr = Vector2LengthSqr(delta);
        float desiredDistance = enemy->size + TREE_COLLISION_RADIUS;
        if (distanceSqr < desiredDistance * desiredDistance)
            EnemyTreeCollision(enemy, level->trees[treeIndex], delta, distanceSqr, desiredDistance);
    }
}

#define ENEMY_PLAYER_KNOCKBACK 1.0f
void CollisionWithPlayer(Enemy *enemy, Player *player)
{
    Vector2 delta = Vector2Subtract(enemy->pos, player->pos);
    float distanceSqr = Vector2LengthSqr(delta);
    float desiredDistance = enemy->size + player->size;
    if (distanceSqr < desiredDistance * desiredDistance)
    {
        // move away from each other
        float distance = sqrtf(distanceSqr);
        float overlap = desiredDistance - distance;
        Vector2 direction = Vector2Normalize(delta);
        enemy->pos = Vector2Add(enemy->pos, Vector2Scale(direction, overlap));
        // float ratio = (player->size * 3 / (enemy->size + player->size * 3));
        // EnemyCollisionWithEntity(enemy, &player->pos, player->size, ratio, delta, distanceSqr, desiredDistance);

        // slow down enemy, knockback player.
        // m1*v1 = m2*v2
        float scale = ENEMY_PLAYER_KNOCKBACK;
        // player->velocity = Vector2Lerp(player->velocity, enemy->velocity, ENEMY_PLAYER_KNOCKBACK);
        // enemy->velocity = Vector2Lerp(enemy->velocity, playerVelocity, ENEMY_PLAYER_KNOCKBACK);
        // v2 = v1*m1/m2
        player->velocity = Vector2Add(player->velocity, Vector2Scale(enemy->velocity, ENEMY_PLAYER_KNOCKBACK * (enemy->size / player->size)));
        enemy->velocity = Vector2Zero();
    }
}

void HandleAllEnemyCollisions(Enemy *allEnemies, int enemyCount, Level *level, Player *player)
{
    for (int i = 0; i < enemyCount; ++i)
    {
        if (!allEnemies[i].spawned)
            continue;
        Enemy *curr = &allEnemies[i];
        CollisionsWithOtherEnemies(curr, allEnemies, i + 1, enemyCount);
        CollisionsWithTrees(curr, level);
        CollisionWithPlayer(curr, player);
    }
}