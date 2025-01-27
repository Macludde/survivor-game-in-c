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
#include "modules/controls.h"
#include "modules/movement.h"
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

int main() {
  double beforeSetup = time_in_seconds();
  world = ecs_init();
  ecs_set_threads(world, 8);
  ECS_IMPORT(world, Base);
  ECS_IMPORT(world, Movement);
  ECS_IMPORT(world, Render);
  ECS_IMPORT(world, Camera);
  ECS_IMPORT(world, Controls);

  SetupWindow();
  level = (Level){
      .width = 10000,
      .height = 10000,
  };
  InitializeLevel(&level);

  ecs_entity_t player = ecs_entity(world, {.name = "Player"});
  ecs_set(world, player, Position, {0, 0});
  ecs_set(world, player, Velocity, {0, 0});
  ecs_set(world, player, Acceleration, {0, 0});
  ecs_set(world, player, CircleShape,
          {.offset = {0, 0}, .radius = 20, .color = BLUE});
  ecs_set(world, player, FollowCam,
          {
              .offset = {DEFAULT_SCREEN_WIDTH / 2, DEFAULT_SCREEN_HEIGHT / 2},
              .target = {0, 0},
              .rotation = 0.0f,
              .zoom = 1.0f,
          });
  ecs_add_id(world, player, Player);

  const FollowCam *cam = ecs_get(world, player, FollowCam);

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
      RemoveAllEnemies();
      // player.entity.body.pos = Vector2Zero();
    }
#endif
    // drawing
    BeginDrawing();
    ClearBackground(WHITE);

    ecs_progress(world, GetFrameTime());
    DrawCircle(40, 40, 10, RED);
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

    // end the frame and get ready for the next one
    EndDrawing();
  }

  free(level.trees);
  free(enemySpawner.enemies);
  arrfree(level.allEntities);
  FreeItems();
  ecs_fini(world);

  // destroy the window and cleanup the OpenGL context
  CloseWindow();
  return 0;
}
