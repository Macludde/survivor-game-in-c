#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#define DEBUG_SHOW_HITBOXES

#include "debug.h"
#include "helpers.h"
#include "level.h"
#include "modules/camera.h"
#include "modules/collisions.h"
#include "modules/controls.h"
#include "modules/enemies.h"
#include "modules/health.h"
#include "modules/item.h"
#include "modules/movement.h"
#include "modules/physics.h"
#include "modules/player.h"
#include "modules/render.h"
#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"  // utility header for SearchAndSetResourceDir
#include "systems.h"
#include "tick.h"
#define STB_DS_IMPLEMENTATION
#include "flecs.h"
#include "lib/stb_ds.h"

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 800
#define GAME_NAME "SurvivorGame"

extern int targetFps;
ecs_world_t *world;
Level level;
extern ecs_world_t *world;

void SetupWindow() {
  unsigned int CONFIG_FLAGS = FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI |
                              FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE |
                              FLAG_WINDOW_ALWAYS_RUN;
  SetConfigFlags(CONFIG_FLAGS & ~FLAG_VSYNC_HINT);  // disable vsync for now
  // Create the window and OpenGL contextREEN
  InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, GAME_NAME);
}

int main() {
  double beforeSetup = time_in_seconds();
  SetupWindow();

  world = ecs_init();
  InitializeTickSource(world);
  ecs_set_threads(world, 8);
  ECS_IMPORT(world,
             Camera);  // define first to get order of render stages correct

  ECS_IMPORT(world, Players);
  ECS_IMPORT(world, Health);
  ECS_IMPORT(world, Item);

  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Collisions);
  ECS_IMPORT(world, Physics);

  ECS_IMPORT(world, Enemies);

  ECS_IMPORT(world, Controls);
  ECS_IMPORT(world, Render);
  SetupSystems(world);

  level = (Level){
      .width = 200 * 5,
      .height = 200 * 5,
  };
  InitializeLevel(world, &level);

  ecs_entity_t player = ecs_entity(world, {.name = "Player"});
  ecs_set(world, player, Position, {0, 0});
  ecs_set(world, player, Velocity, {0, 0});
  ecs_set(world, player, Acceleration, {0, 0});
  ecs_set(world, player, CircleShape,
          {.offset = {0, 0}, .radius = 20, .color = BLUE});
  ecs_set(world, player, Collidable, {.radius = 20});
  ecs_set(world, player, Rigidbody, RIGIDBODY(100));
  ecs_set(world, player, MaxSpeed, {200});
  ecs_set(world, player, Killable, KILLABLE(100));
  ecs_set(world, player, Player, {.level = 1});
  ecs_set(world, player, Healthbar, HEALTHBAR_GIVEN_RADIUS(20));
  ecs_add_id(world, player, WASDMovable);
  // ecs_add_pair(world, SimpleGun(world), EcsChildOf, player);
  ecs_add_pair(world, FlameGrenade(world), EcsChildOf, player);
  ecs_add_pair(world, OrbitOrb(world), EcsChildOf, player);

  ecs_entity_t enemySpawner = ecs_entity(world, {.name = "EnemySpawner"});
  ecs_set(world, enemySpawner, EnemySpawner, {.ticksBetweenSpawns = 1000});

  // game loop
  while (!IsWindowReady());
  DEBUG_PRINT("Time to launch: %f", time_in_seconds() - beforeSetup)
  while (!WindowShouldClose())  // run the loop untill the user presses ESCAPE
                                // or presses the Close button on the window
  {
    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
#ifdef DEBUG
    HandleDebuggingKeys();
    if (IsKeyPressed(KEY_F1)) {
      // RemoveAllEnemies();
      Position *p = ecs_get_mut(world, player, Position);
      *p = Vector2Zero();
    }
#endif
    // drawing
    BeginDrawing();
    ClearBackground(WHITE);
    ecs_progress(world, GetFrameTime());

    // draw UI

    DrawFPS(10, 10);
#ifdef DEBUG
    DrawText(TextFormat("TARGET: %i", targetFps), 10, 40, 20, DARKGRAY);
#endif
    EndDrawing();
  }

  ecs_fini(world);

  // destroy the window and cleanup the OpenGL context
  CloseWindow();
  return 0;
}
