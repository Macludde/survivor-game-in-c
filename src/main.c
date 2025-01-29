#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "enemy.h"
#include "entity.h"
#include "helpers.h"
#include "item.h"
#include "level.h"
#include "modules/base.h"
#include "modules/camera.h"
#include "modules/collisions.h"
#include "modules/controls.h"
#include "modules/movement.h"
#include "modules/physics.h"
#include "modules/render.h"
#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"  // utility header for SearchAndSetResourceDir
#define STB_DS_IMPLEMENTATION
#include "flecs.h"
#include "lib/stb_ds.h"

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 800
#define GAME_NAME "SurvivorGame"

extern int targetFps;
ecs_world_t *world;
Level level;
EnemySpawner enemySpawner;
extern ecs_world_t *world;

void SetupWindow() {
  unsigned int CONFIG_FLAGS = FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI |
                              FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE |
                              FLAG_WINDOW_ALWAYS_RUN;
  SetConfigFlags(CONFIG_FLAGS & ~FLAG_VSYNC_HINT);  // disable vsync for now
  // Create the window and OpenGL contextREEN
  InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, GAME_NAME);
}

static void MovePlayer(ecs_iter_t *it) {
  const InputStates *i = ecs_singleton_get(it->world, InputStates);
  Acceleration *a = ecs_field(it, Acceleration, 0);

  a->x += i->WASD.x * 400;
  a->y += i->WASD.y * 400;
}

int main() {
  double beforeSetup = time_in_seconds();
  SetupWindow();

  world = ecs_init();
  ecs_set_threads(world, 8);
  ECS_IMPORT(world, Base);
  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Camera);
  ECS_IMPORT(world, Controls);
  ECS_IMPORT(world, Collisions);
  ECS_IMPORT(world, Physics);
  ECS_IMPORT(world, Render);

  ECS_SYSTEM(world, MovePlayer, EcsOnLoad, movement.Acceleration, base.Player);

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
  ecs_set(world, player, Mass, {200});
  ecs_set(world, player, MaxSpeed, {200});
  ecs_add_id(world, player, Player);

  // InitializeEnemySpawner(&enemySpawner);

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
      Position *p = ecs_get(world, player, Position);
      *p = Vector2Zero();
    }
#endif
    // drawing
    BeginDrawing();
    ClearBackground(WHITE);
    ecs_progress(world, GetFrameTime());
    // draw world
    // DrawLevelBackground();
    // DrawEnemies();
    // DrawPlayer(&player);
    // DrawLevelForeground();
    // ItemsCallDrawForeground(&player);

    // #ifdef DEBUG_SHOW_HITBOXES
    //     DrawHitboxes(level.allEntities, arrlen(level.allEntities));
    // #endif

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
