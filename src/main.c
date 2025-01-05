#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "debug.h"
#include "enemy.h"
#include "level.h"
#include "player.h"
#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"  // utility header for SearchAndSetResourceDir

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 800
#define GAME_NAME "SurvivorGame"

extern int targetFps;
Level level;
EnemySpawner enemySpawner;
Camera2D camera;

void HandleScreenResizing(Camera2D *camera) {
  static int screenWidth;
  static int screenHeight;
  if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
  if (GetScreenWidth() != screenWidth || GetScreenHeight() != screenHeight) {
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    camera->offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
  }
}

void SetupWindow() {
  unsigned int CONFIG_FLAGS = FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI |
                              FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE |
                              FLAG_WINDOW_ALWAYS_RUN;
  SetConfigFlags(CONFIG_FLAGS & ~FLAG_VSYNC_HINT);  // disable vsync for now
  // Create the window and OpenGL contextREEN
  InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, GAME_NAME);
}

int main() {
  SetupWindow();
  Player player = InitialPlayer();
  camera = (Camera2D){
      .offset = {DEFAULT_SCREEN_WIDTH / 2.0f, DEFAULT_SCREEN_HEIGHT / 2.0f},
      .target = player.pos,
      .rotation = 0.0f,
      .zoom = 1.0f,
  };
  level = (Level){
      .width = 1000,
      .height = 1000,
      .treeCount = 20,
  };
  // Level *level = AllocateLevel(1000, 1000, 20); // I chose to keep the level
  // on the stack for now
  InitializeLevel(&level);

  InitializeEnemySpawner(&enemySpawner);
  assert(sizeof(player) <= 1024);
  assert(sizeof(camera) <= 1024);
  assert(sizeof(level) <= 1024);
  assert(sizeof(enemySpawner) <= 1024);
  // DEBUG("Game started");
  // debug_print("Game started\n");

  // game loop
  while (!WindowShouldClose())  // run the loop untill the user presses ESCAPE
                                // or presses the Close button on the window
  {
    HandleScreenResizing(&camera);
#ifdef DEBUG
    HandleDebuggingKeys();
    if (IsKeyPressed(KEY_F1)) RemoveAllEnemies();
#endif
    TickPlayer(&player, &level);
    TickCamera(&camera, player.pos);
    TickEnemySpawner(&camera, &level, &player);

    // drawing
    BeginDrawing();
    ClearBackground(WHITE);

    BeginMode2D(camera);
    // draw world
    DrawLevelBackground(&level);
    DrawEnemies();
    DrawPlayer(&player);
    DrawLevelForeground(&level);

    EndMode2D();

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
  free(player.weapon.bullets);

  // destroy the window and cleanup the OpenGL context
  CloseWindow();
  return 0;
}
