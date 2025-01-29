#include "./enemy.h"

#include <stdbool.h>

#include "./base.h"
#include "./camera.h"
#include "./collisions.h"
#include "./movement.h"
#include "./physics.h"
#include "./render.h"
#include "flecs.h"
#include "helpers.h"
#include "level.h"
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
      ecs_entity(world, {.name = "Enemy", .add = ecs_ids(EcsPrefab)});
  ecs_add(world, enemy, Position);
  ecs_set(world, enemy, Velocity, {0, 0});
  ecs_set(world, enemy, Acceleration, {0, 0});
  ecs_set(world, enemy, Rotation, {0});
  ecs_add(world, enemy, RectShape);
  ecs_add(world, enemy, Collidable);
  ecs_add(world, enemy, Mass);

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
  ecs_set(world, enemy, Collidable, {.radius = size});
  ecs_set(world, enemy, Mass, {size * size});

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
  ecs_iter_t playerIt = ecs_query_iter(it->world, playerQuery);
  Position closestPlayer = {INFINITY, INFINITY};
  int closestPlayerDistanceSqr = INFINITY;
  for (int i = 0; i < it->count; i++) {
    while (ecs_query_next(&playerIt)) {
      Position *playerPosition = ecs_field(&playerIt, Position, 0);
      if (playerIt.count == 1 && closestPlayerDistanceSqr == INFINITY) {
        closestPlayer = playerPosition[0];
        closestPlayerDistanceSqr =
            Vector2DistanceSqr(position[i], closestPlayer);
      }
      for (int j = 0; j < playerIt.count; j++) {
        int distance = Vector2DistanceSqr(position[i], playerPosition[j]);
        if (distance < closestPlayerDistanceSqr) {
          closestPlayer = playerPosition[j];
          closestPlayerDistanceSqr = distance;
        }
      }
    }
    if (closestPlayerDistanceSqr == INFINITY) continue;
    Vector2 direction = Vector2Subtract(closestPlayer, position[i]);
    float angle = atan2f(direction.y, direction.x);
    rotation[i] = Lerp(rotation[i], angle, ENEMY_ROTATION_SPEED);
    closestPlayerDistanceSqr = INFINITY;
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

void EnemyImport(ecs_world_t *world) {
  ECS_MODULE(world, Enemy);

  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Base);
  ECS_COMPONENT_DEFINE(world, EnemySpawner);
  ECS_TAG_DEFINE(world, Enemy);

  playerQuery = ecs_query(world, {.expr = "movement.Position, base.Player"});

  ECS_SYSTEM_DEFINE(world, SpawnEnemy, EcsOnLoad, EnemySpawner);
  ECS_SYSTEM_DEFINE(world, EnemyTargetPlayer, EcsPreUpdate, Enemy,
                    movement.Rotation);
  ECS_SYSTEM_DEFINE(world, EnemyAccelerate, EcsOnUpdate, Enemy,
                    movement.Rotation, movement.Acceleration);

  // ecs_set_tick_source(world, ecs_id(SpawnEnemy), tick_source);
}