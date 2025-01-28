#include "enemy.h"
#include "level.h"
#include "physics.h"
#include "player.h"
#include "raylib.h"
#include "raymath.h"

extern Level level;

#define ENEMY_INTERNAL_KNOCKBACK 3.0f  // higher => bigger jumps, more jitter?

void CollisionsWithOtherEnemies(Enemy *enemy, Enemy *allEnemies, int startAt,
                                int highestEnemyIndex) {
  for (int j = startAt; j < highestEnemyIndex; ++j) {
    if (!allEnemies[j].spawned) continue;
    if (CheckLenientCollision(enemy->entity.body, allEnemies[j].entity.body,
                              2.2f)) {
      ElasticCollision(&enemy->entity.body, &allEnemies[j].entity.body);

      Vector2 turnAwayDirection = Vector2Normalize(Vector2Subtract(
          enemy->entity.body.pos, allEnemies[j].entity.body.pos));
      float rotationSpeed =
          GetEnemyRotationSpeedGivenVelocity(enemy->entity.body.velocity);
      enemy->rotation = LerpRotationAngle(enemy->rotation, turnAwayDirection,
                                          rotationSpeed * GetFrameTime());
    }
  }
}

void CollisionsWithTrees(Enemy *enemy) {
  // for (int treeIndex = 0; treeIndex < level.treeCount; ++treeIndex) {
  //   PhysicsBody treeBody = GetTreeBody(level.trees[treeIndex]);
  //   if (CheckLenientCollision(enemy->entity.body, treeBody, 4)) {
  //     // enemy is close to tree
  //     ElasticCollision(&enemy->entity.body, &treeBody);

  //     Vector2 turnAwayDirection = Vector2Normalize(
  //         Vector2Subtract(enemy->entity.body.pos, treeBody.pos));
  //     float rotationSpeed =
  //         GetEnemyRotationSpeedGivenVelocity(enemy->entity.body.velocity) /
  //         3;
  //     enemy->rotation = LerpRotationAngle(enemy->rotation, turnAwayDirection,
  //                                         rotationSpeed * GetFrameTime());
  //   }
  // }
}

void HandleAllEnemyCollisions(Enemy *allEnemies, int highestEnemyIndex,
                              OLD_Player *player) {
  for (int i = 0; i < highestEnemyIndex; ++i) {
    if (!allEnemies[i].spawned) continue;
    Enemy *curr = &allEnemies[i];
    CollisionsWithOtherEnemies(curr, allEnemies, i + 1, highestEnemyIndex);
    CollisionsWithTrees(curr);
  }
}