#include "modules/enemies.h"

#include <stdbool.h>

#include "flecs.h"
#include "modules/collisions.h"
#include "modules/health.h"
#include "modules/movement.h"
#include "modules/player.h"
#include "raylib.h"
#include "raymath.h"

ECS_COMPONENT_DECLARE(EnemySpawner);
ECS_TAG_DECLARE(Enemy);

ECS_SYSTEM_DECLARE(SpawnEnemy);

static ecs_entity_t enemyPrefab = 0;
static ecs_query_t *playerQuery;

ecs_entity_t CreateEnemyPrefab(ecs_world_t *world) {
  ecs_entity_t enemy = ecs_entity(world, {.add = ecs_ids(EcsPrefab, Enemy)});
  ecs_add(world, enemy, Position);
  ecs_add(world, enemy, Collidable);
  ecs_set(world, enemy, Killable, KILLABLE(20));
  ecs_set(world, enemy, DropsExperience, {20});

  return enemy;
}

#define MIN_DISTANCE_TO_CAMERA 100
#define MAX_DISTANCE_TO_CAMERA 100
ecs_entity_t CreateEnemy(ecs_world_t *world) {
  if (enemyPrefab == 0) {
    enemyPrefab = CreateEnemyPrefab(world);
  }
  float size = ENEMY_DEFAULT_SIZE;
  ecs_entity_t enemy = ecs_new_w_pair(world, EcsIsA, enemyPrefab);
  ecs_set(world, enemy, Position, {10, 10});
  ecs_set(world, enemy, Collidable, {.radius = size / 2});
  ecs_set(world, enemy, Damage, {10});

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

void EnemiesImport(ecs_world_t *world) {
  ECS_MODULE(world, Enemies);

  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Players);
  ECS_COMPONENT_DEFINE(world, EnemySpawner);
  ECS_TAG_DEFINE(world, Enemy);
  ecs_add_pair(world, Enemy, EcsWith, ecs_id(Position));

  playerQuery =
      ecs_query(world, {.expr = "movement.Position, players.PlayerTeam"});

  ECS_SYSTEM_DEFINE(world, SpawnEnemy, EcsOnLoad, EnemySpawner);
}