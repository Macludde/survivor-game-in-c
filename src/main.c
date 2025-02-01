#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#define DEBUG_SHOW_HITBOXES

#include "modules/collisions.h"
#include "modules/enemies.h"
#include "modules/health.h"
#include "modules/movement.h"
#include "modules/player.h"
#include "raylib.h"
#include "raymath.h"
#define STB_DS_IMPLEMENTATION
#include "flecs.h"
#include "lib/stb_ds.h"

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 800
#define GAME_NAME "SurvivorGame"

extern int targetFps;

int main() {
  ecs_world_t *world = ecs_init();

  ECS_IMPORT(world, Health);

  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Collisions);

  ECS_IMPORT(world, Players);
  ECS_IMPORT(world, Enemies);

  ecs_entity_t player = ecs_entity(world, {.name = "Player"});
  ecs_set(world, player, Position, {0, 0});
  ecs_set(world, player, Collidable, {.radius = 20});
  ecs_set(world, player, Killable, KILLABLE(100));

  ecs_entity_t enemySpawner = ecs_entity(world, {.name = "EnemySpawner"});
  ecs_set(world, enemySpawner, EnemySpawner, {.ticksBetweenSpawns = 10});

  while (true)  // run the loop untill the user presses ESCAPE
                // or presses the Close button on the window
  {
    ecs_progress(world, GetFrameTime());
  }

  ecs_fini(world);

  return 0;
}
