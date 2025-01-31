#include "modules/enemies.h"

#include <stdbool.h>

#include "flecs.h"
#include "helpers.h"
#include "level.h"
#include "modules/camera.h"
#include "modules/collisions.h"
#include "modules/health.h"
#include "modules/movement.h"
#include "modules/player.h"
#include "modules/render.h"
#include "modules/rigidbody.h"
#include "raylib.h"
#include "raymath.h"
#include "tick.h"

extern Level level;
extern Camera2D camera;
extern ecs_entity_t tick_source;

ECS_COMPONENT_DECLARE(EnemySpawner);
ECS_TAG_DECLARE(Enemy);

ECS_SYSTEM_DECLARE(SpawnEnemy);
ECS_SYSTEM_DECLARE(EnemyTargetPlayer);
ECS_SYSTEM_DECLARE(EnemyAccelerate);

static ecs_entity_t enemyPrefab = 0;
static ecs_query_t *playerQuery;

ecs_entity_t CreateEnemyPrefab(ecs_world_t *world) {
  ecs_entity_t enemy =
      ecs_entity(world, {.name = "Enemy", .add = ecs_ids(EcsPrefab, Enemy)});
  ecs_add(world, enemy, Position);
  ecs_set(world, enemy, Velocity, {0, 0});
  ecs_set(world, enemy, Acceleration, {0, 0});
  ecs_set(world, enemy, Rotation, {0});
  ecs_set(world, enemy, MaxSpeed, {400});
  ecs_add(world, enemy, RectShape);
  ecs_add(world, enemy, Rigidbody);
  ecs_add(world, enemy, Collidable);
  ecs_set(world, enemy, Killable, KILLABLE(20));

  return enemy;
}

#define MIN_DISTANCE_TO_CAMERA 100
#define MAX_DISTANCE_TO_CAMERA 100
ecs_entity_t CreateEnemy(ecs_world_t *world) {
  if (enemyPrefab == 0) {
    enemyPrefab = CreateEnemyPrefab(world);
  }
  float size = SlightVariation() * ENEMY_DEFAULT_SIZE;
  ecs_entity_t enemy = ecs_new_w_pair(world, EcsIsA, enemyPrefab);
  Vector2 pos = ClampedRandomPointOffScreen(
      MIN_DISTANCE_TO_CAMERA, MAX_DISTANCE_TO_CAMERA,
      (Area){{-level.width, -level.height}, {level.width, level.height}});
  ecs_set(world, enemy, Position, {pos.x, pos.y});
  ecs_set(world, enemy, RectShape,
          {
              .offset = {0, 0},
              .size = {size, size / 2},
              .color = SlightColorVariation(ENEMY_COLOR),
          });
  ecs_set(world, enemy, Collidable, {.radius = size / 2});
  ecs_set(world, enemy, Rigidbody, RIGIDBODY(size * size / 2));

  return enemy;
}

void SpawnEnemy(ecs_iter_t *it) {
  EnemySpawner *spawner = ecs_field(it, EnemySpawner, 0);

  for (int i = 0; i < it->count; i++) {
    spawner->currentTicks++;
    if (spawner->currentTicks >= spawner->ticksBetweenSpawns) {
      spawner->currentTicks = 0;
      CreateEnemy(it->world);
    }
  }
}

void EnemyTargetPlayer(ecs_iter_t *it) {
  Position *position = ecs_field(it, Position, 1);
  Rotation *rotation = ecs_field(it, Rotation, 2);
  Position closestPlayer[it->count];
  int closestPlayerDistanceSqr[it->count];
  ecs_iter_t targetIt = ecs_query_iter(it->world, playerQuery);
  for (int i = 0; i < it->count; i++) closestPlayerDistanceSqr[i] = INT32_MAX;
  while (ecs_query_next(&targetIt)) {
    if (targetIt.count == 0) {
      continue;
    }
    Position *targetPosition = ecs_field(&targetIt, Position, 0);
    for (int i = 0; i < it->count; i++) {
      for (int j = 0; j < targetIt.count; j++) {
        int distance = Vector2DistanceSqr(position[i], targetPosition[j]);
        if (distance < closestPlayerDistanceSqr[i]) {
          closestPlayer[i] = targetPosition[j];
          closestPlayerDistanceSqr[i] = distance;
        }
      }
    }
  }
  for (int i = 0; i < it->count; i++) {
    if (closestPlayerDistanceSqr[i] == INT32_MAX) continue;
    Vector2 direction =
        Vector2Normalize(Vector2Subtract(closestPlayer[i], position[i]));
    rotation[i] = LerpRotationAngle(rotation[i], direction,
                                    ENEMY_ROTATION_SPEED * it->delta_time);
  }
}

void EnemyAccelerate(ecs_iter_t *it) {
  Rotation *rotation = ecs_field(it, Rotation, 1);
  Acceleration *acceleration = ecs_field(it, Acceleration, 2);
  for (int i = 0; i < it->count; i++) {
    acceleration[i] = Vector2Scale(Vector2Rotate((Vector2){1, 0}, rotation[i]),
                                   ENEMY_MOVEMENT_SPEED);
  }
}

Position *ClosestEnemy(Position origin, ecs_world_t *world, int maxRange) {
  static ecs_query_t *enemyPositionsQuery = NULL;
  if (enemyPositionsQuery == NULL) {
    enemyPositionsQuery =
        ecs_query(world, {.expr = "enemies.Enemy, movement.Position"});
  }
  Position *closestEnemy;
  int closestEnemyDistanceSqr = maxRange * maxRange + 1;
  // find closes enemy
  ecs_iter_t targetIt = ecs_query_iter(world, enemyPositionsQuery);

  while (ecs_query_next(&targetIt)) {
    if (targetIt.count == 0) {
      continue;
    }
    Position *targetPosition = ecs_field(&targetIt, Position, 1);
    for (int i = 0; i < targetIt.count; i++) {
      int distance = Vector2DistanceSqr(origin, targetPosition[i]);
      if (distance <= closestEnemyDistanceSqr) {
        closestEnemy = &targetPosition[i];
        closestEnemyDistanceSqr = distance;
      }
    }
  }
  if (closestEnemyDistanceSqr == maxRange * maxRange + 1) {
    return NULL;
  }
  return closestEnemy;
}

void EnemiesImport(ecs_world_t *world) {
  ECS_MODULE(world, Enemies);

  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Player);
  ECS_COMPONENT_DEFINE(world, EnemySpawner);
  ECS_TAG_DEFINE(world, Enemy);

  playerQuery = ecs_query(world, {.expr = "movement.Position, player.Player"});

  ECS_SYSTEM_DEFINE(world, SpawnEnemy, EcsOnLoad, EnemySpawner);
  ECS_SYSTEM_DEFINE(world, EnemyTargetPlayer, EcsPreUpdate,
                    Enemy, [in] movement.Position, movement.Rotation);
  ECS_SYSTEM_DEFINE(world, EnemyAccelerate, EcsOnUpdate, Enemy,
                    movement.Rotation, movement.Acceleration);

  // ecs_set_tick_source(world, ecs_id(SpawnEnemy), tick_source);
}