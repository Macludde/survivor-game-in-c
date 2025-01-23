#include "enemy.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "enemy_collisions.h"
#include "helpers.h"
#include "physics.h"
#include "player.h"
#include "raylib.h"
#include "raymath.h"

extern EnemySpawner enemySpawner;
extern Level level;

float GetEnemyRotationSpeedGivenVelocity(Vector2 velocity) {
  float currentSpeed = Vector2Length(velocity);
  float rotationSpeed =
      ENEMY_ROTATION_SPEED_VELOCITY_RELATIVE /
      (currentSpeed < ENEMY_MOVEMENT_SPEED ? ENEMY_MOVEMENT_SPEED
                                           : currentSpeed);
  if (rotationSpeed > ENEMY_ROTATION_SPEED) return ENEMY_ROTATION_SPEED;
  return rotationSpeed;
}

void TickEnemy(Enemy *enemy, Vector2 target, Enemy *allEnemies,
               int enemyCount) {
  // calculate angle to target
  Vector2 targetDelta =
      Vector2Normalize(Vector2Subtract(target, enemy->body.pos));
  float rotationSpeed =
      GetEnemyRotationSpeedGivenVelocity(enemy->body.velocity);
  enemy->rotation = LerpRotationAngle(enemy->rotation, targetDelta,
                                      rotationSpeed * GetFrameTime());

  Vector2 force = Vector2Scale(Vector2Rotate((Vector2){1, 0}, enemy->rotation),
                               ENEMY_MOVEMENT_SPEED);

  ApplyAcceleration(&enemy->body, force);
  MoveBody(&enemy->body);
}

void DrawEnemyWithRotation(Enemy *enemy) {
  float angle = enemy->rotation * RAD2DEG;
  Rectangle rect = {enemy->body.pos.x, enemy->body.pos.y,
                    enemy->body.radius * 2, enemy->body.radius};
  DrawRectanglePro(rect, (Vector2){enemy->body.radius, enemy->body.radius / 2},
                   angle, enemy->color);
}
void DrawEnemy(Enemy* enemy) {
    DrawEnemyWithRotation(enemy);
#ifdef DEBUG_SHOW_HITBOXES
  DrawCircleLinesV(enemy->body.pos, enemy->body.radius, PINK);
  DrawCircleLinesV(enemy->body.pos, 1, PINK);
#endif
}

void DrawEnemies() {
  for (int i = 0; i < enemySpawner.highestEnemyIndex + 1; i++)
    if (enemySpawner.enemies[i].spawned) DrawEnemy(&enemySpawner.enemies[i]);
}

void printVector2(Vector2 v, char* name) {
    printf("%s: %f, %f\n", name, v.x, v.y);
}

#define MIN_DISTANCE_TO_CAMERA 100
#define MAX_DISTANCE_TO_CAMERA 100
// generate random point just outside camera, 100 units away from camera bounds,
// and within level bounds
Vector2 RandomPointJustOffScreen(Camera2D *camera) {
  // get camera bounds, given GetScreenToWorld2D
  Vector2 cameraTopLeft = Vector2SubtractValue(
      GetScreenToWorld2D((Vector2){0, 0}, *camera), MIN_DISTANCE_TO_CAMERA);
  Vector2 cameraBottomRight = Vector2AddValue(
      GetScreenToWorld2D((Vector2){GetScreenWidth(), GetScreenHeight()},
                         *camera),
      MIN_DISTANCE_TO_CAMERA);
  // get level bounds
  Vector2 levelTopLeft = {-level.width, -level.height};
  Vector2 levelBottomRight = {level.width, level.height};

  Vector2 clampedTopLeft =
      Vector2Clamp(Vector2SubtractValue(cameraTopLeft, MAX_DISTANCE_TO_CAMERA),
                   levelTopLeft, levelBottomRight);
  Vector2 clampedBottomRight =
      Vector2Clamp(Vector2AddValue(cameraBottomRight, MAX_DISTANCE_TO_CAMERA),
                   levelTopLeft, levelBottomRight);
  Vector2 randomPoint = {0, 0};
  int iterations = 0;
  do {
    ++iterations;
    randomPoint =
        (Vector2){GetRandomValue(clampedTopLeft.x, clampedBottomRight.x),
                  GetRandomValue(cameraTopLeft.y, clampedBottomRight.y)};
    if (iterations > 1000) {
      printf("iterations exceeded 1000\n");
      break;
    }
    // do while inside camera bounds
  } while (
      randomPoint.x > cameraTopLeft.x && randomPoint.x < cameraBottomRight.x &&
      randomPoint.y > cameraTopLeft.y && randomPoint.y < cameraBottomRight.y);
  return randomPoint;
}

Vector2 RandomPointOffScreen(Camera2D *camera, Level *level) {
  Vector2 randomPoint = RandomPointJustOffScreen(camera, level);
  return randomPoint;
}

void InitializeEnemySpawner(EnemySpawner *enemySpawnerToInit) {
  *enemySpawnerToInit =
      (EnemySpawner){.enemies = calloc(MAX_ENEMY_COUNT, sizeof(Enemy)),
                     .enemyCount = 0,
                     .lastSpawnTime = time_in_seconds(),
                     .highestEnemyIndex = -1};
}

void RemoveAllEnemies() {
  for (int i = 0; i < MAX_ENEMY_COUNT; i++) {
    enemySpawner.enemies[i].spawned = false;
  }
  enemySpawner.enemyCount = 0;
  enemySpawner.highestEnemyIndex = -1;
}

void DecreaseHighestEnemyIndex() {
    for (--enemySpawner.highestEnemyIndex; enemySpawner.highestEnemyIndex > 0;
        --enemySpawner.highestEnemyIndex) {
        if (enemySpawner.enemies[enemySpawner.highestEnemyIndex].spawned) break;
    }
}

void EnemyTakeDamage(Enemy* enemy, float damage) {
    enemy->health -= damage;
    if (enemy->health <= 0) {
        if (enemy == &enemySpawner.enemies[enemySpawner.highestEnemyIndex])
            DecreaseHighestEnemyIndex();
        enemy->spawned = false;
        enemySpawner.enemyCount--;
    }
}

// returns true if enemy was spawned, false if not
bool SpawnEnemy(Camera2D* camera, Level* level) {
    int firstFreeSlot;
    for (firstFreeSlot = 0; firstFreeSlot < MAX_ENEMY_COUNT; ++firstFreeSlot) {
        if (!enemySpawner.enemies[firstFreeSlot].spawned) {
            break;
        }
    }
  }
  if (firstFreeSlot == MAX_ENEMY_COUNT) return false;
  float size = ENEMY_DEFAULT_SIZE * (GetRandomValue(8, 12) / 10.0f);
  enemySpawner.enemies[firstFreeSlot] = (Enemy){
      .body =
          (PhysicsBody){
              .pos = RandomPointOffScreen(camera, level),
              .velocity = (Vector2){0, 0},
              .acceleration = (Vector2){0, 0},
              .mass = size * size,
              .radius = size,
          },
      .health = 10,
      .spawned = true,
      .color = SlightColorVariation(ENEMY_COLOR),
  };
  enemySpawner.enemyCount++;
  if (firstFreeSlot > enemySpawner.highestEnemyIndex)
    enemySpawner.highestEnemyIndex = firstFreeSlot;
  return true;
}

void TickEnemySpawner(Camera2D *camera, Player *player) {
  // if 2 seconds has passed, spawn enemy
  if (time_in_seconds() - enemySpawner.lastSpawnTime > 1.2) {
    SpawnEnemy(camera);
    enemySpawner.lastSpawnTime = time_in_seconds();
  }
  for (int i = 0; i < enemySpawner.highestEnemyIndex + 1; ++i)
    if (enemySpawner.enemies[i].spawned)
      TickEnemy(&enemySpawner.enemies[i], player->body.pos,
                enemySpawner.enemies, enemySpawner.enemyCount);
  HandleAllEnemyCollisions(enemySpawner.enemies, enemySpawner.enemyCount,
                           player);
}